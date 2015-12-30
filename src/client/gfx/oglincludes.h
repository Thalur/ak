#ifdef AK_SYSTEM_ANDROID
#include <EGL/egl.h>
#include <GLES/gl.h>
#include <GLES/glext.h>
#elif defined AK_SYSTEM_OSX
#include "OpenGL/gl.h"
#include "OpenGL/glext.h"
#else
#define FREEGLUT_LIB_PRAGMAS 0
#include "GL/glew.h"
#include "GL/freeglut.h"
#endif
