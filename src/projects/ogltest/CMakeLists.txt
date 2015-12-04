project (OGLtest)

include_directories ("../../../lib/freeglut/include")
include_directories ("../../../lib/glew-1.12.0/include")
include_directories ("../../../lib/lpng1619")

IF ("${AK_TARGET}" STREQUAL "WIN64")

  message(STATUS "Windows build")

  include_directories ("../../../lib/zlib-1.2.5/include")
  set (PNG_LIB_TARGETS "PNG_STATIC")
  set (ZLIB_LIBRARY "C:/git/ak/lib/zlib-1.2.5/lib/libz.lib")
  set (ZLIB_INCLUDE_DIR "../../../lib/zlib-1.2.5/include")
  add_subdirectory("../../../lib/lpng1619" ${CMAKE_CURRENT_BINARY_DIR}/libpng)

  SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:mainCRTStartup /SUBSYSTEM:windows /MACHINE:X64")
  add_executable (OGLtest win/winmain.cpp win/wingl.cpp pngLoader.cpp win/multitest1.rc)
  target_link_libraries (OGLtest Common.Log Common.Util Common.Cabinet)
  target_link_libraries (OGLtest opengl32.lib glu32.lib)
  target_link_libraries (OGLtest C:/git/ak/lib/zlib-1.2.5/lib/libz.lib)
  target_link_libraries (OGLtest C:/git/ak/lib/freeglut/lib/x64/freeglut.lib)
  target_link_libraries (OGLtest C:/git/ak/lib/glew-1.12.0/lib/Release/x64/glew32.lib)
  target_link_libraries (OGLtest ${CMAKE_CURRENT_BINARY_DIR}/libpng/Debug/libpng16d.lib)

ELSEIF ("${AK_TARGET}" STREQUAL "ANDROID")

  if (ANDROID)
    message(STATUS "Android build")
  else()
    message(ERROR "NOT ANDROID")
  endif()
  add_library (OGLtest SHARED android/main.cpp android/android_native_app_glue.c android/AndroidApp.cpp)
  target_link_libraries (OGLtest Common.Log Common.Util Common.Cabinet)
  target_link_libraries (OGLtest log android)
  target_link_libraries (OGLtest z EGL GLESv1_CM)
  set_target_properties (OGLtest PROPERTIES COMPILE_DEFINITIONS "ANDROID")

ELSE()

  message(STATUS "Linux build")
  find_package (OpenGL REQUIRED)

  add_executable (OGLtest linux/main.cpp)
  target_link_libraries (OGLtest Common.Log Common.Util Common.Cabinet)
  target_link_libraries (OGLtest ${OPENGL_LIBRARIES})
  target_link_libraries (OGLtest "libz.so")
  target_link_libraries (OGLtest "libglut.so")
  target_link_libraries (OGLtest "libGLEW.so")

ENDIF()
