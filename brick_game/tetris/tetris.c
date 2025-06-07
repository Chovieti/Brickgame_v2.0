#include "../../headers/tetris.h"

/**
 * @param start_time Время с которого идет отсчет.
 * @param current_time Прошедшее время.
 * @return Возвращает разницу между прошедшим временем и временем отсчета.
 */
double getTimeDifference(struct timeval start_time,
                         struct timeval current_time) {
  return (current_time.tv_sec - start_time.tv_sec) * 100.0 +
         (current_time.tv_usec - start_time.tv_usec) / 10000.0;
}

/**
 * @brief Получает информацию о текущей игре.
 * @param switch_flag Флаг переключения. Если true, игра будет сброшена и
 * освободится память, иначе информация о текущей игре будет обновлена и
 * возвращена.
 *
 * Эта функция инициализирует структуру GameInfo_t, обрабатывает состояние игры
 * и возвращает указатель на нее. Если функция вызывается с флагом переключения,
 * происходит сброс игровой информации и освобождается память, занимаемая полями
 * игры.
 *
 * @return Указатель на `GameInfo_t`, содержащий текущую информацию об игре.
 *
 * @note Функция использует статические переменные для хранения состояния игры,
 *       поэтому последующие вызовы будут запоминать предыдущие состояния.
 */
GameInfo_t *getGameInfo_t(bool switch_flag) {
  static GameInfo_t game_info = {0};
  static int initialized = 0;  // 0 - выключено, 1 - включено, 2 -выключение
  static struct timeval start_time = {0};
  if (!initialized) {
    if (initialGameField(&game_info, &switch_flag, &start_time))
      initialized = 1;
  }
  if (initialized == 1 && !switch_flag) {
    static field_fsm state_game = Start_Game;
    struct timeval current_time = {0};
    gettimeofday(&current_time, NULL);
    if (game_info.speed == 1) {
      state_game = Start_Game;
      game_info.pause = 0;
    }
    double elapsed_time = getTimeDifference(start_time, current_time);
    if (((elapsed_time >= (120 - game_info.speed) && state_game == Shift) ||
         (state_game != Shift)) &&
        (game_info.pause == 0) && (game_info.speed > 0)) {
      start_time = current_time;
      state_game = fsmField(&game_info);
    }
  } else if (initialized == 1 && switch_flag) {
    if (game_info.field != NULL && game_info.next != NULL) {
      for (int i = 0; i < ROWS; i++) {
        if (game_info.field[i] != NULL) free(game_info.field[i]);
        if (i < NEXT_ROWS_AND_COLUMNS && game_info.next[i] != NULL) {
          free(game_info.next[i]);
          game_info.next[i] = NULL;
        }
      }
      free(game_info.field), free(game_info.next);
      game_info.field = NULL, game_info.next = NULL;

      if (game_info.score >= game_info.high_score) {
        saveScore(game_info.score);
      }
      game_info.high_score = 0, game_info.score = 0;
      game_info.level = 0, game_info.speed = 0;
      game_info.pause = 0;
      initialized = 2;
    }
  }
  return &game_info;
}

/**
 * @brief Инициализирует игровое поле и поле для следующей фигуры.
 * @param game_info Указатель на структуру GameInfo_t, содержащую информацию об
 * игре.
 * @param switch_flag Флаг, указывающий, нужно ли переключить состояние. Если
 * это значение true и при чтении высокого счета происходит ошибка, то будет
 * установлено значение true для switch_flag.
 * @param start_time Указатель на структуру struct timeval, которая будет
 * заполнена текущим временем.
 *
 * Эта функция выделяет память для текущего игрового поля и следующей фигуры.
 * При успешном выделении памяти функция устанавливает начальное значение
 * высокого счета и получает текущее время.
 *
 * @return true, если игровое поле было успешно инициализировано, иначе false.
 *
 * @note Функция использует динамическое выделение памяти, для освобождения
 * памяти необходимо использовать соответствующую функцию (например, блок
 * освобождения памяти в вызывающей функции).
 */
