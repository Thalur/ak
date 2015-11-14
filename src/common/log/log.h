/**
 * 
 */
#ifndef AK_LOG_H_INCLUDED
#define AK_LOG_H_INCLUDED

#include "common/types.h"
#include <string>


//#if defined(AK_SYSTEM_WINDOWS)

#define PLATFORM_NAME "Windows"
#define LOG_ERROR(s, ...) NLogging::LogAppend(3, (__FILE__), (__FUNCTION__), (__LINE__), s, ##__VA_ARGS__)
#define LOG_WARN(s, ...) NLogging::LogAppend(2, (__FILE__), (__FUNCTION__), (__LINE__), s, ##__VA_ARGS__)
#define LOG_INFO(s, ...) NLogging::LogAppend(1, (__FILE__), (__FUNCTION__), (__LINE__), s, ##__VA_ARGS__)
#if defined(_RELEASE) || defined(NDEBUG)  // disable debug logging in release versions
#define LOG_DEBUG(s, ...)
#else
#define LOG_DEBUG(s, ...) NLogging::LogAppend(0, (__FILE__), (__FUNCTION__), (__LINE__), s, ##__VA_ARGS__)
#endif


/*#elif defined(AK_SYSTEM_LINUX)

#define PLATFORM_NAME "Linux"
#error Linux logging is not implemented yet.


#elif defined(AK_SYSTEM_ANDROID)

#define PLATFORM_NAME "Android"
#error Android logging is not implemented yet.


#elif defined(AK_SYSTEM_OSX)

#define PLATFORM_NAME "MacOS"
#error MacOS logging is not implemented yet.


#elif defined(AK_SYSTEM_IOS)

#define PLATFORM_NAME "iOS"
#error iOS logging is not implemented yet.


#else
#error Target operating system definition not found.
#endif*/

namespace NLogging {

void InitLogFile(std::string aAppName);

void LogAppend(int32_t aLogLevel, std::string aFile, std::string aFunc, int aLine, std::string aMessage, ...);

void CloseLogFile();

}

#endif // AK_LOG_H_INCLUDED
