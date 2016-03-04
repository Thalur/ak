/**
 *
 */
#ifndef AK_GAMECONTROL_H_INCLUDED
#define AK_GAMECONTROL_H_INCLUDED

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
   virtual ~IGameControl() {}

   virtual void SwitchGameState(const TGameStatePtr& aNewState) = 0;
};

#endif // AK_GAMECONTROL_H_INCLUDED
