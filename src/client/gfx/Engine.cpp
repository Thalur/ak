/**
 *
 */
#include "Engine.h"
#include "common/log/log.h"
#include "common/util/clock.h"
#include "pngLoader.h"
#include "oglincludes.h"
#include <chrono>
#include <thread>

namespace Client
{

TEnginePtr IEngine::CreateEngine(TNativePtr aNativePtr, TAppPtr aAppPtr)
{
   return std::make_shared<CEngine>(aNativePtr, aAppPtr);
}

bool CEngine::OnCreate(const void* aSavedState)
{
   LOG_METHOD();
   // Open the required internal cabinets
   for (auto fName : iAppPtr->InternalFiles()) {
      TFilePtr file = iNativePtr->GetInternalFile(fName);
      if (!file) {
         return false;
      }
      iCabinet = CCabinet::Open(file);
      if (!iCabinet) {
         LOG_ERROR("Could not open cabinet %s", fName.c_str());
         return false;
      }
   }
   return true;
}

void CEngine::OnInitWindow(int32_t aWidth, int32_t aHeight)
{
   LOG_PARAMS("width=%u, height=%u", aWidth, aHeight);
   iWidth = aWidth;
   iHeight = aHeight;

   // Initialize OpenGL drawing parameters
   glEnable(GL_TEXTURE_2D);              
   glClearColor(0.0f, 0.0f, 0.0f, 0.0f);         
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
   glViewport(0, 0, iWidth, iHeight);
   glMatrixMode(GL_MODELVIEW);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
#ifdef AK_SYSTEM_ANDROID
   glOrthof(0, iWidth, iHeight, 0, 1, -1);
#else // For some reason, glOrthof() crashes on Windows
   glOrtho(0, iWidth, iHeight, 0, 1, -1);
#endif
   glMatrixMode(GL_MODELVIEW);

   // ... (load graphics for splash screen)
   LOG_DEBUG("Loading image files...");
   TFilePtr file = iCabinet->ReadFileByName("index32x32.png");
   TTexturePtr texture = LoadFromMemory(file, "index32x32.png");
   iTextures.push_back(std::move(texture));
   file = iCabinet->ReadFileByName("index64x48.png");
   texture = LoadFromMemory(file, "index64x48.png");
   iTextures.push_back(std::move(texture));
   file = iCabinet->ReadFileByName("alpha32x32.png");
   texture = LoadFromMemory(file, "alpha32x32.png");
   iTextures.push_back(std::move(texture));
   file = iCabinet->ReadFileByName("alpha320x200.png");
   texture = LoadFromMemory(file, "alpha320x200.png");
   iTextures.push_back(std::move(texture));
   file = iCabinet->ReadFileByName("index184x112.png");
   texture = LoadFromMemory(file, "index184x112.png");
   iTextures.push_back(std::move(texture));
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
   int64_t timeUs = CClock::GetCurrentTicksUs();
   if (timeUs+1000 < nextTick) {
      int32_t sleepTime = static_cast<int32_t>((nextTick-timeUs)/1000);
      LOG_DEBUG("Spleeping for %d ms (diff: %" PRId64 ")", sleepTime, (nextTick-timeUs));
      std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
   } else if (nextTick+500000 < timeUs) {
      LOG_DEBUG("Resetting tick timer (difference was %" PRId64 " ms)", (timeUs-nextTick+500)/1000);
      nextTick = timeUs;
   }

   // Execute ticks and schedule the next tick according to the set tick rate
   do {
      //LOG_DEBUG("Tick");
      int32_t tickRate = 60;//GetTickRate();
      int64_t tickIntervalUs = (1000000 + tickRate/2) / tickRate;
      nextTick += tickIntervalUs;
   } while (CClock::GetCurrentTicksUs()+100 >= nextTick);
}

void CEngine::OnDrawFrame()
{
   int64_t timeUs = CClock::GetCurrentTicksUs();
   int64_t frametimeUs = timeUs - frameStartUs;
   //LOG_DEBUG("time: %" PRId64 " start: %" PRId64 " frametime: %" PRId64, timeUs, frameStartUs, frametimeUs);
   if (frametimeUs >= 999900) {
      fps = frame;
      frameStartUs += 1000000;
      if (frameStartUs + 900000 < timeUs) {
         LOG_DEBUG("Resetting fps timer (difference was %" PRId64 " ms)", (timeUs-frameStartUs+500)/1000);
         frameStartUs = timeUs;
      }
      frame = 0;
      LOG_DEBUG("FPS: %2d (drawTime: %4" PRId64 " µs)", fps, drawTimeUs);
   }
   frame++;

   glClearColor(0.5, 0.5, 0.5, 1);
   glClear(GL_COLOR_BUFFER_BIT);

   glDisable(GL_BLEND);
   blit(iTextures[0]->ID(), 0, 0, 64, 64, iTextures[0]->CropX(), iTextures[0]->CropY());
   blit(iTextures[1]->ID(), 140, 100, 128, 96, iTextures[1]->CropX(), iTextures[1]->CropY());
   blit(iTextures[4]->ID(), 432, 256, 368, 224, iTextures[4]->CropX(), iTextures[4]->CropY());
   glEnable(GL_BLEND);
   blit(iTextures[3]->ID(), 20, 20, 320, 200, iTextures[3]->CropX(), iTextures[3]->CropY());
   blit(iTextures[2]->ID(), 450, 400, 64, 64, iTextures[2]->CropX(), iTextures[2]->CropY());

   if (frame == 3) {
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
   // ...
   return true; // further process the key event
}

bool CEngine::OnMenuKey()
{
   LOG_METHOD();
   // ...
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

bool CEngine::OnTouchEvent(const CTouchEvent& aEvent)
{
   LOG_METHOD();
   // ...
   return true;
}

#ifdef AK_SYSTEM_ANDROID
void CEngine::blit(int32_t texID, float x1, float y1, float dx, float dy, float cropX, float cropY)
{
   glBindTexture(GL_TEXTURE_2D, texID);
   if (glGetError() != 0) LOG_ERROR("Error!");
   // Set the cropping rectangle to only draw part of the source bitmap
   /*int32_t crop[4];
   crop[0] = 0;
   crop[1] = 0;
   crop[2] = 32; // width
   crop[3] = 32; // -height
   glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_CROP_RECT_OES, crop);
   if (glGetError() != 0) LOG_ERROR("Error!");*/
   //glDrawTexiOES(100, iHeight-100-64, 0, 64, 64);
   glDrawTexiOES(x1, iHeight-y1-dy, 0, dx, dy);
   if (glGetError() != 0) LOG_ERROR("Error!");
}
#else
void CEngine::blit(int32_t texID, float x1, float y1, float dx, float dy, float cropX, float cropY)
{
   float right = x1 + dx;
   float bottom = y1 + dy;
   glBindTexture(GL_TEXTURE_2D, texID);
   glBegin(GL_QUADS);
   glTexCoord2f(0, cropY);
   glVertex2f(x1, y1);
   glTexCoord2f(cropX, cropY);
   glVertex2f(right, y1);
   glTexCoord2f(cropX, 0);
   glVertex2f(right, bottom);
   glTexCoord2f(0, 0);
   glVertex2f(x1, bottom);
   glEnd();
}
#endif

} // namespace Client
