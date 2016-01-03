project (OGLtest)

include_directories (".")
include_directories ("../../../lib/freeglut/include")
include_directories ("../../../lib/glew-1.12.0/include")

IF ("${AK_TARGET}" STREQUAL "WIN64")

  message(STATUS "Windows build")
  SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:mainCRTStartup /SUBSYSTEM:windows /MACHINE:X64")
  add_executable (OGLtest mainPosix.cpp OGLTest.cpp multitest1.rc)
  target_link_libraries (OGLtest Client.Main Client.Gfx)
  target_link_libraries (OGLtest Common.Log Common.Util Common.Cabinet akpng)
  target_link_libraries (OGLtest opengl32.lib glu32.lib Boost.Filesystem)
  target_link_libraries (OGLtest C:/git/ak/lib/zlib-1.2.5/lib/libz.lib)
  target_link_libraries (OGLtest C:/git/ak/lib/freeglut/lib/x64/freeglut.lib)
  target_link_libraries (OGLtest C:/git/ak/lib/glew-1.12.0/lib/Release/x64/glew32.lib)

ELSEIF ("${AK_TARGET}" STREQUAL "ANDROID")

  if (ANDROID)
    message(STATUS "Android build")
  else()
    message(ERROR "NOT ANDROID")
  endif()
  add_library (OGLtest SHARED mainAndroid.cpp OGLTest.cpp)
  target_link_libraries (OGLtest Client.Main Client.Gfx)
  target_link_libraries (OGLtest Common.Log Common.Util Common.Cabinet)
  target_link_libraries (OGLtest log android)
  target_link_libraries (OGLtest akpng z EGL GLESv1_CM)
  set_target_properties (OGLtest PROPERTIES COMPILE_DEFINITIONS "ANDROID")

ELSE()

  message(STATUS "Linux/OSX build")
  find_package (OpenGL REQUIRED)
  IF ("${AK_TARGET}" STREQUAL "OSX")
    find_package (GLUT REQUIRED)
  ENDIF ()

  # Add the core resource file into the executable
  IF ("${AK_TARGET}" STREQUAL "LINUX")
    add_custom_command(OUTPUT oglres.o
        COMMAND cd ${CMAKE_CURRENT_SOURCE_DIR}/data && ld -r --format=binary -o ${CMAKE_CURRENT_BINARY_DIR}/oglres.o test.ak
        COMMAND objcopy --rename-section .data=.rodata,alloc,load,readonly,data,contents ${CMAKE_CURRENT_BINARY_DIR}/oglres.o ${CMAKE_CURRENT_BINARY_DIR}/oglres.o)
  ELSE () # OSX
    add_custom_command(OUTPUT oglres.o
        COMMAND rm stub.c && touch stub.c && gcc -o stub.o -c stub.c
        COMMAND ld -r -o oglres.o -sectcreate binary core_resources ${CMAKE_CURRENT_SOURCE_DIR}/data/test.ak stub.o)
  ENDIF ()
  add_library(oglres STATIC oglres.o)
  SET_SOURCE_FILES_PROPERTIES(oglres.o PROPERTIES EXTERNAL_OBJECT true GENERATED true)
  SET_TARGET_PROPERTIES(oglres PROPERTIES LINKER_LANGUAGE C)

  add_executable (OGLtest mainPosix.cpp OGLTest.cpp)
  target_link_libraries (OGLtest oglres Client.Main Client.Gfx)
  target_link_libraries (OGLtest Common.Log Common.Util Common.Cabinet akpng z)
  target_link_libraries (OGLtest ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})
  target_link_libraries (OGLtest Boost.Filesystem)
  IF ("${AK_TARGET}" STREQUAL "LINUX") # not required for OSX
    target_link_libraries (OGLtest "libglut.so")
    target_link_libraries (OGLtest "libGLEW.so")
  ENDIF ()

ENDIF()
