/**
 * 
 */
#ifndef AK_LOG_H_INCLUDED
#define AK_LOG_H_INCLUDED

#include "common/types.h"
#include <string>

enum class ELogLevel : int32_t
{
   EDEBUG = 0,
   EINFO  = 1,
   EWARN  = 2,
   EERR   = 3,
   ENONE  = 4
};

namespace NLogging {

void InitLogFile(const std::string& aAppName, const std::string& aLogFile, ELogLevel aFileLogLevel=ELogLevel::EDEBUG,
                 ELogLevel aConsoleLogLevel=ELogLevel::ENONE, bool aSimplifiedConsoleOutput=true);

void LogAppend(ELogLevel aLogLevel, const char* aFile, const std::string& aFunc, int aLine, const std::string& aMessage, ...);

struct CMethodLogger
{
   const char* iFile;
   const char* iFunction;
   const int iLine;
   CMethodLogger(const char* aFile, const char* aFunction, int aLine)
    : iFile(aFile), iFunction(aFunction), iLine(aLine)
   {
      LogAppend(ELogLevel::EDEBUG, iFile, iFunction, iLine, ">> ENTRY");
   }
   ~CMethodLogger() {
      LogAppend(ELogLevel::EDEBUG, iFile, iFunction, iLine, "<< EXIT");
   }
};

}

#define LOG_ERROR(s, ...) NLogging::LogAppend(ELogLevel::EERR, (__FILE__), (__FUNCTION__), (__LINE__), s, ##__VA_ARGS__)
#define LOG_WARN(s, ...) NLogging::LogAppend(ELogLevel::EWARN, (__FILE__), (__FUNCTION__), (__LINE__), s, ##__VA_ARGS__)
#define LOG_INFO(s, ...) NLogging::LogAppend(ELogLevel::EINFO, (__FILE__), (__FUNCTION__), (__LINE__), s, ##__VA_ARGS__)
#if defined(_RELEASE) || defined(NDEBUG)  // disable debug logging in release versions
#define LOG_DEBUG(s, ...)
#define LOG_METHOD()
#else
#define LOG_DEBUG(s, ...) NLogging::LogAppend(ELogLevel::EDEBUG, (__FILE__), (__FUNCTION__), (__LINE__), s, ##__VA_ARGS__)
#define LOG_METHOD() NLogging::CMethodLogger _method_logger_((__FILE__), (__FUNCTION__), (__LINE__))
#endif


#endif // AK_LOG_H_INCLUDED
