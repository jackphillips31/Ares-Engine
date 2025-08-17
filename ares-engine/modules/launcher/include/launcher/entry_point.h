#ifndef ARES_LAUNCHER_ENTRY_POINT_H
#define ARES_LAUNCHER_ENTRY_POINT_H
#include "core/packed_array.h"
#include "core/avl_tree.h"
#include "core/temp_allocator.h"

int ares_main(int argc, char** argv)
{
	using namespace ares::core;
	avl_tree<uint32_t, uint32_t, eastl::less<uint32_t>, temp_allocator> test_tree(&get_temp_allocator());
	using node = avl_tree<uint32_t, uint32_t, eastl::less<uint32_t>, void>::node;

	packed_array<uint32_t, 16> test_arr = { 33, 44, 243, 135, 643 };

	test_tree.insert({ 32, 32 });
	test_tree.insert({ 43,12 });

	avl_tree<uint32_t, uint32_t, eastl::less<uint32_t>, void> test_tree3;
	avl_tree<uint32_t, uint32_t, eastl::less<uint32_t>, void> test_tree4;

	node node1({ 32, 44 });
	node node2({ 54, 32 });
	node node3({ 33, 45 });
	node node4({ 34, 44 });
	node node5({ 45, 33 });
	node node6({ 23, 44 });
	node node7({ 35, 33 });

	test_tree3 = { &node1, &node2, &node3, &node4, &node5, &node6};
	test_tree3.insert(&node7);
	uint32_t val = test_tree3.at(32);
	test_tree3.erase(45);

	//test_tree = { &node1, &node2 };
	avl_tree<uint32_t, uint32_t, eastl::less<uint32_t>, temp_allocator> test_tree2 = test_tree;
	test_tree2 = test_tree;
	return 0;
}

int main(int argc, char** argv)
{
	return ares_main(argc, argv);
}

#endif // ARES_LAUNCHER_ENTRY_POINT_H