#ifndef ARES_CORE_AVL_TREE_ITERATOR_H
#define ARES_CORE_AVL_TREE_ITERATOR_H
#include <EASTL/iterator.h>
#include <core/avl_node.h>

namespace ares::core {

	template <typename avl_node, typename value>
	class avl_tree_iterator
	{
	private:
		template <typename T>
		struct avl_tree_iterator_traits_helper
		{
			using pointer = T*;
			using reference = T&;
		};

		template <>
		struct avl_tree_iterator_traits_helper<void>
		{
			using pointer = void;
			using reference = void;
		};

	public:
		using value_type = value;
		using pointer = typename avl_tree_iterator_traits_helper<value>::pointer;
		using reference = typename avl_tree_iterator_traits_helper<value>::reference;
		using iterator_category = eastl::bidirectional_iterator_tag;
		using difference_type = std::ptrdiff_t;

		avl_tree_iterator();
		explicit avl_tree_iterator(avl_node* node);

		avl_tree_iterator(const avl_tree_iterator& other);
		avl_tree_iterator& operator=(const avl_tree_iterator& other);

		template <typename other_node, typename other_value, typename = eastl::enable_if_t<
			eastl::is_convertible_v<other_node*, avl_node*>&&
			eastl::is_convertible_v<other_value*, value*>
		>>
		avl_tree_iterator(const avl_tree_iterator<other_node, other_value>& other);

		pointer operator->() const;
		reference operator*() const;

		avl_tree_iterator& operator++();
		avl_tree_iterator operator++(int);

		avl_tree_iterator& operator--();
		avl_tree_iterator operator--(int);

		bool operator==(const avl_tree_iterator& other) const;
		bool operator!=(const avl_tree_iterator& other) const;

		avl_node* get_node() const;

	private:
		avl_node* maximum(avl_node* node) const;
		avl_node* minimum(avl_node* node) const;

	private:
		avl_node* node_ = nullptr;
		template <typename, typename> friend class avl_tree_iterator;
	};

	template <typename avl_node, typename value>
	inline avl_tree_iterator<avl_node, value>::avl_tree_iterator()
		: node_(nullptr)
	{
	}

	template <typename avl_node, typename value>
	inline avl_tree_iterator<avl_node, value>::avl_tree_iterator(avl_node* node)
		: node_(node)
	{
	}

	template <typename avl_node, typename value>
	inline avl_tree_iterator<avl_node, value>::avl_tree_iterator(const avl_tree_iterator<avl_node, value>& other)
		: node_(other.node_)
	{
	}

	template <typename avl_node, typename value>
	inline avl_tree_iterator<avl_node, value>& avl_tree_iterator<avl_node, value>::operator=(const avl_tree_iterator<avl_node, value>& other)
	{
		if (this != &other)
		{
			node_ = other.node_;
		}
		return *this;
	}

	template <typename avl_node, typename value>
	template <typename other_node, typename other_value, typename>
	inline avl_tree_iterator<avl_node, value>::avl_tree_iterator(const avl_tree_iterator<other_node, other_value>& other)
		: node_(other.node_)
	{
	}

	template <typename avl_node, typename value>
	inline typename avl_tree_iterator<avl_node, value>::pointer avl_tree_iterator<avl_node, value>::operator->() const
	{
		return &node_->data;
	}

	template <typename avl_node, typename value>
	inline typename avl_tree_iterator<avl_node, value>::reference avl_tree_iterator<avl_node, value>::operator*() const
	{
		return node_->data;
	}

	template <typename avl_node, typename value>
	inline avl_tree_iterator<avl_node, value>& avl_tree_iterator<avl_node, value>::operator++()
	{
		if (node_->right)
		{
			node_ = minimum(node_->right);
		}
		else
		{
			avl_node* parent = node_->parent;
			while (parent && node_ == parent->right)
			{
				node_ = parent;
				parent = parent->parent;
			}
			node_ = parent;
		}
		return *this;
	}

	template <typename avl_node, typename value>
	inline avl_tree_iterator<avl_node, value> avl_tree_iterator<avl_node, value>::operator++(int)
	{
		avl_tree_iterator temp = *this;
		++(*this);
		return temp;
	}

	template <typename avl_node, typename value>
	inline avl_tree_iterator<avl_node, value>& avl_tree_iterator<avl_node, value>::operator--()
	{
		if (node_->left)
		{
			node_ = maximum(node_->left);
		}
		else
		{
			avl_node* parent = node_->parent;
			while (parent && node_ == parent->left)
			{
				node_ = parent;
				parent = parent->parent;
			}
			node_ = parent;
		}
		return *this;
	}

	template <typename avl_node, typename value>
	inline avl_tree_iterator<avl_node, value> avl_tree_iterator<avl_node, value>::operator--(int)
	{
		avl_tree_iterator temp = *this;
		--(*this);
		return temp;
	}

	template <typename avl_node, typename value>
	inline bool avl_tree_iterator<avl_node, value>::operator==(const avl_tree_iterator& other) const
	{
		return node_ == other.node_;
	}

	template <typename avl_node, typename value>
	inline bool avl_tree_iterator<avl_node, value>::operator!=(const avl_tree_iterator& other) const
	{
		return node_ != other.node_;
	}

	template <typename avl_node, typename value>
	inline avl_node* avl_tree_iterator<avl_node, value>::get_node() const
	{
		return node_;
	}

	template <typename avl_node, typename value>
	inline avl_node* avl_tree_iterator<avl_node, value>::maximum(avl_node* node) const
	{
		while (node->right) node = node->right;
		return node;
	}

	template <typename avl_node, typename value>
	inline avl_node* avl_tree_iterator<avl_node, value>::minimum(avl_node* node) const
	{
		while (node->left) node = node->left;
		return node;
	}
}

#endif // ARES_CORE_AVL_TREE_ITERATOR_H