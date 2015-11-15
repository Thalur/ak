/**
 * Basic type definitions (platform-independend)
 */
#ifndef AK_TYPES_H_INCLUDED
#define AK_TYPES_H_INCLUDED

#define __STDC_FORMAT_MACROS
#include <cinttypes>
#include <cstddef>
#include <memory> // std::unique_ptr

#if defined(AK_SYSTEM_WINDOWS)
#define AK_PLATFORM_NAME "Windows"
#elif defined(AK_SYSTEM_LINUX)
#define AK_PLATFORM_NAME "Linux"
#elif defined(AK_SYSTEM_ANDROID)
#define AK_PLATFORM_NAME "Android"
#elif defined(AK_SYSTEM_OSX)
#define AK_PLATFORM_NAME "MacOS"
#elif defined(AK_SYSTEM_IOS)
#define AK_PLATFORM_NAME "iOS"
#else
#error Target operating system definition not found.
#endif

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

// C++11 replacement formake_unique
template<typename T, typename... Ts>
std::unique_ptr<T> make_unique(Ts&&... params)
{
   return std::unique_ptr<T>(new T(std::forward<Ts>(params)...));
}


#endif // AK_TYPES_H_INCLUDED
