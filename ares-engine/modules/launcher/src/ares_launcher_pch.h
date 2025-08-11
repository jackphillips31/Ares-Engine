#include "core/platform.h"

// STL
#include <cassert>
#include <new>
#include <stdexcept>

// Platform specific
// Windows
#if ARES_PLATFORM_WINDOWS
#include <windows.h>
#endif