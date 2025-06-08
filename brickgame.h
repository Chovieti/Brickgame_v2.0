#ifndef BRICKGAME_V2_0_BRICKGAME_H
#define BRICKGAME_V2_0_BRICKGAME_H

#include "gui/desktop/desktop_screen.h"
#ifdef __cplusplus
extern "C" {
#endif

void BrickGameConsole(void (*userInput)(UserAction_t, bool),
                      GameInfo_t (*updateCurrentState)());

#ifdef __cplusplus
}
#endif
int BrickGameDescktop(void (*userInput)(UserAction_t, bool),
                      GameInfo_t (*updateCurrentState)());

#endif  // BRICKGAME_V2_0_BRICKGAME_H