///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2025 Jack Phillips. All rights reserved.
// 
///////////////////////////////////////////////////////////////////////////////

#ifndef ARES_CORE_AVL_TREE_H
#define ARES_CORE_AVL_TREE_H

#include <EASTL/functional.h>
#include <EASTL/variant.h>
#include <EASTL/utility.h>
#include "core/internal/common.h"
#include "core/internal/avl_node.h"
#include "core/internal/avl_tree_iterator.h"
#include "core/type_traits.h"
#include "core/allocator.h"

namespace ares::core {

	class default_allocator;

	template <typename T>
	class sys_allocator;

	template <typename key, typename value = void, typename compare = eastl::less<key>, typename allocator = default_allocator>
	class avl_tree
	{
	private:
		ARES_STATIC_ASSERT(eastl::is_void_v<allocator> || is_ares_allocator_v<allocator>, "core::avl_tree: invalid allocator type");
		static ARES_CONSTEXPR bool is_allocator_void = eastl::is_void_v<allocator>;
		static ARES_CONSTEXPR bool is_value_void = eastl::is_void_v<value>;
	public:
		using node = avl_node<key, value>;

		using key_type = key;
		using key_compare = compare;
		using mapped_type = value;
		using value_type = eastl::conditional_t<is_value_void, key, eastl::pair<const key, value>>;
		using allocator_type = eastl::conditional_t<is_allocator_void, eastl::monostate, sys_allocator<allocator>>;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;

		using reference = value_type&;
		using const_reference = const value_type&;
		using pointer = value_type*;
		using const_pointer = const value_type*;

		using iterator = avl_tree_iterator<node, value_type>;
		using const_iterator = avl_tree_iterator<const node, const value_type>;
		using reverse_iterator = eastl::reverse_iterator<iterator>;
		using const_reverse_iterator = eastl::reverse_iterator<const_iterator>;

		using pair_type = eastl::pair<iterator, bool>;
		using iterator_pair_type = eastl::pair<iterator, iterator>;
		using const_iterator_pair_type = eastl::pair<const_iterator, const_iterator>;

		avl_tree() {}
		template <typename = eastl::enable_if_t<!is_allocator_void>> avl_tree(std::initializer_list<value_type> init, const key_compare& comp = key_compare(), const allocator_type& alloc = allocator_type());
		template <typename = eastl::enable_if_t<is_allocator_void>> avl_tree(std::initializer_list<node*> init, const key_compare& comp = key_compare());
		template <typename = eastl::enable_if_t<!is_allocator_void>> explicit avl_tree(const key_compare& comp, const allocator_type& alloc = allocator_type()) : compare_(comp), allocator_(alloc) {}
		template <typename = eastl::enable_if_t<is_allocator_void>> explicit avl_tree(const key_compare& comp) : compare_(comp) {}
		template <typename = eastl::enable_if_t<!is_allocator_void>> explicit avl_tree(const allocator_type& alloc) : allocator_(alloc) {}
		~avl_tree() { clear(); }

		avl_tree(const avl_tree& other);
		template <typename = eastl::enable_if_t<!is_allocator_void>> avl_tree(const avl_tree& other, const allocator_type& alloc);
		avl_tree& operator=(const avl_tree& other);

		avl_tree(avl_tree&& other) ARES_NOEXCEPT_IF(eastl::is_nothrow_move_constructible_v<key_compare> && eastl::is_nothrow_move_assignable_v<allocator_type>);
		template <typename = eastl::enable_if_t<!is_allocator_void>>
		avl_tree(avl_tree&& other, const allocator_type& alloc) ARES_NOEXCEPT_IF(eastl::is_nothrow_move_constructible_v<key_compare> && eastl::is_nothrow_copy_constructible_v<allocator_type>);
		avl_tree& operator=(avl_tree&& other) ARES_NOEXCEPT;

		inline node* root() const { return root_; }

		// STL/EASTL methods
		// Iterators
		iterator begin() ARES_NOEXCEPT { return iterator(minimum(root_)); }
		iterator end() ARES_NOEXCEPT { return iterator(nullptr); }
		const_iterator begin() const ARES_NOEXCEPT { return const_iterator(minimum(root_)); }
		const_iterator end() const ARES_NOEXCEPT { return const_iterator(nullptr); }
		const_iterator cbegin() const ARES_NOEXCEPT { return const_iterator(minimum(root_)); }
		const_iterator cend() const ARES_NOEXCEPT { return const_iterator(nullptr); }
		reverse_iterator rbegin() ARES_NOEXCEPT { return reverse_iterator(begin()); }
		reverse_iterator rend() ARES_NOEXCEPT { return reverse_iterator(end()); }
		const_reverse_iterator rbegin() const ARES_NOEXCEPT { return const_reverse_iterator(end()); }
		const_reverse_iterator rend() const ARES_NOEXCEPT { return const_reverse_iterator(begin()); }
		const_reverse_iterator crbegin() const ARES_NOEXCEPT { return const_reverse_iterator(cend()); }
		const_reverse_iterator crend() const ARES_NOEXCEPT { return const_reverse_iterator(cbegin()); }

