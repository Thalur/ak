/**
 *
 */
#include "GameStateDialog.h"
#include "projects/ogltest/resourcelist.h"

CGameStateDialog::CGameStateDialog(IStateSwitchCallback* aStateSwitchCallback,
      TGameStatePtr aParentState, std::string aTitle, const std::string& aText, TDialogButtons aButtons)
 : IGameState(iStateSwitchCallback)
 , iParentState(std::move(aParentState))
 , iTitle(std::move(aTitle))
 , iText(std::vector<std::string>{aText})
 , iButtons(std::move(aButtons))
 , iRequiredResources(1 << (CAT_STARTUP))
{
   // initialize iText properly
}


void CGameStateDialog::OnActivate(EDialogButton aDialogResult)
{

}

void CGameStateDialog::OnDraw(int32_t aFps)
{

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
