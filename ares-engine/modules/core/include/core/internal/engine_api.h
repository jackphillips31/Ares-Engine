///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2025 Jack Phillips. All rights reserved.
// 
///////////////////////////////////////////////////////////////////////////////

#ifndef ARES_CORE_INTERNAL_ENGINE_API_H
#define ARES_CORE_INTERNAL_ENGINE_API_H

#include "core/internal/common.h"

///////////////////////////////////////////////////////////////////////////////
//
// Ares Engine Module API Section
//
// This section defines the per-module API macros (e.g., ARES_CORE_API,
// ARES_PHYSICS_API, etc.) usef ro controlling symbol export/import in dynamic
// builds. Each module defines its own macro, which resolves to
// __declspec(dllexport) when building the module and __declspec(dllimport)
// when used by other modules. In static builds, these macros are empty.
//
// These macros are intended for internal use by the engine and module headers
// and are not meant for direct user code modification.
//
///////////////////////////////////////////////////////////////////////////////

#if ARES_BUILD_STATIC
	#define ARES_CORE_API
#else
	#if ARES_BUILD_CORE_MODULE
		#define ARES_CORE_API __declspec(dllexport)
	#else
		#define ARES_CORE_API __declspec(dllimport)
	#endif
#endif

#endif // ARES_CORE_INTERNAL_ENGINE_API_H