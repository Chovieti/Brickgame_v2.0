#ifndef USER_SCREEN_H
#define USER_SCREEN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lib_spec.h"

#include <ncurses.h>


/**
 * @brief Отступ всех полей от края экрана.
 */
#define BORDER_OFFSET 1
/**
 * @brief В стандартном терминале один символ занимает 2x1(YxX) пикселя. SCALE_X
 * уравнивает и делает 2x2.
 */
#define SCALE_X 2
/**
 * @brief Задает координаты по которым будет выводиться окошко со следующей
 * фигурой.
 *
 * @note Значения:
 * - Y_START_NEXT: начальная координата Y.
 * - X_START_NEXT: начальная координата X.
 */
#define Y_START_NEXT 5  ///< Начальная координата Y для окна с фигурой.
#define X_START_NEXT 31  ///< Начальная координата X для окна с фигурой.
/**
 * @brief Задает координаты, по которым будет выводиться окно со статистикой.
 *
 * @note Значения:
 * - Y_START_STAT: начальная координата Y для окна со статистикой.
 * - X_START_STAT: начальная координата X для окна со статистикой.
 */
#define Y_START_STAT 12  ///< Начальная координата Y для окна со статистикой.
#define X_START_STAT 26  ///< Начальная координата X для окна со статистикой.

/**
 * @brief Задает координаты, по которым будет выводиться экран при проигрыше и
 * текст в нём.
 *
 * @note Значения:
 * - Y_START_LOSE_SCREEN: начальная координата Y для экрана при проигрыше.
 * - X_START_LOSE_SCREEN: начальная координата X для экрана при проигрыше.
 */
#define Y_START_LOSE_SCREEN \
  7  ///< Начальная координата Y для экрана при проигрыше.
#define X_START_LOSE_SCREEN \
  4  ///< Начальная координата X для экрана при проигрыше.
/**
 * @brief Задает координату по оси X по которой будет выводиться надпись на
 * экране старта/проигрыше.
 */
#define X_TEXT_SCREEN 7
/**
 * @brief Задает координату по оси Y по которой будет выведена надпись о старте
 * игры
 */
#define Y_TEXT_START 11

// void brickGame();
void setupNcurses();
// void readInput();
UserAction_t readInput();
void drawField(GameInfo_t game_info);
void drawFieldBorder();
void drawGameField(GameInfo_t game_info);
void drawNextFigure(GameInfo_t game_info);
void drawStatField(GameInfo_t game_info);
void drawLoseScreen(GameInfo_t game_info);
void drawStartText();

#ifdef __cplusplus
}
#endif

#endif