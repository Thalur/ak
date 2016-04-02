/**
 *
 */
#include "OGLTest.h"
#include "resourcelist.h"
#include "state/GameStateDialog.h"
#include "common/log/log.h"
#include <sstream>


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

void COGLTest::OnRequiredFilesMissing()
{
   LOG_METHOD();
   SwitchGameState(std::make_shared<CGameStateDialog>(*this, TGameStatePtr(),
      iAppName,
      "Could not load required game files.\nPlease reinstall the application.",
                                                      TDialogButtons { EDialogResult::OK }));
}

void COGLTest::ShowLoadScreen(double aProgress)
{
   LOG_METHOD();
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
}

void COGLTest::Draw(int32_t aFps)
{
   iGameState->OnDraw();

   // Show FPS on top of the current state
   static int32_t sLastFps = 0;
   static std::string sFps { "FPS: 0" };
   if (aFps != sLastFps) {
      std::stringstream stream;
      stream << "FPS: " << aFps;
      sFps = stream.str();
      sLastFps = aFps;
   }
   Engine()->Text(F_FONT_DUNE3DLARGE, sFps, 0, 0, Engine()->Width(), Engine()->Height() - 10, TFontStyle(EHorizontal::CENTER, EVertical::BOTTOM));
   Engine()->DrawScaled(F_ORC_00 + iOrcOffset/6, 0, 0, 2);
   iOrcOffset = (iOrcOffset + 1) % (72*6);
}

bool COGLTest::Tick()
{
   if (iGameState) {
      return iGameState->OnTick();
   }
   return false;
}

void COGLTest::SwitchGameState(const TGameStatePtr& aNewState)
{
   LOG_METHOD();
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
