#ifndef BRICKGAME_H
#define BRICKGAME_H

#include "brick_game/snake/snake.h"
#ifdef __cplusplus
extern "C" {
#endif

#include "headers/user_screen.h"
#include "headers/lib_spec.h"

void brickGameConsole(void (*userInput)(UserAction_t, bool), GameInfo_t (*updateCurrentState)());

#ifdef __cplusplus
}
#endif
int brickGameDescktop(void (*userInput)(UserAction_t, bool), GameInfo_t (*updateCurrentState)());

#endif