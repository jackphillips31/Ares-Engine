///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2025 Jack Phillips. All rights reserved.
// 
///////////////////////////////////////////////////////////////////////////////

#ifndef ARES_CORE_TYPE_TRAITS_H
#define ARES_CORE_TYPE_TRAITS_H

#include "core/internal/config.h"

#if ARES_CORE_EASTL_DISABLED
#include <type_traits.h>
// STL type traits

namespace ares::core {

	template <typename type_t, type_t v>
	struct integral_constant
	{
		static ARES_CORE_CONSTEXPR type_t value = v;
		using value_type = type_t;
		using type = integral_constant<type_t, v>;
		ARES_CORE_CONSTEXPR operator type_t() const noexcept { return v; }
		ARES_CORE_CONSTEXPR type_t operator()() const noexcept { return v; }
	}

	using false_type = integral_constant<bool, false>;

}

#else
#include <EASTL/type_traits.h>
// EASTL type traits

namespace ares::core {

	// This forward declaration is temporary!
	class allocator;

	template <typename T, typename = void>
	struct is_ares_allocator : eastl::false_type{};

	template <typename T>
	struct is_ares_allocator<T, eastl::void_t<
		decltype(eastl::declval<T&>().allocate(eastl::declval<size_t>())),
		decltype(eastl::declval<T&>().allocate(eastl::declval<size_t>(),
			eastl::declval<size_t>())),
		decltype(eastl::declval<T&>().deallocate(eastl::declval<void*>())),
		decltype(noexcept(eastl::declval<T&>().deallocate(eastl::declval<void*>())))
	>> : eastl::is_base_of<allocator, T> {};

	template <typename T>
	inline constexpr bool is_ares_allocator_v = is_ares_allocator<T>::value;

	template <typename T>
	struct is_nothrow_swappable : eastl::bool_constant<eastl::is_nothrow_move_constructible_v<T> && eastl::is_nothrow_move_assignable_v<T>>
	{
	};

	template <typename T>
	inline constexpr bool is_nothrow_swappable_v = is_nothrow_swappable<T>::value;
}
#endif

// Ares Core type_traits
namespace ares::core {

	template <typename type, typename = void>
	struct is_ares::allocator : public false_type{};

}

#endif // ARES_CORE_TYPE_TRAITS_H