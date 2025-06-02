#include "../../headers/user_screen.h"

/**
 * @brief Инициализирует библиотеку ncurses для работы с интерфейсом.
 *
 * Эта функция настраивает режим работы ncurses, включая:
 * - Включение режима кода управления (cbreak), который позволяет
 *   программе реагировать на ввод символов мгновенно, без необходимости
 * ожидания нажатия клавиши Enter.
 * - Отключение отображения вводимых символов на экране (noecho).
 * - Активирование клавиатурного ввода для специальных клавиш (keypad).
 * - Установка режима не блокирующего ввода (nodelay), позволяющего программе
 * продолжать выполнение, даже если на клавиатуре нет ввода.
 * - Скрытие курсора (curs_set).
 *
 * @note Функция должна быть вызвана перед использованием других функций
 * библиотеки ncurses.
 */
void setupNcurses() {
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, true);
  nodelay(stdscr, true);
  curs_set(0);
}

/**
 * @brief Считывает нажатие от пользователя и передает его в функцию userInput.
 */
UserAction_t readInput() {
  UserAction_t user_action = -1;
  int input = getch();
  switch (input) {
    case '\n':
      user_action = Start;
      break;
    case 'P':
    case 'p':
      user_action = Pause;
      break;
    case 'q':
    case 'Q':
    case 27:
      user_action = Terminate;
      break;
    case 'A':
    case 'a':
    case KEY_LEFT:
      user_action = Left;
      break;
    case 'D':
    case 'd':
    case KEY_RIGHT:
      user_action = Right;
      break;
    case 'W':
    case 'w':
    case KEY_UP:
      user_action = Up;
      break;
    case 'S':
    case 's':
    case KEY_DOWN:
      user_action = Down;
      break;
    case ' ':
      user_action = Action;
      break;
  }
  return user_action;
}

/**
 * @brief Отрисовывает весь экран игры.
 * @param game_info Структура содержащая информацию об игре.
 *
 * Содержит в себе функции отрисовки разных частей игры:
 * -Границы полей
 * -Игровое поле
 * -Поле следующей фигуры
 * -Поле статистики
 */
void drawField(GameInfo_t game_info) {
  drawFieldBorder();
  drawGameField(game_info);
  drawNextFigure(game_info);
  drawStatField(game_info);
  if (game_info.speed < 0) {  // Экран позора
    drawLoseScreen(game_info);
  } else if (game_info.speed == 0) {  // Надпись начала игры
    drawStartText();
  }
}

/**
 * @brief Отрисовывает границы игрового поля.
 */
void drawFieldBorder() {
  for (int y = 0; y < 20 + 2; y++) {
    mvaddch(y + BORDER_OFFSET, BORDER_OFFSET, ACS_CKBOARD);
    mvaddch(y + BORDER_OFFSET, BORDER_OFFSET + (10 * SCALE_X) + 1, ACS_CKBOARD);
  }
  for (int x = 0; x < (10 * SCALE_X) + 1; x++) {
    mvaddch(BORDER_OFFSET, x + BORDER_OFFSET, ACS_CKBOARD);
    mvaddch(BORDER_OFFSET + 21, x + BORDER_OFFSET, ACS_CKBOARD);
  }

  // Границы игрового интерфейса
  for (int y = 0; y < 20 + 2; y++) {
    mvaddch(y + BORDER_OFFSET, BORDER_OFFSET + (10 * SCALE_X) + 3, ACS_CKBOARD);
    mvaddch(y + BORDER_OFFSET, BORDER_OFFSET + (20 * SCALE_X) + 4, ACS_CKBOARD);
  }
  for (int x = 0; x <= (10 * SCALE_X) + 1; x++) {
    mvaddch(BORDER_OFFSET, x + BORDER_OFFSET + (10 * SCALE_X) + 3,
            ACS_CKBOARD);  // Верхняя граница
    mvaddch(BORDER_OFFSET + 9, x + BORDER_OFFSET + (10 * SCALE_X) + 3,
            ACS_CKBOARD);  // Граница которая отделяет поле со следующей фигурой
                           // от информации
    mvaddch(BORDER_OFFSET + 21, x + BORDER_OFFSET + (10 * SCALE_X) + 3,
            ACS_CKBOARD);  // Нижняя граница
  }
}

/**
 * @brief Отрисовывает игровое поле.
 * @param game_info Структура содержащая информацию об игре.
 */
void drawGameField(GameInfo_t game_info) {
  for (int y = 0; y < 20; y++) {
    for (int x = 0; x < 10; x++) {
      if ((game_info.field[y][x] == 1) || (game_info.field[y][x] == 2)) {
        mvaddch(y + 2, (x * SCALE_X) + 2, ' ' | A_REVERSE);
        mvaddch(y + 2, (x * SCALE_X) + 3, ' ' | A_REVERSE);
      }
    }
  }
}