bool initialGameField(GameInfo_t *game_info, bool *switch_flag,
                      struct timeval *start_time) {
  bool initialized = false, error = false;
  game_info->field = (int **)calloc(ROWS, sizeof(int *));
  if (game_info->field == NULL) error = true;
  for (int i = 0; i < ROWS && !error; i++) {
    game_info->field[i] = (int *)calloc(COLUMNS, sizeof(int));
    if (game_info->field[i] == NULL) failFree(game_info->field, i);
  }
  if (!error)
    game_info->next = (int **)calloc(NEXT_ROWS_AND_COLUMNS, sizeof(int *));
  if (game_info->next == NULL) error = true;
  for (int i = 0; i < NEXT_ROWS_AND_COLUMNS && !error; i++) {
    game_info->next[i] = (int *)calloc(NEXT_ROWS_AND_COLUMNS, sizeof(int));
    if (game_info->next[i] == NULL) failFree(game_info->next, i);
  }
  if ((game_info->field != NULL || game_info->next != NULL) && !error) {
    initialized = true;
    gettimeofday(start_time, NULL);
    game_info->high_score = readScore();
    if (game_info->high_score == -1) {
      (*switch_flag) = true;
    }
  }
  return initialized;
}

/**
 * @brief Освобождает память при не успешном выделении.
 * @param field Поле для которого не выделилась полностью память.
 * @param int Индекс на котором память не смогла выделиться.
 */
void failFree(int **field, int i) {
  for (int j = 0; j < i; j++) {
    free(field[j]);
  }
  free(field);
  field = NULL;
}

/**
 * @brief Получает информацию о фигуре на игровом поле.
 *
 * @param game_info Структура GameInfo_t, содержащая информацию об игре.
 * @param type Тип фигуры или управление движением:
 * - Положительные значения указывают на тип новой фигуры.
 * - Ноль может использоваться для других целей.
 * - Отрицательные значения управляют движением (например, -1
 *   для поворота, -2 для проверки столкновения).
 *
 * Эта функция управляет состоянием фигуры в игре, обновляя ее тип и
 * положение в зависимости от переданного параметра type. Если type
 * больше нуля, она устанавливает текущую фигуру в зависимости от
 * next_type_figure. Если type равен или меньше нуля,
 * функция проверяет возможность движения фигуры и обновляет ее
 * координаты на игровом поле.
 *
 * @return Указатель на структуру FigureInfo, содержащую информацию
 *         о текущей и следующей фигуре, а также ее состоянии.
 *
 * @note Функция использует статическую переменную для хранения информации
 *       о фигуре, что означает, что состояние будет сохраняться между
 *       вызовами функции.
 */
FigureInfo *getFigureInfo(GameInfo_t game_info, int type) {
  static FigureInfo info = {0};
  if (type > 0) {
    info.type_figure = info.next_type_figure;
    info.type_position = info.next_type_position;
    if (type == 2 || type == 6) {
      info.next_type_position = 3;
    } else {
      info.next_type_position = 0;
    }
    info.moving = true;
    info.next_type_figure = type;
  } else {
    if (type == -2) info.moving = false;
    getCoorFigure(game_info, &info);
    if (type == -1) {
      info.type_position++;
      if (info.type_figure == 2 || info.type_figure == 3 ||
          info.type_figure == 6)
        info.type_position %= 4;
      else
        info.type_position %= 2;
    }
  }
  return &info;
}

/**
 * @brief Получает координаты фигуры.
 * @param game_info Структура содержащая информацию о поле, на котором и
 * находится фигура.
 * @param figure_info Структура содержащая информацию о фигуре, включая тип и
 * положение.
 *
 * Ищет по полю пока не найдет достаточное количество блоков фигуры. Количество
 * зависит от типа фигуры и в каком она находится положении.
 */
