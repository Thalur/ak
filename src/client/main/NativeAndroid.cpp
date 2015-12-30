/**
 *
 */
#include "NativeAndroid.h"
#include "common/log/log.h"
#include "common/cabinet/AndroidAssetFile.h"
#include "client/gfx/oglincludes.h"

namespace Client
{

namespace
{

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

} // anonymous namespace

CNativeAndroid::CNativeAndroid(ANativeActivity* aNativeActivity)
 : iNativeActivity(aNativeActivity)
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
}

void CNativeAndroid::InitWindow(ANativeWindow* aNativeWindow)
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
   LOG_INFO("OpenGL display created with %dx%d", iWidth, iHeight);
   iSurfaceCreated = true;
}

void CNativeAndroid::DestroyWindow()
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

void CNativeAndroid::Swap()
{
   eglSwapBuffers(iDisplay, iSurface);
}

TFilePtr CNativeAndroid::GetInternalFile(const std::string& aFilename)
{
   return GetResourceFile(aFilename);
}

TFilePtr CNativeAndroid::GetResourceFile(const std::string& aFilename)
{
   return CAndroidAssetFile::OpenAsset(iNativeActivity->assetManager, aFilename);
}

} // namespace Client
