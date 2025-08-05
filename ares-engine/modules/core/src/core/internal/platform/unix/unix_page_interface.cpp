#include "core/internal/platform/unix/unix_page_interface.h"
#include <assert.h>
#include <unistd.h>
#include <sys/mman.h>

constexpr size_t PAGE_SIZE = static_cast<size_t>(sysconf(_SC_PAGESIZE));

namespace ares::core::internal {

	unix_page_interface::unix_page_interface()
	{
	}

	size_t unix_page_interface::page_size() const
	{
		return PAGE_SIZE;
	}

	void* unix_page_interface::reserve_memory(size_t size)
	{
		assert(size % PAGE_SIZE == 0 && "Size must be a multiple of the page size!");
		void* ptr = ::mmap(nullptr, size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		assert(ptr != MAP_FAILED && "Failed to reserve virtual memory!");
		return ptr;
	}

	bool unix_page_interface::commit_memory(void* address, size_t size)
	{
		assert(size % PAGE_SIZE == 0 && "Size must be a multiple of the page size!");
		int result = ::mprotect(address, size, PROT_READ | PROT_WRITE);
		assert(result == 0 && "Failed to commit virtual memory!");
		return result == 0;
	}

	bool unix_page_interface::decommit_memory(void* address, size_t size)
	{
		assert(size % PAGE_SIZE == 0 && "Size must be a multiple of the page size!");
		int result = ::mprotect(address, size, PROT_NONE);
		assert(result == 0 && "Failed to decommit virtual memory!");
		return result == 0;
	}

	bool unix_page_interface::release_memory(void* address, size_t size)
	{
		assert(size % PAGE_SIZE == 0 && "Size must be a multiple of the page size!");
		int result = ::munmap(address, size);
		assert(result == 0 && "Failed to release virtual memory!");
		return result == 0;
	}

}