void getCoorFigure(GameInfo_t game_info, FigureInfo *figure_info) {
  bool found = false;
  int count_found = 0;
  for (int i = 0; i < 20; i++) {
    for (int j = 0; j < 10 && !found; j++) {
      if (game_info.field[i][j] == 1) count_found++;
      if ((figure_info->type_figure == 1 && count_found == 2) ||
          (figure_info->type_figure == 4 && count_found == 1)) {
        figure_info->coor_x = j - 1;
        figure_info->coor_y = i - 1;
        found = true;
      } else if (((figure_info->type_figure == 2 ||
                   figure_info->type_figure == 3) &&
                  figure_info->type_position / 2 == 0 && count_found == 2) ||
                 ((figure_info->type_figure == 2 ||
                   figure_info->type_figure == 3) &&
                  figure_info->type_position / 2 == 1 && count_found == 3)) {
        figure_info->coor_x = j - 1;
        figure_info->coor_y = i - 1;
        found = true;
      } else if ((figure_info->type_figure == 5 && count_found == 1)) {
        figure_info->coor_x = j - 1;
        figure_info->coor_y = i - 1;
        found = true;
      } else if ((figure_info->type_figure == 6 &&
                  figure_info->type_position / 2 == 0 && count_found == 2) ||
                 (figure_info->type_figure == 6 &&
                  figure_info->type_position / 2 == 1 && count_found == 3)) {
        figure_info->coor_x = j - 1;
        figure_info->coor_y = i - 1;
        found = true;
      } else if (figure_info->type_figure == 7 && count_found == 2) {
        figure_info->coor_x = j - 1;
        figure_info->coor_y = i - 1;
        found = true;
      }
    }
  }
}

/**
 * @brief Проверяет врежется ли фигура при перемещение.
 * @param action Направление по которому проверяется может ли фигура сдвинуться
 * @param game_info Информация об игре содержащую поле на котором ищется фигура
 * и смотрится дальше по направлению есть ли дальше другая фигура или край поля.
 */
bool checkCollisionForMoving(UserAction_t action, GameInfo_t game_info) {
  bool collision = false;
  if (action == Left) {
    checkCollisionForMovingLeft(game_info, &collision);
  } else if (action == Right) {
    checkCollisionForMovingRight(game_info, &collision);
  } else if (action == Down) {
    checkCollisionForMovingDown(game_info, &collision);
  }
  return collision;
}

/**
 * @brief Проверяет врежется ли фигура при перемещение влево.
 * @param game_info Информация об игре содержащую поле на котором ищется фигура
 * и смотрится дальше по направлению есть ли дальше другая фигура или край поля.
 * @param collision Флаг который указывает врежется(true) или нет(false)
 */
void checkCollisionForMovingLeft(GameInfo_t game_info, bool *collision) {
  for (int j = 0; j < 10 && !(*collision); j++) {
    for (int i = 0; i < 20 && !(*collision); i++) {
      if (game_info.field[i][j] == 1) {
        if (j == 0)
          (*collision) = true;
        else if (game_info.field[i][j - 1] == 2)
          (*collision) = true;
      }
    }
  }
}

/**
 * @brief Проверяет врежется ли фигура при перемещение вправо.
 * @param game_info Информация об игре содержащую поле на котором ищется фигура
 * и смотрится дальше по направлению есть ли дальше другая фигура или край поля.
 * @param collision Флаг который указывает врежется(true) или нет(false)
 */
void checkCollisionForMovingRight(GameInfo_t game_info, bool *collision) {
  for (int j = 9; j >= 0 && !(*collision); j--) {
    for (int i = 0; i < 20 && !(*collision); i++) {
      if (game_info.field[i][j] == 1) {
        if (j == 9)
          (*collision) = true;
        else if (game_info.field[i][j + 1] == 2)
          (*collision) = true;
      }
    }
  }
}

/**
 * @brief Проверяет врежется ли фигура при перемещение вниз.
 * @param game_info Информация об игре содержащую поле на котором ищется фигура
 * и смотрится дальше по направлению есть ли дальше другая фигура или край поля.
 * @param collision Флаг который указывает врежется(true) или нет(false)
 */
