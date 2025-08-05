#include "core/internal/platform/unix/unix_page_allocator.h"
#include <assert.h>
#include <unistd.h>
#include <sys/mman.h>

constexpr size_t PAGE_SIZE = static_cast<size_t>(sysconf(_SC_PAGESIZE));

namespace ares::core::internal {

	unix_page_allocator::unix_page_allocator()
	{
	}

	size_t unix_page_allocator::page_size() const
	{
		return PAGE_SIZE;
	}

	void* unix_page_allocator::reserve_memory(size_t size)
	{
		assert(size % PAGE_SIZE == 0 && "Size must be a multiple of the page size!");
		void* ptr = ::mmap(nullptr, size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		assert(ptr != MAP_FAILED && "Failed to reserve virtual memory!");
		return ptr;
	}

	bool unix_page_allocator::commit_memory(void* address, size_t size)
	{
		assert(size % PAGE_SIZE == 0 && "Size must be a multiple of the page size!");
		int result = ::mprotect(address, size, PROT_READ | PROT_WRITE);
		assert(result == 0 && "Failed to commit virtual memory!");
		return result == 0;
	}

	bool unix_page_allocator::decommit_memory(void* address, size_t size)
	{
		assert(size % PAGE_SIZE == 0 && "Size must be a multiple of the page size!");
		int result = ::mprotect(address, size, PROT_NONE);
		assert(result == 0 && "Failed to decommit virtual memory!");
		return result == 0;
	}

	bool unix_page_allocator::release_memory(void* address, size_t size)
	{
		assert(size % PAGE_SIZE == 0 && "Size must be a multiple of the page size!");
		int result = ::munmap(address, size);
		assert(result == 0 && "Failed to release virtual memory!");
		return result == 0;
	}

}