#ifndef ARES_CORE_PAGE_ALLOCATOR_H
#define ARES_CORE_PAGE_ALLOCATOR_H
#include "core/core_api.h"
#include "core/avl_tree.h"
#include "core/slab_pool.h"

namespace ares::core {

	class page_allocator
	{
	private:
		struct size_bucket;
		struct range_info;

		using size_tree = avl_tree<size_t, size_bucket, void>;
		using size_node = size_tree::node;
		using address_tree = avl_tree<uintptr_t, range_info, void>;
		using address_node = address_tree::node;

		struct size_bucket
		{
			address_node* head = nullptr;
		};
		struct range_info
		{
			size_t size_in_pages = 0;
			bool committed = false;
			address_node* next = nullptr;
			address_node* parent = nullptr;
		};

	public:
		page_allocator();
		~page_allocator();

		void* commit_pages(size_t bytes);
		void decommit_pages(void* ptr);

	private:
		size_node* get_or_create_size_node(size_t size_in_pages, void* mem = nullptr);
		void remove_address_and_maybe_size(address_node* a_node, size_node* s_node, void*& size_mem_out);
		void remove_from_free_list(address_node* a_node, size_node* s_node);

	private:
		void* arena_;
		void* nodes_range_;
		void* range_start_;
		size_t arena_size_;
		size_t nodes_range_size_;
		size_t page_size_;
		size_tree size_tree_;
		address_tree address_tree_;
		slab_pool<size_node> size_pool_;
		slab_pool<address_node> address_pool_;
	};

}

#endif // ARES_CORE_PAGE_ALLOCATOR_H