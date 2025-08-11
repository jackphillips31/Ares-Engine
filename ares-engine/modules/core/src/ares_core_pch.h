#include "core/platform.h"

// STL
#include <cassert>
#include <new>
#include <shared_mutex>
#include <stdexcept>

// EASTL


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