		// Capacity
		size_type size() const ARES_NOEXCEPT { return size_; }
		ARES_CONSTEXPR size_type max_size() const ARES_NOEXCEPT { return eastl::min(eastl::numeric_limits<size_type>::max(), static_cast<size_type>(eastl::numeric_limits<key>::max()) + 1); }
		bool empty() const ARES_NOEXCEPT { return size_ == 0 || root_ == nullptr; }

		// Modifiers
		void clear() { destroy_node(root_); }
		template <typename = eastl::enable_if_t<!is_allocator_void>> pair_type insert(const value_type& key);
		template <typename = eastl::enable_if_t<!is_allocator_void>> pair_type insert(value_type&& key);
		template <typename = eastl::enable_if_t<is_allocator_void>> pair_type insert(node* node_arg);
		iterator erase(iterator pos);
		iterator erase(iterator first, iterator last);
		size_type erase(const key_type& key) { return delete_internal(root_, key) ? 1 : 0; }
		size_type erase(key_type&& key) { return delete_internal(root_, eastl::move(key)) ? 1 : 0; }
		template <typename = eastl::enable_if_t<is_allocator_void>> size_type erase(node* node_arg);

		// Lookup
		template <typename = eastl::enable_if_t<!is_value_void>> mapped_type& at(const key_type& key);
		template <typename = eastl::enable_if_t<!is_value_void>> const mapped_type& at(const key_type& key) const;
		iterator find(const key_type& key);
		const_iterator find(const key_type& key) const;
		iterator_pair_type equal_range(const key_type& key) { return { lower_bound(key), upper_bound(key) }; }
		const_iterator_pair_type equal_range(const key_type& key) const { return { lower_bound(key), upper_bound(key) }; }
		iterator lower_bound(const key_type& key) { return iterator(lower_bound_internal(key)); }
		const_iterator lower_bound(const key_type& key) const { return const_iterator(lower_bound_internal(key)); }
		iterator upper_bound(const key_type& key) { return iterator(upper_bound_internal(key)); }
		const_iterator upper_bound(const key_type& key) const { return const_iterator(upper_bound_internal(key)); }

		// Other
		template <typename = eastl::enable_if_t<!is_allocator_void>> allocator_type& get_allocator() const { return allocator_; }

	private:
		template <typename = eastl::enable_if_t<!is_allocator_void>> node* allocate_node();
		template <typename = eastl::enable_if_t<!is_allocator_void>> node* allocate_node(const value_type& key_value);
		template <typename = eastl::enable_if_t<!is_allocator_void>> node* allocate_node(value_type&& key_value);
		template <typename = eastl::enable_if_t<!is_allocator_void>> void deallocate_node(node* node_arg) ARES_NOEXCEPT;

		int32_t height(node* node_arg) const ARES_NOEXCEPT { return node_arg ? node_arg->height : -1; }
		int32_t balance_factor(node* node_arg) const ARES_NOEXCEPT { return height(node_arg->left) - height(node_arg->right); }
		void update_height(node* node_arg) ARES_NOEXCEPT { node_arg->height = 1 + eastl::max(height(node_arg->left), height(node_arg->right)); }
		node* rotate_left(node* node_arg) ARES_NOEXCEPT;
		node* rotate_right(node* node_arg) ARES_NOEXCEPT;
		node* balance(node* node_arg) ARES_NOEXCEPT;
		node* insert_internal(node*& root, node* node_arg, bool& inserted) ARES_NOEXCEPT;
		bool delete_internal(node*& root, const key_type& key);
		bool delete_node_internal(node*& root, node* node_arg) ARES_NOEXCEPT;
		void destroy_node(node* node_arg) ARES_NOEXCEPT;
		void swap_nodes(node* node_a, node* node_b) ARES_NOEXCEPT;
		node* find_node_internal(node* root, const key_type& key) const ARES_NOEXCEPT_IF(eastl::is_no_throw_invocable_v<key_compare, key_type, key_type>);
		node* lower_bound_internal(const key_type& key) const ARES_NOEXCEPT_IF(eastl::is_no_throw_invocable_v<key_compare, key_type, key_type>);
		node* upper_bound_internal(const key_type& key) const ARES_NOEXCEPT_IF(eastl::is_no_throw_invocable_v<key_compare, key_type, key_type>);
		node* maximum(node* node_arg) const ARES_NOEXCEPT;
		node* minimum(node* node_arg) const ARES_NOEXCEPT;
		node* prev_node(node* node_arg) const ARES_NOEXCEPT;
		node* next_node(node* node_arg) const ARES_NOEXCEPT;
		static const key_type& extract_key(const node* node_arg);
		/*
		/ not used for now
		/
		template <typename first = key_type, typename... rest, typename = eastl::enable_if_t<!is_allocator_void>>
		first&& extract_key(first&& first_arg, rest&&...) { return eastl::forward<first>(first); }
		template <typename first = key_type, typename... rest, typename = eastl::enable_if_t<!is_allocator_void>>
		eastl::tuple<rest...> extract_value(first&& first_arg, rest&&... rest_args) { return eastl::forward_as_tuple(eastl::forward<rest>(rest)...); }
		*/
	