void checkCollisionForMovingDown(GameInfo_t game_info, bool *collision) {
  for (int i = 19; i >= 0 && !(*collision); i--) {
    for (int j = 0; j < 10 && !(*collision); j++) {
      if (game_info.field[i][j] == 1) {
        if (i == 19)
          (*collision) = true;
        else if (game_info.field[i + 1][j] == 2)
          (*collision) = true;
      }
    }
  }
}

/**
 * @brief Поворачивает фигуру на поле.
 * @param figure_info Структура содержащая информацию о фигуре.
 * @param game_info Структура содержащая информацию об игре включая поле.
 */
void rotateTetramino(FigureInfo *figure_info, GameInfo_t *game_info) {
  switch (figure_info->type_figure) {
    case 1:  // Палка в первой позиции
      rotateFigureI(figure_info, game_info);
      break;
    case 2:  // Буква Г
    case 3:  // Буква L
    case 6:  // Буква T
      rotateFigureGLT(figure_info, game_info);
      break;
    case 5:  // Змея с головой поднятой справа
    case 7:  // Змея с головой поднятой слева
      rotateFigureSZ(figure_info, game_info);
      break;
  }
  getFigureInfo(*game_info, -1);
}

/**
 * @brief Поворачивает фигуру I на поле.
 * @param figure_info Структура содержащая информацию о фигуре.
 * @param game_info Структура содержащая информацию об игре включая поле.
 */
void rotateFigureI(FigureInfo *figure_info, GameInfo_t *game_info) {
  if (figure_info->type_position == 0) {
    for (int i = 0; i < 4; i++) {
      game_info->field[figure_info->coor_y + 1][figure_info->coor_x + i] = 0;
      game_info->field[figure_info->coor_y + i][figure_info->coor_x + 1] = 1;
    }
  } else {
    for (int i = 0; i < 4; i++) {
      game_info->field[figure_info->coor_y + i][figure_info->coor_x + 1] = 0;
      game_info->field[figure_info->coor_y + 1][figure_info->coor_x + i] = 1;
    }
  }
}

/**
 * @brief Поворачивает фигуру Г, L или T на поле.
 * @param figure_info Структура содержащая информацию о фигуре.
 * @param game_info Структура содержащая информацию об игре включая поле.
 */
void rotateFigureGLT(FigureInfo *figure_info, GameInfo_t *game_info) {
  int tmp_field[3][3] = {0};
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      tmp_field[j][2 - i] =
          game_info->field[figure_info->coor_y + i][figure_info->coor_x + j];
    }
  }
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      game_info->field[figure_info->coor_y + i][figure_info->coor_x + j] =
          tmp_field[i][j];
    }
  }
}

/**
 * @brief Поворачивает фигуру S или Z на поле.
 * @param figure_info Структура содержащая информацию о фигуре.
 * @param game_info Структура содержащая информацию об игре включая поле.
 */
void rotateFigureSZ(FigureInfo *figure_info, GameInfo_t *game_info) {
  int tmp_field[3][3] = {0};
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      if (figure_info->type_position == 0 || figure_info->type_figure == 5)
        tmp_field[j][2 - i] =
            game_info
                ->field[figure_info->coor_y + 1 + i][figure_info->coor_x + j];
      else
        tmp_field[j][2 - i] =
            game_info->field[figure_info->coor_y + i][figure_info->coor_x + j];
    }
  }
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      if (figure_info->type_position == 1 || figure_info->type_figure == 7)
        game_info->field[figure_info->coor_y + i][figure_info->coor_x + j] =
            tmp_field[i][j];
      else
        game_info->field[figure_info->coor_y + 1 + i][figure_info->coor_x + j] =
            tmp_field[i][j];
    }
  }
}

/**
 * @brief Проверяет мешает ли край поля поворачивать фигуру или нет.
 * @param figure_info Структура содержащая информацию о фигуре.
 * @return Возращает edge = true если край поля иначе false.
 */
