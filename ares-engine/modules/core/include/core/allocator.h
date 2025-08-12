#ifndef ARES_CORE_ALLOCATOR_BASE_H
#define ARES_CORE_ALLOCATOR_BASE_H

namespace ares::core {

	class allocator
	{
	public:
		virtual ~allocator() = default;

		virtual void* allocate(size_t size) = 0;
		virtual void* allocate(size_t size, size_t alignment) = 0;
		virtual void deallocate(void* ptr) = 0;
	};

}

#endif // ARES_CORE_ALLOCATOR_BASE_H