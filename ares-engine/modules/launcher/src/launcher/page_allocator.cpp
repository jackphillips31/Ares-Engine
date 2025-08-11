#include <ares_launcher_pch.h>
#include "core/page_allocator.h"
#include "core/internal/os_page_interface.h"

namespace ares::core {

	using namespace internal;
	os_page_interface* page_interface = get_os_page_interface();

	page_allocator::page_allocator()
		: size_pool_(), address_pool_(), page_size_(page_interface->page_size())
	{
		const size_t each_node_range_size = page_size_ * 2;
		arena_size_ = size_t{ 1 } << 35; // 32GB
		arena_ = page_interface->reserve_memory(arena_size_);
		nodes_range_ = arena_;
		nodes_range_size_ = each_node_range_size * 2;
		range_start_ = static_cast<void*>(static_cast<char*>(arena_) + nodes_range_size_);

		page_interface->commit_memory(nodes_range_, nodes_range_size_);

		const size_t size_node_size = (sizeof(size_node) + alignof(size_node) - 1) & ~(alignof(size_node) - 1);
		const size_t address_node_size = (sizeof(address_node) + alignof(address_node) - 1) & ~(alignof(address_node) - 1);
		size_pool_ = slab_pool<size_node>(nodes_range_, each_node_range_size);
		address_pool_ = slab_pool<address_node>(static_cast<void*>(static_cast<char*>(arena_) + each_node_range_size), each_node_range_size);

		const size_t block_size_in_pages = (arena_size_ - nodes_range_size_) / page_size_;
		size_node* s_node = new (size_pool_.allocate()) size_node({
			block_size_in_pages,
			size_bucket{nullptr}
		});
		address_node* a_node = new(address_pool_.allocate()) address_node({
			reinterpret_cast<uintptr_t>(range_start_),
			range_info{
				block_size_in_pages,
				false,
				nullptr,
				nullptr
			}
		});

		s_node->data.second.head = a_node;

		size_tree_.insert(s_node);
		address_tree_.insert(a_node);
	}

	page_allocator::~page_allocator()
	{
		size_tree_.clear();
		address_tree_.clear();
		page_interface->decommit_memory(arena_, arena_size_);
		page_interface->release_memory(arena_, arena_size_);
	}

	void* page_allocator::commit_pages(size_t bytes)
	{
		assert(bytes % page_size_ == 0 && "Requested size must be a multiple of the page size!");
		size_t want_pages = bytes / page_size_;

		auto size_it = size_tree_.lower_bound(want_pages);
		assert(size_it != size_tree_.end() && "Out of memory!");

		size_node* s_node = size_it.get_node();
		size_bucket& s_node_bucket = s_node->data.second;

		address_node* a_node = s_node_bucket.head;
		range_info& a_node_info = a_node->data.second;

		bool original_s_node_erased = false;
		size_t original_size_in_pages = a_node_info.size_in_pages;

		s_node_bucket.head = a_node_info.next;
		if (s_node_bucket.head)
		{
			s_node_bucket.head->data.second.parent = nullptr;
		}
		else
		{
			size_tree_.erase(s_node);
			s_node->~size_node();
			original_s_node_erased = true;
		}

		a_node_info.next = nullptr;
		a_node_info.parent = nullptr;

		if (original_size_in_pages == want_pages)
		{
			a_node_info.committed = true;
			void* to_commit = reinterpret_cast<void*>(a_node->data.first);
			page_interface->commit_memory(to_commit, want_pages * page_size_);
			return to_commit;
		}

		const uintptr_t new_node_start = a_node->data.first;
		address_tree_.erase(a_node);
		a_node->~address_node();

		address_node* new_a_node = new (address_pool_.allocate()) address_node({
			new_node_start,
			range_info{
				want_pages,
				true,
				nullptr,
				nullptr
			}
		});
		address_node* split_a_node = new(address_pool_.allocate()) address_node({
			new_node_start + want_pages * page_size_,
			range_info{
				original_size_in_pages - want_pages,
				false,
				nullptr,
				nullptr
			}
		});
		range_info& new_a_info = new_a_node->data.second;
		range_info& split_a_info = split_a_node->data.second;

		size_node* split_s_node = get_or_create_size_node(original_size_in_pages - want_pages, original_s_node_erased ? s_node : nullptr);
		size_bucket& split_s_bucket = split_s_node->data.second;

		split_a_info.next = split_s_bucket.head;
		if (split_a_info.next)
		{
			split_a_info.next->data.second.parent = split_a_node;
		}
		split_s_bucket.head = split_a_node;

		address_tree_.insert(new_a_node);
		address_tree_.insert(split_a_node);

		void* to_commit = reinterpret_cast<void*>(new_a_node->data.first);
		page_interface->commit_memory(to_commit, want_pages * page_size_);
		return to_commit;
	}