bool checkEdgeForRotate(FigureInfo figure_info) {
  bool edge = false;
  if ((figure_info.type_figure == 1) &&
      (figure_info.coor_y < 0 || figure_info.coor_y > 16 ||
       figure_info.coor_x < 0 || figure_info.coor_x > 6))
    edge = true;
  else if (figure_info.coor_y < 0 || figure_info.coor_y > 16 ||
           figure_info.coor_x < 0 || figure_info.coor_x > 7)
    edge = true;
  return edge;
}

/**
 * @brief Проверяет мешают ли другие фигуры поворачивать текущую фигуру или нет.
 * @param figure_info Структура содержащая информацию о фигуре.
 * @param game_info Структура содержащая информацию о поле включая другие
 * фигуры.
 * @return Возращает collision = true если другие фигуры мешают иначе false.
 */
bool checkCollisionForRotate(FigureInfo figure_info, GameInfo_t game_info) {
  bool collision = false;
  int count_occupied_block = 0;
  if (figure_info.type_figure == 1 && (figure_info.coor_y + 1 > 0)) {
    for (int i = figure_info.coor_y; i <= figure_info.coor_y + 1; i++) {
      for (int j = figure_info.coor_x; j <= figure_info.coor_x + 1; j++) {
        if (game_info.field[i][j] == 2) count_occupied_block++;
      }
    }
    for (int i = figure_info.coor_y + 1; i <= figure_info.coor_y + 3; i++) {
      for (int j = figure_info.coor_x + 1; j <= figure_info.coor_x + 3; j++) {
        if (game_info.field[i][j] == 2) count_occupied_block++;
      }
    }
  } else {
    int coor_point_i = 0, coor_point_j = 0;
    if (figure_info.type_figure == 2 || figure_info.type_figure == 3 ||
        figure_info.type_figure == 6) {
      coor_point_i = figure_info.coor_y;
      coor_point_j = figure_info.coor_x;
    } else if ((figure_info.type_figure == 5 || figure_info.type_figure == 7) &&
               figure_info.coor_y + 1 > 0) {
      coor_point_i = figure_info.coor_y;
      coor_point_j = figure_info.coor_x;
    }
    for (int i = coor_point_i; i <= coor_point_i + 2; i++) {
      for (int j = coor_point_j; j <= coor_point_j + 2; j++) {
        if (game_info.field[i][j] == 2) count_occupied_block++;
      }
    }
  }
  if (count_occupied_block > 0) collision = true;
  return collision;
}

/**
 * @brief Реализует конечный автомат и переход из одного состояния игры в
 * другое.
 * @param game_info Структура содержащая информацию об игре.
 * @return Возращает текущее состояние игры.
 */
field_fsm fsmField(GameInfo_t *game_info) {
  static field_fsm game_state = Start_Game;
  static FigureInfo *info = {0};
  info = getFigureInfo(*game_info, 0);
  if (game_info->speed == 1) {
    game_state = Start_Game;
    game_info->speed = 10;
  }
  switch (game_state) {
    case Start_Game:
#ifdef __linux__
      srand(time(NULL));
#else
      srandom(time(NULL));
#endif
      game_state = fsmStartGame(game_info);
      break;
    case Spawn:
      if (!checkCollisionForMoving(Down, *game_info)) {
        game_state = fsmSpawn(game_info);
      }
      break;
    case Shift:
      game_state = fsmShift(game_info, info);
      break;
    case Attaching:
      game_state = fsmAttaching(game_info);
      break;
    case Game_Over:
      game_state = fsmGameOver(game_info);
      break;
  }
  return game_state;
}

/**
 * @brief Реализует состояние игры StartGame.
 * @param game_info Структура содержащая информацию об игре.
 *
 * Считывает сохраненный рекорд, генерирует случайную фигуру которая будет
 * первой появляться, устанавливает счет в 0, а уровень в 1.
 *
 * @return Возвращает следующее состояние игры Spawn.
 */
