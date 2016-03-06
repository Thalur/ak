/**
 *
 */
#ifndef AK_OGLTEST_H_INCLUDED
#define AK_OGLTEST_H_INCLUDED

#include "IGameControl.h"
#include "client/main/IAppInterface.h"
#include "client/main/IGameState.h"


class COGLTest : public Client::IAppInterface, public IGameControl
{
public:
   COGLTest();
   virtual ~COGLTest() {}

   // IAppInterface implementation
   virtual std::string AppName() final { return iAppName; }
   virtual TNames InternalCabinets() final;
   virtual TNames RequiredCabinets() final;
   virtual TNames OptionalCabinets() final;
   virtual Client::TResourceFiles GetResourceFiles() final;
   virtual Client::TCategoryContent GetCategoryContent() final;
   virtual TGameStatePtr GameState() final;
   virtual void OnRequiredFilesMissing() final;
   virtual void ShowLoadScreen(double aProgress) final;

   // IGameControl implementation
   virtual void SwitchGameState(const TGameStatePtr& aNewState) final;
   virtual void DrawSoundUI() final;

   void SetEngineControl(Client::IEngineControl* aEngineControl) { iEngineControl = aEngineControl; }

private:
   const std::string iAppName;
   const std::string iCopyright;

   TGameStatePtr iGameState;
   TGameStatePtr iNextGameState;
};

#endif // AK_OGLTEST_H_INCLUDED
