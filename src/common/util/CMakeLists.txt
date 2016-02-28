project (Common.Util)

SET (COMMON_UTIL_FILES
    clock.cpp
    clock.h
    typeconversions.cpp
    typeconversions.h)

IF ("${AK_TARGET}" STREQUAL "WIN64")
SET (COMMON_UTIL_FILES ${COMMON_UTIL_FILES}
    clock_windows.h)
ELSE ()
SET (COMMON_UTIL_FILES ${COMMON_UTIL_FILES}
    clock_posix.h)
ENDIF ()

add_library(Common.Util ${COMMON_UTIL_FILES})