field_fsm fsmStartGame(GameInfo_t *game_info) {
  if (game_info->score >= game_info->high_score) saveScore(game_info->score);
  game_info->high_score = readScore();
#ifdef __linux__
  int random_number = (rand() % 7);
#else
  int random_number = (random() % 7);
#endif
  getFigureInfo(*game_info, random_number + 1);
  nextTetromino(game_info, random_number);
  for (int i = 0; i < 20; i++) {
    for (int j = 0; j < 10; j++) {
      game_info->field[i][j] = 0;
    }
  }
  game_info->score = 0;
  game_info->level = 1;
  return Spawn;
}

/**
 * @brief Записывает в матрицу next следующую фигуру.
 * @param game_info Структура содержащая информацию об игре.
 * @param tetromino Индекс фигуры которую следует записать.
 */
void nextTetromino(GameInfo_t *game_info, int tetromino) {
  int next[7][4][4] = {{{0, 0, 0, 0},  // 1
                        {1, 1, 1, 1},
                        {0, 0, 0, 0},
                        {0, 0, 0, 0}},
                       {{0, 0, 0, 0},  // 2
                        {0, 1, 1, 0},
                        {0, 1, 0, 0},
                        {0, 1, 0, 0}},
                       {{0, 0, 0, 0},  // 3
                        {0, 1, 0, 0},
                        {0, 1, 0, 0},
                        {0, 1, 1, 0}},
                       {{0, 0, 0, 0},  // 4
                        {0, 1, 1, 0},
                        {0, 1, 1, 0},
                        {0, 0, 0, 0}},
                       {{0, 0, 1, 0},  // 5
                        {0, 0, 1, 1},
                        {0, 0, 0, 1},
                        {0, 0, 0, 0}},
                       {{0, 0, 0, 0},  // 6
                        {0, 1, 0, 0},
                        {1, 1, 1, 0},
                        {0, 0, 0, 0}},
                       {{0, 0, 0, 0},  // 7
                        {0, 1, 1, 0},
                        {0, 0, 1, 1},
                        {0, 0, 0, 0}}};
  for (int i = 0; i < 4; i++) {
    memcpy(game_info->next[i], next[tetromino][i], 4 * sizeof(int));
  }
}

/**
 * @brief Реализует состояние игры Spawn.
 * @param game_info Структура содержащая информацию об игре.
 *
 * Прорисовывает на поле фигуру если может и генерирует следующую которую
 * записывает в next.
 *
 * @return Возвращает следующее состояние игры Shift при возможности нарисовать
 * новую фигуру и GameOver в противном случае.
 */
field_fsm fsmSpawn(GameInfo_t *game_info) {
  int real_spawn = 0, indent = (COLUMNS - NEXT_ROWS_AND_COLUMNS) / 2;
  field_fsm game_state = Spawn;
  if (!checkSpawnPossibility(*game_info, &real_spawn)) {
    for (int i = real_spawn; i < NEXT_ROWS_AND_COLUMNS; i++) {
      for (int j = 0; j < NEXT_ROWS_AND_COLUMNS; j++) {
        if (game_info->next[i][j] == 1) {
          game_info->field[i - real_spawn][j + indent] = 1;
        }
      }
    }
#ifdef __linux__
    int random_number = (rand() % 7);
#else
    int random_number = (random() % 7);
#endif
    nextTetromino(game_info, random_number);
    getFigureInfo(*game_info, random_number + 1);
    game_state = Shift;
  } else
    game_state = Game_Over;
  return game_state;
}

/**
 * @brief Проверяет можно ли нарисовать новую фигуру на поле или там есть
 * преграда.
 * @param game_info Структура содержащая информацию об игре
 * @param real_spawn Для фигур содержащих в своей матрице пустую первую строку,
 * нужно смещать их отрисовку на один пиксель выше.
 * @return Возвращает количество блоков которые нельзя отрисовать, если хотя бы
 * один есть фигуру нельзя отрисовать.
 */
