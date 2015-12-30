/**
 *
 */
#ifndef AK_NATIVE_ANDROID_H_INCLUDED
#define AK_NATIVE_ANDROID_H_INCLUDED

#include "client/gfx/IEngine.h"
#include "IAppInterface.h"
#include <android/native_activity.h>
#include <EGL/egl.h>

namespace Client
{

class CNativeAndroid : public INativeCallback
{
public:
   CNativeAndroid(ANativeActivity* aNativeActivity);
   virtual ~CNativeAndroid() {}

   // INativeCallback implementation
   virtual TFilePtr GetInternalFile(const std::string& aFilename);
   virtual TFilePtr GetResourceFile(const std::string& aFilename);

   // OpenGL window management
   void InitWindow(ANativeWindow* aNativeWindow);
   void DestroyWindow();
   void Swap();

   // Window properties
   bool IsReady() { return (iSurfaceCreated && iDisplay != nullptr); }
   int32_t Width() { return iWidth; }
   int32_t Height() { return iHeight; }

private:
   ANativeActivity* iNativeActivity = nullptr;
   ANativeWindow* iNativeWindow = nullptr;
   bool iSurfaceCreated = false;
   EGLDisplay iDisplay = nullptr;
   EGLSurface iSurface = nullptr;
   EGLContext iContext = nullptr;
   int32_t iWidth = 0;
   int32_t iHeight = 0;
};

} // namespace Client

#endif // AK_NATIVE_ANDROID_H_INCLUDED

