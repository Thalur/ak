/**
 *
 */
#ifndef AK_IENGINE_H_INCLUDED
#define AK_IENGINE_H_INCLUDED

#include "common/types.h"
#include "client/main/IAppInterface.h"

namespace Client
{

// Shared pointer definitions
class IEngine;
typedef std::shared_ptr<IEngine> TEnginePtr;
class INativeCallback;
typedef std::shared_ptr<INativeCallback> TNativePtr;

/**
 * Touch event structure with codes from android/input.h
 */
struct CTouchEvent
{
   int32_t action; // AMOTION_EVENT_ACTION_ (ToDo: make own enum for platform-independent impl)
   int32_t pointerId; // For multi-touch events (mobile only)
   int32_t x;
   int32_t y;
};

/**
 * Interface for platform-dependent operations required by the Engine.
 */
class INativeCallback
{
public:
   virtual ~INativeCallback() {}

   // An internal file is located inside the binaries (except for mobile)
   virtual TFilePtr GetInternalFile(const std::string& aFilename) = 0;

   // Resource files are located in the package (mobile) or in the app folder (posix)
   virtual TFilePtr GetResourceFile(const std::string& aFilename) = 0;
};

/**
 * Interface for all events that go from Android to the application.
 */
class IEngine
{
public:
   static TEnginePtr CreateEngine(TNativePtr aNativePtr, TAppPtr aAppPtr);

   virtual ~IEngine() {}

   /// Application state events

   // Application started, no window yet, possibly old app state (mobile only)
   // Return false to cancel application start.
   virtual bool OnCreate(const void* aSavedState) = 0;

   // Window was created
   virtual void OnInitWindow(int32_t aWidth, int32_t aHeight) = 0;

   // Window is about to be visible
   virtual void OnStart() = 0;

   // Application is running
   virtual void OnResume() = 0;

   // Idle function, run ticks and sleep
   virtual void OnIdle() = 0;

   // Draw the current frame
   virtual void OnDrawFrame() = 0;

   // Save the game state
   virtual void* OnSaveState() = 0; // return nullptr or a saved state allocated with malloc()

   // Application is no longer running actively
   virtual void OnPause() = 0;

   // Application is no longer visible
   virtual void OnStop() = 0;

   // The window is about to be destroyed
   virtual void OnDestroyWindow() = 0;

   // The app will exit shortly
   virtual void OnDestroy() = 0;

   /// Input events (should return true to further process the event)
   virtual bool OnBackKey() = 0;
   virtual bool OnMenuKey() = 0;
   virtual bool OnKeyDown(int32_t aKeyCode, int32_t aScanCode, int32_t aFlags) = 0;
   virtual bool OnKeyUp(int32_t aKeyCode, int32_t aScanCode, int32_t aFlags) = 0;
   virtual bool OnTouchEvent(const CTouchEvent& aEvent) = 0;

};

} // namespace Client

#endif // AK_IENGINE_H_INCLUDED

