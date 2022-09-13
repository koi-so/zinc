#pragma once

// Architecture
#define ZINC_ARCH_32BIT 0
#define ZINC_ARCH_64BIT 0

// Compiler
#define ZINC_COMPILER_CLANG 0
#define ZINC_COMPILER_CLANG_ANALYZER 0
#define ZINC_COMPILER_CLANG_CL 0
#define ZINC_COMPILER_GCC 0
#define ZINC_COMPILER_MSVC 0

// Endianess
#define ZINC_CPU_ENDIAN_BIG 0
#define ZINC_CPU_ENDIAN_LITTLE 0

// CPU
#define ZINC_CPU_ARM 0
#define ZINC_CPU_JIT 0
#define ZINC_CPU_MIPS 0
#define ZINC_CPU_PPC 0
#define ZINC_CPU_RISCV 0
#define ZINC_CPU_X86 0

// C Runtime
#define ZINC_CRT_BIONIC 0
#define ZINC_CRT_GLIBC 0
#define ZINC_CRT_LIBCXX 0
#define ZINC_CRT_MINGW 0
#define ZINC_CRT_MSVC 0
#define ZINC_CRT_NEWLIB 0

#ifndef ZINC_CRT_MUSL
#define ZINC_CRT_MUSL 0
#endif // ZINC_CRT_MUSL

#ifndef ZINC_CRT_NONE
#define ZINC_CRT_NONE 0
#endif // ZINC_CRT_NONE

// Platform
#define ZINC_PLATFORM_ANDROID 0
#define ZINC_PLATFORM_BSD 0
#define ZINC_PLATFORM_EMSCRIPTEN 0
#define ZINC_PLATFORM_HURD 0
#define ZINC_PLATFORM_IOS 0
#define ZINC_PLATFORM_LINUX 0
#define ZINC_PLATFORM_NX 0
#define ZINC_PLATFORM_OSX 0
#define ZINC_PLATFORM_PS4 0
#define ZINC_PLATFORM_PS5 0
#define ZINC_PLATFORM_RPI 0
#define ZINC_PLATFORM_WINDOWS 0
#define ZINC_PLATFORM_WINRT 0
#define ZINC_PLATFORM_XBOXONE 0

// C11 thread_local, Because we are missing threads.h
#if __STDC_VERSION__ >= 201112L
#define thread_local _Thread_local
#endif

// http://sourceforge.net/apps/mediawiki/predef/index.php?title=Compilers
#if defined(__clang__)
// clang defines __GNUC__ or _MSC_VER
#undef ZINC_COMPILER_CLANG
#define ZINC_COMPILER_CLANG                                                     \
  (__clang_major__ * 10000 + __clang_minor__ * 100 + __clang_patchlevel__)
