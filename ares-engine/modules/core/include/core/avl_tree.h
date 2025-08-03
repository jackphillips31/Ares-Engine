#ifndef ARES_CORE_AVL_TREE_H
#define ARES_CORE_AVL_TREE_H
#include <assert.h>
#include <EASTL/functional.h>
#include <EASTL/variant.h>
#include <EASTL/utility.h>
#include "core/avl_node.h"
#include "core/avl_tree_iterator.h"
#include "core/type_traits.h"

namespace ares::core {

	template <typename T>
	class system_allocator;

	template <typename key, typename value = void, typename allocator = void>
	class avl_tree
	{
	private:
		static_assert(eastl::is_void_v<allocator> || is_ares_allocator_v<allocator>, "Invalid allocator type!");
		static constexpr bool is_allocator_void = eastl::is_void_v<allocator>;
	public:
		using node = avl_node<key, value>;

		using key_type = key;
		using key_compare = eastl::less<key>;
		using mapped_type = value;
		using value_type = eastl::conditional_t<eastl::is_void_v<value>, key, eastl::pair<const key, value>>;
		// Allocator system not implemented yet
		// temporary void for all avl_tree's
		//using allocator_type = eastl::conditional_t<eastl::is_void_v<allocator>, eastl::monostate, system_allocator<allocator>>;
		using allocator_type = eastl::conditional_t<eastl::is_void_v<allocator>, eastl::monostate, eastl::monostate>;
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
		~avl_tree() { clear(); }
		avl_tree(const avl_tree&) = delete;
		avl_tree& operator=(const avl_tree&) = delete;

		avl_tree(avl_tree&& other) noexcept;
		avl_tree& operator=(avl_tree&& other) noexcept;

		inline node* root() const { return m_root; }