	private:
		node* root_ = nullptr;
		size_type size_ = 0;
		key_compare compare_;
		allocator_type allocator_;
	};

	//
	// Constructors
	//
	template <typename key, typename value, typename compare, typename allocator>
	template <typename>
	avl_tree<key, value, compare, allocator>::avl_tree(std::initializer_list<value_type> init, const key_compare& comp, const allocator_type& alloc)
		: compare_(comp), allocator_(alloc)
	{
		auto it = init.begin();
		for (size_type i = 0; i < static_cast<size_type>(init.size()); i++, it++)
		{
			insert(*it);
		}
	}

	template <typename key, typename value, typename compare, typename allocator>
	template <typename>
	avl_tree<key, value, compare, allocator>::avl_tree(std::initializer_list<node*> init, const key_compare& comp)
		: compare_(comp)
	{
		auto it = init.begin();
		for (size_type i = 0; i < static_cast<size_type>(init.size()); i++, it++)
		{
			insert(*it);
		}
	}

	template <typename key, typename value, typename compare, typename allocator>
	avl_tree<key, value, compare, allocator>::avl_tree(const avl_tree& other)
		: compare_(other.compare_), allocator_(other.allocator_)
	{
		ARES_STATIC_ASSERT(!is_allocator_void, "core::avl_tree: copy constructor deleted for void allocator");
		for (auto it = other.begin(); it != other.end(); it++)
		{
			insert(*it);
		}
	}

	template <typename key, typename value, typename compare, typename allocator>
	template <typename>
	avl_tree<key, value, compare, allocator>::avl_tree(const avl_tree& other, const allocator_type& alloc)
		: allocator_(alloc)
	{
		ARES_STATIC_ASSERT(!is_allocator_void, "core::avl_tree: copy constructor deleted for void allocator");
		for (auto it = other.begin(); it != other.end(); it++)
		{
			insert(*it);
		}
	}

	template <typename key, typename value, typename compare, typename allocator>
	avl_tree<key, value, compare, allocator>& avl_tree<key, value, compare, allocator>::operator=(const avl_tree& other)
	{
		ARES_STATIC_ASSERT(!is_allocator_void, "core::avl_tree: copy assignment deleted for void allocator");
		if (this == &other) return *this;

		clear();
		compare_ = other.compare_;
		allocator_ = other.allocator_;
		for (auto it = other.begin(); it != other.end(); it++)
		{
			insert(*it);
		}

		return *this;
	}

	template <typename key, typename value, typename compare, typename allocator>
	avl_tree<key, value, compare, allocator>::avl_tree(avl_tree&& other) ARES_NOEXCEPT_IF(eastl::is_nothrow_move_constructible_v<key_compare> && eastl::is_nothrow_move_assignable_v<allocator_type>)
		: root_(eastl::exchange(other.root_, nullptr)), size_(eastl::exchange(other.size_, 0)), compare_(eastl::move(other.compare_)), allocator_()
	{
			if ARES_CONSTEXPR (!is_allocator_void)
			{
				allocator_ = eastl::move(other.allocator_);
			}
	}

	template <typename key, typename value, typename compare, typename allocator>
	template <typename>
	avl_tree<key, value, compare, allocator>::avl_tree(avl_tree&& other, const allocator_type& alloc) ARES_NOEXCEPT_IF(eastl::is_nothrow_move_constructible_v<key_compare> && eastl::is_nothrow_copy_constructible_v<allocator_type>)
		: root_(eastl::exchange(other.root_, nullptr)), size_(eastl::exchange(other.size_, 0)), compare_(eastl::move(other.compare_)), allocator_(alloc)
	{
	}

	template <typename key, typename value, typename compare, typename allocator>
	avl_tree<key, value, compare, allocator>& avl_tree<key, value, compare, allocator>::operator=(avl_tree&& other) ARES_NOEXCEPT
	{
		if (this != &other)
		{
			clear();

			root_ = eastl::exchange(other.root_, nullptr);
			size_ = eastl::exchange(other.size_, 0);
			compare_ = eastl::move(other.compare_);

			if ARES_CONSTEXPR (!is_allocator_void)
			{
				allocator_ = eastl::move(other.allocator_);
			}
		}
		return *this;
	}