#if defined(__clang_analyzer__)
#undef ZINC_COMPILER_CLANG_ANALYZER
#define ZINC_COMPILER_CLANG_ANALYZER 1
#endif // defined(__clang_analyzer__)
#if defined(_MSC_VER)
#undef ZINC_COMPILER_MSVC
#define ZINC_COMPILER_MSVC _MSC_VER
#undef ZINC_COMPILER_CLANG_CL
#define ZINC_COMPILER_CLANG_CL ZINC_COMPILER_CLANG
#endif
#elif defined(_MSC_VER)
#undef ZINC_COMPILER_MSVC
#define ZINC_COMPILER_MSVC _MSC_VER
#elif defined(__GNUC__)
#undef ZINC_COMPILER_GCC
#define ZINC_COMPILER_GCC                                                       \
  (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#else
#error "ZINC_COMPILER_* is not defined!"
#endif //

// http://sourceforge.net/apps/mediawiki/predef/index.php?title=Architectures
#if defined(__arm__) || defined(__aarch64__) || defined(_M_ARM)
#undef ZINC_CPU_ARM
#define ZINC_CPU_ARM 1
#define ZINC_CACHE_LINE_SIZE 64
#elif defined(__MIPSEL__) || defined(__mips_isa_rev) || defined(__mips64)
#undef ZINC_CPU_MIPS
#define ZINC_CPU_MIPS 1
#define ZINC_CACHE_LINE_SIZE 64
#elif defined(_M_PPC) || defined(__powerpc__) || defined(__powerpc64__)
#undef ZINC_CPU_PPC
#define ZINC_CPU_PPC 1
#define ZINC_CACHE_LINE_SIZE 128
#elif defined(__riscv) || defined(__riscv__) || defined(RISCVEL)
#undef ZINC_CPU_RISCV
#define ZINC_CPU_RISCV 1
#define ZINC_CACHE_LINE_SIZE 64
#elif defined(_M_IX86) || defined(_M_X64) || defined(__i386__) ||              \
    defined(__x86_64__)
#undef ZINC_CPU_X86
#define ZINC_CPU_X86 1
#define ZINC_CACHE_LINE_SIZE 64
#else // PNaCl doesn't have CPU defined.
#undef ZINC_CPU_JIT
#define ZINC_CPU_JIT 1
#define ZINC_CACHE_LINE_SIZE 64
#endif //

#if defined(__x86_64__) || defined(_M_X64) || defined(__aarch64__) ||          \
    defined(__64BIT__) || defined(__mips64) || defined(__powerpc64__) ||       \
    defined(__ppc64__) || defined(__LP64__)
#undef ZINC_ARCH_64BIT
#define ZINC_ARCH_64BIT 64
#else
#undef ZINC_ARCH_32BIT
#define ZINC_ARCH_32BIT 32
#endif //

// windef.h error
#if defined(ZINC_COMPILER_MSVC)
#if ZINC_ARCH_64BIT == 64 && ZINC_CPU_X86
#define _AMD64_ 1
#endif
#endif

#if ZINC_CPU_PPC
#undef ZINC_CPU_ENDIAN_BIG
#define ZINC_CPU_ENDIAN_BIG 1
#else
#undef ZINC_CPU_ENDIAN_LITTLE
#define ZINC_CPU_ENDIAN_LITTLE 1
#endif // ZINC_PLATFORM_

// http://sourceforge.net/apps/mediawiki/predef/index.php?title=Operating_Systems
#if defined(_DURANGO) || defined(_XBOX_ONE)
#undef ZINC_PLATFORM_XBOXONE
#define ZINC_PLATFORM_XBOXONE 1
#elif defined(__ANDROID__) || defined(ANDROID)
// Android compiler defines __linux__
#include <sys/cdefs.h> // Defines __BIONIC__ and includes android/api-level.h
#undef ZINC_PLATFORM_ANDROID
#define ZINC_PLATFORM_ANDROID __ANDROID_API__
#elif defined(_WIN32) || defined(_WIN64)
// http://msdn.microsoft.com/en-us/library/6sehtctf.aspx
#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX
//  If _USING_V110_SDK71_ is defined it means we are using the v110_xp or
//  v120_xp toolset.
#if defined(_MSC_VER) && (_MSC_VER >= 1700) && (!_USING_V110_SDK71_)
#include <winapifamily.h>
#endif // defined(_MSC_VER) && (_MSC_VER >= 1700) && (!_USING_V110_SDK71_)
#if !defined(WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)
#undef ZINC_PLATFORM_WINDOWS
#if !defined(WINVER) && !defined(_WIN32_WINNT)
#if ZINC_ARCH_64BIT
//				When building 64-bit target Win7 and above.
#define WINVER 0x0601
#define _WIN32_WINNT 0x0601
#else
//				Windows Server 2003 with SP1, Windows XP with
// SP2 and above
#define WINVER 0x0502
#define _WIN32_WINNT 0x0502
#endif // ZINC_ARCH_64BIT
#endif // !defined(WINVER) && !defined(_WIN32_WINNT)
#define ZINC_PLATFORM_WINDOWS _WIN32_WINNT
#else
#undef ZINC_PLATFORM_WINRT
#define ZINC_PLATFORM_WINRT 1
#endif
#elif defined(__VCCOREVER__) || defined(__RPI__)
// RaspberryPi compiler defines __linux__
#undef ZINC_PLATFORM_RPI
#define ZINC_PLATFORM_RPI 1
#elif defined(__linux__)
#undef ZINC_PLATFORM_LINUX
#define ZINC_PLATFORM_LINUX 1
#elif defined(__ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__) ||               \
    defined(__ENVIRONMENT_TV_OS_VERSION_MIN_REQUIRED__)
#undef ZINC_PLATFORM_IOS
#define ZINC_PLATFORM_IOS 1
#elif defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__)
#undef ZINC_PLATFORM_OSX
#define ZINC_PLATFORM_OSX __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__
#elif defined(__EMSCRIPTEN__)
#undef ZINC_PLATFORM_EMSCRIPTEN
#define ZINC_PLATFORM_EMSCRIPTEN 1
#elif defined(__ORBIS__)
#undef ZINC_PLATFORM_PS4
#define ZINC_PLATFORM_PS4 1
#elif defined(__PROSPERO__)
#undef ZINC_PLATFORM_PS5
#define ZINC_PLATFORM_PS5 1
#elif defined(__FreeBSD__) || defined(__FreeBSD_kernel__) ||                   \
    defined(__NetBSD__) || defined(__OpenBSD__) || defined(__DragonFly__)
#undef ZINC_PLATFORM_BSD
#define ZINC_PLATFORM_BSD 1
#elif defined(__GNU__)
#undef ZINC_PLATFORM_HURD
#define ZINC_PLATFORM_HURD 1
#elif defined(__NX__)
#undef ZINC_PLATFORM_NX
#define ZINC_PLATFORM_NX 1
#endif //

#if !ZINC_CRT_NONE
// https://sourceforge.net/p/predef/wiki/Libraries/
#if defined(__BIONIC__)
#undef ZINC_CRT_BIONIC
#define ZINC_CRT_BIONIC 1
#elif defined(_MSC_VER)
#undef ZINC_CRT_MSVC
#define ZINC_CRT_MSVC 1
#elif defined(__GLIBC__)
#undef ZINC_CRT_GLIBC
#define ZINC_CRT_GLIBC (__GLIBC__ * 10000 + __GLIBC_MINOR__ * 100)
#elif defined(__MINGW32__) || defined(__MINGW64__)
#undef ZINC_CRT_MINGW
#define ZINC_CRT_MINGW 1
#elif defined(__apple_build_version__) || defined(__ORBIS__) ||                \
    defined(__EMSCRIPTEN__) || defined(__llvm__)
#undef ZINC_CRT_LIBCXX
#define ZINC_CRT_LIBCXX 1
#endif //

#if !ZINC_CRT_BIONIC && !ZINC_CRT_GLIBC && !ZINC_CRT_LIBCXX && !ZINC_CRT_MINGW &&  \
    !ZINC_CRT_MSVC && !ZINC_CRT_MUSL && !ZINC_CRT_NEWLIB
#undef ZINC_CRT_NONE
#define ZINC_CRT_NONE 1
#endif // ZINC_CRT_*
#endif // !ZINC_CRT_NONE

#define ZINC_PLATFORM_POSIX                                                     \
  (0 || ZINC_PLATFORM_ANDROID || ZINC_PLATFORM_BSD || ZINC_PLATFORM_EMSCRIPTEN || \
   ZINC_PLATFORM_HURD || ZINC_PLATFORM_IOS || ZINC_PLATFORM_LINUX ||              \
   ZINC_PLATFORM_NX || ZINC_PLATFORM_OSX || ZINC_PLATFORM_PS4 ||                  \
   OI_PLATFORM_PS5 || ZINC_PLATFORM_RPI)

#define ZINC_PLATFORM_NONE                                                      \
  !(0 || ZINC_PLATFORM_ANDROID || ZINC_PLATFORM_BSD ||                           \
    ZINC_PLATFORM_EMSCRIPTEN || ZINC_PLATFORM_HURD || ZINC_PLATFORM_IOS ||        \
    ZINC_PLATFORM_LINUX || ZINC_PLATFORM_NX || ZINC_PLATFORM_OSX ||               \
    ZINC_PLATFORM_PS4 || OI_PLATFORM_PS5 || ZINC_PLATFORM_RPI ||                 \
    ZINC_PLATFORM_WINDOWS || ZINC_PLATFORM_WINRT || ZINC_PLATFORM_XBOXONE)

#if ZINC_COMPILER_GCC
#define ZINC_COMPILER_NAME                                                      \
  "GCC " koi_stringize(__GNUC__) "." koi_stringize(                            \
      __GNUC_MINOR__) "." koi_stringize(__GNUC_PATCHLEVEL__)
#elif ZINC_COMPILER_CLANG
#define ZINC_COMPILER_NAME                                                      \
  "Clang " koi_stringize(__clang_major__) "." koi_stringize(                   \
      __clang_minor__) "." koi_stringize(__clang_patchlevel__)
#elif ZINC_COMPILER_MSVC
#if ZINC_COMPILER_MSVC >= 1920 // Visual Studio 2019
#define ZINC_COMPILER_NAME "MSVC 16.0"
#elif ZINC_COMPILER_MSVC >= 1910 // Visual Studio 2017
#define ZINC_COMPILER_NAME "MSVC 15.0"
#elif ZINC_COMPILER_MSVC >= 1900 // Visual Studio 2015
#define ZINC_COMPILER_NAME "MSVC 14.0"
#elif ZINC_COMPILER_MSVC >= 1800 // Visual Studio 2013
#define ZINC_COMPILER_NAME "MSVC 12.0"
#elif ZINC_COMPILER_MSVC >= 1700 // Visual Studio 2012
#define ZINC_COMPILER_NAME "MSVC 11.0"
#elif ZINC_COMPILER_MSVC >= 1600 // Visual Studio 2010
#define ZINC_COMPILER_NAME "MSVC 10.0"
#elif ZINC_COMPILER_MSVC >= 1500 // Visual Studio 2008
#define ZINC_COMPILER_NAME "MSVC 9.0"
#else
#define ZINC_COMPILER_NAME "MSVC"
#endif //
#endif // ZINC_COMPILER_

#if ZINC_PLATFORM_ANDROID
#define ZINC_PLATFORM_NAME "Android " koi_stringize(ZINC_PLATFORM_ANDROID)
#elif ZINC_PLATFORM_BSD
#define ZINC_PLATFORM_NAME "BSD"
#elif ZINC_PLATFORM_EMSCRIPTEN
#define ZINC_PLATFORM_NAME                                                      \
  "asm.js " koi_stringize(__EMSCRIPTEN_major__) "." koi_stringize(             \
      __EMSCRIPTEN_minor__) "." koi_stringize(__EMSCRIPTEN_tiny__)
#elif ZINC_PLATFORM_HURD
#define ZINC_PLATFORM_NAME "Hurd"
#elif ZINC_PLATFORM_IOS
#define ZINC_PLATFORM_NAME "iOS"
#elif ZINC_PLATFORM_LINUX
#define ZINC_PLATFORM_NAME "Linux"
#elif ZINC_PLATFORM_NONE
#define ZINC_PLATFORM_NAME "None"
#elif ZINC_PLATFORM_NX
#define ZINC_PLATFORM_NAME "NX"
#elif ZINC_PLATFORM_OSX
#define ZINC_PLATFORM_NAME "OSX"
#elif ZINC_PLATFORM_PS4
#define ZINC_PLATFORM_NAME "PlayStation 4"
#elif ZINC_PLATFORM_PS5
#define ZINC_PLATFORM_NAME "PlayStation 5"
#elif ZINC_PLATFORM_RPI
#define ZINC_PLATFORM_NAME "RaspberryPi"
#elif ZINC_PLATFORM_WINDOWS
#define ZINC_PLATFORM_NAME "Windows"
#elif ZINC_PLATFORM_WINRT
#define ZINC_PLATFORM_NAME "WinRT"
#elif ZINC_PLATFORM_XBOXONE
#define ZINC_PLATFORM_NAME "Xbox One"
#else
#error "Unknown ZINC_PLATFORM!"
#endif // ZINC_PLATFORM_

#define ZINC_PLATFORM_APPLE (0 || ZINC_PLATFORM_IOS || ZINC_PLATFORM_OSX)

#if ZINC_CPU_ARM
#define ZINC_CPU_NAME "ARM"
#elif ZINC_CPU_JIT
#define ZINC_CPU_NAME "JIT-VM"
#elif ZINC_CPU_MIPS
#define ZINC_CPU_NAME "MIPS"
#elif ZINC_CPU_PPC
#define ZINC_CPU_NAME "PowerPC"
#elif ZINC_CPU_RISCV
#define ZINC_CPU_NAME "RISC-V"
#elif ZINC_CPU_X86
#define ZINC_CPU_NAME "x86"
#endif // ZINC_CPU_

#if ZINC_CRT_BIONIC
#define ZINC_CRT_NAME "Bionic libc"
#elif ZINC_CRT_GLIBC
#define ZINC_CRT_NAME "GNU C Library"
#elif ZINC_CRT_MSVC
#define ZINC_CRT_NAME "MSVC C Runtime"
#elif ZINC_CRT_MINGW
#define ZINC_CRT_NAME "MinGW C Runtime"
#elif ZINC_CRT_LIBCXX
#define ZINC_CRT_NAME "Clang C Library"
#elif ZINC_CRT_NEWLIB
#define ZINC_CRT_NAME "Newlib"
#elif ZINC_CRT_MUSL
#define ZINC_CRT_NAME "musl libc"
#elif ZINC_CRT_NONE
#define ZINC_CRT_NAME "None"
#else
#error "Unknown ZINC_CRT!"
#endif // ZINC_CRT_

#if ZINC_ARCH_32BIT
#define ZINC_ARCH_NAME "32-bit"
#elif ZINC_ARCH_64BIT
#define ZINC_ARCH_NAME "64-bit"
#endif // ZINC_ARCH_

#define ZINC_PLATFORM_MOBILE (ZINC_PLATFORM_ANDROID || ZINC_PLATFORM_IOS)
#define ZINC_PLATFORM_PC                                                        \
  (ZINC_PLATFORM_WINDOWS || ZINC_PLATFORM_LINUX || ZINC_PLATFORM_OSX)

#ifndef ZINC_CONFIG_SHARED_LIB
#define ZINC_CONFIG_SHARED_LIB 0
#endif

// Debug is forced to be off, so we undef _DEBUG if it's already defined
#if defined(ZINC_DEBUG) && !ZINC_DEBUG
#ifdef _DEBUG
#undef _DEBUG
#endif
#endif