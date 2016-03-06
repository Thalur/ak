/**
 *
 */
#ifndef AK_IGAMECONTROL_H_INCLUDED
#define AK_IGAMECONTROL_H_INCLUDED

#include "client/main/IEngineControl.h"
#include <memory>

class IGameState;
using TGameStatePtr = std::shared_ptr<IGameState>;

/**
 * Callback methods for the game logic (in game states) to the game engine
 * for "global" access to application functionality and data.
 */
class IGameControl
{
public:
   IGameControl() : iEngineControl(nullptr) {}
   virtual ~IGameControl() {}

   virtual void SwitchGameState(const TGameStatePtr& aNewState) = 0;
   virtual void DrawSoundUI() = 0;

   Client::IEngineControl* Engine() { return iEngineControl; }

protected:
   Client::IEngineControl* iEngineControl;
};

#endif // AK_IGAMECONTROL_H_INCLUDED
