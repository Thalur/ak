/**
 *
 */
#include "client/control.h"
#include "common/log/log.h"

namespace {

enum State {
   STATE_INIT, STATE_LOADING, STATE_DISPLAY
};

State state{STATE_INIT};
CGameStatePtr currentState{};
CGameStatePtr newState{};
TRequiredResources loadedResources{GFXCAT_NONE};

void SwitchState()
{
   LOG_INFO("Switching game state to <ToDo>");
   currentState = newState;
   TRequiredResources categories = currentState->GetRequiredResources();
   // ToDo: check if loading is actually required
   state = STATE_LOADING;
}

} // namespace

const CGameStatePtr& GetCurrentState()
{
   return currentState;
}

TInt32 GetTickRate()
{
   return 60;
}

// This needs some more parameters (e.g. data object)
void OnInit()
{
   if (state != STATE_INIT) {
      LOG_ERROR("OnInit() called while not in INIT state");
   }

   // Load base graphics
   // ... LoadGraphics(GFXCAT_DEFAULT, GFX_NO_TIMEOUT);
   // Initialize first game state
   // ... currentState = make_shared<CGameStateIntro>(params);
   state = STATE_LOADING;
}

bool OnTick()
{
   switch (state) {
   case STATE_DISPLAY: // Display the next frame
      if (newState) {
         SwitchState();
         if (state == STATE_LOADING) break;
      }
      return currentState->OnTick();
   case STATE_LOADING:
      // Load some stuff...
      break;
   }
   return true;
}

void OnDraw(TInt32 aFps)
{
   switch (state) {
   case STATE_DISPLAY:
      currentState->OnDraw(aFps);
      break;
   case STATE_LOADING:
      // ...
      break;
   }
}



namespace {

class CGameStateCallbackHandler : public CGameState::IStateSwitchCallback
{
   virtual void SwitchGameState(const CGameStatePtr& aNewState)
   {
      if (newState) {
         LOG_WARN("Overriding previously set next game state");
      }
      newState = aNewState;
   }
};

} // namespace