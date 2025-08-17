///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2025 Jack Phillips. All rights reserved.
// 
///////////////////////////////////////////////////////////////////////////////

#ifndef ARES_CORE_INTERNAL_AVL_NODE_H
#define ARES_CORE_INTERNAL_AVL_NODE_H

#include <EASTL/type_traits.h>
#include <EASTL/utility.h>
#include "core/internal/common.h"

namespace ares::core {

	template <typename key, typename value, typename compare, typename allocator>
	class avl_tree;

	template <typename key, typename value = void>
	struct avl_node
	{
	private:
		template <typename k, typename v, typename c, typename a>
		friend class avl_tree;
		struct empty_value {};

	public:
		using value_type = eastl::conditional_t<
			eastl::is_void_v<value>,
			key,
			eastl::pair<const key, value>
		>;

		avl_node() ARES_NOEXCEPT = default;
		~avl_node() ARES_NOEXCEPT = default;

		avl_node(const avl_node& other) ARES_NOEXCEPT_IF(eastl::is_nothrow_copy_constructible_v<value_type>);
		avl_node& operator=(const avl_node& other) ARES_NOEXCEPT_IF(eastl::is_nothrow_copy_assignable_v<value_type>);
		avl_node(avl_node&& other) ARES_NOEXCEPT_IF(eastl::is_nothrow_move_constructible_v<value_type>);
		avl_node& operator=(avl_node&& other) ARES_NOEXCEPT_IF(eastl::is_nothrow_move_assignable_v<value_type>);

		avl_node(const value_type& kv) ARES_NOEXCEPT_IF(eastl::is_nothrow_copy_constructible_v<value_type>) : data(kv) {}
		avl_node(value_type&& kv) ARES_NOEXCEPT_IF(eastl::is_nothrow_move_constructible_v<value_type>) : data(eastl::move(kv)) {}

		void swap(avl_node& other) ARES_NOEXCEPT;
		void reset() ARES_NOEXCEPT;

	private:
		avl_node* left = nullptr;
		avl_node* right = nullptr;
		avl_node* parent = nullptr;

	public:
		value_type data;

	private:
		void* parent_tree = nullptr;
		int32_t height = 0;
		int8_t balance_factor = 0;

		static ARES_CONSTEXPR size_t base_size = 6 * sizeof(void*) + sizeof(int32_t) + sizeof(int8_t); // 53 bytes
		static ARES_CONSTEXPR size_t data_size = sizeof(value_type);
		static ARES_CONSTEXPR size_t total_size = base_size + data_size;
		static ARES_CONSTEXPR size_t alignment = std::max(alignof(value_type), alignof(void*));
		static ARES_CONSTEXPR size_t padding_needed = (alignment - (total_size % alignment)) % alignment;
		static ARES_CONSTEXPR bool padding_enabled = padding_need > 0;
		typename eastl::conditional_t<
			padding_enabled,
			uint8_t[padding_needed],
			struct{}
		> padding;
	};

	template <typename key, typename value>
	avl_node<key, value>::avl_node(const avl_node& other) ARES_NOEXCEPT_IF(eastl::is_nothrow_copy_constructible_v<value_type>)
		: data(other.data), left(other.left), right(other.right), parent(other.parent), height(other.height), parent_tree(other.parent_tree)
	{
	}

	template <typename key, typename value>
	avl_node<key, value>& avl_node<key, value>::operator=(const avl_node& other) ARES_NOEXCEPT_IF(eastl::is_nothrow_copy_assignable_v<value_type>)
	{
		if (this == &other) return *this;

		data = other.data;
		left = other.left;
		right = other.right;
		parent = other.parent;
		height = other.height;
		parent_tree = other.parent_tree;
		return *this;
	}

	template <typename key, typename value>
	avl_node<key, value>::avl_node(avl_node&& other) ARES_NOEXCEPT_IF(eastl::is_nothrow_move_constructible_v<value_type>)
		: data(eastl::move(other.data)), left(eastl::exchange(other.left, nullptr)), right(eastl::exchange(other.right, nullptr)),
		parent(eastl::exchange(other.parent, nullptr)), height(eastl::exchange(other.height, 0)),
		parent_tree(eastl::exchange(other.parent_tree, nullptr))
	{
	}

	template <typename key, typename value>
	avl_node<key, value>& avl_node<key, value>::operator=(avl_node&& other) ARES_NOEXCEPT_IF(eastl::is_nothrow_move_assignable_v<value_type>)
	{
		if (this == &other) return;

		data = eastl::move(other.data);
		left = eastl::exchange(other.left, nullptr);
		right = eastl::exchange(other.right, nullptr);
		parent = eastl::exchange(other.parent, nullptr);
		height = eastl::exchange(other.height, 0);
		parent_tree = eastl::exchange(other.parent_tree, nullptr);
	}

	template <typename key, typename value>
	void avl_node<key, value>::swap(avl_node& other) ARES_NOEXCEPT
	{
	#if ARES_BUILD_DEBUG
		assert(parent_tree == other.parent_tree && "avl_node: parent_tree mismatch!");
	#endif
		left = eastl::exchange(other.left, left);
		right = eastl::exchange(other.right, right);
		parent = eastl::exchange(other.parent, parent);
		height = eastl::exchange(other.height, height);
	}

	template <typename key, typename value>
	void avl_node<key, value>::reset() ARES_NOEXCEPT
	{
		left = nullptr;
		right = nullptr;
		parent = nullptr;
		height = 0;
		parent_tree = nullptr;
	}
}

#endif // ARES_CORE_INTERNAL_AVL_NODE_H