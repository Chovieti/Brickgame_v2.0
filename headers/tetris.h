#ifndef TETRIS_H
#define TETRIS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#ifdef __linux__
#include <time.h>
#endif

#include "lib_spec.h"
/**
 * @brief Количество строк и столбцов в матрицы отвечающий за игровое поле.
 *
 * @note Значения:
 * - ROWS: Количество строк.
 * - COLUMNS: Количество столбцов.
 */
#define ROWS 20
#define COLUMNS 10
/**
 * @brief Количество строк и столбцов в матрице хранящей следующую фигуру.
 */
#define NEXT_ROWS_AND_COLUMNS 4

/**
 * @struct FigureInfo
 * @brief Структура для хранения информации об текущей и следующей фигуры.
 *
 * Хранит различные параметры фигуры, такие как вид, в каком положении,
 * двигается или нет, координаты для поиска на поле.
 */
typedef struct {
  int coor_x;
  int coor_y;
  int type_figure;
  int type_position;
  int next_type_figure;
  int next_type_position;
  bool moving;
} FigureInfo;

/**
 * @enum FigureInfo
 * @brief Состояние игровой логики.
 *
 * Позволяет определить, на каком этапе сейчас находится игра.
 */
typedef enum {
  Start_Game, /**< Начало игры. */
  Spawn,      /**< Появление фигуры. */
  Shift,      /**< Падение фигуры по таймеру. */
  Attaching, /**< Проверка когда блок упал полностью, будет ли проходить */
             /**< уничтожение линий или нет. */
  Game_Over  /**< Конец игры. */
} field_fsm;

/**
 * @enum tetromino
 * @brief Перечисление всех фигур Тетриса.
 */
typedef enum {
  FIGURE_1,  // Длинная палка
  FIGURE_2,  // Г-фигура
  FIGURE_3,  // L-фигура
  FIGURE_4,  // Квадрат
  FIGURE_5,  // Змея с головой поднятой справа
  FIGURE_6,  // Т-образная фигура
  FIGURE_7   // Змея с головой поднятой слева
} tetromino;

double getTimeDifference(struct timeval start_time,
                         struct timeval current_time);
GameInfo_t *getGameInfo_t(bool switch_flag);
bool initialGameField(GameInfo_t *game_info, bool *switch_flag,
                      struct timeval *start_time);
void failFree(int **field, int i);
FigureInfo *getFigureInfo(GameInfo_t game_info, int type);
void getCoorFigure(GameInfo_t game_info, FigureInfo *figure_info);
bool checkCollisionForMoving(UserAction_t action, GameInfo_t game_info);
void checkCollisionForMovingLeft(GameInfo_t game_info, bool *collision);
void checkCollisionForMovingRight(GameInfo_t game_info, bool *collision);
void checkCollisionForMovingDown(GameInfo_t game_info, bool *collision);
void rotateTetramino(FigureInfo *figure_info, GameInfo_t *game_info);
void rotateFigureI(FigureInfo *figure_info, GameInfo_t *game_info);
void rotateFigureGLT(FigureInfo *figure_info, GameInfo_t *game_info);
void rotateFigureSZ(FigureInfo *figure_info, GameInfo_t *game_info);
bool checkEdgeForRotate(FigureInfo figure_info);
bool checkCollisionForRotate(FigureInfo figure_info, GameInfo_t game_info);
field_fsm fsmField(GameInfo_t *game_info);
field_fsm fsmStartGame(GameInfo_t *game_info);
void nextTetromino(GameInfo_t *game_info, int tetromino);
field_fsm fsmSpawn(GameInfo_t *game_info);
int checkSpawnPossibility(GameInfo_t game_info, int *real_spawn);
field_fsm fsmShift(GameInfo_t *game_info, FigureInfo *figure_info);
field_fsm fsmAttaching(GameInfo_t *game_info);
void erasingLine(GameInfo_t *game_info, int i);
field_fsm fsmGameOver(GameInfo_t *game_info);
void saveScore(int new_score);
int readScore();

#ifdef __cplusplus
}
#endif

#endif