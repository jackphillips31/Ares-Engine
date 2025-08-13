#ifndef ARES_CORE_SYS_ALLOCATOR_H
#define ARES_CORE_SYS_ALLOCATOR_H
#include <EASTL/type_traits.h>
#include <EASTL/utility.h>
#include "core/allocator.h"
#include "core/allocator_manager.h"

namespace ares::core {

	template <typename T>
	class sys_allocator
	{
	private:
		static_assert(eastl::is_base_of<allocator, T>::value, "Allocator must derive from 'allocator'!");
	public:
		sys_allocator(
			const char* name = EASTL_NAME_VAL(EASTL_ALLOCATOR_DEFAULT_NAME),
			allocator* alloc = get_allocator<T>()
		);
		sys_allocator(allocator* alloc);
		sys_allocator(const sys_allocator& other);
		sys_allocator& operator=(const sys_allocator& other);
		sys_allocator(sys_allocator&& other) noexcept;
		sys_allocator& operator=(sys_allocator&& other) noexcept;
		~sys_allocator() = default;

		inline void* allocate(size_t size, int flags = 0) { return allocator_->allocate(size); }
		inline void* allocate(size_t size, size_t alignment, size_t offset = 0, int flags = 0) { return allocator_->allocate(size, alignment); }
		inline void deallocate(void* ptr, size_t size = 0) { allocator_->deallocate(ptr); }
	private:
		const char* name_;
		allocator* allocator_;
	};

	template <typename T>
	sys_allocator<T>::sys_allocator(const char* name, allocator* alloc)
		: name_(name), allocator_(alloc)
	{
	}

	template <typename T>
	sys_allocator<T>::sys_allocator(allocator* alloc)
		: name_(EASTL_NAME_VAL(EASTL_ALLOCATOR_DEFAULT_NAME)), allocator_(alloc)
	{
		if (!alloc)
		{
			allocator_ = get_allocator<T>();
		}
	}

	template <typename T>
	sys_allocator<T>::sys_allocator(const sys_allocator& other)
		: name_(other.name_), allocator_(other.allocator_)
	{
	}

	template <typename T>
	sys_allocator<T>& sys_allocator<T>::operator=(const sys_allocator& other)
	{
		if (this != &other)
		{
			name_ = other.name_;
			allocator_ = other.allocator_;
		}
		return *this;
	}

	template <typename T>
	sys_allocator<T>::sys_allocator(sys_allocator&& other) noexcept
		: name_(eastl::exchange(other.name_, nullptr)), allocator_(eastl::exchange(other.allocator_, nullptr))
	{
	}

	template <typename T>
	sys_allocator<T>& sys_allocator<T>::operator=(sys_allocator&& other) noexcept
	{
		if (this != &other)
		{
			name_ = eastl::exchange(other.name_, nullptr);
			allocator_ = eastl::exchange(other.allocator_, nullptr);
		}
		return *this;
	}

}

#endif // ARES_CORE_SYS_ALLOCATOR_H