	//
	// STL/EASTL - MODIFIERS
	//
	template <typename key, typename value, typename compare, typename allocator>
	template <typename>
	inline typename avl_tree<key, value, compare, allocator>::pair_type avl_tree<key, value, compare, allocator>::insert(const value_type& key)
	{
		node* node_insert = allocate_node(key);

		if (ARES_UNLIKELY(node_insert == nullptr))
		{
			return { end(), false };
		}

		bool inserted = false;
		node* result = insert_internal(root_, node_insert, inserted);

		if (ARES_UNLIKELY(inserted == false))
		{
			deallocate_node(node_insert);
		}

		return { iterator(result), inserted };
	}

	template <typename key, typename value, typename compare, typename allocator>
	template <typename>
	inline typename avl_tree<key, value, compare, allocator>::pair_type avl_tree<key, value, compare, allocator>::insert(value_type&& key)
	{
		node* node_insert = allocate_node(eastl::move(key));

		if (ARES_UNLIKELY(node_insert == nullptr))
		{
			return { end(), false };
		}

		bool inserted = false;
		node* result = insert_internal(root_, node_insert, inserted);

		if (ARES_UNLIKELY(inserted == false))
		{
			deallocate_node(node_insert);
		}
		
		return { iterator(result), inserted };
	}

	template <typename key, typename value, typename compare, typename allocator>
	template <typename>
	inline typename avl_tree<key, value, compare, allocator>::pair_type avl_tree<key, value, compare, allocator>::insert(node* node_arg)
	{
		ARES_DEBUG_ASSERT(ARES_LIKELY(node_arg != nullptr), "core::avl_tree: tried to insert a non-existent node");
		ARES_DEBUG_ASSERT(ARES_LIKELY(node_arg->parent_tree == nullptr || node_arg->parent_tree == static_cast<void*>(this)), "core::avl_tree: tried to insert a node that doesn't belong to this tree");

		node_arg->parent_tree = static_cast<void*>(this);
		bool inserted = false;
		node* result = insert_internal(root_, node_arg, inserted);

		return { iterator(result), inserted };
	}

	template <typename key, typename value, typename compare, typename allocator>
	inline typename avl_tree<key, value, compare, allocator>::iterator avl_tree<key, value, compare, allocator>::erase(iterator pos)
	{
		node* node_erase = pos.get_node();

		ARES_DEBUG_ASSERT(ARES_LIKELY(node_erase != nullptr), "core::avl_tree: tried to erase a non-existent node");
		ARES_DEBUG_ASSERT(ARES_LIKELY(node_erase->parent_tree != nullptr), "core::avl_tree: tried to erase a node with no parent tree");
		ARES_DEBUG_ASSERT(ARES_LIKELY(node_erase->parent_tree == static_cast<void*>(this)), "core::avl_tree: tried to erase a node that doesn't belong to this tree");

		if (pos == end())
		{
			return end();
		}
		
		pos++;
		delete_node_internal(root_, node_erase);
		return pos;
	}

	template <typename key, typename value, typename compare, typename allocator>
	inline typename avl_tree<key, value, compare, allocator>::iterator avl_tree<key, value, compare, allocator>::erase(iterator first, iterator last)
	{
		while (first != last)
		{
			first = erase(first);
		}
		return last;
	}

	template <typename key, typename value, typename compare, typename allocator>
	template <typename>
	inline typename avl_tree<key, value, compare, allocator>::size_type avl_tree<key, value, compare, allocator>::erase(node* node_arg)
	{
		ARES_DEBUG_ASSERT(ARES_LIKELY(node_arg != nullptr), "core::avl_tree: tried to erase a non-existent node");
		ARES_DEBUG_ASSERT(ARES_LIKELY(node_arg->parent_tree != nullptr), "core::avl_tree: tried to erase a node with no parent tree");
		ARES_DEBUG_ASSERT(ARES_LIKELY(node_arg->parent_tree == static_cast<void*>(this)), "core::avl_tree: tried to erase a node that doesn't belong to this tree");

		return delete_node_internal(root_, node_arg) ? 1 : 0;
	}

	//
	// STL/EASTL - LOOKUP
	//
	template <typename key, typename value, typename compare, typename allocator>
	template <typename>
	typename avl_tree<key, value, compare, allocator>::mapped_type& avl_tree<key, value, compare, allocator>::at(const key_type& key)
	{
		if (node* node_find = find_node_internal(root_, key))
		{
			return node_find->data.second;
		}
		else
		{
			throw std::out_of_range("core::avl_tree: key out of range");
		}
	}

	template <typename key, typename value, typename compare, typename allocator>
	template <typename>
	const typename avl_tree<key, value, compare, allocator>::mapped_type& avl_tree<key, value, compare, allocator>::at(const key_type& key) const
	{
		if (node* node_find = find_node_internal(root_, key))
		{
			return node_find->data.second;
		}
		else
		{
			throw std::out_of_range("core::avl_tree: key out of range");
		}
	}

