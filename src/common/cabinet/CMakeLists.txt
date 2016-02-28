project (Common.Cabinet)

include_directories ("../../../lib/zlib-1.2.5/include")

SET (COMMON_CABINET_FILES
    Cabinet.cpp
    Cabinet.h
    CabManager.cpp
    CabManager.h
    CreateCabinet.cpp
    CreateCabinet.h
    File.cpp
    File.h
    JavaRandom.cpp
    JavaRandom.h
    MemoryFile.cpp
    MemoryFile.h
    PosixFile.cpp
    PosixFile.h)

IF ("${AK_TARGET}" STREQUAL "ANDROID")
SET (COMMON_CABINET_FILES ${COMMON_CABINET_FILES}
    AndroidAssetFile.cpp
    AndroidAssetFile.h)
ENDIF ()

add_library(Common.Cabinet ${COMMON_CABINET_FILES})
target_link_libraries(Common.Cabinet Common.Util)
