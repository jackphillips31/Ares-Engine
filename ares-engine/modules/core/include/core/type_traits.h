#ifndef ARES_CORE_TYPE_TRAITS_H
#define ARES_CORE_TYPE_TRAITS_H
#include <EASTL/type_traits.h>

namespace ares::core {

	// This forward declaration is temporary!
	class allocator_base;

	template <typename T, typename = void>
	struct is_ares_allocator : eastl::false_type{};

	template <typename T>
	struct is_ares_allocator<T, eastl::void_t<
		decltype(eastl::declval<T&>().allocate(eastl::declval<size_t>())),
		decltype(eastl::declval<T&>().allocate(eastl::declval<size_t>(),
			eastl::declval<size_t>(),
			eastl::declval<size_t>())),
		decltype(eastl::declval<T&>().deallocate(eastl::declval<void*>()))>
	> : eastl::is_base_of<allocator_base, T> {};

	template <typename T>
	inline constexpr bool is_ares_allocator_v = is_ares_allocator<T>::value;

	template <typename T>
	struct is_nothrow_swappable : eastl::bool_constant<eastl::is_nothrow_move_constructible_v<T> && eastl::is_nothrow_move_assignable_v<T>>
	{
	};

	template <typename T>
	inline constexpr bool is_nothrow_swappable_v = is_nothrow_swappable<T>::value;
}

#endif // ARES_CORE_TYPE_TRAITS_H