/**
 * @brief Отрисовывает поле показываюшее следующую фигуру.
 * @param game_info Структура содержащая информацию об игре.
 */
void drawNextFigure(GameInfo_t game_info) {
  const char *next_header = "NEXT";
  mvaddstr(3, 33, next_header);
  for (int x = 0; x < 4; x++) {
    for (int y = 0; y < 4; y++) {
      if (game_info.next[y][x] == 1) {
        mvaddch(y + Y_START_NEXT, (x * SCALE_X) + X_START_NEXT,
                ' ' | A_REVERSE);
        mvaddch(y + Y_START_NEXT, (x * SCALE_X) + X_START_NEXT + 1,
                ' ' | A_REVERSE);
      }
    }
  }
}

/**
 * @brief Отрисовывает окно статистики.
 * @param game_info Структура содержащая информацию об игре.
 */
void drawStatField(GameInfo_t game_info) {
  int indent = 0;
  char buffer[30] = {0};
  snprintf(buffer, sizeof(buffer), "Level: %d", game_info.level);
  mvaddstr(Y_START_STAT + indent, X_START_STAT, buffer);
  static double speed = 0;
  if (game_info.speed == 0)
    snprintf(buffer, sizeof(buffer), "Speed: 0");
  else if (game_info.speed == -1)
    snprintf(buffer, sizeof(buffer), "Speed: %.4lf", speed);
  else {
    speed = 100 / (120.0 - game_info.speed);
    snprintf(buffer, sizeof(buffer), "Speed: %.4lf", speed);
  }
  mvaddstr(Y_START_STAT + indent, X_START_STAT, buffer);
  indent += 2;
  snprintf(buffer, sizeof(buffer), "Level: %d", game_info.level);
  mvaddstr(Y_START_STAT + indent, X_START_STAT, buffer);
  indent += 2;
  snprintf(buffer, sizeof(buffer), "Score: %d", game_info.score);
  mvaddstr(Y_START_STAT + indent, X_START_STAT, buffer);
  indent += 2;
  if (game_info.high_score > game_info.score) {
    snprintf(buffer, sizeof(buffer), "High: %d", game_info.high_score);
  } else {
    snprintf(buffer, sizeof(buffer), "High: %d", game_info.score);
  }
  mvaddstr(Y_START_STAT + indent, X_START_STAT, buffer);
  indent += 2;
  if (game_info.pause) {
    snprintf(buffer, sizeof(buffer), "Pause: ON");
  } else {
    snprintf(buffer, sizeof(buffer), "Pause: OFF");
  }
  mvaddstr(Y_START_STAT + indent, X_START_STAT, buffer);
}

/**
 * @brief Отрисовывает экран поражения.
 * @param game_info Структура содержащая информацию об игре.
 */
void drawLoseScreen(GameInfo_t game_info) {
  int indent = 0;
  char buffer[30] = {0};
  for (int x = 0; x < 8; x++) {
    for (int y = 0; y < 9; y++) {
      if (x == 0 || x == 7 || y == 0 || y == 8) {
        mvaddch(y + Y_START_LOSE_SCREEN, (x * SCALE_X) + X_START_LOSE_SCREEN,
                ACS_CKBOARD);
        mvaddch(y + Y_START_LOSE_SCREEN,
                (x * SCALE_X) + X_START_LOSE_SCREEN + 1, ACS_CKBOARD);
      } else {
        mvaddch(y + Y_START_LOSE_SCREEN, (x * SCALE_X) + X_START_LOSE_SCREEN,
                ' ');
        mvaddch(y + Y_START_LOSE_SCREEN,
                (x * SCALE_X) + X_START_LOSE_SCREEN + 1, ' ');
      }
    }
  }
  indent += 2;
  snprintf(buffer, sizeof(buffer), "You Lose");
  mvaddstr(Y_START_LOSE_SCREEN + indent, X_TEXT_SCREEN + 1, buffer);
  indent += 2;
  snprintf(buffer, sizeof(buffer), "Your Score");
  mvaddstr(Y_START_LOSE_SCREEN + indent, X_TEXT_SCREEN, buffer);
  indent += 2;
  snprintf(buffer, sizeof(buffer), "%8d", game_info.score);
  mvaddstr(Y_START_LOSE_SCREEN + indent, X_TEXT_SCREEN + 1, buffer);
}

/**
 * @brief Отрисовывает надпись начала игры.
 */
void drawStartText() {
  char buffer[30] = {0};
  snprintf(buffer, sizeof(buffer), "Press Enter");
  mvaddstr(Y_TEXT_START, X_TEXT_SCREEN, buffer);
}