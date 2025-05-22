#include <iostream>


// #include <QApplication>
// #include <QWidget>       // Базовый класс виджетов
// #include <QPainter>        // Для рисования на виджете
// #include <QVector> 


#include "brickgame.h"


int main() {
    int game = 0;
    // brickGameDescktop(userInput, updateCurrentState);
    std::cout << "Выберите игру:\n 1 - тетрис\n 2 - змейка" << std::endl;
    std::cin >> game;
    if (game == 1)
      brickGameConsole(userInput, updateCurrentState);
    else if (game == 2) {
      std::cout << "Змейка ешё не сделана" << std::endl;
      brickGameDescktop(userInput, updateCurrentState);
    }
    else
      std::cout << "Такого нету" << std::endl;
    return 0;
}

/**
 * @brief Движок всей игры включающий создание экрана с помощью ncurses и
 * последующую отрисовку в нем.
 */
void brickGameConsole(void (*userInput)(UserAction_t, bool), GameInfo_t (*updateCurrentState)()) {
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