	template <typename key, typename value, typename compare, typename allocator>
	inline typename avl_tree<key, value, compare, allocator>::iterator avl_tree<key, value, compare, allocator>::find(const key_type& key)
	{
		if (node* node_find = find_node_internal(root_, key))
		{
			return iterator(node_find);
		}

		return end();
	}

	template <typename key, typename value, typename compare, typename allocator>
	inline typename avl_tree<key, value, compare, allocator>::const_iterator avl_tree<key, value, compare, allocator>::find(const key_type& key) const
	{
		if (node* node_find = find_node_internal(root_, key))
		{
			return const_iterator(node_find);
		}

		return cend();
	}

	//
	// PRIVATE METHODS
	//
	template <typename key, typename value, typename compare, typename allocator>
	template <typename>
	inline typename avl_tree<key, value, compare, allocator>::node* avl_tree<key, value, compare, allocator>::allocate_node()
	{
		void* mem = allocator_.allocate(sizeof(node), alignof(node));
		if (ARES_UNLIKELY(mem == nullptr)) return nullptr;
		node* result = new (mem) node();
		result->parent_tree = static_cast<void*>(this);
		return result;
	}

	template <typename key, typename value, typename compare, typename allocator>
	template <typename>
	inline typename avl_tree<key, value, compare, allocator>::node* avl_tree<key, value, compare, allocator>::allocate_node(const value_type& key_value)
	{
		void* mem = allocator_.allocate(sizeof(node), alignof(node));
		if (ARES_UNLIKELY(mem == nullptr)) return nullptr;
		node* result = new (mem) node(key_value);
		result->parent_tree = static_cast<void*>(this);
		return result;
	}

	template <typename key, typename value, typename compare, typename allocator>
	template <typename>
	inline typename avl_tree<key, value, compare, allocator>::node* avl_tree<key, value, compare, allocator>::allocate_node(value_type&& key_value)
	{
		void* mem = allocator_.allocate(sizeof(node), alignof(node));
		if (ARES_UNLIKELY(mem == nullptr)) return nullptr;
		node* result = new (mem) node(eastl::move(key_value));
		result->parent_tree = static_cast<void*>(this);
		return result;
	}

	template <typename key, typename value, typename compare, typename allocator>
	template <typename>
	inline void avl_tree<key, value, compare, allocator>::deallocate_node(node* node_arg) ARES_NOEXCEPT
	{
		if (ARES_LIKELY(node_arg != nullptr))
		{
			node_arg->~node();
			allocator_.deallocate(node_arg, sizeof(node));
		}
	}

	template <typename key, typename value, typename compare, typename allocator>
	inline typename avl_tree<key, value, compare, allocator>::node* avl_tree<key, value, compare, allocator>::rotate_left(node* node_arg) ARES_NOEXCEPT
	{
		node* r = node_arg->right;
		node_arg->right = r->left;
		if (r->left)
		{
			r->left->parent = node_arg;
		}

		r->left = node_arg;
		r->parent = node_arg->parent;
		node_arg->parent = r;

		update_height(node_arg);
		update_height(r);

		return r;
	}

	template <typename key, typename value, typename compare, typename allocator>
	inline typename avl_tree<key, value, compare, allocator>::node* avl_tree<key, value, compare, allocator>::rotate_right(node* node_arg) ARES_NOEXCEPT
	{
		node* l = node_arg->left;
		node_arg->left = l->right;
		if (l->right)
		{
			l->right->parent = node_arg;
		}

		l->right = node_arg;
		l->parent = node_arg->parent;

		node_arg->parent = l;

		update_height(node_arg);
		update_height(l);

		return l;
	}

	template <typename key, typename value, typename compare, typename allocator>
	inline typename avl_tree<key, value, compare, allocator>::node* avl_tree<key, value, compare, allocator>::balance(node* node_arg) ARES_NOEXCEPT
	{
		if (ARES_UNLIKELY(node_arg == nullptr)) return nullptr;

		update_height(node_arg);
		int32_t b_factor = balance_factor(node_arg);

		if (ARES_LIKELY(b_factor >= -1 && b_factor <= 1))
		{
			return node_arg;
		}

		node* new_root = node_arg;

		if (ARES_UNLIKELY(b_factor > 1))
		{
			if (balance_factor(node_arg->left) < 0)
			{
				node_arg->left = rotate_left(node_arg->left);
				if (node_arg->left)
				{
					node_arg->left->parent = node_arg;
				}
			}
			new_root = rotate_right(node_arg);
		}
		else if (ARES_UNLIKELY(b_factor < -1))
		{
			if (balance_factor(node_arg->right) > 0)
			{
				node_arg->right = rotate_right(node_arg->right);
				if (node_arg->right)
				{
					node_arg->right->parent = node_arg;
				}
			}
			new_root = rotate_left(node_arg);
		}

		return new_root;
	}

