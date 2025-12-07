#include "brickgame.h"

#include <iostream>

#include "brick_game/snake/snake.h"
#ifdef __cplusplus
extern "C" {
#endif
#include "brick_game/tetris/tetris.h"
#include "gui/cli/user_screen.h"
#ifdef __cplusplus
}
#endif

int main() {
  int interface = 0, game = 0;
  std::cout << "Выберите интерфейс:\n 1 - Терминал\n 2 - Приложение"
            << std::endl;
  std::cin >> interface;
  std::cout << "Выберите игру:\n 1 - тетрис\n 2 - змейка" << std::endl;
  std::cin >> game;
  if (interface == 1 && game == 1) {
    BrickGameConsole(userInput, updateCurrentState);
  } else if (interface == 2 && game == 1) {
    BrickGameDescktop(userInput, updateCurrentState);
  } else if (interface == 1 && game == 2) {
    BrickGameConsole(s21::SnakeAdapter::userInput,
                     s21::SnakeAdapter::updateCurrentState);
  } else if (interface == 2 && game == 2) {
    BrickGameDescktop(s21::SnakeAdapter::userInput,
                      s21::SnakeAdapter::updateCurrentState);
  } else {
    std::cout << "Такого нету" << std::endl;
  }
  return 0;
}

void BrickGameConsole(void (*userInput)(UserAction_t, bool),
                      GameInfo_t (*updateCurrentState)()) {
  SetupNcurses();
  GameInfo_t game_info = {0};
  while (true) {
    userInput(ReadInput(), true);
    game_info = updateCurrentState();
    if (!game_info.field || !game_info.next) break;
    clear();
    DrawField(game_info);
    refresh();
    napms(20);
  }
  curs_set(1);
  endwin();
}

int BrickGameDescktop(void (*userInput)(UserAction_t, bool),
                      GameInfo_t (*updateCurrentState)()) {
  char **argv = nullptr;
  int argc = 0;
  QApplication app(argc, argv);
  s21::GameWidget window(userInput, updateCurrentState);
  window.setWindowTitle("BrickGameV2.0");
  window.setFixedSize(380, 420);
  window.show();
  return app.exec();
}