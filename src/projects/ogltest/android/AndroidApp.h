/**
 *
 */
#ifndef AK_ANDROIDAPP_H_INCLUDED
#define AK_ANDROIDAPP_H_INCLUDED

#include "IAndroidApp.h"
#include <android/native_activity.h>


class CAndroidApp : public IAndroidApp
{
public:
   CAndroidApp(ANativeActivity* aNativeActivity)
    : iNativeActivity(aNativeActivity)
   {}
   virtual ~CAndroidApp() {}

   // Application state events
   virtual void OnCreate(const void* aSavedState);
   virtual void OnDestroy();
   virtual void OnStart();
   virtual void OnStop();
   virtual void OnResume();
   virtual void OnPause();
   virtual void* OnSaveState(); // return nullptr or a saved state allocated with malloc()
   virtual void OnInitWindow();
   virtual void OnDestroyWindow();
   virtual void OnIdle();

   // Input events (should return true to further process the event)
   virtual bool OnBackKey();
   virtual bool OnMenuKey();
   virtual bool OnKeyDown(int32_t aKeyCode, int32_t aScanCode, int32_t aFlags);
   virtual bool OnKeyUp(int32_t aKeyCode, int32_t aScanCode, int32_t aFlags);
   virtual bool OnTouchEvent(const CTouchEvent& aEvent);

private:
   ANativeActivity* iNativeActivity;
};


#endif // AK_ANDROIDAPP_H_INCLUDED

