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

namespace {

bool loggerInitialized = false;
bool uninitializedLoggerUseReported = false;
boost::optional<std::ofstream> logFile;

const std::string logDebug("DEBUG");
const std::string logInfo("INFO ");
const std::string logWarn("WARN ");
const std::string logError("ERROR");
const std::string logInvalid("<INVALID>");

inline const std::string& GetLevelString(TInt32 aLogLevel)
{
   switch (aLogLevel)
   {
   case 0:
      return logDebug;
   case 1:
      return logInfo;
   case 2:
      return logWarn;
   case 3:
      return logError;
   default:
      return logInvalid;
   }
}

inline void WriteLogEntry(std::ostream& aOut, std::string aTimeStamp, std::string aLogLevel,
                   std::string aFile, std::string aFunc, int aLine, char* aMessage)
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

}

namespace NLogging {

void InitLogFile(std::string aAppName)
{
   if (loggerInitialized) {
      LOG_WARN("Ignoring additional init call.");
   } else {
      std::string filename("log.txt");
      logFile = boost::in_place(filename.c_str(), std::ofstream::out);
      CClock now;
      *logFile << "*** " << aAppName << " log file started at " << now.GetDateLong() << std::endl;
      *logFile << "*** " PLATFORM_NAME " system detected as "
               << (sizeof(int*)*8) << " bit (max integral: " 
               << _INTEGRAL_MAX_BITS << " bit, size_t: " << (sizeof(TSize)*8) << ")" << std::endl;
      loggerInitialized = true;
   }
}

void LogAppend(TInt32 aLogLevel, std::string aFile, std::string aFunc, int aLine, std::string aMessage, ...)
{
   if (!loggerInitialized && !uninitializedLoggerUseReported) {
      uninitializedLoggerUseReported = true;
      std::cout << "WARNING: Logger used without initialization. Call InitLogFile() first!" << std::endl;
   }

   // Assemble the user-generated message
   char buffer[510];
   va_list args;
   va_start(args, aMessage);
   vsnprintf_s(buffer, 510, _TRUNCATE, aMessage.c_str(), args);
   va_end(args);

   CClock now;
   std::string timestamp(now.GetTimeLong());
   if (loggerInitialized) {
      WriteLogEntry(*logFile, timestamp, GetLevelString(aLogLevel), aFile, aFunc, aLine, buffer);
   }
   WriteLogEntry(std::cout, timestamp, GetLevelString(aLogLevel), aFile, aFunc, aLine, buffer);
}

} // namespace NLogging
