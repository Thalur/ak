/**
 * 
 */
#ifndef AK_LOG_H_INCLUDED
#define AK_LOG_H_INCLUDED

#include "common/types.h"
#include <string>

enum class ELogLevel : int32_t
{
   EVERBOSE = 0,
   EDEBUG   = 1,
   EINFO    = 2,
   EWARN    = 3,
   EERR     = 4,
   ENONE    = 5
};

namespace NLogging {

void InitLogFile(const std::string& aAppName, const std::string& aLogFile, ELogLevel aFileLogLevel=ELogLevel::EDEBUG,
                 ELogLevel aConsoleLogLevel=ELogLevel::ENONE, bool aSimplifiedConsoleOutput=true);

void FinishLogger();

void LogAppend(ELogLevel aLogLevel, const char* aFile, const std::string& aFunc, const std::string& aMessage, int aLine, ...);

struct CMethodLogger
{
   const char* iFile;
   const char* iFunction;
   const int iLine;
   CMethodLogger(const char* aFile, const char* aFunction, int aLine)
    : iFile(aFile), iFunction(aFunction), iLine(aLine)
   {
      LogAppend(ELogLevel::EDEBUG, iFile, iFunction, ">> ENTRY", iLine);
   }
   CMethodLogger(const char* aFile, const char* aFunction, const std::string& aMessage, int aLine, ...);
   ~CMethodLogger() {
      LogAppend(ELogLevel::EDEBUG, iFile, iFunction, "<< EXIT", iLine);
   }
   CMethodLogger(const CMethodLogger&) = delete;
   CMethodLogger& operator=(const CMethodLogger&) = delete;
};

}

#define LOG_ERROR(s, ...) NLogging::LogAppend(ELogLevel::EERR, (__FILE__), (__FUNCTION__), s, (__LINE__), ##__VA_ARGS__)
#define LOG_WARN(s, ...) NLogging::LogAppend(ELogLevel::EWARN, (__FILE__), (__FUNCTION__), s, (__LINE__), ##__VA_ARGS__)
#define LOG_INFO(s, ...) NLogging::LogAppend(ELogLevel::EINFO, (__FILE__), (__FUNCTION__), s, (__LINE__), ##__VA_ARGS__)
#if defined(_RELEASE) || defined(NDEBUG)  // disable debug logging in release versions
#define LOG_DEBUG(s, ...)
#define LOG_VERBOSE(s, ...)
#define LOG_METHOD()
#define LOG_PARAMS(s, ...)
#define ASSERT(x)
#else
#define LOG_DEBUG(s, ...) NLogging::LogAppend(ELogLevel::EDEBUG, (__FILE__), (__FUNCTION__), s, (__LINE__), ##__VA_ARGS__)
#define LOG_VERBOSE(s, ...) NLogging::LogAppend(ELogLevel::EVERBOSE, (__FILE__), (__FUNCTION__), s, (__LINE__), ##__VA_ARGS__)
#define LOG_METHOD() NLogging::CMethodLogger _method_logger_((__FILE__), (__FUNCTION__), (__LINE__))
#define LOG_PARAMS(s, ...) NLogging::CMethodLogger _method_logger_((__FILE__), (__FUNCTION__), s, (__LINE__), ##__VA_ARGS__)
#define ASSERT(x) { if (!(x)) LOG_ERROR("ASSERTION FAILURE: %s", #x); }
#endif


#endif // AK_LOG_H_INCLUDED
