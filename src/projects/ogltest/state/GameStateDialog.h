/**
 *
 */
#ifndef AK_GAME_STATE_DIALOG_H_INCLUDED
#define AK_GAME_STATE_DIALOG_H_INCLUDED

#include "client/main/IGameState.h"

class CGameStateDialog : public IGameState
{
public:
   CGameStateDialog(TGameStatePtr aParentState, std::string aTitle, std::string aText, const TDialogButtons& aButtons);
   virtual ~CGameStateDialog() {}

   // IGameState implementation
   virtual void OnActivate(EDialogButton aDialogResult);
   virtual void OnDraw(int32_t aFps);
   virtual bool OnTick();
   virtual Client::TRequiredResources GetRequiredResources() const { return iRequiredResources; }
   virtual void OnTouch(const TTouchEvent& aEvent);
   virtual void OnKeyDown(int32_t aKeyCode);
   virtual bool OnBackKey();
   virtual void OnMenuKey();
   virtual int32_t GetDesiredFrameRate() const;

private:
   const Client::TRequiredResources iRequiredResources;
};

#endif // AK_GAME_STATE_DIALOG_H_INCLUDED
