project (Common.Cabinet)

include_directories ("../../../lib/zlib-1.2.5/include")

IF ("${AK_TARGET}" STREQUAL "ANDROID")
add_library(Common.Cabinet File.cpp MemoryFile.cpp PosixFile.cpp AndroidAssetFile.cpp Cabinet.cpp JavaRandom.cpp CabManager.cpp CreateCabinet.cpp)
ELSE ()
add_library(Common.Cabinet File.cpp MemoryFile.cpp PosixFile.cpp Cabinet.cpp JavaRandom.cpp CabManager.cpp CreateCabinet.cpp)
ENDIF ()
target_link_libraries(Common.Cabinet Common.Util)
