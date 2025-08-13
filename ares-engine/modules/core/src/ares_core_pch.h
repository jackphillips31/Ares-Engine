#include "core/platform.h"

// STL
#include <cassert>
#include <mutex>
#include <new>
#include <shared_mutex>
#include <stdexcept>

// EASTL
#include <EASTL/functional.h>
#include <EASTL/type_traits.h>
#include <EASTL/utility.h>

// Platform specific
// Windows
#if ARES_PLATFORM_WINDOWS
#include <windows.h>
#endif

// Unix
#if ARES_PLATFORM_UNIX
#include <unistd.h>
#include <sys/mman.h>
#endif