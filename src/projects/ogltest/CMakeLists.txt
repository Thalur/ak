project (OGLtest)

include_directories ("../../../lib/freeglut/include")
include_directories ("../../../lib/glew-1.12.0/include")

SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:mainCRTStartup /SUBSYSTEM:windows /MACHINE:X64")

add_executable (OGLtest winmain.cpp wingl.cpp multitest1.rc)
target_link_libraries (OGLtest Common.Log Common.Util Common.Cabinet)
#target_link_libraries (OGLtest ${BOOST_LIBRARYDIR}/libboost_date_time-vc120-mt-1_58.lib)
target_link_libraries (OGLtest opengl32.lib glu32.lib)
target_link_libraries (OGLtest C:/git/ak/lib/zlib-1.2.5/lib/libz.a)
target_link_libraries (OGLtest C:/git/ak/lib/freeglut/lib/x64/freeglut.lib)
target_link_libraries (OGLtest C:/git/ak/lib/glew-1.12.0/lib/Release/x64/glew32.lib)
#target_link_libraries (OGLtest ${Boost_LIBRARIES})
