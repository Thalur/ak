/**
 *
 */
#include "OGLTest.h"
#include "resourcelist.h"
#include "state/GameStateDialog.h"
#include "common/log/log.h"

const std::string COGLTest::iAppName = "OGLTest by AK";

Client::TAppPtr CreateApplication()
{
   return std::make_shared<COGLTest>();
}

COGLTest::COGLTest()
{
   //iGameState = std::make_shared<CGameStateDialog>();
}

TGameStatePtr COGLTest::GameState()
{
   return TGameStatePtr();
}

void COGLTest::SwitchGameState(const TGameStatePtr& aNewState)
{
   if (iNextGameState) {
      LOG_WARN("Overriding previously set next game state");
   }
   iNextGameState = aNewState;
}
