#include <ares_launcher_pch.h>
#include "core/internal/os_page_interface.h"
#include "core/platform.h"

#if ARES_PLATFORM_WINDOWS
#include "core/internal/platform/windows/win32_page_interface.h"
#elif ARES_PLATFORM_UNIX
#include "core/internal/platform/unix/unix_page_interface.h"
#endif

namespace ares::core::internal {

	os_page_interface& get_os_page_interface()
	{
	#if ARES_PLATFORM_WINDOWS
		static win32_page_interface result;
	#elif ARES_PLATFORM_UNIX
		static unix_page_interface result;
	#else
	#error "Unsupported platform"
	#endif
		return result;
	}

}