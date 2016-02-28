project (Common.Log)

SET (COMMON_LOG_FILES
    log.cpp
    log.h)

add_library(Common.Log ${COMMON_LOG_FILES})
target_link_libraries (Common.Log Common.Util)
