#include "core/internal/os_page_interface.h"
#include "core/platform.h"
#include <new>

#if ARES_PLATFORM_WINDOWS
#include "core/internal/platform/windows/win32_page_interface.h"
#elif ARES_PLATFORM_UNIX
#include "core/internal/platform/unix/unix_page_interface.h"
#endif

namespace ares::core::internal {

	os_page_interface* get_page_interface()
	{
		static os_page_interface* result = []() -> os_page_interface*
		{
			static alignas(os_page_interface) char buffer[sizeof(os_page_interface)];
		#if ARES_PLATFORM_WINDOWS
			return new (buffer) win32_page_interface();
		#elif ARES_PLATFORM_UNIX
			return new (buffer) unix_page_interface();
		#else
		#error "Unsupported platform"
		#endif
		}();
		return result;
	}

}