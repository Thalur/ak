#ifdef AK_SYSTEM_ANDROID
  #define GL_GLEXT_PROTOTYPES
  #include <EGL/egl.h>
  #include <GLES/gl.h>
  #include <GLES/glext.h>
#else
  #define FREEGLUT_LIB_PRAGMAS 0
  #include "GL/glew.h"
  #include "GL/freeglut.h"
  #ifdef AK_SYSTEM_OSX
    #include "OpenGL/gl.h"
    #include "OpenGL/glext.h"
  #endif

#endif
