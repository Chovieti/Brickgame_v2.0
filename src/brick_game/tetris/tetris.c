#include "tetris.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../lib_struct.h"

void userInput(UserAction_t action, bool hold) {
  GameInfo_t *game_info = GetGameInfo_t(false);
  bool collision = CheckCollisionForMoving(action, *game_info);
  FigureInfo *info = GetFigureInfo(*game_info, 0);
  if (action == Left && hold == true && !collision && game_info->pause == 0 &&
      info->moving) {
    for (int i = 0; i < 20; ++i) {
      for (int j = 0; j < 10; ++j) {
        if (game_info->field[i][j] == 1) {
          game_info->field[i][j - 1] = 1;
          game_info->field[i][j] = 0;
        }
      }
    }
  }
  if (action == Right && hold == true && !collision && game_info->pause == 0 &&
      info->moving) {
    for (int i = 0; i < 20; ++i) {
      for (int j = 9; j >= 0; --j) {
        if (game_info->field[i][j] == 1 && j != 9) {
          game_info->field[i][j + 1] = 1;
          game_info->field[i][j] = 0;
        }
      }
    }
  }
  if (action == Action && hold == true && game_info->pause == 0) {
    if (!(CheckEdgeForRotate(*info) ||
          CheckCollisionForRotate(*info, *game_info) ||
          info->moving == false)) {
      RotateTetramino(info, game_info);
    }
  }
  if (action == Down && hold == true && game_info->pause == 0) {
    while (FsmShift(game_info, info) != kAttaching);
    GetFigureInfo(*game_info, -2);
  }
  if (action == Pause && hold == true) {
    if (game_info->pause == 0 && game_info->speed > 0) {
      game_info->pause = 1;
    } else {
      game_info->pause = 0;
    }
  }
  if (action == Start && hold == true &&
      (game_info->speed <= 0 || game_info->pause == 1)) {
    game_info->speed = 1;
  }
  if (action == Terminate && hold == true) {
    GetGameInfo_t(true);
  }
}

GameInfo_t updateCurrentState() {
  GameInfo_t *game_info = GetGameInfo_t(false);
  return *game_info;
}

GameInfo_t *GetGameInfo_t(bool switch_flag) {
  static GameInfo_t game_info = {0};
  static int initialized = 0;  // 0 - выключено, 1 - включено, 2 -выключение
  static struct timeval start_time = {0};
  if (!initialized) {
    if (InitialGameField(&game_info, &switch_flag, &start_time)) {
      initialized = 1;
    }
  }
  if (initialized == 1 && !switch_flag) {
    static FieldState state_game = kStartGame;
    struct timeval current_time = {0};
    gettimeofday(&current_time, NULL);
    if (game_info.speed == 1) {
      state_game = kStartGame;
      game_info.pause = 0;
    }
    double elapsed_time = GetTimeDifference(start_time, current_time);
    if (((elapsed_time >= (120 - game_info.speed) && state_game == kShift) ||
         (state_game != kShift)) &&
        (game_info.pause == 0) && (game_info.speed > 0)) {
      start_time = current_time;
      state_game = FsmField(&game_info);
    }
  } else if (initialized == 1 && switch_flag) {
    if (game_info.field != NULL && game_info.next != NULL) {
      for (int i = 0; i < ROWS; ++i) {
        if (game_info.field[i] != NULL) free(game_info.field[i]);
        if (i < NEXT_ROWS_AND_COLUMNS && game_info.next[i] != NULL) {
          free(game_info.next[i]);
          game_info.next[i] = NULL;
        }
      }
      free(game_info.field), free(game_info.next);
      game_info.field = NULL, game_info.next = NULL;

      if (game_info.score >= game_info.high_score) {
        SaveScore(game_info.score);
      }
      game_info.high_score = 0, game_info.score = 0;
      game_info.level = 0, game_info.speed = 0;
      game_info.pause = 0;
      initialized = 2;
    }
  }
  return &game_info;
}

