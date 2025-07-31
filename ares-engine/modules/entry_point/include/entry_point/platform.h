#ifndef ARES_PLATFORM_H
#define ARES_PLATFORM_H

/******************************************************/
/*     Platform detection using predefined macros     */
/******************************************************/
/*       Taken from EABase/config/eaplatform.h        */

#if defined(__CYGWIN__)
	#define AR_PLATFORM_CYGWIN 1
	#define AR_PLATFORM_DESKTOP 1
#endif

#if defined(__MINGW32__) || defined(__MINGW64__)
	#define AR_PLATFORM_MINGW 1
	#define AR_PLATFORM_DESKTOP 1
#endif


#if defined(AR_PLATFORM_PS4) || defined(__ORBIS__)
	#if defined(AR_PLATFORM_PS4)
		#undef AR_PLATFORM_PS4
	#endif
	#define AR_PLATFORM_PS4 1

	#define AR_PLATFORM_NAME		"PS4"
	#define AR_PLATFORM_DESCRIPTION	"PS4 on x64"
	#define AR_PLATFORM_SONY		1
	#define AR_PLATFORM_CONSOLE		1
	#define AR_PLATFORM_POSIX		1
	#define AR_PROCESSOR_X86_64		1
	#define AR_SYSTEM_LITTLE_ENDIAN 1

	#if defined(__GNUC__) || defined(__clang__)
		#define AR_ASM_STYLE_ATT 1
	#endif


#elif defined(AR_PLATFORM_XBOXONE) || defined(_DURANGO) || defined(_XBOX_ONE) || defined(_GAMING_XBOX)
	#if defined(AR_PLATFORM_XBOXONE)
		#undef AR_PLATFORM_XBOXONE
	#endif
	#define AR_PLATFORM_XBOXONE 1

	#if !defined(_DURANGO)
		#define _DURANGO
	#endif

	#define AR_PLATFORM_NAME		"XBox One"
	#define AR_PLATFORM_DESCRIPTION	"XBox One on x64"
	#define AR_PLATFORM_MICROSOFT	1
	#define AR_PLATFORM_CONSOLE		1
	#define AR_PROCESSOR_X86_64		1
	#define AR_SYSTEM_LITTLE_ENDIAN 1
	#define AR_ASM_STYLE_INTEL		1

	/* WINAPI_FAMILY defines - mirrored from winapifamily.h */
	#define AR_WINAPI_FAMILY_APP			1000
	#define AR_WINAPI_FAMILY_DESKTOP_APP	1001
	#define AR_WINAPI_FAMILY_PHONE_APP		1002
	#define AR_WINAPI_FAMILY_TV_APP			1003
	#define AR_WINAPI_FAMILY_TV_TITLE		1004
	#define AR_WINAPI_FAMILY_GAMES			1006

	#if defined (WINAPI_FAMILY)
		#include <winapifamily.h>
		#if defined(WINAPI_FAMILY_TV_TITLE) && WINAPI_FAMILY == WINAPI_FAMILY_TV_TITLE
			#define AR_WINAPI_FAMILY AR_WINAPI_FAMILY_TV_TITLE
		#elif defined(WINAPI_FAMILY_DESKTOP_APP) && WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP
			#define AR_WINAPI_FAMILY AR_WINAPI_FAMILY_DESKTOP_APP
		#elif defined(WINAPI_FAMILY_GAMES) && WINAPI_FAMILY == WINAPI_FAMILY_GAMES
			#define AR_WINAPI_FAMILY AR_WINAPI_FAMILY_GAMES
		#else
			#error Unsupported WINAPI_FAMILY
		#endif
	#else
		#error WINAPI_FAMILY should always be defined on XBox.
	#endif
	
	/* Macro to determine if a partition is enabled. */
	#define AR_WINAPI_FAMILY_PARTITION(Partition)	(Partition)

	#if AR_WINAPI_FAMILY == AR_WINAPI_FAMILY_DESKTOP_APP
		#define AR_WINAPI_PARTITION_CORE		1
		#define AR_WINAPI_PARTITION_DESKTOP		1
		#define AR_WINAPI_PARTITION_APP			1
		#define AR_WINAPI_PARTITION_PC_APP		0
		#define AR_WINAPI_PARTITION_PHONE		0
		#define AR_WINAPI_PARTITION_TV_APP		0
		#define AR_WINAPI_PARTITION_TV_TITLE	0
		#define AR_WINAPI_PARTITION_GAMES		0
	#elif AR_WINAPI_FAMILY == AR_WINAPI_FAMILY_TV_TITLE
		#define AR_WINAPI_PARTITION_CORE		1
		#define AR_WINAPI_PARTITION_DESKTOP		0
		#define AR_WINAPI_PARTITION_APP			0
		#define AR_WINAPI_PARTITION_PC_APP		0
		#define AR_WIANPI_PARTITION_PHONE		0
		#define AR_WINAPI_PARTITION_TV_APP		0
		#define AR_WINAPI_PARTITION_TV_TITLE	1
		#define AR_WINAPI_PARTITION_GAMES		0
	#elif AR_WINAPI_FAMILY == AR_WINAPI_FAMILY_GAMES
		#define AR_WINAPI_PARTITION_CORE		1
		#define AR_WINAPI_PARTITION_DESKTOP		0
		#define AR_WINAPI_PARTITION_APP			0
		#define AR_WINAPI_PARTITION_PC_APP		0
		#define AR_WIANPI_PARTITION_PHONE		0
		#define AR_WINAPI_PARTITION_TV_APP		0
		#define AR_WINAPI_PARTITION_TV_TITLE	0
		#define AR_WINAPI_PARTITION_GAMES		1
	#else
		#error Unsupported WINAPI_FAMILY
	#endif

	#if AR_WINAPI_FAMILY_PARTITION(AR_WINAPI_PARTITION_GAMES)
		#define CS_UNDEFINED_STRING 			1
		#define CS_UNDEFINED_STRING 		1
	#endif

	#if AR_WINAPI_FAMILY_PARTITION(AR_WINAPI_PARTITION_TV_TITLE)
		#define AR_PLATFORM_XBOXONE_XDK 	1
	#endif

	#if AR_WINAPI_FAMILY_PARTITION(AR_WINAPI_PARTITION_TV_TITLE)
		#define AR_PLATFORM_XBOXONE_XDK 	1
	#endif


