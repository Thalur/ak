/**
 *
 */
#include "AndroidApp.h"
#include "common/log/log.h"

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


void CAndroidApp::OnCreate(const void* aSavedState)
{
   LOG_METHOD();
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

void CAndroidApp::OnInitWindow()
{
   LOG_METHOD();
}

void CAndroidApp::OnDestroyWindow()
{
   LOG_METHOD();
}

void CAndroidApp::OnIdle()
{
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

