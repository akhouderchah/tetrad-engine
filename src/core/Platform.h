#pragma once

#include <cstdint>

#include "Config.h"

namespace tetrad {

extern const std::string kVersionString;

// Run at the beginning of execution to do platform-specific initialization
bool programInitialize();

#define EP_INVALID 0
#define EP_LINUX 1
#define EP_WINDOWS 2
#define EP_MAC_OSX 3

#define EBT_DEBUG 0
#define EBT_RELEASE 1

#define __FILE_RELATIVE__ (__FILE__ + BASE_PATH_SIZE)

#if (BUILD_TYPE == EBT_DEBUG && !defined(NDEBUG))
#undef _DEBUG
#define _DEBUG
#else
#undef _DEBUG
#endif  // BUILD_TYPE == EBT_DEBUG

#if (SYSTEM_TYPE == EP_WINDOWS)
#include <stdlib.h>
#ifdef WIN32
//#include <Windows.h>
#endif  // WIN32
#elif (SYSTEM_TYPE == EP_LINUX)
#include <endian.h>
#elif (SYSTEM_TYPE == EP_MAC_OSX)
// OSX-specific includes
#else
// Platform-indendent alternatives only
#endif

// Define the disabling and enabling of compiler warnings
// #pragma GCC diagnostic added to GCC 4.6
#if (defined(COMPILER_IS_GCC) &&                                  \
     (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6))) || \
    defined(__clang__)
#define DISABLE_WARNINGS()                                      \
  _Pragma("GCC diagnostic push")                                \
  _Pragma("GCC diagnostic ignored \"-Wunused-parameter\"")      \
  _Pragma("GCC diagnostic ignored \"-Wimplicit-fallthrough\"")  \
  _Pragma("GCC diagnostic ignored \"-Wunused-function\"")
#define ENABLE_WARNINGS() _Pragma("GCC diagnostic pop")
#elif defined(COMPILER_IS_MSVC)
#define DISABLE_WARNINGS() __pragma(warning(push, 0))
#define ENABLE_WARNINGS() __pragma(warning(pop))
#else
#define DISABLE_WARNINGS()  // @TODO define for more compilers
#define ENABLE_WARNINGS()   // @TODO define for more compilers
#endif

// Define __method__ as the readable name of a function/method
#if COMPILER_IS_GCC
#define __method__ __PRETTY_FUNCTION__
#elif COMPILER_IS_MSVC
#define __method__ __FUNCTION__
#else
#define __method__ __func__
#endif

// Byte-swap for handling endianness issues
#if COMPILER_IS_GCC && ((__GNUC__ == 4 && __GNUC_MINOR__ >= 3) || __GNUC__ > 4)
#define bswap16 __builtin_bswap16
#define bswap32 __builtin_bswap32
#define bswap64 __builtin_bswap64
#elif COMPILER_IS_MSVC
#define bswap16 _byteswap_ushort
#define bswap32 _byteswap_ulong
#define bswap64 _byteswap_uint64
#else
#define _CUSTOM_BSWAP
uint16_t bswap16(uint16_t value);
uint32_t bswap32(uint32_t value);
uint64_t bswap64(uint64_t value);
#endif

#define bxchg16(x) ((x) = bswap16(x))
#define bxchg32(x) ((x) = bswap32(x))
#define bxchg64(x) ((x) = bswap64(x))

}  // namespace tetrad