#elif defined(AR_PLATFORM_LRB) || defined(__LRB__) || (defined(__EDG__) && defined(__ICC) && defined(__x86_64__))
	#undef AR_PLATFORM_LRB
	#define AR_PLATFORM_LRB			1
	#define AR_PLATFORM_NAME		"Larrabee"
	#define AR_PLATFORM_DESCRIPTION	"Larrabee on LRB1"
	#define AR_PLATFORM_DESKTOP		1
	#define AR_PROCESSOR_X86_64		1
	#define AR_PROCESSOR_LRB		1
	#define AR_PROCESSOR_LRB1		1

	#if defined(BYTE_ORDER) && (BYTE_ORDER == 4321)
		#define AR_SYSTEM_BIG_ENDIAN 1
	#else
		#define AR_SYSTEM_LITTLE_ENDIAN 1
	#endif

	#define AR_ASM_STYLE_ATT		1		// Larrabee version 1
	#define AR_ASM_STYLE_INTEL		1		// Both types of asm style are supported.


#elif defined(AR_PLATFORM_ANDROID) || defined(__ANDROID__)
	/* Android (Google phone OS) */
	#undef AR_PLATFORM_ANDROID
	#define AR_PLATFORM_ANDROID		1
	#define AR_PLATFORM_NAME		"Android"
	#define AR_PLATFORM_LINUX		1
	#define AR_PLATFORM_UNIX		1
	#define AR_PLATFORM_POSIX		1
	#define AR_PLATFORM_MOBILE		1

	#if !defined(AR_SYSTEM_BIG_ENDIAN) && !defined(AR_SYSTEM_LITTLE_ENDIAN)
		#define AR_SYSTEM_LITTLE_ENDIAN 1
	#endif

	#define AR_ASM_STYLE_ATT		1

	#if defined(__arm__)
		#define AR_ABI_ARM_LINUX		1	// a.k.a. "ARM eabi"
		#define AR_PROCESSOR_ARM32		1
		#define AR_PLATFORM_DESCRIPTION "Android on ARM"
	#elif defined(__aarch64__)
		#define AR_PROCESSOR_ARM64		1
		#define AR_PLATFORM_DESCRIPTION "Android on ARM64"
	#elif defined(__i386__)
		#define AR_PROCESSOR_X86		1
		#define AR_PLATFORM_DESCRIPTION "Android on x86"
	#elif defined(__x86_64)
		#define AR_PROCESSOR_X86_64		1
		#define AR_PLATFORM_DESCRIPTION "Android on x64"
	#else
		#error Unknown processor
	#endif
	

