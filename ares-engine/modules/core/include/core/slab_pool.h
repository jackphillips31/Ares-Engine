#ifndef ARES_CORE_SLAB_POOL_H
#define ARES_CORE_SLAB_POOL_H
#include "core/atomic.h"

namespace ares::core {

	template <typename T>
	class slab_pool
	{
	public:
		slab_pool()
			: buffer_(nullptr),
			buffer_size_(0),
			block_size_(get_block_size()),
			block_count_(0),
			free_list_(nullptr)
		{
		}
		slab_pool(void* buffer, size_t buffer_size)
			: buffer_(buffer),
			buffer_size_(buffer_size),
			block_size_(get_block_size()),
			block_count_(0),
			free_list_(nullptr)
		{
			block_count_ = buffer_size_ / block_size_;
			free_list_.store(nullptr, memory_order_relaxed);

			for (size_t i = 0; i < block_count_; i++)
			{
				void* block = static_cast<char*>(buffer_) + i * block_size_;
				free_block* f_block = static_cast<free_block*>(block);
				f_block->next = free_list_.load(memory_order_relaxed);
				free_list_.store(f_block, memory_order_relaxed);
			}
		}

		slab_pool(const slab_pool&) = delete;
		slab_pool& operator=(const slab_pool&) = delete;
		slab_pool(slab_pool&& other) noexcept
			: buffer_(eastl::exchange(other.buffer_, nullptr)),
			buffer_size_(eastl::exchange(other.buffer_size_, 0)),
			block_size_(other.block_size_),
			block_count_(eastl::exchange(other.block_count_, 0)),
			free_list_(other.free_list_.exchange(nullptr))
		{
		}
		slab_pool& operator=(slab_pool&& other) noexcept
		{
			if (this != &other)
			{
				buffer_ = eastl::exchange(other.buffer_, nullptr);
				buffer_size_ = eastl::exchange(other.buffer_size_, 0);
				block_size_ = other.block_size_;
				block_count_ = eastl::exchange(other.block_count_, 0);
				free_list_.store(other.free_list_.exchange(nullptr));
			}
			return *this;
		}

		T* allocate()
		{
			free_block* head = nullptr;
			free_block* next = nullptr;

			do
			{
				head = free_list_.load(memory_order_aquire);
				if (!head)
				{
					return nullptr;
				}
				next = head->next;
			}
			while (!free_list_.compare_exchange_weak(
				head, next,
				memory_order_aquire,
				memory_order_relaxed));

			new (head) T();
			return reinterpret_cast<T*>(head);
		}
		void deallocate(void* ptr)
		{
			if (!ptr) return;

			reinterpret_cast<T*>(ptr)->~T();
			free_block* block = static_cast<free_block*>(ptr);
			free_block* head = nullptr;

			do
			{
				head = free_list_.load(memory_order_relaxed);
				block->next = head;
			}
			while (!free_list_.compare_exchange_weak(
				head, block,
				memory_order_release,
				memory_order_relaxed));
		}

	private:
		size_t get_block_size()
		{
			if (type_size_ < sizeof(free_block))
			{
				return (sizeof(free_block) + alignof(free_block) - 1) & ~(alignof(free_block) - 1);
			}
			return (type_size_ + alignof(T) - 1) & ~(alignof(T) - 1);
		}

	private:
		struct free_block
		{
			free_block* next;
		};

		void* buffer_;
		size_t buffer_size_;
		size_t block_size_;
		size_t block_count_;
		atomic<free_block*> free_list_;

		static constexpr size_t type_size_ = sizeof(T);
	};

}

#endif // ARES_CORE_SLAB_POOL_H