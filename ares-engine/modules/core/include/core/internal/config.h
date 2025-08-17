///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2025 Jack Phillips. All rights reserved.
// 
///////////////////////////////////////////////////////////////////////////////

#ifndef ARES_CORE_INTERNAL_CONFIG_H
#define ARES_CORE_INTERNAL_CONFIG_H

///////////////////////////////////////////////////////////////////////////////
//
// ReadMe
//
// This is the Ares Core module configuration file. All configurable parameters
// of the Ares Engine are controlled through this file. However, all the
// settings here can be manually overridden by the user. There are three ways
// for a user to override the settings in this file:
//
//     - Simply edit this file.
//     - Define ARES_USER_CONFIG_HEADER.
//     - Predefine individual defines (e.g. ARES_ASSERT).
//
///////////////////////////////////////////////////////////////////////////////




///////////////////////////////////////////////////////////////////////////////
//
// ARES_USER_CONFIG_HEADER
//
// This allows the user to define a header file to be #included before the Ares
// Core module config.h contents are compiled. A primary use of this is to
// override the contents of this config.h file. Note that all the settings
// below in this file are user-configurable.
//
// NOTE: If this macro is defined in the build system or IDE (e.g., via
// compiler definitions), the override applies globally across the entire
// Ares Engine. If instead it is defined inside a single .cpp or .cxx file, the
// override applies only to that translation unit.
//
// Example usage:
//     #define ARES_USER_CONFIG_HEADER "MyCoreConfigOverrides.h"
//     #include <core/vector.h>
//
///////////////////////////////////////////////////////////////////////////////

#ifdef ARES_USER_CONFIG_HEADER
	#include ARES_USER_CONFIG_HEADER
#endif




///////////////////////////////////////////////////////////////////////////////
//
// ARES_EASTL_DISABLED
//
// The user can disable EASTL usage and manually fall back to standard STL
// containers. When ARES_EASTL_DISABLED is set to 1, all Ares Core
// container types (vector, string, map, etc.) will use their std:: equivalents
// instead of EASTL implementations. This is useful for projeccts that cannot
// use EASTL or prefer standard library containers.
//
// Values: 0 = enabled (default), 1 = disabled
//
// Example usage:
//     #define ARES_EASTL_DISABLED 1
//     #include <core/vector.h>
//
///////////////////////////////////////////////////////////////////////////////

#ifndef ARES_EASTL_DISABLED
	#define ARES_EASTL_DISABLED 0
#endif




///////////////////////////////////////////////////////////////////////////////
//
// ARES_ASSERTS_DISABLED
// ARES_DEBUG_ASSERTS_DISABLED
// ARES_STATIC_ASSERTS_DISABLED
//
// Controls whether runtime and compile-time assertion checks are included in a
// build. Assertions can enforce checks in all build configurations. Debug
// assertions run only in debug builds, providing additional validation without
// impacting release performance. Static assertions enforce compile-time
// conditions. Any of these can be disabled entirely to remove runtime or
// compile-time validation for maximum performance or increased build times.
//
// Values: 0 = enabled (default for all), 1 = disabled
//
// Example usage:
//     #define ARES_ASSERTS_DISABLED 1
//     #define ARES_STATIC_ASSERTS_DISABLED 1
//     #include <core/vector.h>
//
///////////////////////////////////////////////////////////////////////////////
//
// ARES_ASSERT(condition, message)
// ARES_DEBUG_ASSERT(condition, message)
// ARES_STATIC_ASSERT(condition, message)
//
// Provides lightweight assertion macros for runtime or compile-time condition
// checking. These macros allow enabling/disabling assertions at compile time
// without incurring cost when disabled.
//
// Example usage:
//     int* ptr = get_ptr();
//     int value = *ptr;
//     ARES_ASSERT(value > 0, "value must be positive");
//     ARES_DEBUG_ASSERT(ptr != nullptr, "ptr must not be null in debug");
//     ARES_STATIC_ASSERT(sizeof(int) == 4, "int must be 4 bytes");
//
///////////////////////////////////////////////////////////////////////////////

#ifndef ARES_DEBUG_ASSERTS_DISABLED
	#define ARES_DEBUG_ASSERTS_DISABLED 0
#endif
#ifndef ARES_ASSERTS_DISABLED
	#define ARES_ASSERTS_DISABLED 0
#endif
#ifndef ARES_STATIC_ASSERTS_DISABLED
	#define ARES_STATIC_ASSERTS_DISABLED 0
#endif

#if (ARES_DEBUG_ASSERTS_DISABLED && ARES_BUILD_DEBUG) || ARES_ASSERTS_DISABLED
	#include <cassert>
#endif
#if ARES_ASSERTS_DISABLED
	#define ARES_ASSERT(condition, message) ((void)0)
#else
	#define ARES_ASSERT(condition, message) assert((condition) && (message))
#endif
#if ARES_DEBUG_ASSERTS_DISABLED && ARES_BUILD_DEBUG
	#define ARES_DEBUG_ASSERT(condition, message) ((void)0)
#else
	#define ARES_DEBUG_ASSERT(condition, message) assert((condition) && (message))
