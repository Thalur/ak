/**
 *
 */
#ifndef AK_CONTROL_H_INCLUDED
#define AK_CONTROL_H_INCLUDED

#include "client/state/GameState.h"


const CGameStatePtr& GetCurrentState();
TInt32 GetTickRate();

void OnInit();
bool OnTick();
void OnDraw(TInt32 aFps);

#endif // AK_CONTROL_H_INCLUDED
