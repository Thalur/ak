/**
 * Main entry point for the application on Windows, Linux and OSX.
 */
#include "NativePosix.h"
#include "client/gfx/IEngine.h"
#include "common/log/log.h"
#include "client/gfx/oglincludes.h"
#include <boost/filesystem.hpp>

namespace Client
{

namespace
{

// Pointer to the main engine
Client::TEnginePtr engine;
bool bVisible = false;
bool bWindowClosed = false;

void ExitApplication();

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
   LOG_PARAMS("key=%d, x=%d, y=%d", key, mouseX, mouseY);
   if (key == 27) {
      if (engine->OnBackKey()) {
         ExitApplication();
      }
   } else {
      //int mod = glutGetModifiers();
      engine->OnKeyDown(key, key, 0);
      engine->OnKeyUp(key, key, 0);
   }
}

void OnSpecialKeydown(int key, int mouseX, int mouseY)
{
   LOG_PARAMS("key=%d, x=%d, y=%d", key, mouseX, mouseY);
   // ...
}

void OnMouseButton(int button, int state, int x, int y)
{
   LOG_PARAMS("button=%d, state=%d, x=%d, y=%d", button, state, x, y);
   //bool bDown = (state == 0);
   // ...
}

void OnMouseMoved(int x, int y)
{
   // ...
}

void OnWindowClosed()
{
   LOG_METHOD();
   bWindowClosed = true;
#ifdef AK_SYSTEM_OSX
   ExitApplication();
#else
   engine->OnSaveState();
   engine->OnPause();
   engine->OnStop();
   engine->OnDestroyWindow();
#endif
}

void OnIdle()
{
   if (bWindowClosed) return;
   engine->OnIdle();

   // Draw the next frame
   if (bVisible) glutPostRedisplay();
}


void ExitApplication()
{
   LOG_METHOD();
#ifndef AK_SYSTEM_OSX
   glutLeaveMainLoop();
#else
   // On OSX, we cannot leave the main loop so first clean up and then destroy the application
   bWindowClosed = true;
   engine->OnSaveState();
   engine->OnPause();
   engine->OnStop();
   engine->OnDestroyWindow();
   engine->OnDestroy();
   NLogging::FinishLogger();
   std::exit(0);
#endif
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
   glutCreateWindow(aTitle.c_str());
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
   // This is not available on OSX
   glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
#endif
   glutMainLoop();
}

#if defined(AK_SYSTEM_LINUX) || defined(AK_SYSTEM_OSX)
#include <signal.h>
void SignalHandler(int signum)
{
   char* pSignalString = strsignal(signum);
   LOG_INFO("SIGNAL received: %s (%d)", (pSignalString != nullptr ? pSignalString : "?"), signum);
   signal(SIGTERM, NULL);
   signal(SIGQUIT, NULL);
   signal(SIGINT, NULL);
   signal(SIGHUP, NULL);
   signal(SIGABRT, NULL);
   ExitApplication();
}
#endif

/**
 * Get the absolute path where the executable resides.
 * If argv[0] contains an absolute path, we use that, otherwise build it with boost.
 */
std::string GetApplicationPath(const char* const argv0)
{
   std::string appPath(argv0);
   if ((appPath[0] != '/') && ((appPath[1] != ':') || (appPath[2] != '\\'))) {
      boost::system::error_code ec;
      const boost::filesystem::path path = boost::filesystem::initial_path(ec);
      appPath = path.string() + std::string("/") + appPath;
   }
   const TSize pos = appPath.find_last_of("\\/");
   appPath = appPath.substr(0, pos+1);
   LOG_INFO("Determined application path as %s", appPath.c_str());
   return appPath;
}

} // anonymous namespace

/**
 * Main Application entry point.
 * Initialize the native interface and start the engine.
 */
void Run(TAppPtr aAppPtr, int argc, char** argv)
{
   LOG_METHOD();
   std::string path = GetApplicationPath(argv[0]);
   TNativePtr native = std::make_shared<CNativePosix>(std::move(path));
   engine = IEngine::CreateEngine(native, aAppPtr);
   if (SetupOpenGL(argc, argv, aAppPtr->AppName())) {
#if defined(AK_SYSTEM_LINUX) || defined(AK_SYSTEM_OSX)
      signal(SIGTERM, &SignalHandler);
      signal(SIGQUIT, &SignalHandler);
      signal(SIGINT, &SignalHandler);
      signal(SIGHUP, &SignalHandler);
      signal(SIGABRT, &SignalHandler);
#endif
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
