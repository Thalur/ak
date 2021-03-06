# Build with: build.py (in build dir)
cmake_minimum_required (VERSION 2.8) # no particular reason for that version
cmake_policy(SET CMP0003 NEW)

# Self-defined variables from build.py
if (NOT DEFINED _AK_PROJECT)
  message(FATAL_ERROR "No project definition found")
elseif (NOT DEFINED _AK_TARGET)
  message(FATAL_ERROR "No target definition found")
elseif (NOT DEFINED _AK_CLIENT)
  message(FATAL_ERROR "No client definition found")
endif()
SET(AK_PROJECT ${_AK_PROJECT})
SET(AK_TARGET ${_AK_TARGET})
SET(AK_CLIENT ${_AK_CLIENT})

# C++ compiler settings
if ("${AK_TARGET}" STREQUAL "WIN64")
  add_definitions(-DAK_SYSTEM_WINDOWS)
  #set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /wd9025")
  STRING(REGEX REPLACE "/W3" "/W4" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
elseif ("${AK_TARGET}" STREQUAL "LINUX")
  add_definitions(-DAK_SYSTEM_LINUX)
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra -Wzero-as-null-pointer-constant")
elseif ("${AK_TARGET}" STREQUAL "ANDROID")
  add_definitions(-DAK_SYSTEM_ANDROID)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -ffor-scope -pipe -ffunction-sections -fdata-sections -ffast-math -Wnon-virtual-dtor -Wreorder -Wsign-promo -fvisibility=hidden -fvisibility-inlines-hidden -Wstrict-null-sentinel -Os -funroll-all-loops -fpeel-loops -ftree-vectorize")
  set(LINKER_FLAGS "${LINKER_FLAGS} -Wl,--as-needed -Wl,--gc-sections -Wl,--no-undefined -Wl,--strip-all -Wl,-rpath-link=${ANDROID_NDK_SYSROOT}/usr/lib/ -L${ANDROID_NDK_SYSROOT}/usr/lib/")
elseif ("${AK_TARGET}" STREQUAL "OSX")
  add_definitions(-DAK_SYSTEM_OSX)
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra")
elseif ("${AK_TARGET}" STREQUAL "IOS")
  add_definitions(-DAK_SYSTEM_IOS)
  message(FATAL_ERROR "iOS build is not supported yet")
else()
  message(FATAL_ERROR "Invalid system specified")
endif()
  
# Global include paths
include_directories ("./")
include_directories ("../lib/boost_1_59_0")
include_directories ("../lib/lpng1619")
include_directories ("../lib/zlib-1.2.5/include")

# Boost settings
add_definitions (-DBOOST_ALL_NO_LIB)
set (Boost_USE_STATIC_LIBS ON)
set (Boost_USE_MULTITHREADED ON)
set (Boost_USE_STATIC_RUNTIME OFF)
add_subdirectory (../lib/boost_1_59_0 ${CMAKE_CURRENT_BINARY_DIR}/boost)

# zlib and libpng
IF ("${AK_TARGET}" STREQUAL "WIN64")
  set (ZLIB_LIBRARY "C:/git/ak/lib/zlib-1.2.5/lib/libz.lib")
ELSE ()
  set (ZLIB_LIBRARY z)
ENDIF ()
set (ZLIB_INCLUDE_DIR "../lib/zlib-1.2.5/include")
add_subdirectory (../lib/lpng1619 ${CMAKE_CURRENT_BINARY_DIR}/libpng)

# Targets
add_subdirectory(common)
add_subdirectory(projects)
IF ("${AK_CLIENT}" STREQUAL "1")
  add_subdirectory(client)
ENDIF()
