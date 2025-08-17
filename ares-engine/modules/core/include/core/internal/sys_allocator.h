///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2025 Jack Phillips. All rights reserved.
// 
///////////////////////////////////////////////////////////////////////////////

#ifndef ARES_CORE_INTERNAL_SYS_ALLOCATOR_H
#define ARES_CORE_INTERNAL_SYS_ALLOCATOR_H

#include "core/internal/common.h"
#include "core/allocator_base.h"
#include "core/allocator_manager.h"

#if ARES_CORE_EASTL_DISABLED
// STL Configuration
#include <type_traits.h>

namespace ares::core::internal {

	template <typename allocator_type>
	class sys_allocator
	{
	private:
		static_assert(std::is_base_of<allocator_base, allocator_type>, "core::internal::sys_allocator: allocator_type must derive from 'core::allocator_base'");

	public:
		using value_type = char;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;
		using propagate_on_container_copy_assignment = std::true_type;
		using propagate_on_container_move_assignment = std::true_type;
		using propagate_on_container_swap = std::true_type;

		sys_allocator(allocator_base* alloc = get_allocator<allocator_type>())
		~sys_allocator() = defualt;

		sys_allocator(const sys_allocator& other) = default;
		sys_allocator& operator=(const sys_allocator& other) = default;
		sys_allocator(sys_allocator&& other) ARES_NOEXCEPT = default;
		sys_allocator& operator=(sys_allocator&& other) ARES_NOEXCEPT = default;

		template <typename alloc_type>
		sys_allocator(const sys_allocator<alloc_type>& other) : allocator_(other.get_raw_allocator()) {}

		void* allocate(size_t size) { return allocator_->allocate(size); }
		void* allocate(size_t size, size_t alignment) { return allocator_->allocate(size, alignment); }
		void* allocate(void* ptr, size_t size) ARES_NOEXCEPT { allocator_->deallocate(ptr); }

		bool operator==(const sys_allocator& other) const ARES_NOEXCEPT { return allocator_ == other.allocator_; }
		bool operator!=(const sys_allocator& other) const ARES_NOEXCEPT { return !(*this == other); }

		allocator_base* get_raw_allocator() const { return allocator_; }

	private:
		allocator_base* allocator_;
	};

	template <typename allocator_type>
	sys_allocator<allocator_type>::sys_allocator(allocator_base* alloc)
		: allocator_(alloc)
	{
		if (!alloc)
		{
			allocator_ = get_allocator<allocator_type>();
		}
	}

	template <typename allocator_type>
	template <typename other_type>
	struct sys_allocator<allocator_type>::rebind
	{
		using other = sys_allocator<allocator_type>;
	};

}

#else
// EASTL Configuration
#include <EASTL/type_traits.h>
#include <EASTL/utility.h>

namespace ares::core::internal {

	template <typename allocator_type>
	class sys_allocator
	{
	private:
		static_assert(eastl::is_base_of<allocator_base, allocator_type>::value, "internal/sys_allocator must derive from 'allocator_base'");
	};

}
#endif
namespace ares::core::internal {

	template <typename allocator_type>
	class sys_allocator
	{
	private:
		static_assert(eastl::is_base_of<allocator_base, allocator_type>::value, "Allocator must derive from 'allocator'!");
	public:
		sys_allocator(
			const char* name = EASTL_NAME_VAL(EASTL_ALLOCATOR_DEFAULT_NAME),
			allocator_base* alloc = get_allocator<allocator_type>()
		);
		~sys_allocator() = default;

		sys_allocator(allocator_type* alloc);
		sys_allocator(const sys_allocator& other);
		sys_allocator& operator=(const sys_allocator& other);
		sys_allocator(sys_allocator&& other) noexcept;
		sys_allocator& operator=(sys_allocator&& other) noexcept;

		inline void* allocate(size_t size, int flags = 0) { return allocator_->allocate(size); }
		inline void* allocate(size_t size, size_t alignment, size_t offset = 0, int flags = 0) { return allocator_->allocate(size, alignment); }
		inline void deallocate(void* ptr, size_t size = 0) { allocator_->deallocate(ptr); }
	private:
		const char* name_;
		allocator_type* allocator_;
	};

	template <typename allocator_type>
	sys_allocator<allocator_base>::sys_allocator(const char* name, allocator* alloc)
		: name_(name), allocator_(alloc)
	{
	}

	template <typename allocator_type>
	sys_allocator<allocator_base>::sys_allocator(allocator* alloc)
		: name_(EASTL_NAME_VAL(EASTL_ALLOCATOR_DEFAULT_NAME)), allocator_(alloc)
	{
		if (!alloc)
		{
			allocator_ = get_allocator<allocator_base>();
		}
	}

	template <typename allocator_type>
	sys_allocator<allocator_base>::sys_allocator(const sys_allocator& other)
		: name_(other.name_), allocator_(other.allocator_)
	{
	}

	template <typename allocator_type>
	sys_allocator<allocator_base>& sys_allocator<allocator_base>::operator=(const sys_allocator& other)
	{
		if (this != &other)
		{
			name_ = other.name_;
			allocator_ = other.allocator_;
		}
		return *this;
	}

	template <typename allocator_type>
	sys_allocator<allocator_base>::sys_allocator(sys_allocator&& other) noexcept
		: name_(eastl::exchange(other.name_, nullptr)), allocator_(eastl::exchange(other.allocator_, nullptr))
	{
	}

	template <typename allocator_type>
	sys_allocator<allocator_base>& sys_allocator<allocator_base>::operator=(sys_allocator&& other) noexcept
	{
		if (this != &other)
		{
			name_ = eastl::exchange(other.name_, nullptr);
			allocator_ = eastl::exchange(other.allocator_, nullptr);
		}
		return *this;
	}

}

#endif // ARES_CORE_INTERNAL_SYS_ALLOCATOR_H