		// STL/EASTL methods
		// Iterators
		iterator begin() noexcept { return iterator(minimum(m_root)); }
		iterator end() noexcept { return iterator(nullptr); }
		const_iterator begin() const noexcept { return const_iterator(minimum(m_root)); }
		const_iterator end() const noexcept { return const_iterator(nullptr); }
		const_iterator cbegin() const noexcept { return const_iterator(minimum(m_root)); }
		const_iterator cend() const noexcept { return const_iterator(nullptr); }
		reverse_iterator rbegin() noexcept { return reverse_iterator(begin()); }
		reverse_iterator rend() noexcept { return reverse_iterator(end()); }
		const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
		const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }
		const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(cend()); }
		const_reverse_iterator crend() const noexcept { return const_reverse_iterator(cbegin()); }

		// Capacity
		size_type size() const noexcept { return m_size; }
		bool empty() const noexcept { return m_size == 0 || m_root == nullptr; }

		// Modifiers
		void clear() { destroy_node(m_root); }
		template <typename = eastl::enable_if_t<!is_allocator_void>> pair_type insert(const value_type& key);
		template <typename = eastl::enable_if_t<!is_allocator_void>> pair_type insert(value_type&& key);
		template <typename = eastl::enable_if_t<is_allocator_void>> pair_type insert(node* node_arg);
		iterator erase(iterator pos);
		iterator erase(iterator first, iterator last);
		size_type erase(const key_type& key) { return delete_internal(m_root, key) ? 1 : 0; }
		size_type erase(key_type&& key) { return delete_internal(m_root, eastl::move(key)) ? 1 : 0; }
		template <typename = eastl::enable_if_t<is_allocator_void>> size_type erase(node* node_arg);

		// Lookup
		iterator find(const key_type& key);
		const_iterator find(const key_type& key) const;
		iterator_pair_type equal_range(const key_type& key) { return { lower_bound(key), upper_bound(key) }; }
		const_iterator_pair_type equal_range(const key_type& key) const { return { lower_bound(key), upper_bound(key) }; }
		iterator lower_bound(const key_type& key) { return iterator(lower_bound_internal(key)); }
		const_iterator lower_bound(const key_type& key) const { return const_iterator(lower_bound_internal(key)); }
		iterator upper_bound(const key_type& key) { return iterator(upper_bound_internal(key)); }
		const_iterator upper_bound(const key_type& key) const { return const_iterator(upper_bound_internal(key)); }

	private:
		template <typename = eastl::enable_if_t<!is_allocator_void>> node* allocate_node();
		template <typename = eastl::enable_if_t<!is_allocator_void>> node* allocate_node(const value_type& key_value);
		template <typename = eastl::enable_if_t<!is_allocator_void>> node* allocate_node(value_type&& key_value);
		template <typename = eastl::enable_if_t<!is_allocator_void>> void deallocate_node(node* node_arg);

		int32_t height(node* node_arg) const noexcept { return node_arg ? node_arg->height : -1; }
		int32_t balance_factor(node* node_arg) const noexcept { return height(node_arg->left) - height(node_arg->right); }
		void update_height(node* node_arg) noexcept { node_arg->height = 1 + eastl::max(height(node_arg->left), height(node_arg->right)); }
		node* rotate_left(node* node_arg) noexcept;
		node* rotate_right(node* node_arg) noexcept;
		node* balance(node* node_arg) noexcept;
		node* insert_internal(node*& root, node* node_arg, bool& inserted) noexcept;
		bool delete_internal(node*& root, const key_type& key);
		bool delete_node_internal(node*& root, node* node_arg);
		void destroy_node(node* node_arg);
		void swap_nodes(node* node_a, node* node_b);
		node* find_node_internal(node* root, const key_type& key) const;
		node* lower_bound_internal(const key_type& key) const;
		node* upper_bound_internal(const key_type& key) const;
		node* maximum(node* node_arg) const;
		node* minimum(node* node_arg) const;
		node* prev_node(node* node_arg) const;
		node* next_node(node* node_arg) const;

		/*
		/ not used for now
		/
		template <typename first = key_type, typename... rest, typename = eastl::enable_if_t<!is_allocator_void>>
		first&& extract_key(first&& first_arg, rest&&...) { return eastl::forward<first>(first); }
		template <typename first = key_type, typename... rest, typename = eastl::enable_if_t<!is_allocator_void>>
		eastl::tuple<rest...> extract_value(first&& first_arg, rest&&... rest_args) { return eastl::forward_as_tuple(eastl::forward<rest>(rest)...); }
		*/
	
	private:
		node* m_root = nullptr;
		size_type m_size = 0;
		key_compare m_compare;
		allocator_type m_allocator;
	};

	//
	// Constructors
	//
	template <typename key, typename value, typename allocator>
	inline avl_tree<key, value, allocator>::avl_tree(avl_tree<key, value, allocator>&& other) noexcept
		: m_root(eastl::exchange(other.m_root, nullptr))
		m_size(eastl::exchange(other.m_size, 0))
		m_compare(eastl::move(other.m_compare)),
		m_allocator()
	{
			if constexpr (!is_allocator_void)
			{
				m_allocator = other.m_allocator;
			}
	}

	template <typename key, typename value, typename allocator>
	inline avl_tree<key, value, allocator>& avl_tree<key, value, allocator>::operator=(avl_tree<key, value, allocator>&& other) noexcept
	{
		if (this != &other)
		{
			clear();

			m_root = eastl::exchange(other.m_root, nullptr);
			m_size = eastl::exchange(other.m_size, 0);
			m_compare = eastl::move(other.m_compare);

			if constexpr (!is_allocator_void)
			{
				m_allocator = other.m_allocator;
			}
		}
		return *this;
	}

	//
	// STL/EASTL - MODIFIERS
	template <typename key, typename value, typename allocator>
	template <typename>
	inline typename avl_tree<key, value, allocator>::pair_type avl_tree<key, value, allocator>::insert(const value_type& key)
	{
		node* node_insert = allocate_node(key);

		if (!node_insert)
		{
			return { end(), false };
		}

		bool inserted = false;
		node* result = insert_internal(m_root, node_insert, inserted);

		if (!inserted)
		{
			deallocate_node(node_insert);
		}

		return { iterator(result), inserted };
	}

	template <typename key, typename value, typename allocator>
	template <typename>
	inline typename avl_tree<key, value, allocator>::pair_type avl_tree<key, value, allocator>::insert(value_type&& key)
	{
		node* node_insert = allocate_node(eastl::move(key));

		if (!node_insert)
		{
			return { end(), false };
		}

		bool inserted = false;
		node* result = insert_internal(m_root, node_insert, inserted);

		if (!inserted)
		{
			deallocate_node(node_insert);
		}
		
		return { iterator(result), inserted };
	}

	template <typename key, typename value, typename allocator>
	template <typename>
	inline typename avl_tree<key, value, allocator>::pair_type avl_tree<key, value, allocator>::insert(node* node_arg)
	{
		if (!node_arg)
		{
			//return { end(), false };
			throw std::invalid_argument("Tried to insert a non-existent node.");
		}

		if (node_arg->parent_tree && node_arg->parent_tree != static_cast<void*>(this))
		{
			throw std::invalid_argument("Tried to insert a node that doesn't belong to this tree.");
		}

		node_arg->parent_tree = static_cast<void*>(this);
		bool inserted = false;
		node* result = insert_internal(m_root, node_arg, inserted);

		return { iterator(result), inserted };
	}

	template <typename key, typename value, typename allocator>
	inline typename avl_tree<key, value, allocator>::iterator avl_tree<key, value, allocator>::erase(iterator pos)
	{
		node* node_erase = pos.get_node();
		if (node_erase && (!node_erase->parent_tree || node_erase->parent_tree != static_cast<void*>(this)))
		{
			throw std::invalid_argument("Tried to erase an iterator that doesn't belong to this tree.");
		}

		if (pos == end())
		{
			return end();
		}
		
		pos++;
		delete_node_internal(m_root, node_erase);
		return pos;
	}

	template <typename key, typename value, typename allocator>
	inline typename avl_tree<key, value, allocator>::iterator avl_tree<key, value, allocator>::erase(iterator first, iterator last)
	{
		while (first != last)
		{
			first = erase(first);
		}
		return last;
	}

	template <typename key, typename value, typename allocator>
	template <typename>
	inline typename avl_tree<key, value, allocator>::size_type avl_tree<key, value, allocator>::erase(node* node_arg)
	{
		if (node_arg && (!node_arg->parent_tree || node_arg->parent_tree != static_cast<void*>(this)))
		{
			throw std::invalid_argument("Tried to erase a node that doesn't belong to this tree.");
		}

		return delete_node_internal(m_root, node_arg) ? 1 : 0;
	}

	//
	// STL/EASTL - LOOKUP
	//
	/*
	template <typename key, typename value, typename allocator>
	inline typename avl_tree<key, value, allocator>::iterator avl_tree<key, value, allocator>::find(const key_type& key) const
	{
		if (node* node_find = find_node_internal(m_root, key))
		{
			return iterator(node_find);
		}

		return end();
	}

	//const_iterator find(const key_type& key) const;
	template <typename key, typename value, typename allocator>
	inline typename avl_tree<key, value, allocator>::const_iterator avl_tree<key, value, allocator>::find(const key_type& key) const
	{

	}

	template <typename key, typename value, typename allocator>
	inline typename avl_tree<key, value, allocator>::const_iterator avl_tree<key, value, allocator>::find(const key_type& key) const
	{
		if (node* node_find = find_node_internal(m_root, key))
		{
			return const_iterator(node_find);
		}

		return cend();
	}
	*/

	//
	// PRIVATE METHODS
	//
	template <typename key, typename value, typename allocator>
	template <typename>
	inline typename avl_tree<key, value, allocator>::node* avl_tree<key, value, allocator>::allocate_node()
	{
		void* mem = m_allocator.allocate(sizeof(node), alignof(node));
		if (!mem) return nullptr;
		node* result = new (mem) node();
		result->parent_tree = static_cast<void*>(this);
		return result;
	}

	template <typename key, typename value, typename allocator>
	template <typename>
	inline typename avl_tree<key, value, allocator>::node* avl_tree<key, value, allocator>::allocate_node(const value_type& key_value)
	{
		void* mem = m_allocator.allocate(sizeof(node), alignof(node));
		if (!mem) return nullptr;
		node* result = new (mem) node(key_value);
		result->parent_tree = static_cast<void*>(this);
		return result;
	}

	template <typename key, typename value, typename allocator>
	template <typename>
	inline typename avl_tree<key, value, allocator>::node* avl_tree<key, value, allocator>::allocate_node(value_type&& key_value)
	{
		void* mem = m_allocator.allocate(sizeof(node), alignof(node));
		if (!mem) return nullptr;
		node* result = new (mem) node(eastl::move(key_value));
		result->parent_tree = static_cast<void*>(this);
		return result;
	}

	template <typename key, typename value, typename allocator>
	template <typename>
	inline void avl_tree<key, value, allocator>::deallocate_node(node* node_arg)
	{
		if (node_arg)
		{
			node_arg->~node();
			m_allocator.deallocate(node_arg, sizeof(node));
		}
	}

	template <typename key, typename value, typename allocator>
	inline typename avl_tree<key, value, allocator>::node* avl_tree<key, value, allocator>::rotate_left(node* node_arg) noexcept
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

	template <typename key, typename value, typename allocator>
	inline typename avl_tree<key, value, allocator>::node* avl_tree<key, value, allocator>::rotate_right(node* node_arg) noexcept
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

	template <typename key, typename value, typename allocator>
	inline typename avl_tree<key, value, allocator>::node* avl_tree<key, value, allocator>::balance(node* node_arg) noexcept
	{
		if (!node_arg) return nullptr;

		update_height(node_arg);
		int32_t balance = balance_factor(node_arg);

		node* new_root = node_arg;

		if (balance > 1)
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
		else if (balance < -1)
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

	template <typename key, typename value, typename allocator>
	inline typename avl_tree<key, value, allocator>::node* avl_tree<key, value, allocator>::insert_internal(node*& root, node* node_arg, bool& inserted) noexcept
	{
		if (!root)
		{
			root = node_arg;
			node_arg->parent = nullptr;
			inserted = true;
			m_size++;
			return node_arg;
		}

		node* current = root;
		node* parent = nullptr;

		const key_type& node_key = [node_arg]() -> const key_type&
		{
			if constexpr (eastl::is_void_v<value>)
			{
				return node_arg->data;
			}
			else
			{
				return node_arg->data.first;
			}
		}();

		while (current)
		{
			parent = current;
			const key_type& current_key = [current]() -> const key_type&
			{
				if constexpr (eastl::is_void_v<value>)
				{
					return current->data;
				}
				else
				{
					return current->data.first;
				}
			}();

			if (m_compare(node_key, current_key))
			{
				if (!current->left)
				{
					current->left = node_arg;
					node_arg->parent = current;
					inserted = true;
					m_size++;
					break;
				}
				current = current->left;
			}
			else if (m_compare(current_key, node_key))
			{
				if (!current->right)
				{
					current->right = node_arg;
					node_arg->parent = current;
					inserted = true;
					m_size++;
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
	}

	template <typename key, typename value, typename allocator>
	inline bool avl_tree<key, value, allocator>::delete_internal(node*& root, const key_type& key)
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

	template <typename key, typename value, typename allocator>
	inline bool avl_tree<key, value, allocator>::delete_node_internal(node*& root, node* node_arg)
	{
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

		if constexpr (!is_allocator_void)
		{
			deallocate_node(node_arg);
		}

		node_arg->left = nullptr;
		node_arg->right = nullptr;
		node_arg->parent = nullptr;
		node_arg->height = 0;
		node_arg->parent_tree = nullptr;
		m_size--;
		return true;
	}

	template <typename key, typename value, typename allocator>
	inline void avl_tree<key, value, allocator>::destroy_node(node* node_arg)
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

				if constexpr (!is_allocator_void)
				{
					deallocate_node(to_delete);
				}

				m_size--;

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
					m_root = nullptr;
				}
			}
		}

		while (parent)
		{
			balance(parent);

			if (!parent->parent)
			{
				m_root = parent;
			}

			parent = parent->parent;
		}
	}

	template <typename key, typename value, typename allocator>
	inline void avl_tree<key, value, allocator>::swap_nodes(node* node_a, node* node_b)
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

	template <typename key, typename value, typename allocator>
	inline typename avl_tree<key, value, allocator>::node* avl_tree<key, value, allocator>::find_node_internal(node* root, const key_type& key) const
	{
		while (root)
		{
			const key_type& current_key = []() -> const key_type&
			{
				if constexpr (eastl::is_void_v<value>)
				{
					return root->data;
				}
				else
				{
					return root->data.first;
				}
			}();

			if (key == current_key) return root;

			if (m_compare(key, current_key))
			{
				root = root->left;
			}
			else
			{
				root = root->right;
			}
		}
		return nullptr;
	}

	template <typename key, typename value, typename allocator>
	inline typename avl_tree<key, value, allocator>::node* avl_tree<key, value, allocator>::lower_bound_internal(const key_type& key) const
	{
		node* current = m_root;
		node* result = nullptr;

		while (current)
		{
			const key_type& current_key = []() -> const key_type&
			{
				if constexpr (eastl::is_void_v<value>)
				{
					return current->data;
				}
				else
				{
					return current->data.first;
				}
			}();

			if (key == current_key)
			{
				return current;
			}

			if (m_compare(key, current_key))
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

	template <typename key, typename value, typename allocator>
	inline typename avl_tree<key, value, allocator>::node* avl_tree<key, value, allocator>::upper_bound_internal(const key_type& key) const
	{
		node* current = m_root;
		node* result = nullptr;

		while (current)
		{
			const key_type& current_key = []() -> const key_type&
			{
				if constexpr (eastl::is_void_v<value>)
				{
					return current->data;
				}
				else
				{
					return current->data.first;
				}
			}();

			if (m_compare(key, current_key))
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

	template <typename key, typename value, typename allocator>
	inline typename avl_tree<key, value, allocator>::node* avl_tree<key, value, allocator>::maximum(node* node_arg) const
	{
		if (!node_arg) return nulltpr;
		while (node_arg->right) node_arg = node_arg->right;
		return node_arg;
	}

	template <typename key, typename value, typename allocator>
	inline typename avl_tree<key, value, allocator>::node* avl_tree<key, value, allocator>::minimum(node* node_arg) const
	{
		if (!node_arg) return nullptr;
		while (node_arg->left) node_arg = node_arg->left;
		return node_arg;
	}

	template <typename key, typename value, typename allocator>
	inline typename avl_tree<key, value, allocator>::node* avl_tree<key, value, allocator>::prev_node(node* node_arg) const
	{
		if (!node_arg) return nullptr;

		if(node_arg->left) return maximum(node_arg->left);

		node* parent = node_arg->parent;
		while (parent && node_arg == parent->left)
		{
			node_arg = parent;
			parent = parent->parent;
		}

		return parent;
	}

	template <typename key, typename value, typename allocator>
	inline typename avl_tree<key, value, allocator>::node* avl_tree<key, value, allocator>::next_node(node* node_arg) const
	{
		if (!node_arg) return nullptr;

		if (node_arg->right) return minimum(node_arg->right);

		node* parent = node_arg->parent;
		while (parent && node_arg == parent->right)
		{
			node_arg = parent;
			parent = parent->parent
		}

		return parent;
	}

}

#endif // ARES_CORE_AVL_TREE_H