#elif defined(AR_PLATFORM_SAMSUNG_TV)
	#undef AR_PLATFORM_SAMSUNG_TV
	#define AR_PLATFORM_SAMSUNG_TV	1
	#define AR_PLATFORM_NAME		"SamsungTV"
	#define AR_PLATFORM_DESCRIPTION	"Samsung SMART TV on ARM"
	#define AR_PLATFORM_LINUX		1
	#define AR_PLATFORM_UNIX		1
	#define AR_PLATFORM_POSIX		1
	#define AR_PROCESSOR_ARM32		1
	#define AR_PROCESSOR_ARM7		1
	#define AR_ASM_STYLE_ATT		1
	#define AR_ABI_ARM_LINUX		1	// a.k.a. "ARM eabi"


#elif defined(__APPLE__) && __APPLE__
	#include <TargetConditionals.h>

	/* Apple family of operating systems */
	#define AR_PLATFORM_APPLE 1
	#define AR_PLATFORM_POSIX 1
	
	/* iPhone */
	/* TARGET_OS_IPHONE will be undefined on an unknown compiler, and will be defined on gcc. */
	#if defined(AR_PLATFORM_IPHONE) || defined(__IPHONE__) || (defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE) || (defined(TARGET_IPHONE_SIMULATOR) && TARGET_IPHONE_SIMULATOR)
		#undef AR_PLATFORM_IPHONE
		#define AR_PLATFORM_IPHONE				1
		#define AR_PLATFORM_NAME				"iPhone"
		#define AR_ASM_STYLE_ATT				1
		#define AR_POSIX_THREADS_AVAILABLE		1
		#if defined(__arm__)
			#define AR_PLATFORM_DESCRIPTION			"iPhone on ARM"
			#define AR_PROCESSOR_ARM32				1
			#define AR_SYSTEM_LITTLE_ENDIAN			1
			#define AR_ABI_ARM_APPLE				1
		#elif defined(__aarch64__) || defined(__AARCH64)
			#define AR_PLATFORM_DESCRIPTION			"iPhone on ARM64"
			#define AR_PROCESSOR_ARM64				1
			#define AR_SYSTEM_LITTLE_ENDIAN			1
			#define AR_ABI_ARM64_APPLE				1
		#elif defined(__i386__)
			#define AR_PLATFORM_IPHONE_SIMULATOR	1
			#define AR_PLATFORM_DESCRIPTION			"iPhone simulator on x86"
			#define AR_PROCESSOR_X86				1
			#define AR_SYSTEM_LITTLE_ENDIAN			1
		#elif defined(__x86_64) || defined(__amd64)
			#define AR_PLATFORM_DESCRIPTION			"iPhone simulator on x64"
			#define AR_PROCESSOR_X86_64				1
			#define AR_SYSTEM_LITTLE_ENDIAN			1
		#else
			#error Unknown processor
		#endif

		#define AR_PLATFORM_MOBILE 1
	
	#elif defined(AR_PLATFORM_OSX) || defined(__MACH__) || (defined(__MSL__) && (__dest_os == __mac_os_x))
		#undef AR_PLATFORM_OSX
		#define AR_PLATFORM_OSX		1
		#define AR_PLATFORM_NAME	"OSX"
		#define AR_PLATFORM_UNIX	1
		#define AR_PLATFORM_POSIX	1

		#if defined(__i386__) || defined(__intel__)
			#define AR_PLATFORM_DESCRIPTION		"OSX on x86"
			#define AR_PROCESSOR_X86			1
			#define AR_SYSTEM_LITTLE_ENDIAN		1
		#elif defined(__x86_64) || defined(__amd64)
			#define AR_PLATFORM_DESCRIPTION		"OSX on ARM"
			#define AR_PROCESSOR_X86_64			1
			#define AR_SYSTEM_LITTLE_ENDIAN		1
		#elif defined(__aarch64__) || defined(__AARCH64)
			#define AR_PLATFORM_DESCRIPTION		"OSX on ARM64"
			#define AR_PROCESSOR_ARM64			1
			#define AR_SYSTEM_LITTLE_ENDIAN		1
			#define AR_ABI_ARM64_APPLE			1
		#elif defined(__POWERPC64__) || defined(__powerpc64__)
			#define AR_PLATFORM_DESCRIPTION		"OSX on PowerPC 64"
			#define AR_PROCESSOR_POWERPC_64		1
			#define AR_PROCESSOR_POWERPC		1
			#define AR_SYSTEM_BIG_ENDIAN		1
		#elif defined(__POWERPC__) || defined(__powerpc__)
			#define AR_PLATFORM_DESCRIPTION		"OSX on PowerPC"
			#define AR_PROCESSOR_POWERPC_32		1
			#define AR_PROCESSOR_POWERPC		1
			#define AR_SYSTEM_BIG_ENDIAN		1
		#else
			#error Unknown processor
		#endif

		#if defined(__GNUC__)
			#define AR_ASM_STYLE_ATT		1
		#else
			#define AR_ASM_STYLE_MOTOROLA	1
		#endif

		#define AR_PLATFORM_DESKTOP			1

	#else
		#error Unknown Apple Platform
	#endif


