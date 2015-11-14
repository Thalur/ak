/**
 * 
 */
#ifndef AK_LOG_H_INCLUDED
#define AK_LOG_H_INCLUDED

#include "common/types.h"
#include <string>

#if defined(_WIN32)

#define PLATFORM_NAME "Windows"
#define LOG_ERROR(s, ...) NLogging::LogAppend(3, (__FILE__), (__FUNCTION__), (__LINE__), s, ##__VA_ARGS__)
#define LOG_WARN(s, ...) NLogging::LogAppend(2, (__FILE__), (__FUNCTION__), (__LINE__), s, ##__VA_ARGS__)
#define LOG_INFO(s, ...) NLogging::LogAppend(1, (__FILE__), (__FUNCTION__), (__LINE__), s, ##__VA_ARGS__)
#ifdef _RELEASE // disable debug logging in release versions
#define LOG_DEBUG(s, ...)
#else
#define LOG_DEBUG(s, ...) NLogging::LogAppend(0, (__FILE__), (__FUNCTION__), (__LINE__), s, ##__VA_ARGS__)
#endif

#elif defined(__ANDROID__)

#define PLATFORM_NAME "Android"
#error Android logging is not implemented yet.

#elif defined(unix)

#define PLATFORM_NAME "Linux"
#error Linux logging is not implemented yet.

#elif defined(__APPLE__)
#if !defined(TARGET_OS_MAC)

#define PLATFORM_NAME "iOS"
#error iOS logging is not implemented yet.

#else

#define PLATFORM_NAME "MacOS"
#error MacOS logging is not implemented yet.

#endif
#else
#error Target operating system definition not found.
#endif

namespace NLogging {

void InitLogFile(std::string aAppName);

void LogAppend(TInt32 aLogLevel, std::string aFile, std::string aFunc, int aLine, std::string aMessage, ...);

void CloseLogFile();

}

#endif // AK_LOG_H_INCLUDED