	template <typename key, typename value, typename compare, typename allocator>
	inline typename avl_tree<key, value, compare, allocator>::node* avl_tree<key, value, compare, allocator>::insert_internal(node*& root, node* node_arg, bool& inserted) ARES_NOEXCEPT
	{
		if (!root)
		{
			root = node_arg;
			node_arg->parent = nullptr;
			inserted = true;
			size_++;
			return node_arg;
		}

		const key_type& node_key = extract_key(node_arg);
		node* current = root;

		while (true)
		{
			const key_type& current_key = extract_key(current);

			if (compare_(node_key, current_key))
			{
				if (!current->left)
				{
					current->left = node_arg;
					node_arg->parent = current;
					inserted = true;
					size_++;
					break;
				}
				current = current->left;
			}
			else if (compare_(current_key, node_key))
			{
				if (!current->right)
				{
					current->right = node_arg;
					node_arg->parent = current;
					inserted = true;
					size_++;
					break;
				}
				current = current->right;
			}
			else
			{
				inserted = false;
				return current;
			}

			for (current = node_arg->parent; current; current = current->parent)
			{
				node* balanced = balance(current);
				if (current == root)
				{
					root = balanced;
				}
				else if (balanced->parent)
				{
					if (balanced->parent->left == current)
					{
						balanced->parent->left = balanced;
					}
					else
					{
						balanced->parent->right = balanced;
					}
				}
			}
			return node_arg;
		}

		/*
		node* current = root;
		node* parent = nullptr;

		const key_type& node_key = extract_key(node_arg);

		while (current)
		{
			parent = current;
			const key_type& current_key = [current]() -> const key_type&
			{
				if ARES_CONSTEXPR (is_value_void)
				{
					return current->data;
				}
				else
				{
					return current->data.first;
				}
			}();

			if (compare_(node_key, current_key))
			{
				if (!current->left)
				{
					current->left = node_arg;
					node_arg->parent = current;
					inserted = true;
					size_++;
					break;
				}
				current = current->left;
			}
			else if (compare_(current_key, node_key))
			{
				if (!current->right)
				{
					current->right = node_arg;
					node_arg->parent = current;
					inserted = true;
					size_++;
					break;
				}
				current = current->right;
			}
			else
			{
				inserted = false;
				return current;
			}
		}

		node* child = node_arg;
		parent = node_arg->parent;

		while (parent)
		{
			node* balanced_root = balance(parent);

			if (parent == root)
			{
				root = balanced_root;
			}

			if (balanced_root->parent)
			{
				node* grandparent = balanced_root->parent;
				if (grandparent->left == parent)
				{
					grandparent->left = balanced_root;
				}
				else
				{
					grandparent->right = balanced_root;
				}
			}
			else
			{
				// nothing to do for now - root is already updated
			}

			child = balanced_root;
			parent = balanced_root->parent;
		}

		return node_arg;
		*/
	}

	template <typename key, typename value, typename compare, typename allocator>
	inline bool avl_tree<key, value, compare, allocator>::delete_internal(node*& root, const key_type& key)
	{
		if (node* to_delete = find_node_internal(root, key))
		{
			return delete_node_internal(root, to_delete);
		}
		else
		{
			return false;
		}
	}

	template <typename key, typename value, typename compare, typename allocator>
	inline bool avl_tree<key, value, compare, allocator>::delete_node_internal(node*& root, node* node_arg) ARES_NOEXCEPT
	{
		/*
		if (!node_arg) return false;

		node* to_delete = node_arg;

		if (node_arg->left && node_arg->right)
		{
			node* successor = minimum(node_arg->right);

			to_delete = successor;

			node* child = to_delete->right;
			if (child)
			{
				child->parent = to_delete->parent;
			}

			if (to_delete->parent->left == to_delete)
			{
				to_delete->parent->left = child;
			}
			else
			{
				to_delete->parent->right = child;
			}

			if (node_arg->parent)
			{
				if (node_arg->parent->left == node_arg)
				{
					node_arg->parent->left = to_delete;
				}
				else
				{
					node_arg->parent->right = to_delete;
				}
			}
			else
			{
				root = to_delete;
			}

			to_delete->parent = node_arg->parent;
			to_delete->left = node_arg->left;
			if (to_delete->left)
			{
				to_delete->left->parent = to_delete;
			}

			to_delete->right = node_arg->right;
			if (to_delete->right)
			{
				to_delete->right->parent = to_delete;
			}

			to_delete->height = node_arg->height;
		}
		else
		{
			node* child = node_arg->left ? node_arg->left : node_arg->right;
			if (child)
			{
				child->parent = node_arg->parent;
			}

			if (node_arg->parent)
			{
				if (node_arg->parent->left == node_arg)
				{
					node_arg->parent->left = child;
				}
				else
				{
					node_arg->parent->right = child;
				}
			}
			else
			{
				root = child;
			}
		}

		node* current = to_delete->parent;
		while (current)
		{
			node* balanced_root = balance(current);
			if (!balanced_root->parent)
			{
				root = balanced_root;
			}
			current = balanced_root->parent;
		}

		if ARES_CONSTEXPR (!is_allocator_void)
		{
			deallocate_node(node_arg);
		}
		else
		{
			node_arg->left = nullptr;
			node_arg->right = nullptr;
			node_arg->parent = nullptr;
			node_arg->height = 0;
			node_arg->parent_tree = nullptr;
		}
		
		size_--;
		return true;
		*/

		if (!node_arg) return false;
		
		node* to_delete = node_arg;

		auto replace_node_in_parent = [](node* n, node* new_child, node*& root)
		{
			if (n->parent)
			{
				if (n->parent->left == n) n->parent->left = new_child;
				else n->parent->right = new_child;
			}
			else
			{
				root = new_child;
			}
			if (new_child) new_child->parent = n->parent;
		};

		if (node_arg->left && node_arg->right)
		{
			node* successor = minimum(node_arg->right);
			to_delete = successor;
			node* child = successor->right;

			replace_node_in_parent(successor, child, root);

			successor->swap(*node_arg);

			if (successor->left) successor->left->parent = successor;
			if (successor->right) successor->right->parent = successor;

			if (node_arg->parent)
			{
				if (node_arg->parent->left == node_arg) node_arg->parent->left = successor;
				else node_arg->parent->right = successor;
			}
			else
			{
				root = successor;
			}
		}
		else
		{
			node* child = node_arg->left ? node_arg->left : node_arg->right;
			replace_node_in_parent(node_arg, child, root);
		}

		for (node* current = to_delete->parent; current; current = current->parent)
		{
			node* balanced_root = balance(current);
			if (!balanced_root->parent) root = balanced_root;
		}

		if ARES_CONSTEXPR (!is_allocator_void)
		{
			deallocate_node(node_arg);
		}
		else
		{
			node_arg->reset();
		}

		size_--;
		return true;
	}

