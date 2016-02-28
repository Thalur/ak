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

namespace {

#ifdef AK_SYSTEM_ANDROID
void Crop(int32_t aLeft, int32_t aTop, int32_t aRight, int32_t aBottom)
{
   // Set the cropping rectangle to only draw part of the source bitmap
   int32_t crop[4];
   crop[0] = aLeft;
   crop[1] = aTop;
   crop[2] = aRight - aLeft;
   crop[3] = aBottom - aTop;
   glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_CROP_RECT_OES, crop);
   if (glGetError() != 0) LOG_ERROR("Error!");
}
void NativeBlitAndroid(int32_t x, int32_t y, int32_t aWidth, int32_t aHeight,
    int32_t cropLeft, int32_t cropTop, int32_t cropRight, int32_t cropBottom, int32_t orgWidth, int32_t orgHeight)
{
   bool bCrop = (cropLeft != 0) || (cropTop != 0) || (cropRight != orgWidth) || (cropBottom != orgHeight);
   if (bCrop) {
      Crop(cropLeft, cropTop, cropRight, cropBottom);
   }
   glDrawTexiOES(x, y, 0, aWidth, aHeight);
   if (glGetError() != 0) LOG_ERROR("Error!");
   if (bCrop) {
      Crop(0, 0, orgWidth, orgHeight);
   }
}
void NativeBlitAndroid(int32_t x, int32_t y, int32_t aWidth, int32_t aHeight)
{
   glDrawTexiOES(x, y, 0, aWidth, aHeight);
   if (glGetError() != 0) LOG_ERROR("Error!");
}
#else
void NativeBlitPosix(float x, float y, float aWidth, float aHeight,
    float cropLeft, float cropTop, float cropRight, float cropBottom)
{
   float right = x + aWidth;
   float bottom = y + aHeight;
   glBegin(GL_QUADS);
   glTexCoord2f(cropLeft, cropBottom);
   glVertex2f(x, y);
   glTexCoord2f(cropRight, cropBottom);
   glVertex2f(right, y);
   glTexCoord2f(cropRight, cropTop);
   glVertex2f(right, bottom);
   glTexCoord2f(cropLeft, cropTop);
   glVertex2f(x, bottom);
   glEnd();
}
#endif

} // anonymous namespace

TEnginePtr IEngine::CreateEngine(TNativePtr aNativePtr, TAppPtr aAppPtr)
{
   return std::make_shared<CEngine>(aNativePtr, aAppPtr);
}

bool CEngine::OnCreate(const void* aSavedState)
{
   LOG_METHOD();
   // Open the required internal cabinets
   for (auto fName : iAppPtr->InternalCabinets()) {
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
   const int64_t timeUs = CClock::GetCurrentTicksUs();
   if (timeUs+1000 < nextTick) {
      const int32_t sleepTime = static_cast<int32_t>((nextTick-timeUs)/1000);
      LOG_DEBUG("Spleeping for %d ms (diff: %" PRId64 ")", sleepTime, (nextTick-timeUs));
      std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
   } else if (nextTick+500000 < timeUs) {
      LOG_DEBUG("Resetting tick timer (difference was %" PRId64 " ms)", (timeUs-nextTick+500)/1000);
      nextTick = timeUs;
   }

   // Execute ticks and schedule the next tick according to the set tick rate
   do {
      //LOG_DEBUG("Tick");
      const int32_t tickRate = 60;//GetTickRate();
      const int64_t tickIntervalUs = (1000000 + tickRate/2) / tickRate;
      nextTick += tickIntervalUs;
   } while (CClock::GetCurrentTicksUs()+100 >= nextTick);
}

void CEngine::OnDrawFrame()
{
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

   glClearColor(0.5, 0.5, 0.5, 1);
   glClear(GL_COLOR_BUFFER_BIT);
   iCurrentTexture = -1;

   DrawTexture(*iTextures[0], 0, 0, 64, 64);
   DrawTexture(*iTextures[1], 140, 100, 128, 96);
   DrawTexture(*iTextures[4], 432, 256, 368, 224);
   DrawTexture(*iTextures[3], 20, 20);
   DrawTexture(*iTextures[2], 450, 400, 64, 64);
   DrawTexturePart(*iTextures[4], 500, 10, 200, 200, 50, 50, 100, 100);

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

bool CEngine::OnTouchEvent(const TTouchEvent& aEvent)
{
   LOG_METHOD();
   // ...
   return true;
}

void CEngine::DrawTexture(const CTexture& aTexture, int32_t x, int32_t y)
{
   DrawTexture(aTexture, x, y, aTexture.Width(), aTexture.Height());
}

void CEngine::DrawTexture(const CTexture& aTexture, int32_t x, int32_t y, int32_t aWidth, int32_t aHeight)
{
   BindTexture(aTexture);
#ifdef AK_SYSTEM_ANDROID
   NativeBlitAndroid(x, iHeight-y-aHeight, aWidth, aHeight);
#else
   NativeBlitPosix(static_cast<float>(x), static_cast<float>(y), static_cast<float>(aWidth), static_cast<float>(aHeight),
      0, 0, aTexture.CropX(), aTexture.CropY());
#endif
}

void CEngine::DrawTexturePart(const CTexture& aTexture, int32_t x, int32_t y, int32_t aWidth, int32_t aHeight,
                              int32_t aTexLeft, int32_t aTexTop, int32_t aTexRight, int32_t aTexBottom)
{
   BindTexture(aTexture);
   float texHeight = static_cast<float>(aTexture.TexHeight());
#ifdef AK_SYSTEM_ANDROID
   NativeBlitAndroid(x, iHeight-y-aHeight, aWidth, aHeight, aTexLeft, texHeight - aTexBottom,
                     aTexRight, texHeight - aTexTop, aTexture.Width(), aTexture.Height());
#else
   float texWidth = static_cast<float>(aTexture.TexWidth());
   NativeBlitPosix(static_cast<float>(x), static_cast<float>(y), static_cast<float>(aWidth), static_cast<float>(aHeight),
      static_cast<float>(aTexLeft) / texWidth, (texHeight - static_cast<float>(aTexBottom)) / texHeight,
      static_cast<float>(aTexRight) / texWidth, (texHeight - static_cast<float>(aTexTop)) / texHeight);
#endif
}

void CEngine::BindTexture(const CTexture& aTexture)
{
   if (iCurrentTexture != aTexture.ID()) {
      iCurrentTexture = aTexture.ID();
      glBindTexture(GL_TEXTURE_2D, iCurrentTexture);
      if (iBlending != aTexture.HasTransparency()) {
         iBlending = !iBlending;
         if (iBlending) {
            glEnable(GL_BLEND);
         } else {
            glDisable(GL_BLEND);
         }
      }
   }
}

} // namespace Client
