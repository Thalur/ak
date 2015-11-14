/**
 * Abstract base class for all game states.
 */
#ifndef AK_GAMESTATE_H_INCLUDED
#define AK_GAMESTATE_H_INCLUDED

#include "common/types.h"
#include "client/resources.h"
#include <memory>

class CGameState;

typedef std::shared_ptr<CGameState> CGameStatePtr;

class CGameState
{
public:
   // Callback interface to let the game control know when to switch game states
   class IStateSwitchCallback
   {
      virtual void SwitchGameState(const CGameStatePtr& aNewState) = 0;
   };

   // Possible actions for the touch input (and mouse input)
   enum ETouchAction {
      TOUCH_DOWN, TOUCH_POINTER_DOWN, TOUCH_MOVE, TOUCH_UP, TOUCH_POINTER_UP, TOUCH_CANCEL
   };

   enum EDialogButton {
      BUTTON_NONE, BUTTON_OK, BUTTON_YES, BUTTON_NO, BUTTON_CANCEL
   };

   virtual ~CGameState() {}

   // If the game state needs to do some time-consuming work, this is the place
   virtual void OnLoadData() = 0;

   // The game state is set to active (i.e. foreground).
   virtual void OnActivate(EDialogButton aDialogResult) = 0;

   // Draw the current frame
   virtual void OnDraw(TInt32 aFps) = 0;

   // Execute a tick event (usually right before drawing)
   // Returns true, if the tick counter should be reset (e.g. after loading data)
	virtual bool OnTick() = 0;

   // Return the required graphics for this state
   virtual TRequiredResources GetRequiredResources() = 0;

   /**
	 * Handle an input action from the touch screen or mouse.
	 *
	 * @arg eventID Identifier for the action pointer for POINTER_* and MOVE action.
	 * @arg action The input action type.
	 * @arg x X coordinate of the touch or mouse action.
	 * @arg y Y coordinate of the touch or mouse action.
	 */
	virtual void OnTouch(TInt32 aEventID, ETouchAction aAction, TInt32 aX, TInt32 aY) = 0;

   // ToDo
   virtual void OnKeyDown(TInt32 aKeyCode)
	{
		// do nothing
	}

   /**
	 * onBackKey is called when the back key is pressed.
	 * The default implementation returns true to exit the application.
	 * However, specific game states can return false to not exit.
	 * This method is only called on Android.
	 */
	virtual bool OnBackKey()
	{
		return true; // exit the application
	}

   /**
	 * onMenuKey is called when the menu key is pressed.
	 * The default implementation does nothing, but specific
	 * game states can use this as an input (e.g. show the menu).
	 */
	virtual void OnMenuKey()
	{
		// do nothing
	}

   virtual bool GetDesiredFrameRate(TInt32& aTicksPerSecond, TInt32 aFramesPerSecond) = 0;

   // Returns a shared pointer to the base state of the state provided as parameter
   static CGameStatePtr GetBaseState(CGameStatePtr aCurrentState)
   {
      CGameStatePtr baseState = aCurrentState->GetParentState();
      while (baseState) {
         aCurrentState = baseState;
         baseState = aCurrentState->GetParentState();
      }
      return aCurrentState;
   }

protected:
   CGameState(IStateSwitchCallback* aStateSwitchCallback) : iStateSwitchCallback(aStateSwitchCallback)
   {}

   // Execute a tick while another window has focus (usually do nothing)
	virtual void OnBackgroundTick() { }

   // Return the parent state of the this state, or empty if this is the base state
   virtual CGameStatePtr GetParentState()
	{
		return CGameStatePtr(); // Default: no parent state
	}

   IStateSwitchCallback* iStateSwitchCallback;
};

#endif // AK_GAMESTATE_H_INCLUDED