bool InitialGameField(GameInfo_t *game_info, bool *switch_flag,
                      struct timeval *start_time) {
  bool initialized = false, error = false;
  game_info->field = (int **)calloc(ROWS, sizeof(int *));
  if (game_info->field == NULL) error = true;
  for (int i = 0; i < ROWS && !error; ++i) {
    game_info->field[i] = (int *)calloc(COLUMNS, sizeof(int));
    if (game_info->field[i] == NULL) FailFree(game_info->field, i);
  }
  if (!error) {
    game_info->next = (int **)calloc(NEXT_ROWS_AND_COLUMNS, sizeof(int *));
  }
  if (game_info->next == NULL) error = true;
  for (int i = 0; i < NEXT_ROWS_AND_COLUMNS && !error; ++i) {
    game_info->next[i] = (int *)calloc(NEXT_ROWS_AND_COLUMNS, sizeof(int));
    if (game_info->next[i] == NULL) FailFree(game_info->next, i);
  }
  if ((game_info->field != NULL || game_info->next != NULL) && !error) {
    initialized = true;
    gettimeofday(start_time, NULL);
    game_info->high_score = ReadScore();
    if (game_info->high_score == -1) {
      (*switch_flag) = true;
    }
  }
  return initialized;
}

void FailFree(int **field, int i) {
  for (int j = 0; j < i; ++j) {
    free(field[j]);
  }
  free(field);
  field = NULL;
}

int ReadScore() {
  int high_score = 0;
  char buffer[15] = {0};
  FILE *score_file = fopen("tetris_score.txt", "r");
  if (score_file == NULL) {
    score_file = fopen("tetris_score.txt", "w");
    if (score_file == NULL) {
      high_score = -1;
    } else {
      fprintf(score_file, "0");
    }
  } else {
    if (fgets(buffer, sizeof(buffer), score_file) != NULL) {
      high_score = atoi(buffer);
    }
  }
  fclose(score_file);
  return high_score;
}

void SaveScore(int new_score) {
  FILE *score_file = fopen("tetris_score.txt", "w");
  fprintf(score_file, "%d", new_score);
  fclose(score_file);
}

double GetTimeDifference(struct timeval start_time,
                         struct timeval current_time) {
  return (current_time.tv_sec - start_time.tv_sec) * 100.0 +
         (current_time.tv_usec - start_time.tv_usec) / 10000.0;
}

bool CheckCollisionForMoving(UserAction_t action, GameInfo_t game_info) {
  bool collision = false;
  if (action == Left) {
    CheckCollisionForMovingLeft(game_info, &collision);
  } else if (action == Right) {
    CheckCollisionForMovingRight(game_info, &collision);
  } else if (action == Down) {
    CheckCollisionForMovingDown(game_info, &collision);
  }
  return collision;
}

void CheckCollisionForMovingLeft(GameInfo_t game_info, bool *collision) {
  for (int j = 0; j < 10 && !(*collision); ++j) {
    for (int i = 0; i < 20 && !(*collision); ++i) {
      if (game_info.field[i][j] == 1) {
        if (j == 0) {
          (*collision) = true;
        } else if (game_info.field[i][j - 1] == 2) {
          (*collision) = true;
        }
      }
    }
  }
}

void CheckCollisionForMovingRight(GameInfo_t game_info, bool *collision) {
  for (int j = 9; j >= 0 && !(*collision); --j) {
    for (int i = 0; i < 20 && !(*collision); ++i) {
      if (game_info.field[i][j] == 1) {
        if (j == 9) {
          (*collision) = true;
        } else if (game_info.field[i][j + 1] == 2) {
          (*collision) = true;
        }
      }
    }
  }
}

