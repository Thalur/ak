project (OGLtest)

include_directories ("../../../lib/freeglut/include")
include_directories ("../../../lib/glew-1.12.0/include")

IF ("${AK_TARGET}" STREQUAL "WIN64")

  SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:mainCRTStartup /SUBSYSTEM:windows /MACHINE:X64")
  add_executable (OGLtest win/winmain.cpp win/wingl.cpp win/multitest1.rc)
  target_link_libraries (OGLtest Common.Log Common.Util)
  target_link_libraries (OGLtest opengl32.lib glu32.lib)
# target_link_libraries (OGLtest C:/git/ak/lib/zlib-1.2.5/lib/libz.a)
  target_link_libraries (OGLtest C:/git/ak/lib/freeglut/lib/x64/freeglut.lib)
  target_link_libraries (OGLtest C:/git/ak/lib/glew-1.12.0/lib/Release/x64/glew32.lib)

ELSE()

  find_package (OpenGL REQUIRED)

  add_executable (OGLtest linux/main.cpp)
  target_link_libraries (OGLtest Common.Log Common.Util)
  target_link_libraries (OGLtest ${OPENGL_LIBRARIES})
  target_link_libraries (OGLtest "libglut.so")
  target_link_libraries (OGLtest "libGLEW.so")

ENDIF()
