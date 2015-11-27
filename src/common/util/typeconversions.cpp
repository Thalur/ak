/**
 *
 */
#include "common/util/typeconversions.h"

// Work-around the missing to_string support in the Android NDK
#ifdef AK_SYSTEM_ANDROID
#include <sstream>
namespace {

template <typename T>
std::string to_string(T value)
{
   std::ostringstream os;
   os << value ;
   return os.str();
}

}
#else // AK_SYSTEM_ANDROID
using std::to_string;
#endif // AK_SYSTEM_ANDROID

std::string FormatSize(uint64_t aSize)
{
   using namespace std;
	if (aSize < 10) return "   "+to_string(aSize)+" B ";
	if (aSize < 100) return "  "+to_string(aSize)+" B ";
	if (aSize < 1000) return " "+to_string(aSize)+" B ";
	if (aSize < 1024) return to_string(aSize)+" B ";
	if (aSize < 10*1024) return " "+to_string(aSize/1024)+"."+to_string((aSize%1024)/103)+" KB";
	if (aSize < 100*1024) return to_string(aSize/1024)+"."+to_string((aSize%1024)/103)+" KB";
	if (aSize < 1000*1024) return " "+to_string(aSize/1024)+" KB";
	if (aSize < 1024*1024) return to_string(aSize/1024)+" KB";
	if (aSize < 10*1048576) return " "+to_string(aSize/1048576)+"."+to_string((aSize%1048576)/10486)+" MB";
	if (aSize < 100*1048576) return to_string(aSize/1048576)+"."+to_string((aSize%1048576)/10486)+" MB";
	if (aSize < 1000*1048576) return " "+to_string(aSize/1048576)+" MB";
	return to_string(aSize/1048576)+" MB";
}
