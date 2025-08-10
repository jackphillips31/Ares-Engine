#include <ares_core_pch.h>
#include "core/internal/platform/windows/win32_page_interface.h"

namespace ares::core::internal {

	win32_page_interface::win32_page_interface()
	{
	}

	size_t win32_page_interface::page_size() const
	{
		static size_t page_size = [] {
			SYSTEM_INFO sys_info;
			::GetSystemInfo(&sys_info);
			return static_cast<size_t>(sys_info.dwPageSize);
		}();
		return page_size;
	}

	void* win32_page_interface::reserve_memory(size_t size)
	{
		assert(size % page_size() == 0 && "Size must be a multiple of the page size!");
		void* ptr = ::VirtualAlloc(nullptr, size, MEM_RESERVE, PAGE_READWRITE);
		assert(ptr && "Failed to reserve virtual memory!");
		return ptr;
	}

	bool win32_page_interface::commit_memory(void* address, size_t size)
	{
		assert(size % page_size() == 0 && "Size must be a multiple of the page size!");
		void* ptr = ::VirtualAlloc(address, size, MEM_COMMIT, PAGE_READWRITE);
		assert(ptr && "Failed to commit virtual memory!");
		return ptr != nullptr;
	}

	bool win32_page_interface::decommit_memory(void* address, size_t size)
	{
		assert(size % page_size() == 0 && "Size must be a multiple of the page size!");
		BOOL result = ::VirtualFree(address, size, MEM_DECOMMIT);
		assert(result && "Failed to decommit virtual memory!");
		return result != 0;
	}

	bool win32_page_interface::release_memory(void* address, size_t size)
	{
		BOOL result = ::VirtualFree(address, 0, MEM_RELEASE);
		assert(result && "Failed to release virtual memory!");
		return result != 0;
	}

}