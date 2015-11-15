/**
 *
 */
#ifndef AK_CLOCK_WINDOWS_H_INCLUDED
#define AK_CLOCK_WINDOWS_H_INCLUDED

#define UNICODE
#include <windows.h>

#include "common/types.h"
#include <string>
#include <vector>
#include <stdio.h>

class CClock
{
   SYSTEMTIME iTime;

   static bool sHighPrecisionInitialized;
   static bool sPerformanceCounterAvailable;
   static int64_t sTicksPerSecond;

   static void InitPrecisionTimer();

   static const std::vector<std::string> kWeekDay;

public:

   CClock()
   {
      GetLocalTime(&iTime);
   }

   std::string GetDateLong() const
   {
      char cString[100];
      _snprintf_s(cString, 99, _TRUNCATE, "%s, %04d-%02d-%02d %02d:%02d:%02d.%03d",
                  kWeekDay[iTime.wDayOfWeek].c_str(), iTime.wYear, iTime.wMonth, iTime.wDay,
                  iTime.wHour, iTime.wMinute, iTime.wSecond, iTime.wMilliseconds);
      return std::string(cString);
   }

   std::string GetTimeLong() const
   {
      char cString[100];
      _snprintf_s(cString, 99, _TRUNCATE, "%02d:%02d:%02d.%03d",
                  iTime.wHour, iTime.wMinute, iTime.wSecond, iTime.wMilliseconds);
      return std::string(cString);
   }

   static int64_t GetCurrentTicksUs()
   {
      bool bContinue = false;
      do {
         if (sPerformanceCounterAvailable) {
            LARGE_INTEGER data {};
            if (QueryPerformanceCounter(&data)) {
               if (data.QuadPart > 9000000000000) {
                  return (int64_t)((double)data.QuadPart * 1000000.0 / (double)sTicksPerSecond);
               } else {
                  return data.QuadPart * 1000000 / sTicksPerSecond;
               }
            }
         } else if (!sHighPrecisionInitialized) {
            InitPrecisionTimer();
            bContinue = true;
         }
      } while (bContinue);
      return static_cast<int64_t>(GetTickCount64()) * 1000;
   }
};

#endif // AK_CLOCK_WINDOWS_H_INCLUDED

