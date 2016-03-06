/**
 *
 */
#ifndef AK_ENGINE_H_INCLUDED
#define AK_ENGINE_H_INCLUDED

#include "IEngine.h"
#include "IEngineControl.h"
#include "common/cabinet/CabManager.h"
#include "client/gfx/GraphicsComponent.h"

namespace Client
{

class CEngine : public IEngine, public IEngineControl
{
public:
   CEngine(TNativePtr aNativePtr, TAppPtr aAppPtr)
    : iNativePtr(aNativePtr)
    , iAppPtr(aAppPtr)
    , iResourceManager(aAppPtr->GetResourceFiles(), aAppPtr->GetCategoryContent())
    , iCabinetManager()
    , iLastState(nullptr)
   {}
   virtual ~CEngine() {}

   // IEngine implementation (see there for documentation)
   virtual bool OnCreate(const void* aSavedState) final;
   virtual void OnInitWindow(int32_t aWidth, int32_t aHeight) final;
   virtual void OnStart() final;
   virtual void OnResume() final;
   virtual void OnIdle() final;
   virtual void OnDrawFrame() final;
   virtual void* OnSaveState() final;
   virtual void OnPause() final;
   virtual void OnStop() final;
   virtual void OnDestroyWindow() final;
   virtual void OnDestroy() final;
   virtual bool OnBackKey() final;
   virtual bool OnMenuKey() final;
   virtual bool OnKeyDown(int32_t aKeyCode, int32_t aScanCode, int32_t aFlags) final;
   virtual bool OnKeyUp(int32_t aKeyCode, int32_t aScanCode, int32_t aFlags) final;
   virtual bool OnTouchEvent(const TTouchEvent& aEvent) final;

   // IEngineControl implementation (see there for documentation)
   virtual void Draw(TResourceFileId aTexture, int32_t x, int32_t y) final {
      iGraphicsComponent->Draw(aTexture, x, y);
   }
   virtual void Draw(TResourceFileId aTexture, int32_t x, int32_t y, int32_t aWidth, int32_t aHeight) final {
      iGraphicsComponent->Draw(aTexture, x, y, aWidth, aHeight);
   }
   virtual void Draw(TResourceFileId aTexture, int32_t x, int32_t y, int32_t aWidth, int32_t aHeight,
                     int32_t aTexLeft, int32_t aTexTop, int32_t aTexRight, int32_t aTexBottom) final {
      iGraphicsComponent->Draw(aTexture, x, y, aWidth, aHeight, aTexLeft, aTexTop, aTexRight, aTexBottom);
   }
private:
   void LoadData(TRequiredResources aRequiredResources);

   // Game engines
   TNativePtr iNativePtr;
   TAppPtr iAppPtr;
   CResourceManager iResourceManager;
   CCabManager iCabinetManager;
   std::unique_ptr<CGraphicsComponent> iGraphicsComponent;

   IGameState* iLastState; // to compare with current state, not used for accessing

   // Window state
   int32_t iWidth = 0;
   int32_t iHeight = 0;
   int32_t fps = 0;
   int32_t frame = 0;
   int64_t frameStartUs = 0;
   int64_t drawTimeUs = 0;
   int64_t nextTick = 0;
};

} // namespace Client

#endif // AK_ENGINE_H_INCLUDED
