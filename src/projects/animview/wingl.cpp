/**
 *
 */
#include "common/types.h"
#include "StringWindows.h"
#include "common/log/log.h"
#include "common/util/clock.h"
#include "wingl.h"

#define FREEGLUT_LIB_PRAGMAS 0
#include "GL/glew.h"
#include "GL/freeglut.h"

#include "client/control.h"


namespace WinGL {

CStringWindows appCaption;
bool bWindowClosed = false;
bool bVisible = false;
TInt32 fps;
TInt32 frame = 0;
TInt64 frameStartUs = 0;
TInt64 drawTimeUs = 0;

TInt64 nextTick = 0;

float angle = 0.0f;


std::string GetGlewErrorString(const std::string& aMessage, int aErrorCode)
{
   std::string msg(aMessage);
   msg += std::to_string(aErrorCode);
   const GLubyte* errorBytes = glewGetErrorString(aErrorCode);
   if (errorBytes != NULL) {
      msg += " (" + std::string(reinterpret_cast<const char*>(errorBytes)) + ")";
   }
   return msg;
}

std::string GetGluErrorString(const std::string& aMessage, int aErrorCode)
{
   std::string msg(aMessage);
   msg += std::to_string(aErrorCode);
   const GLubyte* errorBytes = gluErrorString(aErrorCode);
   if (errorBytes != NULL) {
      msg += " (" + std::string(reinterpret_cast<const char*>(errorBytes)) + ")";
   }
   return msg;
}

void renderBitmapString(float x, float y, void *font, const std::string& text)
{
   glRasterPos2f(x, y);
   const char *cText = text.c_str();
   std::size_t l = text.length();
   for (int i = 0; i < l; i++) {
      glutBitmapCharacter(font, cText[i]);
   }
}

void OnRenderScene(void)
{
   TInt64 timeUs = CClock::GetCurrentTicksUs();
   TInt64 frametimeUs = timeUs - frameStartUs;
   if (frametimeUs >= 999900) {
      fps = frame;
      frameStartUs += 1000000;
      if (frameStartUs + 900000 < timeUs) {
         LOG_DEBUG("Resetting fps timer (difference was %"PRId64" ms)", (timeUs-frameStartUs+500)/1000);
         frameStartUs = timeUs;
      }
      frame = 0;
      LOG_DEBUG("FPS: %2d (drawTime: %4"PRId64" µs)", fps, drawTimeUs);
   }
   frame++;

   // Clear Color and Depth Buffers
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   // Reset transformations and set the camera
   glLoadIdentity();
   gluLookAt(0.0f, 0.0f, 10.0f,
             0.0f, 0.0f,  0.0f,
             0.0f, 1.0f,  0.0f);

   glPushMatrix();
	glRotatef(angle, 0.0f, 1.0f, 0.0f);

   glColor3f(0, 1, 1);
   glBegin(GL_TRIANGLES);
   glVertex3f(-0.5,-0.5,0.0);
   glVertex3f(0.5,0.0,0.0);
   glVertex3f(0.0,0.5,0.0);
   glEnd();
   glPopMatrix();

   angle += 1.0f;
   glColor3f(1, 1, 1);
   renderBitmapString(-1, 1, GLUT_BITMAP_TIMES_ROMAN_24, std::to_string(fps));

   // Actual Drawing:
   OnDraw(fps);

   if (frame == 3) {
      drawTimeUs = CClock::GetCurrentTicksUs() - timeUs;
   }
   glutSwapBuffers();
}

void OnChangeSize(int width, int height)
{
   LOG_INFO("width=%d, height=%d", width, height);

   // Prevent a divide by zero, when window is too short
   // (you cant make a window of zero width).
   if (height == 0) height = 1;
   float ratio = 1.0f * (float)width / (float)height;

   // Use the Projection Matrix
   glMatrixMode(GL_PROJECTION);
   // Reset Matrix
   glLoadIdentity();

   // Set the viewport to be the entire window
   glViewport(0, 0, width, height);

   // Set the correct perspective.
   gluPerspective(45, ratio, 1, 1000);

   // Get Back to the Modelview
   glMatrixMode(GL_MODELVIEW);
}

/*void OnTimer(int t)
{
   if (bWindowClosed) return;
   OnTick();
   if (bVisible) glutPostRedisplay();

   // Restart the timer
   TInt32 tickRate = GetTickRate();
   glutTimerFunc(1000/tickRate, OnTimer, 7);
}*/

void OnVisibilityChanged(int state)
{
   LOG_INFO("state=%d", state);
   bVisible = (state == GLUT_VISIBLE);
}

void OnNormalKeydown(unsigned char key, int mouseX, int mouseY)
{
   if (key == 27) glutLeaveMainLoop();
   else {
      int mod = glutGetModifiers();
   }
}

void OnSpecialKeydown(int key, int x, int y)
{
}

void OnMouseButton(int button, int state, int x, int y)
{
   LOG_INFO("button=%d, state=%d, x=%d, y=%d", button, state, x, y);
   bool bDown = (state == 0);
}

void OnMouseMoved(int x, int y)
{
}

void OnWindowClosed()
{
   LOG_INFO("");
   bWindowClosed = true;
}

void OnIdle()
{
   // Sleep until the next game tick is due
   if (bWindowClosed) return;
   TInt64 timeUs = CClock::GetCurrentTicksUs();
   if (timeUs+1000 < nextTick) {
      TInt32 sleepTime = static_cast<TInt32>((nextTick-timeUs)/1000);
      //LOG_DEBUG("Spleeping for %d ms (diff: %"PRId64")", sleepTime, (nextTick-timeUs));
      Sleep(sleepTime);
   } else if (nextTick+100000 < timeUs) {
      LOG_DEBUG("Resetting tick timer (difference was %"PRId64" ms)", (timeUs-nextTick+500)/1000);
      nextTick = timeUs;
   }

   // Execute one tick and schedule the next tick according to the set tick rate
   bool bResetTicks = OnTick();
   TInt32 tickRate = GetTickRate();
   TInt64 tickIntervalUs = (1000000 + tickRate/2) / tickRate;
   /*if (bResetTicks) {
      nextTick = CClock::GetCurrentTicksUs();
   }*/
   nextTick += tickIntervalUs;

   // Draw the next frame
   if (bVisible) glutPostRedisplay();
}

bool SetupOpenGL(int& argc, char** argv,const std::string& aWindowTitle)
{
   LOG_INFO("Initializing OpenGL");
   glutInit(&argc, argv);
   glutInitWindowSize(800, 480);
   glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_ALPHA);
   appCaption = CStringWindows(aWindowTitle);