void CheckCollisionForMovingDown(GameInfo_t game_info, bool *collision) {
  for (int i = 19; i >= 0 && !(*collision); --i) {
    for (int j = 0; j < 10 && !(*collision); ++j) {
      if (game_info.field[i][j] == 1) {
        if (i == 19) {
          (*collision) = true;
        } else if (game_info.field[i + 1][j] == 2) {
          (*collision) = true;
        }
      }
    }
  }
}

FigureInfo *GetFigureInfo(GameInfo_t game_info, int type) {
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
    GetCoorFigure(game_info, &info);
    if (type == -1) {
      ++info.type_position;
      if (info.type_figure == 2 || info.type_figure == 3 ||
          info.type_figure == 6) {
        info.type_position %= 4;
      } else {
        info.type_position %= 2;
      }
    }
  }
  return &info;
}

void GetCoorFigure(GameInfo_t game_info, FigureInfo *figure_info) {
  bool found = false;
  int count_found = 0;
  for (int i = 0; i < 20; ++i) {
    for (int j = 0; j < 10 && !found; ++j) {
      if (game_info.field[i][j] == 1) ++count_found;
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

bool CheckEdgeForRotate(FigureInfo figure_info) {
  bool edge = false;
  if ((figure_info.type_figure == 1) &&
      (figure_info.coor_y < 0 || figure_info.coor_y > 16 ||
       figure_info.coor_x < 0 || figure_info.coor_x > 6)) {
    edge = true;
  } else if (figure_info.coor_y < 0 || figure_info.coor_y > 16 ||
             figure_info.coor_x < 0 || figure_info.coor_x > 7) {
    edge = true;
  }
  return edge;
}

bool CheckCollisionForRotate(FigureInfo figure_info, GameInfo_t game_info) {
  bool collision = false;
  int count_occupied_block = 0;
  if (figure_info.type_figure == 1 && (figure_info.coor_y + 1 > 0)) {
    for (int i = figure_info.coor_y; i <= figure_info.coor_y + 1; ++i) {
      for (int j = figure_info.coor_x; j <= figure_info.coor_x + 1; ++j) {
        if (game_info.field[i][j] == 2) ++count_occupied_block;
      }
    }
    for (int i = figure_info.coor_y + 1; i <= figure_info.coor_y + 3; ++i) {
      for (int j = figure_info.coor_x + 1; j <= figure_info.coor_x + 3; ++j) {
        if (game_info.field[i][j] == 2) ++count_occupied_block;
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
    for (int i = coor_point_i; i <= coor_point_i + 2; ++i) {
      for (int j = coor_point_j; j <= coor_point_j + 2; ++j) {
        if (game_info.field[i][j] == 2) ++count_occupied_block;
      }
    }
  }
  if (count_occupied_block > 0) collision = true;
  return collision;
}

void RotateTetramino(FigureInfo *figure_info, GameInfo_t *game_info) {
  switch (figure_info->type_figure) {
    case 1:  // Фигура I
      RotateFigureI(figure_info, game_info);
      break;
    case 2:  // Фигура Г
    case 3:  // Фигура L
    case 6:  // Фигура T
      RotateFigureGLT(figure_info, game_info);
      break;
    case 5:  // Фигура S
    case 7:  // Фигура Z
      RotateFigureSZ(figure_info, game_info);
      break;
  }
  GetFigureInfo(*game_info, -1);
}

void RotateFigureI(FigureInfo *figure_info, GameInfo_t *game_info) {
  if (figure_info->type_position == 0) {
    for (int i = 0; i < 4; ++i) {
      game_info->field[figure_info->coor_y + 1][figure_info->coor_x + i] = 0;
      game_info->field[figure_info->coor_y + i][figure_info->coor_x + 1] = 1;
    }
  } else {
    for (int i = 0; i < 4; ++i) {
      game_info->field[figure_info->coor_y + i][figure_info->coor_x + 1] = 0;
      game_info->field[figure_info->coor_y + 1][figure_info->coor_x + i] = 1;
    }
  }
}

void RotateFigureGLT(FigureInfo *figure_info, GameInfo_t *game_info) {
  int tmp_field[3][3] = {0};
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      tmp_field[j][2 - i] =
          game_info->field[figure_info->coor_y + i][figure_info->coor_x + j];
    }
  }
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      game_info->field[figure_info->coor_y + i][figure_info->coor_x + j] =
          tmp_field[i][j];
    }
  }
}

void RotateFigureSZ(FigureInfo *figure_info, GameInfo_t *game_info) {
  int tmp_field[3][3] = {0};
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      if (figure_info->type_position == 0 || figure_info->type_figure == 5) {
        tmp_field[j][2 - i] =
            game_info
                ->field[figure_info->coor_y + 1 + i][figure_info->coor_x + j];
      } else {
        tmp_field[j][2 - i] =
            game_info->field[figure_info->coor_y + i][figure_info->coor_x + j];
      }
    }
  }
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      if (figure_info->type_position == 1 || figure_info->type_figure == 7) {
        game_info->field[figure_info->coor_y + i][figure_info->coor_x + j] =
            tmp_field[i][j];
      } else {
        game_info->field[figure_info->coor_y + 1 + i][figure_info->coor_x + j] =
            tmp_field[i][j];
      }
    }
  }
}

