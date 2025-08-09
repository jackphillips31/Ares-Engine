#ifndef ARES_CORE_SYS_ALLOCATOR_H
#define ARES_CORE_SYS_ALLOCATOR_H
#include <EASTL/type_traits.h>
#include "core/allocator.h"
#include "core/allocator_manager.h"

namespace ares::core::internal {

	template <typename T>
	class sys_allocator
	{
	private:
		static_assert(eastl::is_base_of<allocator, T>::value, "Allocator must derive from 'allocator'!");
	public:
		sys_allocator(
			const char* name = EASTL_NAME_VAL(EASTL_ALLOCATOR_DEFAULT_NAME),
			allocator* alloc = get_allocator_manager.get_allocator<T>()
		)	: name_(name), allocator_(alloc)
		{
		}
		~sys_allocator() = default;
		sys_allocator(const sys_allocator& other)
			: name_(other.name_), allocator_(other.allocator_)
		{
		}
		sys_allocator& operator=(const sys_allocator& other)
		{
			if (this != &other)
			{
				name_ = other.name_;
				allocator_ = other.allocator_;
			}
			return *this;
		}

		inline void* allocate(size_t size, int flags = 0) { return allocator_->allocate(size); }
		inline void* allocate(size_t size, size_t alignment, size_t offset = 0, int flags = 0) { return allocator_->allocate(size, alignment); }
		inline void deallocate(void* ptr, size_t size = 0) { allocator_->deallocate(ptr); }
	private:
		const char* name_;
		allocator* allocator_;
	};

}

#endif // ARES_CORE_SYS_ALLOCATOR_H