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
#include <boost/date_time/local_time/local_time.hpp>

class CClock
{
   boost::local_time::local_date_time iTimestamp;

   static bool sTimeZoneInitialized;
   static boost::local_time::time_zone_ptr sTimeZone;
   static bool sHighPrecisionInitialized;
   static bool sPerformanceCounterAvailable;
   static TInt64 sTicksPerSecond;


   static void InitTimeZone();
   static void InitPrecisionTimer();

   static boost::local_time::local_date_time GetCurrentTime()
   {
      if (!sTimeZoneInitialized) {
         InitTimeZone();
      }
      return boost::local_time::local_microsec_clock::local_time(sTimeZone);
   }

public:
   enum EResolution {
      EResolutionSeconds,
      EResolutionMilliseconds,
      EResolutionMicroseconds,
      EResolutionNanoSeconds
   };

   CClock() : iTimestamp(GetCurrentTime()) {}

   //CClock(TUint64 aTimestamp, EResolution aResolution) : iTimestamp(aTimestamp), iResolution(aResolution) {}

   std::string GetDateLong() const
   {
      std::stringstream ss;
      boost::local_time::local_time_facet* output_facet = new boost::local_time::local_time_facet();
      ss.imbue(std::locale(std::locale::classic(), output_facet));
      output_facet->format("%A, %Y-%m-%d %H:%M:%S.%f%Q");
      ss.str("");
      ss << iTimestamp;
      return ss.str();
   }

   std::string GetTimeLong() const
   {
      std::stringstream ss;
      boost::local_time::local_time_facet* output_facet = new boost::local_time::local_time_facet();
      ss.imbue(std::locale(std::locale::classic(), output_facet));
      output_facet->format("%H:%M:%s");
      ss.str("");
      ss << iTimestamp;
      return ss.str();
   }

#ifdef _WIN32
   static TInt64 GetCurrentTicksUs()
   {
      do {
         if (sPerformanceCounterAvailable) {
            LARGE_INTEGER data {};
            if (QueryPerformanceCounter(&data)) {
               return data.QuadPart * 100000 / sTicksPerSecond;
            }
         } else if (!sHighPrecisionInitialized) {
            InitPrecisionTimer();
            continue;
         }
      } while (false);
      return static_cast<TInt64>(GetTickCount64()) * 1000;
   }
#else
  #error OS not supported yet
#endif
};

#endif // AK_CLOCK_H_INCLUDED
