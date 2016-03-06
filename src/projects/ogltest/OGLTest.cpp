/**
 *
 */
#include "OGLTest.h"
#include "resourcelist.h"
#include "state/GameStateDialog.h"
#include "common/log/log.h"


Client::TAppPtr CreateApplication()
{
   return std::make_shared<COGLTest>();
}

COGLTest::COGLTest()
 : iAppName("OGLTest - Platform-independent C++")
 , iCopyright("Copyright (C) 2016 by Andre Koschmieder")
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
   int32_t x = Engine()->Width()/2 - 150;
   int32_t y = Engine()->Height()/2;
   Engine()->Draw(F_STARTUP_BACKGROUND, x-90, y-100);
   Engine()->Draw(F_STARTUP_LOADTEXT, x-26, y+20);
   Engine()->Draw(F_STARTUP_PROGRESS_EMPTY, x, y+100);
   if (aProgress > 0) {
      Engine()->Draw(F_STARTUP_PROGRESS_FULL, x, y+100, static_cast<int32_t>(300 * aProgress), 30);
   }
   DrawSoundUI();
   Engine()->Text(F_FONT_DUNE3DSMALL, iAppName, 10, Engine()->Height(), TFontStyle(EHorizontal::LEFT, EVertical::BOTTOM));
   Engine()->Text(F_FONT_DUNE3DSMALL, iCopyright, Engine()->Width() - 10, Engine()->Height(), TFontStyle(EHorizontal::RIGHT, EVertical::BOTTOM));

   /*Engine()->Draw(F_INDEX32X32, 0, 0, 64, 64);
   Engine()->Draw(F_INDEX64_48, 140, 100, 128, 96);
   Engine()->Draw(F_INDEX184x112, 432, 256, 368, 224);
   Engine()->Draw(F_ALPHA320X200, 20, 20);
   Engine()->Draw(F_ALPHA32X32, 450, 400, 64, 64);
   Engine()->Draw(F_INDEX184x112, 500, 10, 200, 200, 50, 50, 100, 100);*/
}

void COGLTest::SwitchGameState(const TGameStatePtr& aNewState)
{
   if (iNextGameState) {
      LOG_WARN("Overriding previously set next game state");
   }
   iNextGameState = aNewState;
}

void COGLTest::DrawSoundUI()
{
   const int32_t width = Engine()->Width();
   Engine()->Draw(F_STARTUP_MUSIC, width - 152, 8);
   Engine()->Draw(F_STARTUP_SOUND, width - 72, 8);
   /*if (!Global.settings.bMusic)*/ {
      Engine()->Draw(F_STARTUP_DISABLED, width - 152, 8);
   }
   /*if (!Global.settings.bSound) {
      Engine()->Draw(F_STARTUP_DISABLED, width - 72, 8);
   }*/
}