#elif defined(AR_PLATFORM_LINUX) || (defined(__linux) || defined(__linux__))
	#undef AR_PLATFORM_LINUX
	#define AR_PLATFORM_LINUX	1
	#define AR_PLATFORM_NAME	"Linux"
	#define AR_PLATFORM_UNIX	1
	#define AR_PLATFORM_POSIX	1

	#if defined(__i386__) || defined(__intel__) || defined(_M_IX86)
		#define AR_PLATFORM_DESCRIPTION		"Linux on x86"
		#define AR_PROCESSOR_X86			1
		#define AR_SYSTEM_LITTLE_ENDIAN		1
	#elif defined(__ARM_ARCH_7A__) || defined(__ARM_EABI__)
		#define AR_PLATFORM_DESCRIPTION		"Linux on ARM 6/7 32-bits"
		#define AR_ABI_ARM_LINUX			1
		#define AR_PROCESSOR_ARM32			1
	#elif defined(__aarch64__) || defined(__AARCH64)
		#define AR_PLATFORM_DESCRIPTION		"Linux on ARM64"
		#define AR_PROCESSOR_ARM64			1
	#elif defined(__x86_64__)
		#define AR_PLATFORM_DESCRIPTION		"Linux on x64"
		#define AR_PROCESSOR_X86_64			1
		#define AR_SYSTEM_LITTLE_ENDIAN		1
	#elif defined(__powerpc64__)
		#define AR_PLATFORM_DESCRIPTION		"Linux on PowerPC 64"
		#define AR_PROCESSOR_POWERPC_64		1
		#define AR_PROCESSOR_POWERPC		1
		#define AR_SYSTEM_BIG_ENDIAN		1
	#elif defined(__powerpc__)
		#define AR_PLATFORM_DESCRIPTION		"Linux on PowerPC"
		#define AR_PROCESSOR_POWERPC_32		1
		#define AR_PROCESSOR_POWERPC		1
		#define AR_SYSTEM_BIG_ENDIAN		1
	#else
		#error Unknown processor
		#error Unknown endianness
	#endif

	#if defined(__GNUC__)
		#define AR_ASM_STYLE_ATT	1
	#endif

	#define AR_PLATFORM_DESKTOP		1


#elif defined(AR_PLATFORM_BSD) || (defined(__BSD__) || defined(__FreeBSD__))
	#undef  AR_PLATFORM_BSD
	#define AR_PLATFORM_BSD		1
	#define AR_PLATFORM_NAME	"BSD Unix"
	#define AR_PLATFORM_UNIX	1
	#define AR_PLATFORM_POSIX	1	// BSD's posix compliance is not identical to Linux's

	#if defined(__i386__) || defined(__intel__)
		#define AR_PLATFORM_DESCRIPTION		"BSD on x86"
		#define AR_PROCESSOR_X86			1
		#define AR_SYSTEM_LITTLE_ENDIAN		1
	#elif defined(__x86_64__)
		#define AR_PLATFORM_DESCRIPTION		"BSD on x64"
		#define AR_PROCESSOR_X86_64			1
		#define AR_SYSTEM_LITTLE_ENDIAN		1
	#elif defined(__powerpc64__)
		#define AR_PLATFORM_DESCRIPTION		"BSD on PowerPC 64"
		#define AR_PROCESSOR_POWERPC_64		1
		#define AR_PROCESSOR_POWERPC		1
		#define AR_SYSTEM_BIG_ENDIAN		1
	#elif defined(__powerpc__)
		#define AR_PLATFORM_DESCRIPTION		"BSD on PowerPC"
		#define AR_PROCESSOR_POWERPC_32		1
		#define AR_PROCESSOR_POWERPC		1
		#define AR_SYSTEM_BIG_ENDIAN		1
	#else
		#error Unknown processor
		#error Unknown endianness
	#endif

	#if !defined(AR_PLATFORM_FREEBSD) && defined(__FreeBSD__)
		#define AR_PLATFORM_FREEBSD		1	// This is a variation of BSD.
	#endif

	#if defined(__GNUC__)
		#define AR_ASM_STYLE_ATT		1
	#endif

	#define AR_PLATFORM_DESKTOP			1


