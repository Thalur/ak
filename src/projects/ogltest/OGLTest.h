/**
 *
 */
#ifndef AK_OGLTEST_H_INCLUDED
#define AK_OGLTEST_H_INCLUDED

#include "client/main/IAppInterface.h"
#include "client/main/IGameState.h"


class COGLTest : public Client::IAppInterface, public IGameState::IStateSwitchCallback
{
public:
   COGLTest();
   virtual ~COGLTest() {}

   // IAppInterface implementation
   virtual std::string AppName() { return iAppName; }
   virtual TNames InternalFiles();
   virtual TNames ResourceFiles();
   virtual TGameStatePtr GameState();

   // IStateSwitchCallback implementation
   virtual void SwitchGameState(const TGameStatePtr& aNewState);

private:
   static const std::string iAppName;

   TGameStatePtr iGameState;
   TGameStatePtr iNextGameState;
};

#endif // AK_OGLTEST_H_INCLUDED
