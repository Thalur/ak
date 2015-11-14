/**
 *
 */
#ifndef AK_CLOCK_H_INCLUDED
#define AK_CLOCK_H_INCLUDED

#ifdef AK_SYSTEM_WINDOWS
  #include "clock_windows.h"
#else
  #include "clock_posix.h"
#endif

#endif // AK_CLOCK_H_INCLUDED

