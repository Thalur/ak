/**
 *
 */
#ifndef AK_CLOCK_H_INCLUDED
#define AK_CLOCK_H_INCLUDED

#ifdef _WIN32
#define UNICODE
#include <windows.h>
#endif

#include "common/types.h"
#include <string>
#include <vector>
#include <stdio.h>

#ifdef _WIN32
class CClock
{
   SYSTEMTIME iTime;

   static bool sHighPrecisionInitialized;
   static bool sPerformanceCounterAvailable;
   static TInt64 sTicksPerSecond;

   static void InitPrecisionTimer();

   static const std::vector<std::string> kWeekDay;

public:
   enum EResolution {
      EResolutionSeconds,
      EResolutionMilliseconds,
      EResolutionMicroseconds,
      EResolutionNanoSeconds
   };

   CClock()
   {
      GetLocalTime(&iTime);
   }

   //CClock(TUint64 aTimestamp, EResolution aResolution) : iTimestamp(aTimestamp), iResolution(aResolution) {}

   std::string GetDateLong() const
   {
      /*std::stringstream ss;
      boost::local_time::local_time_facet* output_facet = new boost::local_time::local_time_facet();
      ss.imbue(std::locale(std::locale::classic(), output_facet));
      output_facet->format("%A, %Y-%m-%d %H:%M:%S.%f%Q");
      ss.str("");
      ss << iTimestamp;
      return ss.str();*/
      char cString[100];
      _snprintf_s(cString, 99, _TRUNCATE, "%s, %04d-%02d-%02d %02d:%02d:%02d.%03d",
                  kWeekDay[iTime.wDayOfWeek].c_str(), iTime.wYear, iTime.wMonth, iTime.wDay,
                  iTime.wHour, iTime.wMinute, iTime.wSecond, iTime.wMilliseconds);
      return std::string(cString);
   }

   std::string GetTimeLong() const
   {
      /*std::stringstream ss;
      boost::local_time::local_time_facet* output_facet = new boost::local_time::local_time_facet();
      ss.imbue(std::locale(std::locale::classic(), output_facet));
      output_facet->format("%H:%M:%s");
      ss.str("");
      ss << iTimestamp;
      return ss.str();*/
      char cString[100];
      _snprintf_s(cString, 99, _TRUNCATE, "%02d:%02d:%02d.%03d",
                  iTime.wHour, iTime.wMinute, iTime.wSecond, iTime.wMilliseconds);
      return std::string(cString);
   }

   static TInt64 GetCurrentTicksUs()
   {
      bool bContinue = false;
      do {
         if (sPerformanceCounterAvailable) {
            LARGE_INTEGER data {};
            if (QueryPerformanceCounter(&data)) {
               if (data.QuadPart > 9000000000000) {
                  return (TInt64)((double)data.QuadPart * 1000000.0 / (double)sTicksPerSecond);
               } else {
                  return data.QuadPart * 1000000 / sTicksPerSecond;
               }
            }
         } else if (!sHighPrecisionInitialized) {
            InitPrecisionTimer();
            bContinue = true;
         }
      } while (bContinue);
      return static_cast<TInt64>(GetTickCount64()) * 1000;
   }
};

#else // ifdef _WIN32
  #error OS not supported yet
#endif


#endif // AK_CLOCK_H_INCLUDED