   // This does not accept wide chars. It's possible to create a window ourselves though...
   LOG_INFO("Creating the main window");
   int glutWindow = glutCreateWindow("AK_GL_MAIN_WND");
   GLenum err = glewInit();
   if (err != GLEW_OK) {
      std::string errorMsg = GetGlewErrorString("Could not initialize GLEW: ", err);
      LOG_ERROR(errorMsg);
      CStringWindows winMsg(errorMsg);
      MessageBox(0, winMsg.GetLPWSTR(), appCaption.GetLPWSTR(), MB_OK | MB_SETFOREGROUND | MB_ICONSTOP);
      return false;
   }

   // Set the window properties
   HWND hwnd = FindWindow(NULL, L"AK_GL_MAIN_WND");
   if (hwnd != 0) {
      SetWindowText(hwnd, appCaption.GetLPWSTR());
   }

   // Register callback functions
   glutDisplayFunc(OnRenderScene);
   glutReshapeFunc(OnChangeSize);
   glutKeyboardFunc(OnNormalKeydown);
   glutMouseFunc(OnMouseButton);
   glutMotionFunc(OnMouseMoved);
   glutVisibilityFunc(OnVisibilityChanged);
   glutSpecialFunc(OnSpecialKeydown);
   glutIdleFunc(OnIdle);
   //glutTimerFunc(50, OnTimer, 7);
   glutWMCloseFunc(OnWindowClosed);
   return true;
}

void RunGame()
{
   //nextTick = frameStartUs = CClock::GetCurrentTicksUs();
   OnInit();
   LOG_INFO("Starting the main OpenGL loop");
   glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
   glutMainLoop();
}

} // namespace WinGL
