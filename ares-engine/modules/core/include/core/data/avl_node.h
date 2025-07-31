#ifndef ARES_CORE_AVL_NODE_H
#define ARES_CORE_AVL_NODE_H
#include <EASTL/type_traits.h>

namespace ares::core {

	template <typename key, typename value = void>
	struct avl_node
	{
	private:
		struct empty_value {};

	public:
		using value_type = eastl::conditional_t<
			eastl::is_void_v<value>,
			key,
			eastl::pair<const key, value>
		>;

		avl_node() = default;
		~avl_node() = default;

		avl_node(const value_type& kv) : data(kv) {}
		avl_node(value_type&& kv) : data(eastl::move(kv)) {}

		value_type data;

		avl_node* left = nullptr;
		avl_node* right = nullptr;
		avl_node* parent = nullptr;
		int32_t height = 0;
	};

}

#endif // ARES_CORE_AVL_NODE_H