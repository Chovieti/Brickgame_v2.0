#ifndef SNAKE_H
#define SNAKE_H

#include "../../headers/lib_spec.h"

namespace s21 {
class Snake {
    public:
    Snake();
    ~Snake();

    void userInput(UserAction_t action, bool hold);
    GameInfo_t updateCurrentState();
    private:
};
}

#endif