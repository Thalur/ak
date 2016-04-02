/**
 *
 */
#include "Engine.h"
#include "common/log/log.h"
#include "common/util/clock.h"
#include <chrono>
#include <thread>

namespace Client
{

TEnginePtr IEngine::CreateEngine(TNativePtr aNativePtr, TAppPtr aAppPtr)
{
   const TEnginePtr engine = std::make_shared<CEngine>(aNativePtr, aAppPtr);
   aAppPtr->SetEngineControl(dynamic_cast<CEngine*>(&(*engine)));
   return engine;
}

bool CEngine::OnCreate(const void* aSavedState)
{
   LOG_PARAMS(aSavedState != nullptr ? "with saved state" : "NULL");

   // Open the required internal cabinets
   return InitCabinets(true);
}

void CEngine::OnInitWindow(int32_t aWidth, int32_t aHeight)
{
   LOG_PARAMS("width=%u, height=%u", aWidth, aHeight);
   iWidth = aWidth;
   iHeight = aHeight;

   // Initialize the graphics engine
   iGraphicsComponent = make_unique<CGraphicsComponent>(iCabinetManager, iWidth, iHeight);
   iGraphicsComponent->InitOpenGL();

   // Load graphics for splash screen
   LoadData(TRequiredResources(1));
}

void CEngine::OnStart()
{
   LOG_METHOD();
}

void CEngine::OnResume()
{
   LOG_METHOD();
   // ...
}

void CEngine::OnIdle()
{
   const int64_t timeUs = CClock::GetCurrentTicksUs();
   if (timeUs+1000 < nextTick) {
      const int32_t sleepTime = static_cast<int32_t>((nextTick-timeUs)/1000);
      LOG_VERBOSE("Spleeping for %d ms (diff: %" PRId64 ")", sleepTime, (nextTick-timeUs));
      std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
   } else if (nextTick+500000 < timeUs) {
      LOG_DEBUG("Resetting tick timer (difference was %" PRId64 " ms)", (timeUs-nextTick+500)/1000);
      nextTick = timeUs;
   }

   // Execute ticks and schedule the next tick according to the set tick rate
   do {
      if (iAppPtr->Tick()) {
         // ... (reset tick rate)
      }
      const int32_t tickRate = 60;
      const int64_t tickIntervalUs = (1000000 + tickRate/2) / tickRate;
      nextTick += tickIntervalUs;
   } while (CClock::GetCurrentTicksUs()+100 >= nextTick);
}

void CEngine::OnDrawFrame()
{
   if (!iAppPtr->GameState()) {
      LOG_ERROR("Draw() called without active game state");
      return;
   }
   IGameState& gameState = *iAppPtr->GameState();

   // On the first frame, create the cabinet index
   if (iLastState == nullptr) {
      ShowLoadScreen(0);
      iLoadState = ELoadState::INIT;
      iLastState = &gameState;
      return;
   }
   if (iLoadState == ELoadState::INIT) {
      if (!InitCabinets(false)) {
         iAppPtr->OnRequiredFilesMissing();
         iLoadState = ELoadState::DONE;
      } else {
         ShowLoadScreen(0);
         iLoadState = ELoadState::LOAD;
      }
      return;
   }

   // When a new game state gets activated, check if we need to load new resources
   if (iLastState != &gameState) {
      // ...
      iLastState = &gameState;
      iLoadState = ELoadState::LOAD;
   }

   // We need to load some data
   if (iLoadState == ELoadState::LOAD) {
      LoadData(gameState.GetRequiredResources());
      iLoadState = ELoadState::DONE;
      ShowLoadScreen(1);
      gameState.OnActivate(EDialogResult::NONE);
      return;
   }

   const int64_t timeUs = CClock::GetCurrentTicksUs();
   const int64_t frametimeUs = timeUs - frameStartUs;
   //LOG_DEBUG("time: %" PRId64 " start: %" PRId64 " frametime: %" PRId64, timeUs, frameStartUs, frametimeUs);
   if (frametimeUs >= 999900) {
      fps = frame;
      frameStartUs += 1000000;
      if (frameStartUs + 900000 < timeUs) {
         LOG_DEBUG("Resetting fps timer (difference was %" PRId64 " ms)", (timeUs-frameStartUs+500)/1000);
         frameStartUs = timeUs;
      } else {
         LOG_DEBUG("FPS: %2d (drawTime: %4" PRId64 " µs)", fps, drawTimeUs);
      }
      frame = 0;
   }
   frame++;

   iGraphicsComponent->StartFrame();

   iAppPtr->Draw(fps);

   if (frame == 3) { // Record drawing duration once per second
      drawTimeUs = CClock::GetCurrentTicksUs() - timeUs;
   }
}

void* CEngine::OnSaveState()
{
   LOG_METHOD();
   // ...
   return nullptr;
}

void CEngine::OnPause()
{
   LOG_METHOD();
   // ...
}

void CEngine::OnStop()
{
   LOG_METHOD();
   // ...
}

void CEngine::OnDestroyWindow()
{
   LOG_METHOD();
   // ...
}

void CEngine::OnDestroy()
{
   LOG_METHOD();
   // ...
}

bool CEngine::OnBackKey()
{
   LOG_METHOD();
   const TGameStatePtr state = iAppPtr->GameState();
   if (state) {
      return state->OnBackKey();
   }
   return true; // exit the application
}

bool CEngine::OnMenuKey()
{
   LOG_METHOD();
   const TGameStatePtr state = iAppPtr->GameState();
   if (state) {
      state->OnMenuKey();
   }
   return true;
}

bool CEngine::OnKeyDown(int32_t aKeyCode, int32_t aScanCode, int32_t aFlags)
{
   LOG_PARAMS("key=%c (%d), scanCode=%d, flags=%d", (char)aKeyCode, aKeyCode, aScanCode, aFlags);
   // ...
   return true;
}

bool CEngine::OnKeyUp(int32_t aKeyCode, int32_t aScanCode, int32_t aFlags)
{
   LOG_PARAMS("key=%c (%d), scanCode=%d, flags=%d", (char)aKeyCode, aKeyCode, aScanCode, aFlags);
   // ...
   return true;
}

bool CEngine::OnTouchEvent(const TTouchEvent& aEvent)
{
   LOG_METHOD();
   // ...
   return true;
}

/**
 * Load all data required for the selected categories.
 * The order we load the data in is important because loading
 * fonts can add more images to the image list.
 */
void CEngine::LoadData(TRequiredResources aRequiredResources)
{
   LOG_INFO("Loading resources for categories %s", aRequiredResources.to_string().c_str());
   // 1. Load fonts
   const TFileList fontFiles = iResourceManager.GetFileList(aRequiredResources, EFileType::FONT);
   iGraphicsComponent->LoadFonts(fontFiles, iResourceManager);
   // 2. Load images
   const TFileList gfxFiles = iResourceManager.GetFileList(aRequiredResources, EFileType::GFX);
   iGraphicsComponent->LoadGraphics(gfxFiles);
   // 3. Load SFX
   const TFileList sfxFiles = iResourceManager.GetFileList(aRequiredResources, EFileType::SFX);
   // ...
   // 4. Load text files
   const TFileList textFiles = iResourceManager.GetFileList(aRequiredResources, EFileType::TEXT);
   // ...
}

bool CEngine::InitCabinets(const bool aAddInternalOnly)
{
   LOG_METHOD();
   std::vector<TCabinetPtr> cabinets;
   for (const auto& fName : iAppPtr->InternalCabinets()) {
      const TCabinetPtr cabinet = OpenCabinet(fName, true);
      if (!cabinet) {
         LOG_ERROR("Could not open internal cabinet %s", fName.c_str());
         return false;
      }
      cabinets.emplace_back(std::move(cabinet));
   }

   if (!aAddInternalOnly) {
      for (const auto& fName : iAppPtr->RequiredCabinets()) {
         const TCabinetPtr cabinet = OpenCabinet(fName, false);
         if (!cabinet) {
            LOG_ERROR("Could not open required cabinet %s", fName.c_str());
            return false;
         }
         cabinets.emplace_back(std::move(cabinet));
      }
      // ... (optional cabinets)
   }
   return iCabinetManager.Init(cabinets, aAddInternalOnly ? iResourceManager.GetStartupFileList()
                                                          : iResourceManager.GetFileList());
}

TCabinetPtr CEngine::OpenCabinet(const std::string& aFilename, const bool aInternal)
{
   TFilePtr file;
   if (aInternal) {
      file = iNativePtr->GetInternalFile(aFilename);
   } else {
      file = iNativePtr->GetResourceFile(aFilename);
   }
   if (!file) {
      return TCabinetPtr();
   }
   return CCabinet::Open(file);
}

} // namespace Client