int checkSpawnPossibility(GameInfo_t game_info, int *real_spawn) {
  int cant_spawn = 0, indent = (COLUMNS - NEXT_ROWS_AND_COLUMNS) / 2;
  for (int i = 0; i < NEXT_ROWS_AND_COLUMNS; i++) {
    int no_need_spawn = 0;
    for (int j = 0; j < NEXT_ROWS_AND_COLUMNS; j++) {
      if (game_info.next[i][j] == 0)
        no_need_spawn++;
      else if (game_info.next[i][j] == 1 &&
               game_info.field[i - (*real_spawn)][j + indent] == 2)
        cant_spawn++;
    }
    if (no_need_spawn == 4 && (*real_spawn) == 0 && i == 0) (*real_spawn)++;
  }
  return cant_spawn;
}

/**
 * @brief Реализует состояние игры Shfit.
 * @param game_info Структура содержащая информацию об игре.
 * @param figure_info Структура содержащая информацию об фигуре.
 * @return Возвращает следующее состояние игры - Shift если фигура сдвинулась
 * вниз и Attaching если не может сдвинуться.
 */
field_fsm fsmShift(GameInfo_t *game_info, FigureInfo *figure_info) {
  bool collision = checkCollisionForMoving(Down, *game_info);
  field_fsm game_state = Shift;
  for (int i = 19; i >= 0; i--) {
    for (int j = figure_info->coor_x;
         j < figure_info->coor_x + NEXT_ROWS_AND_COLUMNS && j < 10; j++) {
      if (game_info->field[i][j] == 1) {
        if (collision) {
          game_state = Attaching;
        } else {
          game_info->field[i][j] = 0;
          game_info->field[i + 1][j] = 1;
        }
      }
    }
  }
  return game_state;
}

/**
 * @brief Реализует состояние игры Attaching.
 * @param game_info Структура содержащая информацию об игре.
 *
 * Проверяет снизу вверх есть ли на поле полные линии, есть ли есть стирает их
 * сдвигая все остальное поле вниз. При этом считает количество сотренных линий
 * и в зависимости от этого увеличивает счет.
 *
 * @return Возвращает следующее состояние игры Spawn.
 */
field_fsm fsmAttaching(GameInfo_t *game_info) {
  getFigureInfo(*game_info, -2);
  int count_full_line = 0;
  for (int i = 19; i >= 0; i--) {
    int check = 10;
    for (int j = 0; j < 10; j++) {
      if (game_info->field[i][j] == 1) game_info->field[i][j] = 2;
      if (game_info->field[i][j] == 2 || game_info->field[i][j] == 1) {
        check--;
      }
    }
    if (check == 0) {
      count_full_line++;
      erasingLine(game_info, i);
      i++;
    }
  }
  switch (count_full_line) {
    case 1:
      game_info->score += 100;
      break;
    case 2:
      game_info->score += 300;
      break;
    case 3:
      game_info->score += 700;
      break;
    case 4:
      game_info->score += 1500;
      break;
  }
  if (count_full_line && game_info->level < 10) {
    game_info->level = game_info->score / 600 + 1;
    if (game_info->level > 10) game_info->level = 10;
    game_info->speed = 10 * game_info->level;
  }
  return Spawn;
}

/**
 * @brief Стирает линию и сдвигает все остальное поле вниз.
 * @param game_info Структура содержащая информацию об игре.
 * @param i Координата строки которую нужно стереть.
 */
void erasingLine(GameInfo_t *game_info, int i) {
  for (int new_i = i; new_i >= 0; new_i--) {
    for (int j = 0; j < 10; j++) {
      if (new_i == 0)
        game_info->field[new_i][j] = 0;
      else
        game_info->field[new_i][j] = game_info->field[new_i - 1][j];
    }
  }
}

/**
 * @brief Реализует состояние игры GameOver.
 * @param game_info Структура содержащая информацию об игре.
 *
 * Проверяет нужно ли переписывать рекорд, при необходимости делает это,
 * устанавливает скорость в значение -1, что является флагом для других функций.
 *
 * @return Возвращает следующее состояние игры Start.
 */
field_fsm fsmGameOver(GameInfo_t *game_info) {
  field_fsm game_state = (field_fsm)Start;
  if (game_info->score >= game_info->high_score) {
    saveScore(game_info->score);
  }
  game_info->speed = -1;
  return game_state;
}