#include "android_native_app_glue.h"
#include "common/log/log.h"
#include "IAndroidApp.h"

// The application must provide this method to initialize the IAndroidApp interface
// through which it will receive all application callbacks
#ifdef __cplusplus
extern "C" {
#endif
extern std::unique_ptr<IAndroidApp> SetupApplication(ANativeActivity* aNativeActivity);
#ifdef __cplusplus
}
#endif

namespace {

std::unique_ptr<IAndroidApp> pAndroidApp;

const char* GetKeyEventAction(const int32_t aAction)
{
   switch (aAction) {
   case AKEY_EVENT_ACTION_DOWN:
      return "KEY_DOWN";
   case AKEY_EVENT_ACTION_UP:
      return "KEY_UP";
   case AKEY_EVENT_ACTION_MULTIPLE:
      return "KEY_MULTIPLE";
   default:
      return "Unknown key action event";
   }
}

/**
 * Process the next input event.
 */
int32_t HandleInput(android_app* const aApp, AInputEvent* const aEvent)
{
   const int32_t type = AInputEvent_getType(aEvent); // AINPUT_EVENT_TYPE_
   if (type == AINPUT_EVENT_TYPE_KEY) {
      const int32_t action = AKeyEvent_getAction(aEvent); // AKEY_EVENT_ACTION_
      const int32_t flags = AKeyEvent_getFlags(aEvent); // AKEY_EVENT_FLAG_
      const int32_t keyCode = AKeyEvent_getKeyCode(aEvent); // Physical key code
      const int32_t scanCode = AKeyEvent_getScanCode(aEvent); // Hardware key id
      switch (keyCode) {
      case AKEYCODE_HOME:
         LOG_DEBUG("%s: HOME key", GetKeyEventAction(action));
         return 0;
      case AKEYCODE_BACK:
         LOG_DEBUG("%s: BACK key", GetKeyEventAction(action));
         return 0;
      case AKEYCODE_VOLUME_UP:
         LOG_DEBUG("%s: VOLUME UP", GetKeyEventAction(action));
         return 0;
      case AKEYCODE_VOLUME_DOWN:
         LOG_DEBUG("%s: VOLUME DOWN", GetKeyEventAction(action));
         return 0;
      case AKEYCODE_POWER:
         LOG_DEBUG("%s: POWER", GetKeyEventAction(action));
         return 0;
      case AKEYCODE_MENU:
         LOG_DEBUG("%s: MENU key", GetKeyEventAction(action));
         return 0;
      case AKEYCODE_MUTE:
         LOG_DEBUG("%s: MUTE key", GetKeyEventAction(action));
         return 0;
      default:
         LOG_DEBUG("Unhandled key input %s: %d (%d)", GetKeyEventAction(action), keyCode, scanCode);
         return 0;
      }
   } else if (type == AINPUT_EVENT_TYPE_MOTION) {
      LOG_DEBUG("Handling Motion Event");
      return 1;
   } else {
      LOG_ERROR("Unknown input event type %d", type);
      return 0;
   }
}

void HandleCommand(android_app* const aApp, const int32_t aCommand)
{
   switch (aCommand) {
   case APP_CMD_INPUT_CHANGED:
      LOG_DEBUG("Input handling changed");
      break;
   case APP_CMD_INIT_WINDOW:
      pAndroidApp->OnInitWindow(aApp->window);
      break;
   case APP_CMD_TERM_WINDOW:
      pAndroidApp->OnDestroyWindow();
      break;
   case APP_CMD_WINDOW_RESIZED:
      // This should not happen on a device
      LOG_ERROR("Application window was resized");
      break;
   case APP_CMD_WINDOW_REDRAW_NEEDED:
      LOG_DEBUG("Application window needs redraw");
      break;
   case APP_CMD_CONTENT_RECT_CHANGED:
      LOG_DEBUG("Application content rect changed");
      break;
   case APP_CMD_GAINED_FOCUS:
      LOG_DEBUG("Application gained focus");
      break;
   case APP_CMD_LOST_FOCUS:
      LOG_DEBUG("Application lost focus");
      break;
   case APP_CMD_CONFIG_CHANGED:
      LOG_DEBUG("Application configuration changed");
      break;
   case APP_CMD_LOW_MEMORY:
      LOG_WARN("The system is low on memory");
      break;
   case APP_CMD_START:
      pAndroidApp->OnStart();
      break;
   case APP_CMD_RESUME:
      pAndroidApp->OnResume();
      break;
   case APP_CMD_SAVE_STATE:
      aApp->savedState = pAndroidApp->OnSaveState();
      break;
   case APP_CMD_PAUSE:
      pAndroidApp->OnPause();
      break;
   case APP_CMD_STOP:
      pAndroidApp->OnStop();
      break;
   case APP_CMD_DESTROY:
      pAndroidApp->OnDestroy();
      break;
   default:
      LOG_ERROR("Unknown command: %d", aCommand);
   }
}

}

void android_main(android_app* const aApplication)
{
   app_dummy(); // Make sure glue isn't stripped
   pAndroidApp = SetupApplication(aApplication->activity);
   LOG_INFO("MAIN START");
   //ANativeActivity_finish(state->activity);

   // Init callbacks for the event loop
   android_poll_source* pSource;
   aApplication->onAppCmd = HandleCommand;
   aApplication->onInputEvent = HandleInput;

   // Main activity loop
   pAndroidApp->OnCreate(aApplication->savedState);
   while (!aApplication->destroyRequested)
   {
      // Process all pending events
      int result;
      int fileDescriptors;
      int events;
      android_poll_source* pSource;
      while ((result = ALooper_pollAll(0, &fileDescriptors, &events, (void**)&pSource)) >= 0)
      {
         if (pSource != NULL) { // Lifecycle or input event
            // Launch event app processing via aApplication->onAppCmd or aApplication->onInputEvent
            pSource->process(aApplication, pSource); 
         }

         // ... (check other data like sensors)

         // Check if we are exiting.
         if (aApplication->destroyRequested) {
            break;
         }
      }

      // Input handler is idle - so the app can draw a frame or do some stuff
      if (!aApplication->destroyRequested) {
         pAndroidApp->OnIdle();
      }
   }
   LOG_INFO("MAIN EXIT");
}

