# Build with: build.py (in build dir)
cmake_minimum_required (VERSION 3.3)

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


include_directories ("./")

add_definitions (-DBOOST_ALL_NO_LIB)
set (Boost_USE_STATIC_LIBS ON)
set (Boost_USE_MULTITHREADED ON)
set (Boost_USE_STATIC_RUNTIME OFF)

include_directories ("../lib/boost_1_59_0")
add_subdirectory (../lib/boost_1_59_0 ${CMAKE_CURRENT_BINARY_DIR}/boost)

add_subdirectory(common)
add_subdirectory(projects)
IF ("${AK_CLIENT}" STREQUAL "1")
  add_subdirectory(client)
ENDIF()
