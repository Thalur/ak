/**
 *
 */
#include "GameStateDialog.h"
#include "projects/ogltest/resourcelist.h"

CGameStateDialog::CGameStateDialog(IGameControl &aGameControl,
      TGameStatePtr aParentState, std::string aTitle, const std::string& aText, TDialogButtons aButtons)
 : iGame(aGameControl)
 , iParentState(std::move(aParentState))
 , iTitle(std::move(aTitle))
 , iText(std::vector<std::string>{aText})
 , iButtons(std::move(aButtons))
 , iRequiredResources(1 << (CAT_STARTUP))
{
   // initialize iText properly
}


void CGameStateDialog::OnActivate(EDialogResult aDialogResult)
{

}

void CGameStateDialog::OnDraw(int32_t aFps)
{
   std::string msg = "GameStateDialog";
   iGame.Engine()->Text(F_FONT_DUNE3DLARGE, msg, iGame.Engine()->Width()/2, iGame.Engine()->Height()/2, TFontStyle(EHorizontal::CENTER, EVertical::CENTER));
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
