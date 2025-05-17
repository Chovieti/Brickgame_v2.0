#include "../../headers/tetris.h"
// #include "../headers/user_screen.h"

/**
 * @brief Обрабатывает ввод.
 * @param action Тип нажатой клавиши.
 * @param hold Зажатие клавиши.
 */
void userInput(UserAction_t action, bool hold) {
  GameInfo_t *game_info = getGameInfo_t(false);
  bool collision = checkCollisionForMoving(action, *game_info);
  FigureInfo *info = getFigureInfo(*game_info, 0);
  if (action == Left && hold == true && !collision && game_info->pause == 0 &&
      info->moving) {
    for (int i = 0; i < 20; i++) {
      for (int j = 0; j < 10; j++) {
        if (game_info->field[i][j] == 1) {
          game_info->field[i][j - 1] = 1;
          game_info->field[i][j] = 0;
        }
      }
    }
  }
  if (action == Right && hold == true && !collision && game_info->pause == 0 &&
      info->moving) {
    for (int i = 0; i < 20; i++) {
      for (int j = 9; j >= 0; j--) {
        if (game_info->field[i][j] == 1 && j != 9) {
          game_info->field[i][j + 1] = 1;
          game_info->field[i][j] = 0;
        }
      }
    }
  }
  if (action == Action && hold == true && game_info->pause == 0) {
    if (!(checkEdgeForRotate(*info) ||
          checkCollisionForRotate(*info, *game_info) ||
          info->moving == false)) {
      rotateTetramino(info, game_info);
    }
  }
  if (action == Down && hold == true && game_info->pause == 0) {
    while (fsmShift(game_info, info) != Attaching);
    getFigureInfo(*game_info, -2);
  }
  if (action == Pause && hold == true) {
    if (game_info->pause == 0 && game_info->speed > 0)
      game_info->pause = 1;
    else
      game_info->pause = 0;
  }
  if (action == Start && hold == true &&
      (game_info->speed <= 0 || game_info->pause == 1)) {
    game_info->speed = 1;
  }
  if (action == Terminate && hold == true) {
    getGameInfo_t(true);
  }
}

/**
 * @return Возвращает копию структуры для отрисовк.
 */
GameInfo_t updateCurrentState() {
  GameInfo_t *game_info = getGameInfo_t(false);
  return *game_info;
}