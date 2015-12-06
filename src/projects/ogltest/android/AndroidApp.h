/**
 *
 */
#ifndef AK_ANDROIDAPP_H_INCLUDED
#define AK_ANDROIDAPP_H_INCLUDED

#include "IAndroidApp.h"
#include <EGL/egl.h>
#include "../pngLoader.h"


class CAndroidApp : public IAndroidApp
{
public:
   CAndroidApp(ANativeActivity* aNativeActivity)
    : iNativeActivity(aNativeActivity), iNativeWindow(nullptr), iSurfaceCreated(false)
    , iDisplay(0), iSurface(0), iContext(0), iWidth(0), iHeight(0)
   {}
   virtual ~CAndroidApp();

   // Application state events
   virtual void OnCreate(const void* aSavedState);
   virtual void OnDestroy();
   virtual void OnStart();
   virtual void OnStop();
   virtual void OnResume();
   virtual void OnPause();
   virtual void* OnSaveState(); // return nullptr or a saved state allocated with malloc()
   virtual void OnInitWindow(ANativeWindow* aNativeWindow);
   virtual void OnDestroyWindow();
   virtual void OnIdle();

   // Input events (should return true to further process the event)
   virtual bool OnBackKey();
   virtual bool OnMenuKey();
   virtual bool OnKeyDown(int32_t aKeyCode, int32_t aScanCode, int32_t aFlags);
   virtual bool OnKeyUp(int32_t aKeyCode, int32_t aScanCode, int32_t aFlags);
   virtual bool OnTouchEvent(const CTouchEvent& aEvent);

private:
   void OnDrawFrame();

   ANativeActivity* iNativeActivity;
   ANativeWindow* iNativeWindow;

   bool iSurfaceCreated;
   EGLDisplay iDisplay;
   EGLSurface iSurface;
   EGLContext iContext;
   int32_t iWidth;
   int32_t iHeight;

   int32_t fps = 0;
   int32_t frame = 0;
   int64_t frameStartUs = 0;
   int64_t drawTimeUs = 0;
   int64_t nextTick = 0;
   float angle = 0.0f;
   TTexturePtr texture;
};


#endif // AK_ANDROIDAPP_H_INCLUDED

