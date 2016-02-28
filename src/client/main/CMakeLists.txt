project (Client.Main)

include_directories ("../../../lib/freeglut/include")
include_directories ("../../../lib/glew-1.12.0/include")

SET (CLIENT_MAIN_FILES
    IAppInterface.h
    IGameState.h
    INativeInterface.h)

IF ("${AK_TARGET}" STREQUAL "ANDROID")

SET (CLIENT_MAIN_FILES ${CLIENT_MAIN_FILES}
    android_native_app_glue.c
    android_native_app_glue.h
    appAndroid.cpp
    NativeAndroid.cpp
    NativeAndroid.h)

ELSEIF ("${AK_TARGET}" STREQUAL "IOS")

message(FATAL_ERROR "IOS main is not implemented yet")

ELSE()

SET (CLIENT_MAIN_FILES ${CLIENT_MAIN_FILES}
    appPosix.cpp
    NativePosix.cpp
    NativePosix.h)

ENDIF()

add_library(Client.Main ${CLIENT_MAIN_FILES})
