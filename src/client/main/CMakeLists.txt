project (Client.Main)

include_directories ("../../../lib/freeglut/include")
include_directories ("../../../lib/glew-1.12.0/include")

IF ("${AK_TARGET}" STREQUAL "ANDROID")

add_library(Client.Main android_native_app_glue.c appAndroid.cpp NativeAndroid.cpp)

ELSEIF ("${AK_TARGET}" STREQUAL "IOS")

message(FATAL_ERROR "IOS main is not implemented yet")

ELSE()

add_library(Client.Main appPosix.cpp NativePosix.cpp)

ENDIF()
