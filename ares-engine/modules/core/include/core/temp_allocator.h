#ifndef ARES_CORE_TEMP_ALLOCATOR_H
#define ARES_CORE_TEMP_ALLOCATOR_H
#include <stdlib.h>
#include "core/allocator.h"

namespace ares::core {

	class temp_allocator : public allocator
	{
	public:
		temp_allocator() {}

		void* allocate(size_t size) override { return malloc(size); }
		void* allocate(size_t size, size_t alignment) override { return malloc(size); }
		void deallocate(void* ptr) override { free(ptr); }
	};

	temp_allocator& get_temp_allocator();

}

#endif // ARES_CORE_TEMP_ALLOCATOR_H