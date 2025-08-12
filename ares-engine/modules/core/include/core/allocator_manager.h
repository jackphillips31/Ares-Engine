#ifndef ARES_CORE_ALLOCATOR_MANAGER_H
#define ARES_CORE_ALLOCATOR_MANAGER_H

namespace ares::core {

	class allocator;

	template <typename allocator_type>
	allocator* get_allocator()
	{
		assert(false && "This has not been implemented!");
		return nullptr;
	}

}

#endif // ARES_CORE_ALLOCATOR_MANAGER_H