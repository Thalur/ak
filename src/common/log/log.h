/**
 * 
 */
#ifndef AK_LOG_H_INCLUDED
#define AK_LOG_H_INCLUDED

#include "common/types.h"
#include <string>

enum class ELogLevel : int32_t
{
   DEBUG = 0,
   INFO  = 1,
   WARN  = 2,
   ERR   = 3,
   NONE  = 4
};

namespace NLogging {

void InitLogFile(const std::string& aAppName, ELogLevel aFileLogLevel=ELogLevel::DEBUG,
                 ELogLevel aConsoleLogLevel=ELogLevel::NONE, bool aSimplifiedConsoleOutput=true);

void LogAppend(ELogLevel aLogLevel, const std::string& aFile, const std::string& aFunc, int aLine, const std::string& aMessage, ...);

struct CMethodLogger
{
   const char* iFile;
   const char* iFunction;
   const int iLine;
   CMethodLogger(const char* aFile, const char* aFunction, int aLine)
    : iFile(aFile), iFunction(aFunction), iLine(aLine)
   {
      LogAppend(ELogLevel::DEBUG, iFile, iFunction, iLine, ">> ENTRY");
   }
   ~CMethodLogger() {
      LogAppend(ELogLevel::DEBUG, iFile, iFunction, iLine, "<< EXIT");
   }
};

}

#define LOG_ERROR(s, ...) NLogging::LogAppend(ELogLevel::ERR, (__FILE__), (__FUNCTION__), (__LINE__), s, ##__VA_ARGS__)
#define LOG_WARN(s, ...) NLogging::LogAppend(ELogLevel::WARN, (__FILE__), (__FUNCTION__), (__LINE__), s, ##__VA_ARGS__)
#define LOG_INFO(s, ...) NLogging::LogAppend(ELogLevel::INFO, (__FILE__), (__FUNCTION__), (__LINE__), s, ##__VA_ARGS__)
#if defined(_RELEASE) || defined(NDEBUG)  // disable debug logging in release versions
#define LOG_DEBUG(s, ...)
#define LOG_METHOD()
#else
#define LOG_DEBUG(s, ...) NLogging::LogAppend(ELogLevel::DEBUG, (__FILE__), (__FUNCTION__), (__LINE__), s, ##__VA_ARGS__)
#define LOG_METHOD() NLogging::CMethodLogger _method_logger_((__FILE__), (__FUNCTION__), (__LINE__))
#endif


#endif // AK_LOG_H_INCLUDED
