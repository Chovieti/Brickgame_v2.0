#ifndef LIB_SPEC_H
#define LIB_SPEC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../lib_struct.h"
#include <stdbool.h>

void userInput(UserAction_t action, bool hold);
GameInfo_t updateCurrentState();

#ifdef __cplusplus
}
#endif

#endif