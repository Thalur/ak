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
   iGameState = std::make_shared<CGameStateDialog>(*this, TGameStatePtr(),
      "Title",
      "Message",
      TDialogButtons { EDialogResult::OK });
}

TGameStatePtr COGLTest::GameState()
{
   return iGameState;
}

void COGLTest::OnRequiredFilesMissing()
{
   SwitchGameState(std::make_shared<CGameStateDialog>(*this, TGameStatePtr(),
      iAppName,
      "Could not load required game files.\nPlease reinstall the application.",
                                                      TDialogButtons { EDialogResult::OK }));
}

void COGLTest::ShowLoadScreen(double aProgress)
{
   Engine()->Draw(F_INDEX32X32, 0, 0, 64, 64);
   Engine()->Draw(F_INDEX64_48, 140, 100, 128, 96);
   Engine()->Draw(F_INDEX184x112, 432, 256, 368, 224);
   Engine()->Draw(F_ALPHA320X200, 20, 20);
   Engine()->Draw(F_ALPHA32X32, 450, 400, 64, 64);
   Engine()->Draw(F_INDEX184x112, 500, 10, 200, 200, 50, 50, 100, 100);
}

void COGLTest::SwitchGameState(const TGameStatePtr& aNewState)
{
   if (iNextGameState) {
      LOG_WARN("Overriding previously set next game state");
   }
   iNextGameState = aNewState;
}