#elif defined(AR_PLATFORM_WINDOWS_PHONE)
	#undef AR_PLATFORM_WINDOWS_PHONE
	#define AR_PLATFORM_WINDOWS_PHONE		1
	#define AR_PLATFORM_NAME				"Windows Phone"

	#if defined(_M_AMD64) || defined(_AMD64_) || defined(__x86_64__)
		#define AR_PLATFORM_DESCRIPTION		"Windows Phone on x64"
		#define AR_PROCESSOR_X86_64			1
		#define AR_SYSTEM_LITTLE_ENDIAN		1
	#elif defined(_M_IX86) || defined(_X86_)
		#define AR_PLATFORM_DESCRIPTION		"Windows Phone on X86"
		#define AR_PROCESSOR_X86			1
		#define AR_SYSTEM_LITTLE_ENDIAN		1
	#elif defined(_M_ARM)
		#define AR_PLATFORM_DESCRIPTION		"Windows Phone on ARM"
		#define AR_PROCESSOR_ARM32			1
		#define AR_SYSTEM_LITTLE_ENDIAN		1
		#define AR_ABI_ARM_WINCE			1
	#else //Possibly other Windows Phone variants
		#error Unknown processor
		#error Unknown endianness
	#endif

	#define AR_PLATFORM_MICROSOFT			1
	
	// WINAPI_FAMILY defines - mirrored from winapifamily.h
	#define AR_WINAPI_FAMILY_APP			1
	#define AR_WINAPI_FAMILY_DESKTOP_APP	2
	#define AR_WINAPI_FAMILY_PHONE_APP		3
	
	#if defined(WINAPI_FAMILY)
		#include <winapifamily.h>
		#if WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP
			#define AR_WINAPI_FAMILY AR_WINAPI_FAMILY_PHONE_APP
		#else
			#error Unsupported WINAPI_FAMILY for Windows Phone
		#endif
	#else
		#error WINAPI_FAMILY should always be defined on Windows Phone.
	#endif
	
	// Macro to determine if a partition is enabled.
	#define AR_WINAPI_FAMILY_PARTITION(Partition)	(Partition)
	
	// Enable the appropriate partitions for the current family
	#if AR_WINAPI_FAMILY == AR_WINAPI_FAMILY_PHONE_APP
		#define AR_WINAPI_PARTITION_CORE	1
		#define AR_WINAPI_PARTITION_PHONE	1
		#define AR_WINAPI_PARTITION_APP		1
	#else
		#error Unsupported WINAPI_FAMILY for Windows Phone
	#endif


