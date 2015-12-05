/**
 * Main application entry point for Linux
 */
#include "common/log/log.h"
#include "common/util/clock.h"
#include "common/cabinet/PosixFile.h"
#include "common/cabinet/Cabinet.h"
#include "../pngLoader.h"
#include <chrono>
#include <thread>

#define FREEGLUT_LIB_PRAGMAS 0
#include "GL/glew.h"
#include "GL/freeglut.h"


bool bWindowClosed = false;
bool bVisible = false;
int32_t fps;
int32_t frame = 0;
int64_t frameStartUs = 0;
int64_t drawTimeUs = 0;

int64_t nextTick = 0;

float angle = 0.0f;
TTexturePtr texture;


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

void blit(int32_t texID, float x1, float y1, float dx, float dy, float cropX, float cropY)
{
   glBindTexture(GL_TEXTURE_2D, texID);
   glBegin(GL_QUADS);
   glTexCoord2f(0, 0);
   glVertex2f(x1, y1);
   glTexCoord2f(cropX, 0);
   glVertex2f(x1 + dx, y1);
   glTexCoord2f(cropX, cropY);
   glVertex2f(x1 + dx, y1 + dy);
   glTexCoord2f(0, cropY);
   glVertex2f(x1, y1 + dy);
   glEnd();
}

void OnRenderScene(void)
{
   int64_t timeUs = CClock::GetCurrentTicksUs();
   int64_t frametimeUs = timeUs - frameStartUs;
   if (frametimeUs >= 999900) {
      fps = frame;
      frameStartUs += 1000000;
      if (frameStartUs + 900000 < timeUs) {
         LOG_DEBUG("Resetting fps timer (difference was %" PRId64 " ms)", (timeUs-frameStartUs+500)/1000);
         frameStartUs = timeUs;
      }
      frame = 0;
      LOG_DEBUG("FPS: %2d (drawTime: %4" PRId64 " µs)", fps, drawTimeUs);
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

   glLoadIdentity();
   glEnable(GL_TEXTURE_2D);
   glTranslatef(0.0, 0.0, -5.0);
   blit(texture->ID(), 0, 0, 0.5, 0.5, 1.0f, 1.0f);
   glDisable(GL_TEXTURE_2D);

   // Actual Drawing:
   //OnDraw(fps);

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
   int64_t timeUs = CClock::GetCurrentTicksUs();
   if (timeUs+1000 < nextTick) {
      int32_t sleepTime = static_cast<int32_t>((nextTick-timeUs)/1000);
      //LOG_DEBUG("Spleeping for %d ms (diff: %" PRId64 ")", sleepTime, (nextTick-timeUs));
      std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
   } else if (nextTick+100000 < timeUs) {
      LOG_DEBUG("Resetting tick timer (difference was %" PRId64 " ms)", (timeUs-nextTick+500)/1000);
      nextTick = timeUs;
   }

   // Execute one tick and schedule the next tick according to the set tick rate
   //bool bResetTicks = OnTick();
   int32_t tickRate = 60;//GetTickRate();
   int64_t tickIntervalUs = (1000000 + tickRate/2) / tickRate;
   /*if (bResetTicks) {
      nextTick = CClock::GetCurrentTicksUs();
   }*/
   nextTick += tickIntervalUs;

   // Draw the next frame
   if (bVisible) glutPostRedisplay();
}

bool SetUpOGL(int argc, char** argv)
{
   LOG_METHOD();
   glutInit(&argc, argv);
   glutInitWindowSize(800, 480);
   glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_ALPHA);
   int glutWindow = glutCreateWindow("AK_GL_MAIN_WND");
   GLenum err = glewInit();
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
   TFilePtr cabFile = CPosixFile::OpenExistingFile("test.ak", false);
   if (cabFile) {
      TCabinetPtr cabinet = CCabinet::Open(cabFile);
      if (cabinet) {
         TFilePtr file = cabinet->ReadFileByName("icon.png");
         if (file) {
            texture = CTexture::LoadFromMemory(file, "icon.png");
         }
      }
   }
   LOG_INFO("Starting the main OpenGL loop");
   glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
   glutMainLoop();
}

int main(int argc, char** argv)
{
   NLogging::InitLogFile("OpenGLtest by AK", "log.txt");
   LOG_METHOD();
   if (SetUpOGL(argc, argv)) {
      RunApplication();
   }
}

