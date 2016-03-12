/**
 * Abstract base class for all game states.
 */
#ifndef AK_IGAMESTATE_H_INCLUDED
#define AK_IGAMESTATE_H_INCLUDED

#include "common/types.h"
#include "client/main/ResourceManager.h"
#include <memory>
#include <vector>


// Possible actions for the touch input (and mouse input)
enum class ETouchAction {
   DOWN, POINTER_DOWN, MOVE, UP, POINTER_UP, CANCEL
};

enum class EDialogResult {
   NONE, OK, YES, NO, CANCEL
};

/**
 * Touch event structure with codes from android/input.h
 */
struct TTouchEvent
{
   ETouchAction action;
   int32_t pointerId; // For multi-touch events (mobile only)
   int32_t x;
   int32_t y;
};

class IGameState;

typedef std::shared_ptr<IGameState> TGameStatePtr;

class IGameState
{
public:
   virtual ~IGameState() {}

   // If the game state needs to do some time-consuming work, this is the place
   virtual void OnLoadData() {}

   // The game state is set to active (i.e. foreground).
   virtual void OnActivate(EDialogResult aDialogResult) = 0;

   // Draw the current frame
   virtual void OnDraw() = 0;

   // Execute a tick event (usually right before drawing)
   // Returns true, if the tick counter should be reset (e.g. after loading data)
   virtual bool OnTick() = 0;

   // Return the required graphics for this state
   virtual Client::TRequiredResources GetRequiredResources() const = 0;

   /**
    * Handle an input action from the touch screen or mouse.
    *
    * @arg eventID Identifier for the action pointer for POINTER_* and MOVE action.
    * @arg action The input action type.
    * @arg x X coordinate of the touch or mouse action.
    * @arg y Y coordinate of the touch or mouse action.
    */
   virtual void OnTouch(const TTouchEvent& aEvent) = 0;

   // ToDo
   virtual void OnKeyDown(int32_t /*aKeyCode*/) {
      // do nothing
   }

   /**
    * onBackKey is called when the back key is pressed.
    * The default implementation returns true to exit the application.
    * However, specific game states can return false to not exit.
    * This method is only called on Android.
    */
   virtual bool OnBackKey() {
      return true; // exit the application
   }

   /**
    * onMenuKey is called when the menu key is pressed.
    * The default implementation does nothing, but specific
    * game states can use this as an input (e.g. show the menu).
    */
   virtual void OnMenuKey() {
      // do nothing
   }

   // Get the currently desired FPS (ticks per second are fixed at 60)
   virtual int32_t GetDesiredFrameRate() const = 0;

protected:
   // Execute a tick while another window has focus (usually do nothing)
   virtual void OnBackgroundTick() {}

   // Return the parent state of the this state, or empty if this is the base state
   virtual TGameStatePtr GetParentState() const
   {
      return TGameStatePtr(); // Default: no parent state
   }

   // Return the base state of the current state (if the current state is the base state, it returns itself)
   IGameState& GetBaseState()
   {
      TGameStatePtr parent = GetParentState();
      if (parent) {
         return parent->GetBaseState();
      }
      return *this;
   }
};

#endif // AK_IGAMESTATE_H_INCLUDED
