/**
 *
 */
#ifndef AK_CLOCK_POSIX_H_INCLUDED
#define AK_CLOCK_POSIX_H_INCLUDED

#include "common/types.h"
#include <string>
#include <ctime>
#include <boost/date_time/local_time/local_time.hpp>

class CClock
{
   boost::local_time::local_date_time iTimestamp;

   static bool sTimeZoneInitialized;
   static boost::local_time::time_zone_ptr sTimeZone;

   static void InitTimeZone();

   static boost::local_time::local_date_time GetCurrentTime()
   {
      if (!sTimeZoneInitialized) {
         InitTimeZone();
      }
      return boost::local_time::local_microsec_clock::local_time(sTimeZone);
   }

public:

   CClock() : iTimestamp(GetCurrentTime())
   {
   }

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

   static int64_t GetCurrentTicksUs()
   {
      timespec time;
      if (0 == clock_gettime(CLOCK_MONOTONIC, &time)) {
         return time.tv_sec * 1000000L + time.tv_nsec / 1000;
      } else {
         return 0;
      }
   }
};

#endif // AK_CLOCK_POSIX_H_INCLUDED

