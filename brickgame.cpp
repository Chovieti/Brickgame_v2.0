#include <iostream>

// #include <QApplication>
// #include <QWidget>       // Базовый класс виджетов
// #include <QPainter>        // Для рисования на виджете
// #include <QVector>

#include "brickgame.h"

int main() {
  int interface = 0, game = 0;
  // brickGameDescktop(s21::s_io::userInput, s21::s_io::updateCurrentState);
  std::cout << "Выберите интерфейс:\n 1 - Терминал\n 2 - Приложение"
            << std::endl;
  std::cin >> interface;
  std::cout << "Выберите игру:\n 1 - тетрис\n 2 - змейка" << std::endl;
  std::cin >> game;
  if (interface != 2 && game == 1) {
    brickGameConsole(userInput, updateCurrentState);
  } else if (interface == 2 && game == 1) {
    std::cout << "Змейка ешё не сделана" << std::endl;
    brickGameDescktop(userInput, updateCurrentState);
  } else if (interface == 1 && game == 2) {
    brickGameConsole(s21::userInput, s21::updateCurrentState);
  } else if (interface == 2 && game == 2) {
    brickGameDescktop(s21::userInput, s21::updateCurrentState);
  } else
    std::cout << "Такого нету" << std::endl;
  return 0;
}

/**
 * @brief Движок всей игры включающий создание экрана с помощью ncurses и
 * последующую отрисовку в нем.
 */
void brickGameConsole(void (*userInput)(UserAction_t, bool),
                      GameInfo_t (*updateCurrentState)()) {
  setupNcurses();
  static GameInfo_t game_info = {0};
  while (true) {
    userInput(readInput(), true);
    game_info = updateCurrentState();
    if (game_info.field == NULL || game_info.next == NULL) break;
    clear();
    drawField(game_info);
    refresh();
    napms(20);
  }
  curs_set(1);
  endwin();
}

// int brickGameDescktop() {
//   char **argv = nullptr;
//   int argc = 0;
//   QApplication app(argc, argv);

//   // Главное окно
//   MatrixWidget window;
//   window.setWindowTitle("Qt Frame Example");

//   window.show();
//   return app.exec();
// }