#endif
#if ARES_STATIC_ASSERTS_DISABLED
	#define ARES_STATIC_ASSERT(condition, message) ((void)0)
#else
	#define ARES_STATIC_ASSERT(condition, message) static_assert((condition), (message))
#endif




///////////////////////////////////////////////////////////////////////////////
//
// Compiler Configuration Section
//
// The following secction contains compiler-specific feature detection
// compatibility macros. These settings allow the Ares Core module to adapt to
// different compiler capabilities and C++ standard versions while maintaining
// a consistent API across platforms.
//
//     - constexpr support detection and fallback mechanisms
//     - noexcept support detection and fallback mechanisms
//     - other compiler-specific optimizations and workarounds
//
///////////////////////////////////////////////////////////////////////////////




///////////////////////////////////////////////////////////////////////////////
//
// ARES_COMPILER_CONSTEXPR_DISABLED
//
// Disables the use of constexpr keywords throughout the Ares Core module and
// therefore the entire Ares Engine, and falling back to const or inline
// alternatives wherever appropriate. This is useful for older compilers that
// don't support constexpr or when debugging constexpr evaluation issues. When
// disabled, ARES_CONSTEXPR becomes empty and ARES_CONSTEXPR_OR_CONST
// becomes const.
//
// Values: 0 = enabled (default), 1 = disabled
//
// Example usage:
//     #define ARES_COMPILER_CONSTEXPR_DISABLED 1
//     #include <core/avl_tree.h>
//
///////////////////////////////////////////////////////////////////////////////
//
// ARES_CONSTEXPR
// ARES_CONSTEXPR_OR_CONST
//
// Portable wrapper for C++ 11's 'constexpr' support.
//
// See http://www.cprogramming.com/c++11/c++11-compile-time-processing-with-constexpr.html
// for more information.
//
// Example usage:
//     ARES_CONSTEXPR int get_value() { return 32; }
//     ARES_CONSTEXPR_OR_CONST double get_sin(const double& value)
//     {
//         return std::sin(value);
//     }
//
///////////////////////////////////////////////////////////////////////////////

#ifndef ARES_COMPILER_CONSTEXPR_DISABLED
	#define ARES_COMPILER_CONSTEXPR_DISABLED 0
#endif

#ifndef ARES_CONSTEXPR
	#if ARES_COMPILER_CONSTEXPR_DISABLED
		#define ARES_CONSTEXPR
	#else
		#define ARES_CONSTEXPR constexpr
	#endif
#endif
#ifndef ARES_CONSTEXPR_OR_CONST
	#if ARES_COMPILER_CONSTEXPR_DISABLED
		#define ARES_CONSTEXPR_OR_CONST const
	#else
		#define ARES_CONSTEXPR_OR_CONST constexpr
	#endif
#endif




///////////////////////////////////////////////////////////////////////////////
//
// ARES_COMPILER_NOEXCEPT_DISABLED
//
// Disables the use of noexcept specifications throughout the Ares Core module
// and therefore the entire Ares Engine. This can be useful for older compilers
// that don't support noexcept (pre C++ 11) or when you need to maintain
// compatibility with exception-throwing code. When disabled,
// ARES_NOEXCEPT becomes empty, removing noexcept specifications from
// function declarations.
//
// Values: 0 = enabled (default), 1 = disabled
//
// Example usage:
//     #define ARES_COMPILER_NOEXCEPT_DISABLED 1
//     #include <core/avl_tree.h>
//
///////////////////////////////////////////////////////////////////////////////
//
// ARES_NOEXCEPT
// ARES_NOEXCEPT_IF(predicate)
// ARES_NOEXCEPT_EXPR(expression)
//
// Portable wrapper for C++ 11 noexcept.
// See for more information:
// http://en.cppreference.com/w/cpp/language/noexcept
// http://en.cppreference.com/w/cpp/language/noexcept_spec
//
// Example usage:
//     ARES_NOEXCEPT
//     ARES_NOEXCEPT_IF(predicate)
//     ARES_NOEXCEPT_EXPR(expression)
//
//     // This function never throws an exception.
//     void do_nothing() ARES_NOEXCEPT
//     {
//     }
//
//     // This function throws an exception of value_type::value_type() throws
//     // an exception.
//     template <typename value_type>
//     void do_nothing() ARES_NOEXCEPT_IF(ARES_NOEXCEPT_EXPR(value_type()))
//     {
//         value_type result;
//     }
//
///////////////////////////////////////////////////////////////////////////////

#ifndef ARES_COMPILER_NOEXCEPT_DISABLED
	#define ARES_COMPILER_NOEXCEPT_DISABLED 0
#endif

#ifndef ARES_NOEXCEPT
	#if ARES_COMPILER_NOEXCEPT_DISABLED
		#define ARES_NOEXCEPT
		#define ARES_NOEXCEPT_IF(predicate)
		#define ARES_NOEXCEPT_EXPR(expression) false
	#else
		#define ARES_NOEXCEPT noexcept
		#define ARES_NOEXCEPT_IF(predicate) noexcept((predicate))
		#define ARES_NOEXCEPT_EXPR(expression) noexcept((expression))
	#endif
#endif

#endif // ARES_CORE_INTERNAL_CONFIG_H