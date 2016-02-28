project (Common)

SET (COMMON_FILES
    empty.cpp
    types.h)

add_library(Common ${COMMON_FILES})
target_link_libraries (Common Common.Log Common.Util Common.Cabinet)

add_subdirectory (log)
add_subdirectory (util)
add_subdirectory (cabinet)
