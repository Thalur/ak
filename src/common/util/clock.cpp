/**
 *
 */
#include "common/util/clock.h"
#include "boost/date_time/time_zone_base.hpp"

#ifdef AK_SYSTEM_WINDOWS
bool CClock::sHighPrecisionInitialized = false;
bool CClock::sPerformanceCounterAvailable = false;
int64_t CClock::sTicksPerSecond = 0;
const std::vector<std::string> CClock::kWeekDay { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };

void CClock::InitPrecisionTimer()
{
   LARGE_INTEGER data {};
   sPerformanceCounterAvailable = QueryPerformanceFrequency(&data) != false;
   sTicksPerSecond = data.QuadPart;
   sHighPrecisionInitialized = true;
}

#else // AK_SYSTEM_WINDOWS

bool CClock::sTimeZoneInitialized = false;
boost::local_time::time_zone_ptr CClock::sTimeZone = boost::local_time::time_zone_ptr();

void CClock::InitTimeZone()
{
   boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
   boost::posix_time::ptime utc = boost::posix_time::second_clock::universal_time();
   boost::posix_time::time_duration tz_offset = (now - utc);
   std::stringstream ss;
   boost::local_time::local_time_facet* output_facet = new boost::local_time::local_time_facet();
   ss.imbue(std::locale(std::locale::classic(), output_facet));
   output_facet->format("%H:%M:%S");
   ss.str("");
   ss << tz_offset;
   boost::local_time::time_zone_ptr zone(new boost::local_time::posix_time_zone(ss.str().c_str()));
   sTimeZone = zone;
   sTimeZoneInitialized = true;
}

#endif // AK_SYSTEM_WINDOWS

