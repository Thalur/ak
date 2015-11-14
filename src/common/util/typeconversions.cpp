/**
 *
 */
#include "common/util/typeconversions.h"


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
