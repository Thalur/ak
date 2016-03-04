/**
 *
 */
#ifndef AK_OGLTEST_H_INCLUDED
#define AK_OGLTEST_H_INCLUDED

#include "GameControl.h"
#include "client/main/IAppInterface.h"
#include "client/main/IGameState.h"


class COGLTest : public Client::IAppInterface, public IGameControl
{
public:
   COGLTest();
   virtual ~COGLTest() {}

   // IAppInterface implementation
   virtual std::string AppName() { return iAppName; }
   virtual TNames InternalCabinets();
   virtual TNames ResourceCabinets();
   virtual Client::TResourceFiles GetResourceFiles();
   virtual Client::TCategoryContent GetCategoryContent();
   virtual TGameStatePtr GameState();

   // IGameControl implementation
   virtual void SwitchGameState(const TGameStatePtr& aNewState);

private:
   static const std::string iAppName;

   TGameStatePtr iGameState;
   TGameStatePtr iNextGameState;
};

#endif // AK_OGLTEST_H_INCLUDED
