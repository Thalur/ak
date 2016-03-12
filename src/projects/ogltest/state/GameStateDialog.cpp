/**
 *
 */
#include "GameStateDialog.h"
#include "projects/ogltest/resourcelist.h"
#include "common/log/log.h"

CGameStateDialog::CGameStateDialog(IGameControl &aGameControl,
      TGameStatePtr aParentState, std::string aTitle, const std::string& aText, TDialogButtons aButtons)
 : iGame(aGameControl)
 , iParentState(std::move(aParentState))
 , iTitle(std::move(aTitle))
 , iText(std::vector<std::string>{aText})
 , iButtons(std::move(aButtons))
 , iRequiredResources((1 << CAT_STARTUP) | (1 << CAT_COMMON) | (1 << CAT_TEST))
{
   LOG_METHOD();
   // initialize iText properly
}


void CGameStateDialog::OnActivate(EDialogResult aDialogResult)
{
   LOG_PARAMS("%d", aDialogResult);
}

void CGameStateDialog::OnDraw()
{
   iGame.Engine()->Draw(F_INDEX32X32, 0, 0, 64, 64);
   iGame.Engine()->Draw(F_INDEX64_48, 140, 100, 128, 96);
   iGame.Engine()->Draw(F_INDEX184x112, 432, 256, 368, 224);
   iGame.Engine()->Draw(F_ALPHA320X200, 20, 20);
   iGame.Engine()->Draw(F_ALPHA32X32, 450, 400, 64, 64);
   iGame.Engine()->Draw(F_INDEX184x112, 500, 10, 200, 200, 50, 50, 100, 100);

   std::string msg = "GameStateDialog, © by André Koschmieder";
   iGame.Engine()->Text(F_FONT_DUNE3DLARGE, msg, iGame.Engine()->Width()/2, iGame.Engine()->Height()/2, TFontStyle(EHorizontal::CENTER, EVertical::CENTER));

   std::string test = "This is a rather long text, drawn in a 140x80-sized area. Let's see how it looks :)";
   iGame.Engine()->Text(F_FONT_DUNE3DSMALL, test, 10, 300, 140, 80, TFontStyle(EHorizontal::CENTER, EVertical::CENTER, ELines::MULTILINE));
}

bool CGameStateDialog::OnTick()
{
   return false;
}

void CGameStateDialog::OnTouch(const TTouchEvent& aEvent)
{

}

void CGameStateDialog::OnKeyDown(int32_t aKeyCode)
{

}

bool CGameStateDialog::OnBackKey()
{
   return true;
}

void CGameStateDialog::OnMenuKey()
{

}

int32_t CGameStateDialog::GetDesiredFrameRate() const
{
   return 20;
}
