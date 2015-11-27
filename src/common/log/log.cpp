/**
 *
 */
#include "common/log/log.h"
#include "common/util/clock.h"
#include <boost/optional.hpp>
#include <boost/utility/in_place_factory.hpp>
#include <iostream>
#include <fstream>
#include <cstdarg>

#ifdef AK_SYSTEM_ANDROID
#include <android/log.h>
#endif

namespace {

bool loggerInitialized = false;
bool uninitializedLoggerUseReported = false;
bool simplifiedConsoleOutput = false;
ELogLevel fileLogLevel = ELogLevel::EDEBUG;
ELogLevel consoleLogLevel = ELogLevel::EDEBUG;
boost::optional<std::ofstream> logFile;
std::string appName;

const std::string logDebug("DEBUG");
const std::string logInfo("INFO ");
const std::string logWarn("WARN ");
const std::string logError("ERROR");
const std::string logInvalid("<INVALID>");

inline const std::string& GetLevelString(ELogLevel aLogLevel)
{
   switch (aLogLevel)
   {
   case ELogLevel::EDEBUG:
      return logDebug;
   case ELogLevel::EINFO:
      return logInfo;
   case ELogLevel::EWARN:
      return logWarn;
   case ELogLevel::EERR:
      return logError;
   default:
      return logInvalid;
   }
}

inline void WriteLogEntry(std::ostream& aOut, const std::string& aTimeStamp,
                          const std::string& aLogLevel, const std::string& aFile,
                          const std::string& aFunc, int aLine, const char* const aMessage)
{
   aOut << aTimeStamp
        << " "
        << aLogLevel
        << " " 
        << aFile 
        << "(" 
        << aLine 
        << "): " 
        << aFunc 
        << " - " 
        << aMessage 
        << std::endl;
}

} // namespace

namespace NLogging {

void InitLogFile(const std::string& aAppName, ELogLevel aFileLogLevel, ELogLevel aConsoleLogLevel,
                 bool aSimplifiedConsoleOutput)
{
   if (loggerInitialized) {
      LOG_WARN("Ignoring repeated init call");
   } else {
      fileLogLevel = aFileLogLevel;
      consoleLogLevel = aConsoleLogLevel;
      simplifiedConsoleOutput = aSimplifiedConsoleOutput;
      appName = aAppName;
      if (fileLogLevel < ELogLevel::ENONE) {
         std::string filename("log.txt");
         logFile = boost::in_place(filename.c_str(), std::ofstream::out);
         CClock now;
         *logFile << "*** " << appName << " log file started at " << now.GetDateLong() << std::endl;
         *logFile << "*** " AK_PLATFORM_NAME " system detected as "
                  << (sizeof(int*)*8) << " bit" << std::endl;
      }
      loggerInitialized = true;
   }
}

void LogAppend(ELogLevel aLogLevel, const std::string& aFile, const std::string& aFunc,
               int aLine, const std::string& aMessage, ...)
{
   if (!loggerInitialized && !uninitializedLoggerUseReported) {
      uninitializedLoggerUseReported = true;
      std::cout << "WARNING: Logger used without initialization. Call InitLogFile() first!" << std::endl;
   }

   // Assemble the user-generated message
   char buffer[510];
   va_list args;
   va_start(args, aMessage);
#ifdef AK_SYSTEM_WINDOWS
   vsnprintf_s(buffer, 500, _TRUNCATE, aMessage.c_str(), args);
#else
   vsnprintf(buffer, 500, aMessage.c_str(), args);
#endif
   va_end(args);

   CClock now;
   std::string timestamp(now.GetTimeLong());
   if (loggerInitialized && aLogLevel >= fileLogLevel) {
      WriteLogEntry(*logFile, timestamp, GetLevelString(aLogLevel), aFile, aFunc, aLine, buffer);
   }
   if (aLogLevel >= consoleLogLevel) {
#ifdef AK_SYSTEM_ANDROID
         __android_log_print(ANDROID_LOG_DEBUG + static_cast<int32_t>(aLogLevel), appName.c_str(), buffer);
#else
      if (simplifiedConsoleOutput) {
         std::cout << buffer << std::endl;
      } else {
         WriteLogEntry(std::cout, timestamp, GetLevelString(aLogLevel), aFile, aFunc, aLine, buffer);
      }
#endif
   }
}

} // namespace NLogging
