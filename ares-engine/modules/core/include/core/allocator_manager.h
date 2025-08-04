#ifndef ARES_CORE_ALLOCATOR_MANAGER_H
#define ARES_CORE_ALLOCATOR_MANAGER_H
#include <shared_mutex>
#include "core/core_api.h"

namespace ares::core {

	class allocator;

	class ARES_CORE_API allocator_manager
	{
	public:
		allocator_manager();
		~allocator_manager();

		template <typename T, typename... Args>
		void register_allocator(Args&&... args);

		template <typename T>
		void unregister_allocator();

		template <typename T>
		allocator* get_allocator();

	private:
		struct allocator_entry
		{
			allocator_entry() = default;
			allocator_entry(allocator_entry&& other) noexcept;
			allocator_entry& operator=(allocator_entry&& other) noexcept;

			allocator* allocator = nullptr;
			size_t hash_id = 0;
			bool occupied = false;
		};

	private:
		void allocate_new_entries(size_t new_capacity);
		size_t find_index(size_t hash, size_t capacity = 0, allocator_entry* entries = nullptr);

	private:
		mutable std::shared_mutex mutex_;
		allocator_entry* entries_;
		size_t capacity_;
		size_t count_;
		float load_factor_ = 0.7f;
	};

	allocator_manager& get_allocator_manager();

}

#endif // ARES_CORE_ALLOCATOR_MANAGER_H