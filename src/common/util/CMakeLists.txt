project (Common.Util)

SET (COMMON_UTIL_FILES
    clock.cpp
    clock.h
    typeconversions.cpp
    typeconversions.h
    UTF8String.cpp
    UTF8String.h
)

IF ("${AK_TARGET}" STREQUAL "WIN64")
SET (COMMON_UTIL_FILES ${COMMON_UTIL_FILES}
    clock_windows.h)
ELSE ()
SET (COMMON_UTIL_FILES ${COMMON_UTIL_FILES}
    clock_posix.h)
ENDIF ()

add_library(Common.Util ${COMMON_UTIL_FILES})


IF ("${AK_TARGET}" STREQUAL "WIN64" OR "${AK_TARGET}" STREQUAL "LINUX" OR "${AK_TARGET}" STREQUAL "OSX")
add_executable(Common.Util.Test
    TestUTF8String.cpp)
target_link_libraries(Common.Util.Test Common.Util Common.Log)
ENDIF ()