	template <typename key, typename value, typename compare, typename allocator>
	inline void avl_tree<key, value, compare, allocator>::destroy_node(node* node_arg) ARES_NOEXCEPT
	{
		if (!node_arg) return;

		node* parent = node_arg->parent;
		node* current = node_arg;
		while (current)
		{
			if (current->left)
			{
				current = current->left;
			}
			else if (current->right)
			{
				current = current->right;
			}
			else
			{
				node* current_parent = current->parent;
				node* to_delete = current;

				current = current_parent;
				to_delete->left = nullptr;
				to_delete->right = nullptr;
				to_delete->parent = nullptr;
				to_delete->height = 0;

				if ARES_CONSTEXPR (!is_allocator_void)
				{
					deallocate_node(to_delete);
				}

				size_--;

				if (current)
				{
					if (current->left == to_delete)
					{
						current->left = nullptr;
					}
					else if (current->right == to_delete)
					{
						current->right = nullptr;
					}
				}
				else
				{
					root_ = nullptr;
				}
			}
		}

		while (parent)
		{
			balance(parent);

			if (!parent->parent)
			{
				root_ = parent;
			}

			parent = parent->parent;
		}
	}

	template <typename key, typename value, typename compare, typename allocator>
	void avl_tree<key, value, compare, allocator>::swap_nodes(node* node_a, node* node_b) ARES_NOEXCEPT
	{
		if (node_a == node_b || !node_a || !node_b) return;

		bool a_is_parent_of_b = (node_b->parent == node_a);
		bool b_is_parent_of_a = (node_a->parent == node_b);

		node* a_parent = node_a->parent;
		node* a_left = node_a->left;
		node* a_right = node_a->right;

		node* b_parent = node_b->parent;
		node* b_left = node_b->left;
		node* b_right = node_b->right;

		int32_t a_height = node_a->height;
		int32_t b_height = node_b->height;

		if (!b_is_parent_of_a)
		{
			node_a->parent = b_parent == node_a ? node_b : b_parent;
		}
		else
		{
			node_a->parent = node_b;
		}

		if (!a_is_parent_of_b)
		{
			node_b->parent = a_parent == node_b ? node_a : a_parent;
		}
		else
		{
			node_b->parent = node_a;
		}

		node_a->left = (b_left == node_a) ? node_b : b_left;
		node_b->left = (a_left == node_b) ? node_a : a_left;

		node_a->right = (b_right == node_a) ? node_b : b_right;
		node_b->right = (a_right == node_b) ? node_a : a_right;

		if (node_a->left) node_a->left->parent = node_a;
		if (node_a->right) node_a->right->parent = node_a;
		if (node_b->left) node_b->left->parent = node_b;
		if (node_b->right) node_b->right->parent = node_b;

		if (node_a->parent)
		{
			if (node_a->parent->left == node_b)
			{
				node_a->parent->left = node_a;
			}
			else if (node_a->parent->right == node_b)
			{
				node_a->parent->right = node_a;
			}
			else if (node_a->parent->left == node_a || node_a->parent->right == node_a)
			{

			}
		}

		if (node_b->parent)
		{
			if (node_b->parent->left == node_a)
			{
				node_b->parent->left = node_b;
			}
			else if (node_b->parent->right == node_a)
			{
				node_b->parent->right = node_b;
			}
			else if (node_b->parent->left == node_a || node_b->parent->right == node_a)
			{
			}
		}

		node_a->height = b_height;
		node_b->height = a_height;
	}

