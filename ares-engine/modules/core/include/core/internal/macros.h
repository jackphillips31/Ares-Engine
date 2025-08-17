///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2025 Jack Phillips. All rights reserved.
// 
///////////////////////////////////////////////////////////////////////////////

#ifndef ARES_CORE_INTERNAL_MACROS_H
#define ARES_CORE_INTERNAL_MACROS_H

#ifdef __GNUC__
	#define ARES_LIKELY(x)   __builtin_expect(!!(x), 1)
	#define ARES_UNLIKELY(x) __builtin_expect(!!(x), 0)
#elif defined(_MSC_VER)
	#define ARES_LIKELY(x)   (x)
	#define ARES_UNLIKELY(x) (x)
#else
	#define ARES_LIKELY(x)   (x)
	#define ARES_UNLIKELY(x) (x)
#endif

#endif // ARES_CORE_INTERNAL_MACROS_H