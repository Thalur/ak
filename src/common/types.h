/**
 * Basic type definitions (platform-independend)
 */
#ifndef AK_TYPES_H_INCLUDED
#define AK_TYPES_H_INCLUDED

#define __STDC_FORMAT_MACROS
#include <cinttypes>
#include <cstddef>

/*#if defined(_WIN32)
  #include "windows/types_win.h"
#elif defined(__ANDROID__)
  #include "android/types_and.h"
#elif defined(unix)
  #include "linux/types_lin.h"
#elif defined(__APPLE__)
  #if !defined(TARGET_OS_MAC)
    #include "ios/types_ios.h"
  #else
    #include "mac/types_mac.h"
  #endif
#else
  #error Target operating system definition not found.
#endif*/

typedef int8_t   TInt8;
typedef uint8_t  TUint8;
typedef int16_t  TInt16;
typedef uint16_t TUint16;
typedef int32_t  TInt32;
typedef uint32_t TUint32;
typedef int64_t  TInt64;
typedef uint64_t TUint64;
typedef std::size_t TSize;

// (GOOGLE) printf macros for size_t, in the style of inttypes.h
#ifdef _LP64
#define __PRIS_PREFIX "z"
#else
#define __PRIS_PREFIX
#endif

#define PRIdS __PRIS_PREFIX "d"
#define PRIxS __PRIS_PREFIX "x"
#define PRIuS __PRIS_PREFIX "u"
#define PRIXS __PRIS_PREFIX "X"
#define PRIoS __PRIS_PREFIX "o"


#endif // AK_TYPES_H_INCLUDED
