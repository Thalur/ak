/**
 * Main entry point for the application on Windows, Linux and OSX.
 */
#include "NativePosix.h"
#include "common/log/log.h"
#include "client/gfx/oglincludes.h"

namespace Client
{

namespace
{

// Pointer to the main engine
Client::TEnginePtr engine;
bool bVisible = false;
bool bWindowClosed = false;

/**
 * Get a string description of a GLU error from OpenGL.
 */
std::string GetGlewErrorString(const std::string& aMessage, int aErrorCode)
{
   std::string msg(aMessage);
   msg += std::to_string(aErrorCode);
#ifndef AK_SYSTEM_OSX
   const GLubyte* errorBytes = glewGetErrorString(aErrorCode);
   if (errorBytes != NULL) {
      msg += " (" + std::string(reinterpret_cast<const char*>(errorBytes)) + ")";
   }
#endif
   return msg;
}

void OnRenderScene(void)
{
   engine->OnDrawFrame();
   glutSwapBuffers();
}

void OnChangeSize(int aWidth, int aHeight)
{
   engine->OnInitWindow(aWidth, aHeight);
}

void OnVisibilityChanged(int aState)
{
   LOG_PARAMS("state=%d", aState);
   bVisible = (aState == GLUT_VISIBLE);
   // ...
}

void OnNormalKeydown(unsigned char key, int mouseX, int mouseY)
{
   if (key == 27) {
      if (engine->OnBackKey()) {
#ifndef AK_SYSTEM_OSX
         glutLeaveMainLoop();
#endif
      }
   } else {
      int mod = glutGetModifiers();
      engine->OnKeyDown(key, key, 0);
      engine->OnKeyUp(key, key, 0);
   }
}

void OnSpecialKeydown(int key, int x, int y)
{
   LOG_PARAMS("key=%d, x=%d, y=%d", key, x, y);
   // ...
}

void OnMouseButton(int button, int state, int x, int y)
{
   LOG_PARAMS("button=%d, state=%d, x=%d, y=%d", button, state, x, y);
   bool bDown = (state == 0);
   // ...
}

void OnMouseMoved(int x, int y)
{
   // ...
}

void OnWindowClosed()
{
   engine->OnSaveState();
   engine->OnPause();
   engine->OnStop();
   engine->OnDestroyWindow();
   bWindowClosed = true;
}

void OnIdle()
{
   if (bWindowClosed) return;
   engine->OnIdle();

   // Draw the next frame
   if (bVisible) glutPostRedisplay();
}


/**
 * Initialize GLUT, create the main window, and register the callback functions.
 */
bool SetupOpenGL(int argc, char** argv, const std::string& aTitle)
{
   LOG_METHOD();
   glutInit(&argc, argv);
   glutInitWindowSize(800, 480);
   glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_ALPHA);
   int glutWindow = glutCreateWindow(aTitle.c_str());
   GLenum err = GLEW_OK;
#ifndef AK_SYSTEM_OSX
   // glewInit() on OSX is neither necessary nor available
   err = glewInit();
#endif
   if (err != GLEW_OK) {
      std::string errorMsg = GetGlewErrorString("Could not initialize GLEW: ", err);
      LOG_ERROR(errorMsg);
      return false;
   }
   glutDisplayFunc(OnRenderScene);
   glutReshapeFunc(OnChangeSize);
   glutKeyboardFunc(OnNormalKeydown);
   glutMouseFunc(OnMouseButton);
   glutMotionFunc(OnMouseMoved);
   glutVisibilityFunc(OnVisibilityChanged);
   glutSpecialFunc(OnSpecialKeydown);
   glutIdleFunc(OnIdle);
   glutWMCloseFunc(OnWindowClosed);
   return true;
}

void RunApplication()
{
   LOG_METHOD();
   LOG_INFO("Starting the main OpenGL loop");
#ifndef AK_SYSTEM_OSX
   glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
#endif
   glutMainLoop();
}

} // anonymous namespace

/**
 * Main Application entry point.
 * Initialize the native interface and start the engine.
 */
void Run(TAppPtr aAppPtr, int argc, char** argv)
{
   LOG_METHOD();
   TNativePtr native = std::make_shared<CNativePosix>();
   engine = IEngine::CreateEngine(native, aAppPtr);
   if (SetupOpenGL(argc, argv, aAppPtr->AppName())) {
      if (engine->OnCreate(nullptr)) {
         engine->OnStart();
         engine->OnResume();
         RunApplication();
         engine->OnDestroy();
      }
   }
   engine.reset(); // Release the engine
}

} // namespace Client
