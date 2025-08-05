#include "core/internal/page_allocator.h"
#include "core/platform.h"
#include <new>

#if ARES_PLATFORM_WINDOWS
#include "core/internal/platform/windows/win32_page_allocator.h"
#elif ARES_PLATFORM_UNIX
#include "core/internal/platform/unix/unix_page_allocator.h"
#endif

namespace ares::core::internal {

	page_allocator* get_page_allocator()
	{
		static page_allocator* result = []() -> page_allocator*
		{
			static alignas(page_allocator) char buffer[sizeof(page_allocator)];
		#if ARES_PLATFORM_WINDOWS
			return new (buffer) win32_page_allocator();
		#elif ARES_PLATFORM_UNIX
			return new (buffer) unix_page_allocator();
		#else
		#error "Unsupported platform"
		#endif
		}();
		return result;
	}

}