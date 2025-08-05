#ifndef ARES_CORE_PAGE_ALLOCATOR_H
#define ARES_CORE_PAGE_ALLOCATOR_H

namespace ares::core::internal {

	class page_allocator
	{
	public:
		virtual ~page_allocator() = default;

		virtual size_t page_size() const = 0;
		virtual void* reserve_memory(size_t size) = 0;
		virtual bool commit_memory(void* address, size_t size) = 0;
		virtual bool decommit_memory(void* address, size_t size) = 0;
		virtual bool release_memory(void* address, size_t size) = 0;
	};

	page_allocator* get_page_allocator();

}

#endif // ARES_CORE_PAGE_ALLOCATOR_H