#elif (defined(AR_PLATFORM_WINDOW) || (defined(_WIN32) || defined(__WIN32__) || defined(_WIN64))) && !defined(CS_UNDEFINED_STRING)
	#undef AR_PLATFORM_WINDOWS
	#define AR_PLATFORM_WINDOWS		1
	#define AR_PLATFORM_NAME		"Windows"

	#ifdef _WIN64	// VC++ defines both _WIN32 and _WIN64 when compiling for Win64
		#define AR_PLATFORM_WIN64	1
	#else
		#define AR_PLATFORM_WIN32	1
	#endif

	#if defined(_M_AMD64) || defined(_AMD64_) || defined(__x86_64__)
		#define AR_PLATFORM_DESCRIPTION		"Windows on x64"
		#define AR_PROCESSOR_X86_64			1
		#define AR_SYSTEM_LITTLE_ENDIAN		1
	#elif defined(_M_IX86) || defined(_X86_)
		#define AR_PLATFORM_DESCRIPTION		"Windows on x86"
		#define AR_PROCESSOR_X86			1
		#define AR_SYSTEM_LITTLE_ENDIAN		1
	#elif defined(_M_IA64) || defined(_IA64_)
		#define AR_PLATFORM_DESCRIPTION		"Windows on IA-64"
		#define AR_PROCESSOR_IA64			1
		#define AR_SYSTEM_LITTLE_ENDIAN		1
	#elif defined(_M_ARM)
		#define AR_PLATFORM_DESCRIPTION		"Windows on ARM"
		#define AR_PROCESSOR_ARM32			1
		#define AR_SYSTEM_LITTLE_ENDIAN		1
		#define AR_ABI_ARM_WINCE			1
	#elif defined(_M_ARM64)
		#define AR_PLATFORM_DESCRIPTION		"Windows on ARM64"
		#define AR_PROCESSOR_ARM64			1
		#define AR_SYSTEM_LITTLE_ENDIAN		1
	#else // Possibly other Windows CE variants
		#error Unknown processor
		#error Unknown endianness
	#endif

	#if defined(__GNUC__)
		#define AR_ASM_STYLE_ATT			1
	#elif defined(_MSC_VER) || defined(__BORLANDC__) || defined(__ICL)
		#define AR_ASM_STYLE_INTEL			1
	#endif

	#define AR_PLATFORM_DESKTOP				1
	#define AR_PLATFORM_MICROSOFT			1
	
	#if defined(_KERNEL_MODE)
		#define AR_PLATFORM_WINDOWS_KERNEL	1
	#endif
	
	/* WINAPI_FAMILY defines to support Windows 8 Metro Apps - mirroring winapifamily.h in the Windows 8 SDK */
	#define AR_WINAPI_FAMILY_APP			1000
	#define AR_WINAPI_FAMILY_DESKTOP_APP	1001
	#define AR_WINAPI_FAMILY_GAMES			1006
	
	#if defined(WINAPI_FAMILY)
		#if defined(_MSC_VER)
			#pragma warning(push, 0)
		#endif
		#include <winapifamily.h>
		#if defined(_MSC_VER)
			#pragma warning(pop)
		#endif
		#if defined(WINAPI_FAMILY_DESKTOP_APP) && WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP
			#define AR_WINAPI_FAMILY AR_WINAPI_FAMILY_DESKTOP_APP
		#elif defined(WINAPI_FAMILY_APP) && WINAPI_FAMILY == WINAPI_FAMILY_APP
			#define AR_WINAPI_FAMILY AR_WINAPI_FAMILY_APP
		#elif defined(WINAPI_FAMILY_GAMES) && WINAPI_FAMILY == WINAPI_FAMILY_GAMES
			#define AR_WINAPI_FAMILY AR_WINAPI_FAMILY_GAMES
		#else
			#error "Unsupported WINAPI_FAMILY"
		#endif
	#else
		#define AR_WINAPI_FAMILY AR_WINAPI_FAMILY_DESKTOP_APP
	#endif
	
	#define AR_WINAPI_PARTITION_DESKTOP	1
	#define AR_WINAPI_PARTITION_APP		1
	#define AR_WINAPI_PARTITION_GAMES	(AR_WINAPI_FAMILY == AR_WINAPI_FAMILY_GAMES)
	
	#define AR_WINAPI_FAMILY_PARTITION(Partition)	(Partition)

	#if defined(__cplusplus_winrt)
		#define AR_PLATFORM_WINRT
	#endif

/**
 * Sun (Solaris)
 * __SUNPRO_CC is defined by the Sun compiler.!!
 * __sun is defined by the GCC compiler.
 * __i386 is defined by the Sun And GCC compilers.
 * __sparc is defined by the Sun and GCC compilers.
 */
#else
	#error Unknown platform
	#error Unknown processor
	#error Unknown endianness
#endif

#ifndef AR_PROCESSOR_ARM
	#if defined(AR_PROCESSOR_ARM32) || defined(AR_PROCESSOR_ARM64) || defined(AR_PROCESSOR_ARM7)
		#define AR_PROCESSOR_ARM
	#endif
#endif
/******************************************************/
/*              End of platform detection             */
/******************************************************/

#endif // ARES_PLATFORM_H