	template <typename key, typename value, typename compare, typename allocator>
	typename avl_tree<key, value, compare, allocator>::node* avl_tree<key, value, compare, allocator>::find_node_internal(node* root, const key_type& key) const ARES_NOEXCEPT_IF(eastl::is_no_throw_invocable_v<key_compare, key_type, key_type>)
	{
		while (ARES_LIKELY(root != nullptr))
		{
			const key_type& current_key = extract_key(root);

			if (compare_(key, current_key))
			{
				root = root->left;
			}
			else if (compare_(current_key, key))
			{
				root = root->right;
			}
			else
			{
				return root;
			}
		}
		return nullptr;
	}

	template <typename key, typename value, typename compare, typename allocator>
	typename avl_tree<key, value, compare, allocator>::node* avl_tree<key, value, compare, allocator>::lower_bound_internal(const key_type& key) const ARES_NOEXCEPT_IF(eastl::is_no_throw_invocable_v<key_compare, key_type, key_type>)
	{
		node* current = root_;
		node* result = nullptr;

		while (current)
		{
			const key_type& current_key = extract_key(current);

			if (key == current_key)
			{
				return current;
			}

			if (compare_(key, current_key))
			{
				result = current;
				current = current->left;
			}
			else
			{
				current = current->right;
			}
		}
		return result;
	}

	template <typename key, typename value, typename compare, typename allocator>
	typename avl_tree<key, value, compare, allocator>::node* avl_tree<key, value, compare, allocator>::upper_bound_internal(const key_type& key) const ARES_NOEXCEPT_IF(eastl::is_no_throw_invocable_v<key_compare, key_type, key_type>)
	{
		node* current = root_;
		node* result = nullptr;

		while (current)
		{
			const key_type& current_key = extract_key(current);

			if (compare_(key, current_key))
			{
				result = current;
				current = current->left;
			}
			else
			{
				current = current->right;
			}
		}
		return result;
	}

	template <typename key, typename value, typename compare, typename allocator>
	typename avl_tree<key, value, compare, allocator>::node* avl_tree<key, value, compare, allocator>::maximum(node* node_arg) const ARES_NOEXCEPT
	{
		if (!node_arg) return nullptr;
		while (node_arg->right) node_arg = node_arg->right;
		return node_arg;
	}

	template <typename key, typename value, typename compare, typename allocator>
	typename avl_tree<key, value, compare, allocator>::node* avl_tree<key, value, compare, allocator>::minimum(node* node_arg) const ARES_NOEXCEPT
	{
		if (!node_arg) return nullptr;
		while (node_arg->left) node_arg = node_arg->left;
		return node_arg;
	}

	template <typename key, typename value, typename compare, typename allocator>
	typename avl_tree<key, value, compare, allocator>::node* avl_tree<key, value, compare, allocator>::prev_node(node* node_arg) const ARES_NOEXCEPT
	{
		if (ARES_UNLIKELY(node_arg == nullptr)) return nullptr;

		if(ARES_LIKELY(node_arg->left != nullptr)) return maximum(node_arg->left);

		node* parent = node_arg->parent;
		//while (parent && node_arg == parent->left)
		while (ARES_UNLIKELY(parent != nullptr && node_arg == parent->left))
		{
			node_arg = parent;
			parent = parent->parent;
		}

		return parent;
	}

	template <typename key, typename value, typename compare, typename allocator>
	typename avl_tree<key, value, compare, allocator>::node* avl_tree<key, value, compare, allocator>::next_node(node* node_arg) const ARES_NOEXCEPT
	{
		if (ARES_UNLIKELY(node_arg == nullptr)) return nullptr;

		if (ARES_LIKELY(node_arg != nullptr)) return minimum(node_arg->right);

		node* parent = node_arg->parent;
		//while (parent && node_arg == parent->right)
		while (ARES_UNLIKELY(parent != nullptr && node_arg == parent->right))
		{
			node_arg = parent;
			parent = parent->parent;
		}

		return parent;
	}

	template <typename key, typename value, typename ccompare, typename allocator>
	typename avl_tree<key, value, compare, allocator>::key_type avl_tree<key, value, compare, allocator>::extract_key(node* node_arg)
	{
		ARES_DEBUG_ASSERT(ARES_LIKELY(node_arg != nullptr), "core::avl_tree: tried to extract key from nullptr node");
		if ARES_CONSTEXPR (is_value_void)
		{
			return node_arg->data;
		}
		else
		{
			return node_arg->data.first;
		}
	}

}

#endif // ARES_CORE_AVL_TREE_H