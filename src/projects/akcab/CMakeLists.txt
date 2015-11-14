project (AKCab)

IF (DEFINED AK_SYSTEM_WINDOWS)
  SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:mainCRTStartup /SUBSYSTEM:console /MACHINE:X64")
ENDIF()

add_executable (AKCab akcab.cpp)
target_link_libraries (AKCab Common.Cabinet Common.Util Common.Log)
IF (DEFINED AK_SYSTEM_WINDOWS)
  target_link_libraries (AKCab "C:/git/ak/lib/zlib-1.2.5/lib/libz.a")
ELSE()
  target_link_libraries (AKCab "libz.so")
ENDIF()
target_link_libraries (AKCab Boost.Filesystem)