	void page_allocator::decommit_pages(void* ptr)
	{
		address_node* a_node = nullptr;

		auto address_it = address_tree_.find(reinterpret_cast<uintptr_t>(ptr));
		if (address_it != address_tree_.end())
		{
			a_node = address_it.get_node();
		}
		else
		{
			assert(false && "Corresponding range node not found! This shouldn't happen!");
		}

		page_interface->decommit_memory(ptr, a_node->data.second.size_in_pages * page_size_);

		auto prev_a_it = eastl::prev(address_it);
		auto next_a_it = eastl::next(address_it);

		address_node* prev_a_node = nullptr;
		address_node* next_a_node = nullptr;
		size_node* prev_s_node = nullptr;
		size_node* next_s_node = nullptr;

		uint8_t coalesce_mask = 0;
		if (prev_a_it != address_tree_.end() && !prev_a_it.get_node()->data.second.committed)
		{
			coalesce_mask |= 1;
			prev_a_node = prev_a_it.get_node();
			prev_s_node = get_or_create_size_node(prev_a_node->data.second.size_in_pages);
		}
		if (next_a_it != address_tree_.end() && !next_a_it.get_node()->data.second.committed)
		{
			coalesce_mask |= 2;
			next_a_node = next_a_it.get_node();
			next_s_node = get_or_create_size_node(next_a_node->data.second.size_in_pages);
		}

		size_t new_size_in_pages = 0;
		uintptr_t new_address = 0;
		void* size_mem = nullptr;

		switch (coalesce_mask)
		{
		case 0: // Neither valid and uncommitted
		{
			size_node* s_node = get_or_create_size_node(a_node->data.second.size_in_pages);

			range_info& a_node_info = a_node->data.second;
			a_node_info.committed = false;
			a_node_info.next = s_node->data.second.head;
			a_node_info.parent = nullptr;
			if (a_node_info.next)
			{
				a_node_info.next->data.second.parent = a_node;
			}
			s_node->data.second.head = a_node;
			return;
		}
		case 1: // Only previous
		{
			new_size_in_pages = prev_a_node->data.second.size_in_pages + a_node->data.second.size_in_pages;
			new_address = prev_a_node->data.first;

			address_tree_.erase(a_node);
			a_node->~address_node();

			remove_address_and_maybe_size(prev_a_node, prev_s_node, size_mem);
			break;
		}
		case 2: // Only next
		{
			new_size_in_pages = prev_a_node->data.second.size_in_pages + a_node->data.second.size_in_pages;
			new_address = next_a_node->data.first;

			address_tree_.erase(a_node);
			a_node->~address_node();

			remove_address_and_maybe_size(next_a_node, next_s_node, size_mem);
			break;
		}
		case 3: // Both available
		{
			new_size_in_pages = prev_a_node->data.second.size_in_pages + a_node->data.second.size_in_pages + next_a_node->data.second.size_in_pages;
			new_address = prev_a_node->data.first;

			address_tree_.erase(a_node);
			a_node->~address_node();

			void* temp_mem = nullptr;

			remove_address_and_maybe_size(prev_a_node, prev_s_node, size_mem);
			remove_address_and_maybe_size(next_a_node, next_s_node, temp_mem);

			size_pool_.deallocate(temp_mem);
			break;
		}
		}

		if (new_size_in_pages != 0 && new_address != 0)
		{
			size_node* new_s_node = new (size_mem) size_node({
				new_size_in_pages,
				size_bucket{nullptr}
			});
			address_node* new_a_node = new (a_node) address_node({
				new_address,
				range_info{
					new_size_in_pages,
					false,
					nullptr,
					nullptr
				}
			});

			auto [new_s_it, new_s_inserted] = size_tree_.insert(new_s_node);
			if (!new_s_inserted)
			{
				new_s_node->~size_node();
				size_pool_.deallocate(new_s_node);
				new_s_node = new_s_it.get_node();
			}

			new_a_node->data.second.next = new_s_node->data.second.head;
			if (new_a_node->data.second.next)
			{
				new_a_node->data.second.next->data.second.parent = new_a_node;
			}
			new_s_node->data.second.head = new_a_node;
		}
		else
		{
			assert(false && "Decommit failed.. This shouldn't happen!");
		}
	}

	inline page_allocator::size_node* page_allocator::get_or_create_size_node(size_t size_in_pages, void* mem)
	{
		size_node* s_node = new (mem ? mem : size_pool_.allocate()) size_node({
			size_in_pages,
			size_bucket{nullptr}
		});

		auto [s_it, s_inserted] = size_tree_.insert(s_node);
		if (!s_inserted)
		{
			s_node->~size_node();
			size_pool_.deallocate(s_node);
		}

		return s_it.get_node();
	}

	inline void page_allocator::remove_address_and_maybe_size(address_node* a_node, size_node* s_node, void*& size_mem_out)
	{
		remove_from_free_list(a_node, s_node);

		address_tree_.erase(a_node);
		a_node->~address_node();
		address_pool_.deallocate(a_node);

		if (!s_node->data.second.head)
		{
			size_tree_.erase(s_node);
			s_node->~size_node();
			size_mem_out = s_node;
		}
		else
		{
			size_mem_out = size_pool_.allocate();
		}
	}

	inline void page_allocator::remove_from_free_list(address_node* a_node, size_node* s_node)
	{
		if (a_node->data.second.parent && a_node->data.second.next)
		{
			a_node->data.second.parent->data.second.next = a_node->data.second.next;
			a_node->data.second.next->data.second.parent = a_node->data.second.parent;
		}
		else if (a_node->data.second.next)
		{
			a_node->data.second.next->data.second.parent = nullptr;
			s_node->data.second.head = a_node->data.second.next;
		}
		else
		{
			s_node->data.second.head = nullptr;
		}
		a_node->data.second.next = nullptr;
		a_node->data.second.parent = nullptr;
	}
}