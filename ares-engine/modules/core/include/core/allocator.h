#ifndef ARES_CORE_ALLOCATOR_BASE_H
#define ARES_CORE_ALLOCATOR_BASE_H

namespace ares::core {

	class virtual_memory_manager;

	class allocator
	{
	public:
		allocator(virtual_memory_manager* vmm) : m_virtual_memory_manager(vmm) {}
		virtual ~allocator() = default;

		virtual void* allocate(size_t size) = 0;
		virtual void* allocate(size_t size, size_t alignment) = 0;
		virtual void deallocate(void* ptr) = 0;

	private:
		virtual_memory_manager* m_virtual_memory_manager;
	}

}

#endif // ARES_CORE_ALLOCATOR_BASE_H