project (AKCab)

if ("${AK_TARGET}" STREQUAL "WIN64")
  SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:mainCRTStartup /SUBSYSTEM:console /MACHINE:X64")
ENDIF()

add_executable (AKCab akcab.cpp)
target_link_libraries (AKCab Common.Cabinet Common.Util Common.Log)
if ("${AK_TARGET}" STREQUAL "WIN64")
  target_link_libraries (AKCab "C:/git/ak/lib/zlib-1.2.5/lib/libz.a")
ELSEIF ("${AK_TARGET}" STREQUAL "OSX")
  target_link_libraries (AKCab "z")
ELSE()
  target_link_libraries (AKCab "libz.so")
ENDIF()
target_link_libraries (AKCab Boost.Filesystem)