FieldState FsmField(GameInfo_t *game_info) {
  static FieldState game_state = kStartGame;
  static FigureInfo *info = {0};
  info = GetFigureInfo(*game_info, 0);
  if (game_info->speed == 1) {
    game_state = kStartGame;
    game_info->speed = 10;
  }
  switch (game_state) {
    case kStartGame:
#ifdef __linux__
      srand(time(NULL));
#else
      srandom(time(NULL));
#endif
      game_state = FsmStartGame(game_info);
      break;
    case kSpawn:
      if (!CheckCollisionForMoving(Down, *game_info)) {
        game_state = FsmSpawn(game_info);
      }
      break;
    case kShift:
      game_state = FsmShift(game_info, info);
      break;
    case kAttaching:
      game_state = FsmAttaching(game_info);
      break;
    case kGameOver:
      game_state = FsmGameOver(game_info);
      break;
  }
  return game_state;
}

FieldState FsmStartGame(GameInfo_t *game_info) {
  if (game_info->score >= game_info->high_score) SaveScore(game_info->score);
  game_info->high_score = ReadScore();
#ifdef __linux__
  int random_number = (rand() % 7);
#else
  int random_number = (random() % 7);
#endif
  GetFigureInfo(*game_info, random_number + 1);
  NextTetromino(game_info, random_number);
  for (int i = 0; i < 20; ++i) {
    for (int j = 0; j < 10; ++j) {
      game_info->field[i][j] = 0;
    }
  }
  game_info->score = 0;
  game_info->level = 1;
  return kSpawn;
}

void NextTetromino(GameInfo_t *game_info, int tetromino) {
  int next[7][4][4] = {{{0, 0, 0, 0},  // 1.Фигура I
                        {1, 1, 1, 1},
                        {0, 0, 0, 0},
                        {0, 0, 0, 0}},
                       {{0, 0, 0, 0},  // 2.Фигура Г
                        {0, 1, 1, 0},
                        {0, 1, 0, 0},
                        {0, 1, 0, 0}},
                       {{0, 0, 0, 0},  // 3.Фигура L
                        {0, 1, 0, 0},
                        {0, 1, 0, 0},
                        {0, 1, 1, 0}},
                       {{0, 0, 0, 0},  // 4.Фигура квадрата
                        {0, 1, 1, 0},
                        {0, 1, 1, 0},
                        {0, 0, 0, 0}},
                       {{0, 0, 1, 0},  // 5.Фигура S
                        {0, 0, 1, 1},
                        {0, 0, 0, 1},
                        {0, 0, 0, 0}},
                       {{0, 0, 0, 0},  // 6.Фигура T
                        {0, 1, 0, 0},
                        {1, 1, 1, 0},
                        {0, 0, 0, 0}},
                       {{0, 0, 0, 0},  // 7.Фигура Z
                        {0, 1, 1, 0},
                        {0, 0, 1, 1},
                        {0, 0, 0, 0}}};
  for (int i = 0; i < 4; ++i) {
    memcpy(game_info->next[i], next[tetromino][i], 4 * sizeof(int));
  }
}

