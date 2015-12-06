/**
 *
 */
#include "AndroidApp.h"
#include "common/log/log.h"
#include "common/util/clock.h"
#include "common/cabinet/AndroidAssetFile.h"
#include "common/cabinet/Cabinet.h"
#include <chrono>
#include <thread>
#include <EGL/egl.h>
#include <GLES/gl.h>

namespace {

/**
 * Call Activity::getFilesDir() through JNI and return the reult as a string.
 */
std::string GetJNIDataPath(ANativeActivity* aNativeActivity)
{
   JNIEnv* env = nullptr;
   aNativeActivity->vm->AttachCurrentThread(&env, 0);
   jclass cls_Env = env->FindClass("android/app/NativeActivity");
   jmethodID mid_getExtStorage = env->GetMethodID(cls_Env, "getFilesDir", "()Ljava/io/File;");
   env->DeleteLocalRef(cls_Env);
   jobject obj_File = env->CallObjectMethod(aNativeActivity->clazz, mid_getExtStorage);
   jclass cls_File = env->FindClass("java/io/File");
   jmethodID mid_getPath = env->GetMethodID(cls_File, "getPath", "()Ljava/lang/String;");
   env->DeleteLocalRef(cls_File);
   jstring obj_Path = (jstring) env->CallObjectMethod(obj_File, mid_getPath);
   env->DeleteLocalRef(obj_File);
   const char* dataPath = env->GetStringUTFChars(obj_Path, NULL);
   std::string result(dataPath != NULL ? dataPath : "");
   env->ReleaseStringUTFChars(obj_Path, dataPath);
   env->DeleteLocalRef(obj_Path);
   aNativeActivity->vm->DetachCurrentThread();
   return result;
}

}

#ifdef __cplusplus
extern "C" {
#endif
std::unique_ptr<IAndroidApp> SetupApplication(ANativeActivity* aNativeActivity)
{
   std::string dataPath;
   if (aNativeActivity->internalDataPath != nullptr) {
      dataPath = aNativeActivity->internalDataPath;
   } else {
      // In Android 2.3 there is a bug that the data paths are null. Need to use JNI.
      LOG_INFO("Retrieving internal files directory from JNI");
      dataPath = GetJNIDataPath(aNativeActivity);
   }
   if (dataPath.empty()) {
      NLogging::InitLogFile("AK_OGLtest", dataPath, ELogLevel::ENONE, ELogLevel::EDEBUG);
      LOG_ERROR("Internal data path is not available, log file disabled");
   } else {
      std::string logFileName(dataPath);
      logFileName += std::string("/log.txt");
      NLogging::InitLogFile("AK_OGLtest", logFileName, ELogLevel::EDEBUG, ELogLevel::EINFO);
      LOG_INFO("Writing log to %s", logFileName.c_str());
   }
   return make_unique<CAndroidApp>(aNativeActivity);
}
#ifdef __cplusplus
}
#endif

CAndroidApp::~CAndroidApp()
{
   NLogging::FinishLogger();
}

void CAndroidApp::OnCreate(const void* aSavedState)
{
   LOG_METHOD();
   /*AAssetManager* manager = iNativeActivity->assetManager;
   if (manager == nullptr) {
      LOG_ERROR("Asset manager is unavailable");
      return;
   }
   AAsset* assetFile = AAssetManager_open(manager, "test.ak", AASSET_MODE_BUFFER);
   if (assetFile == nullptr) {
      LOG_ERROR("Could not open asset file");
      return;
   }
   const void* buffer = AAsset_getBuffer(assetFile);
   if (buffer != nullptr) {
      int fileSize = AAsset_getLength(assetFile);
      LOG_INFO("Loaded asset file: %d bytes", fileSize);
   } else {
      LOG_ERROR("Could not read asset content");
   }
   AAsset_close(assetFile);*/
   TFilePtr asset = CAndroidAssetFile::OpenAsset(iNativeActivity->assetManager, "test.ak");
   if (asset) {
      TCabinetPtr cabinet = CCabinet::Open(asset);
      if (cabinet) {
         LOG_INFO("Cabinet successfully opened");
         TFilePtr file = cabinet->ReadFileByName("icon.png");
         if (file) {
            texture = CTexture::LoadFromMemory(file, "icon.png");
         } else {
            LOG_ERROR("Could not read icon.png");
         }
         /*TMemoryFilePtr file = cabinet->ReadFileByName("CMakeLists.txt");
         if (file) {
            CMemoryFile::TStringStreamPtr stream = file->CreateStream();
            char buffer[100];
            stream->getline(buffer, 99);
            LOG_INFO("DATA: %s", buffer);
         } else {
            LOG_ERROR("Could not read CMakeLists.txt");
         }*/
      } else {
         LOG_ERROR("Could not open the cabinet");
      }
   } else {
      LOG_ERROR("Asset file test.ak is not available");
   }
}

void CAndroidApp::OnDestroy()
{
   LOG_METHOD();
}

void CAndroidApp::OnStart()
{
   LOG_METHOD();
}

void CAndroidApp::OnStop()
{
   LOG_METHOD();
}

void CAndroidApp::OnResume()
{
   LOG_METHOD();
}

void CAndroidApp::OnPause()
{
   LOG_METHOD();
}

void* CAndroidApp::OnSaveState()
{
   LOG_METHOD();
   return nullptr;
}

