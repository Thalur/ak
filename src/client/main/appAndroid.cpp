#include "android_native_app_glue.h"
#include "common/log/log.h"
#include "NativeAndroid.h"

// The application must provide this method to initialize the IAppInterface
// through which it will receive all application callbacks
extern Client::TAppPtr CreateApplication();

namespace
{

Client::TEnginePtr engine;
std::shared_ptr<Client::CNativeAndroid> native;

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
      native->InitWindow(aApp->window);
      engine->OnInitWindow(native->Width(), native->Height());
      break;
   case APP_CMD_TERM_WINDOW:
      engine->OnDestroyWindow();
      native->DestroyWindow();
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
      engine->OnStart();
      break;
   case APP_CMD_RESUME:
      engine->OnResume();
      break;
   case APP_CMD_SAVE_STATE:
      aApp->savedState = engine->OnSaveState();
      break;
   case APP_CMD_PAUSE:
      engine->OnPause();
      break;
   case APP_CMD_STOP:
      engine->OnStop();
      break;
   case APP_CMD_DESTROY:
      engine->OnDestroy();
      break;
   default:
      LOG_ERROR("Unknown command: %d", aCommand);
   }
}

} // anonymous namespace

void android_main(android_app* const aApplication)
{
   app_dummy(); // Make sure glue isn't stripped
   Client::TAppPtr app = CreateApplication();
   native = std::make_shared<Client::CNativeAndroid>(aApplication->activity);
   LOG_INFO("MAIN START");
   engine = Client::IEngine::CreateEngine(native, app);

   // Init callbacks for the event loop
   android_poll_source* pSource;
   aApplication->onAppCmd = HandleCommand;
   aApplication->onInputEvent = HandleInput;

   // Main activity loop
   if (!engine->OnCreate(aApplication->savedState)) {
      ANativeActivity_finish(aApplication->activity);
   } else {
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
         if (!aApplication->destroyRequested && native->IsReady()) {
            engine->OnIdle();
            engine->OnDrawFrame();
            native->Swap();
         }
      }
   }
   LOG_INFO("MAIN EXIT");
   engine.reset();
   native.reset();
   NLogging::FinishLogger();
}