FieldState FsmSpawn(GameInfo_t *game_info) {
  int real_spawn = 0, indent = (COLUMNS - NEXT_ROWS_AND_COLUMNS) / 2;
  FieldState game_state = kSpawn;
  if (!CheckSpawnPossibility(*game_info, &real_spawn)) {
    for (int i = real_spawn; i < NEXT_ROWS_AND_COLUMNS; ++i) {
      for (int j = 0; j < NEXT_ROWS_AND_COLUMNS; ++j) {
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
    NextTetromino(game_info, random_number);
    GetFigureInfo(*game_info, random_number + 1);
    game_state = kShift;
  } else
    game_state = kGameOver;
  return game_state;
}

int CheckSpawnPossibility(GameInfo_t game_info, int *real_spawn) {
  int cant_spawn = 0, indent = (COLUMNS - NEXT_ROWS_AND_COLUMNS) / 2;
  for (int i = 0; i < NEXT_ROWS_AND_COLUMNS; ++i) {
    int no_need_spawn = 0;
    for (int j = 0; j < NEXT_ROWS_AND_COLUMNS; ++j) {
      if (game_info.next[i][j] == 0) {
        ++no_need_spawn;
      } else if (game_info.next[i][j] == 1 &&
                 game_info.field[i - (*real_spawn)][j + indent] == 2) {
        ++cant_spawn;
      }
    }
    if (no_need_spawn == 4 && (*real_spawn) == 0 && i == 0) ++(*real_spawn);
  }
  return cant_spawn;
}

FieldState FsmShift(GameInfo_t *game_info, FigureInfo *figure_info) {
  bool collision = CheckCollisionForMoving(Down, *game_info);
  FieldState game_state = kShift;
  for (int i = 19; i >= 0; --i) {
    for (int j = figure_info->coor_x;
         j < figure_info->coor_x + NEXT_ROWS_AND_COLUMNS && j < 10; ++j) {
      if (game_info->field[i][j] == 1) {
        if (collision) {
          game_state = kAttaching;
        } else {
          game_info->field[i][j] = 0;
          game_info->field[i + 1][j] = 1;
        }
      }
    }
  }
  return game_state;
}

FieldState FsmAttaching(GameInfo_t *game_info) {
  GetFigureInfo(*game_info, -2);
  int count_full_line = 0;
  for (int i = 19; i >= 0; --i) {
    int check = 10;
    for (int j = 0; j < 10; ++j) {
      if (game_info->field[i][j] == 1) game_info->field[i][j] = 2;
      if (game_info->field[i][j] == 2 || game_info->field[i][j] == 1) {
        --check;
      }
    }
    if (check == 0) {
      ++count_full_line;
      ErasingLine(game_info, i);
      ++i;
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
  return kSpawn;
}

void ErasingLine(GameInfo_t *game_info, int i) {
  for (int new_i = i; new_i >= 0; --new_i) {
    for (int j = 0; j < 10; ++j) {
      if (new_i == 0) {
        game_info->field[new_i][j] = 0;
      } else {
        game_info->field[new_i][j] = game_info->field[new_i - 1][j];
      }
    }
  }
}

FieldState FsmGameOver(GameInfo_t *game_info) {
  FieldState game_state = (FieldState)Start;
  if (game_info->score >= game_info->high_score) {
    SaveScore(game_info->score);
  }
  game_info->speed = -1;
  return game_state;
}