// https://github.com/googlesamples/android-ndk/blob/master/native-activity/app/src/main/jni/main.c
void CAndroidApp::OnInitWindow(ANativeWindow* aNativeWindow)
{
   LOG_METHOD();
   iNativeWindow = aNativeWindow;
   const EGLint attribs[] = { EGL_SURFACE_TYPE, EGL_WINDOW_BIT, EGL_BLUE_SIZE, 8, EGL_GREEN_SIZE, 8,
                              EGL_RED_SIZE, 8, EGL_ALPHA_SIZE, 8, EGL_NONE };
   EGLint dummy, format;
   EGLint numConfigs;
   EGLConfig config;
   LOG_DEBUG("Getting the EGL display");
   iDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
   LOG_DEBUG("Initializing EGL display");
   eglInitialize(iDisplay, 0, 0);
   LOG_DEBUG("Getting EGL config");
   eglChooseConfig(iDisplay, attribs, &config, 1, &numConfigs);
   LOG_DEBUG("Retrieving EGL format");
   eglGetConfigAttrib(iDisplay, config, EGL_NATIVE_VISUAL_ID, &format);
   LOG_DEBUG("Setting native window geometry");
   ANativeWindow_setBuffersGeometry(iNativeWindow, 0, 0, format);
   LOG_DEBUG("Creating EGL Surface");
   iSurface = eglCreateWindowSurface(iDisplay, config, iNativeWindow, NULL);
   LOG_DEBUG("Creating EGL Context");
   iContext = eglCreateContext(iDisplay, config, NULL, NULL);
   LOG_DEBUG("Setting EGL display as current");
   if (eglMakeCurrent(iDisplay, iSurface, iSurface, iContext) == EGL_FALSE) {
      LOG_ERROR("Could not create OpenGL display");
      return;
   }
   LOG_DEBUG("Getting screen size");
   eglQuerySurface(iDisplay, iSurface, EGL_WIDTH, &iWidth);
   eglQuerySurface(iDisplay, iSurface, EGL_HEIGHT, &iHeight);
   iSurfaceCreated = true;
   LOG_INFO("OpenGL display created with %dx%d", iWidth, iHeight);
   glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
   glEnable(GL_CULL_FACE);
   glShadeModel(GL_SMOOTH);
   //glDisable(GL_DEPTH_TEST);
}

void CAndroidApp::OnDestroyWindow()
{
   LOG_METHOD();
   iSurfaceCreated = false;
   if (iDisplay != EGL_NO_DISPLAY) {
      eglMakeCurrent(iDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
      if (iContext != EGL_NO_CONTEXT) {
         eglDestroyContext(iDisplay, iContext);
      }
      if (iSurface != EGL_NO_SURFACE) {
         eglDestroySurface(iDisplay, iSurface);
      }
      eglTerminate(iDisplay);
   }
   iDisplay = EGL_NO_DISPLAY;
   iContext = EGL_NO_CONTEXT;
   iSurface = EGL_NO_SURFACE;
}

void CAndroidApp::OnIdle()
{
   if (!iSurfaceCreated) {
      return;
   }
   int64_t timeUs = CClock::GetCurrentTicksUs();
   if (timeUs+1000 < nextTick) {
      int32_t sleepTime = static_cast<int32_t>((nextTick-timeUs)/1000);
      LOG_DEBUG("Spleeping for %d ms (diff: %" PRId64 ")", sleepTime, (nextTick-timeUs));
      std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
   } else if (nextTick+500000 < timeUs) {
      LOG_DEBUG("Resetting tick timer (difference was %" PRId64 " ms)", (timeUs-nextTick+500)/1000);
      nextTick = timeUs;
   }

   // Execute one tick and schedule the next tick according to the set tick rate
   LOG_DEBUG("Tick");
   //bool bResetTicks = OnTick();
   int32_t tickRate = 60;//GetTickRate();
   int64_t tickIntervalUs = (1000000 + tickRate/2) / tickRate;
   /*if (bResetTicks) {
      nextTick = CClock::GetCurrentTicksUs();
   }*/
   if (timeUs-100000 < nextTick) {
      OnDrawFrame();
   }
   nextTick += tickIntervalUs;
}


bool CAndroidApp::OnBackKey()
{
   LOG_METHOD();
   return true;
}

bool CAndroidApp::OnMenuKey()
{
   LOG_METHOD();
   return true;
}

bool CAndroidApp::OnKeyDown(const int32_t aKeyCode, const int32_t aScanCode, const int32_t aFlags)
{
   return true;
}

bool CAndroidApp::OnKeyUp(const int32_t aKeyCode, const int32_t aScanCode, const int32_t aFlags)
{
   return true;
}

bool CAndroidApp::OnTouchEvent(const CTouchEvent& aEvent)
{
   return true;
}


void CAndroidApp::OnDrawFrame()
{
   if (iDisplay == nullptr) {
      LOG_ERROR("Display not initialized");
      return;
   }
   int64_t timeUs = CClock::GetCurrentTicksUs();
   int64_t frametimeUs = timeUs - frameStartUs;
   //LOG_DEBUG("time: %" PRId64 " start: %" PRId64 " frametime: %" PRId64, timeUs, frameStartUs, frametimeUs);
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
   glClearColor(angle/360.0f, angle/360.0f, angle/360.0f, 1);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   // Reset transformations and set the camera
   /*glLoadIdentity();
   xgluLookAt(0.0f, 0.0f, 10.0f,
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
   glPopMatrix();*/

   angle += 1.0f;
   if (angle > 360.0f) angle -= 360.0f;
   //glColor3f(1, 1, 1);
   //renderBitmapString(-1, 1, GLUT_BITMAP_TIMES_ROMAN_24, std::to_string(fps));

   // Actual Drawing:
   //OnDraw(fps);

   if (frame == 3) {
      drawTimeUs = CClock::GetCurrentTicksUs() - timeUs;
   }
   eglSwapBuffers(iDisplay, iSurface);
}


