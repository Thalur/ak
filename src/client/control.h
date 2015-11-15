/**
 *
 */
#ifndef AK_CONTROL_H_INCLUDED
#define AK_CONTROL_H_INCLUDED

#include "client/state/GameState.h"


const CGameStatePtr& GetCurrentState();
int32_t GetTickRate();

void OnInit();
bool OnTick();
void OnDraw(int32_t aFps);

#endif // AK_CONTROL_H_INCLUDED
