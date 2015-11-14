project (AKCab)

SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:mainCRTStartup /SUBSYSTEM:console /MACHINE:X64")

add_executable (AKCab akcab.cpp)
target_link_libraries (AKCab Common.Log Common.Util Common.Cabinet)
target_link_libraries (AKCab "C:/git/ak/lib/zlib-1.2.5/lib/libz.a")
target_link_libraries (AKCab Boost.Filesystem)
