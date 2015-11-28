/**
 *
 */
#ifndef AK_IANDROIDAPP_H_INCLUDED
#define AK_IANDROIDAPP_H_INCLUDED

#include "common/types.h"

/**
 * Touch event structure with codes from android/input.h
 */
struct CTouchEvent
{
   int32_t action; // AMOTION_EVENT_ACTION_
   int32_t pointerId;
   int32_t x;
   int32_t y;
};

/**
 * Interface for all events that go from Android to the application.
 */
class IAndroidApp
{
public:
   virtual ~IAndroidApp() {}

   // Application state events
   virtual void OnCreate(const void* aSavedState) = 0;
   virtual void OnDestroy() = 0;
   virtual void OnStart() = 0;
   virtual void OnStop() = 0;
   virtual void OnResume() = 0;
   virtual void OnPause() = 0;
   virtual void* OnSaveState() = 0; // return nullptr or a saved state allocated with malloc()
   virtual void OnInitWindow() = 0;
   virtual void OnDestroyWindow() = 0;
   virtual void OnIdle() = 0;

   // Input events (should return true to further process the event)
   virtual bool OnBackKey() = 0;
   virtual bool OnMenuKey() = 0;
   virtual bool OnKeyDown(int32_t aKeyCode, int32_t aScanCode, int32_t aFlags) = 0;
   virtual bool OnKeyUp(int32_t aKeyCode, int32_t aScanCode, int32_t aFlags) = 0;
   virtual bool OnTouchEvent(const CTouchEvent& aEvent) = 0;
};

#endif // AK_IANDROIDAPP_H_INCLUDED

