/**
 *
 */
#ifndef AK_ENGINE_H_INCLUDED
#define AK_ENGINE_H_INCLUDED

#include "IEngine.h"
#include "common/cabinet/CabManager.h"
#include "client/gfx/GraphicsComponent.h"

namespace Client
{

class CEngine : public IEngine
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
   virtual bool OnCreate(const void* aSavedState);
   virtual void OnInitWindow(int32_t aWidth, int32_t aHeight);
   virtual void OnStart();
   virtual void OnResume();
   virtual void OnIdle();
   virtual void OnDrawFrame();
   virtual void* OnSaveState();
   virtual void OnPause();
   virtual void OnStop();
   virtual void OnDestroyWindow();
   virtual void OnDestroy();
   virtual bool OnBackKey();
   virtual bool OnMenuKey();
   virtual bool OnKeyDown(int32_t aKeyCode, int32_t aScanCode, int32_t aFlags);
   virtual bool OnKeyUp(int32_t aKeyCode, int32_t aScanCode, int32_t aFlags);
   virtual bool OnTouchEvent(const TTouchEvent& aEvent);

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
