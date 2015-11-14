project (Common.Cabinet)

include_directories ("../../../lib/zlib-1.2.5/include")

add_library(Common.Cabinet File.cpp MemoryFile.cpp PosixFile.cpp Cabinet.cpp JavaRandom.cpp CabManager.cpp CreateCabinet.cpp)

