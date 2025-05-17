#include "test.h"

START_TEST(tetris_getGameInfo_t_first) {
  GameInfo_t* test_GameInfo = {0};
  test_GameInfo = getGameInfo_t(false);
  ck_assert_ptr_ne(NULL, test_GameInfo);
  for (int i = 0; i < 20; i++) {
    for (int j = 0; j < 10; j++) {
      ck_assert_int_eq(0, test_GameInfo->field[i][j]);
      if (i < 4 && j < 4) {
        ck_assert_int_eq(0, test_GameInfo->next[i][j]);
      }
    }
  }
  ck_assert_int_eq(0, test_GameInfo->score);
  ck_assert_int_eq(0, test_GameInfo->pause);
  ck_assert_int_eq(0, test_GameInfo->level);
  ck_assert_int_eq(0, test_GameInfo->speed);
  getGameInfo_t(true);
}
END_TEST

START_TEST(tetris_getGameInfo_t) {
  GameInfo_t* test_GameInfo = {0};
  for (int i = 0; i < 10; i++) {
    test_GameInfo = getGameInfo_t(false);
  }

  ck_assert_ptr_ne(NULL, test_GameInfo);
  for (int i = 0; i < 20; i++) {
    for (int j = 0; j < 10; j++) {
      ck_assert_int_eq(0, test_GameInfo->field[i][j]);
      if (i < 4 && j < 4) {
        ck_assert_int_eq(0, test_GameInfo->next[i][j]);
      }
    }
  }
  ck_assert_int_eq(0, test_GameInfo->score);
  ck_assert_int_eq(0, test_GameInfo->pause);
  ck_assert_int_eq(0, test_GameInfo->level);
  ck_assert_int_eq(0, test_GameInfo->speed);
  getGameInfo_t(true);
}
END_TEST

START_TEST(tetris_checkCollisionForMoving_Empty) {
  GameInfo_t* test_GameInfo = getGameInfo_t(false);
  bool result = checkCollisionForMoving(Left, *test_GameInfo);
  ck_assert_int_eq(0, result);
  result = checkCollisionForMoving(Right, *test_GameInfo);
  ck_assert_int_eq(0, result);
  result = checkCollisionForMoving(Down, *test_GameInfo);
  ck_assert_int_eq(0, result);
  getGameInfo_t(true);
}
END_TEST

START_TEST(tetris_checkCollisionForMoving_DownRight_Edge) {
  GameInfo_t* test_GameInfo = getGameInfo_t(false);
  test_GameInfo->field[19][9] = 1;
  bool result = checkCollisionForMoving(Left, *test_GameInfo);
  ck_assert_int_eq(0, result);
  result = checkCollisionForMoving(Right, *test_GameInfo);
  ck_assert_int_eq(1, result);
  result = checkCollisionForMoving(Down, *test_GameInfo);
  ck_assert_int_eq(1, result);
  test_GameInfo->field[19][9] = 0;
  getGameInfo_t(true);
}
END_TEST

START_TEST(tetris_checkCollisionForMoving_DownLeft_Edge) {
  GameInfo_t* test_GameInfo = getGameInfo_t(false);
  test_GameInfo->field[19][0] = 1;
  bool result = checkCollisionForMoving(Left, *test_GameInfo);
  ck_assert_int_eq(1, result);
  result = checkCollisionForMoving(Right, *test_GameInfo);
  ck_assert_int_eq(0, result);
  result = checkCollisionForMoving(Down, *test_GameInfo);
  ck_assert_int_eq(1, result);
  test_GameInfo->field[19][0] = 0;
  getGameInfo_t(true);
}
END_TEST

START_TEST(tetris_checkCollisionForMoving_UpRight_Edge) {
  GameInfo_t* test_GameInfo = getGameInfo_t(false);
  test_GameInfo->field[0][9] = 1;
  bool result = checkCollisionForMoving(Left, *test_GameInfo);
  ck_assert_int_eq(0, result);
  result = checkCollisionForMoving(Right, *test_GameInfo);
  ck_assert_int_eq(1, result);
  result = checkCollisionForMoving(Down, *test_GameInfo);
  ck_assert_int_eq(0, result);
  test_GameInfo->field[0][9] = 0;
  getGameInfo_t(true);
}
END_TEST

START_TEST(tetris_checkCollisionForMoving_UpLeft_Edge) {
  GameInfo_t* test_GameInfo = getGameInfo_t(false);
  test_GameInfo->field[0][0] = 1;
  bool result = checkCollisionForMoving(Left, *test_GameInfo);
  ck_assert_int_eq(1, result);
  result = checkCollisionForMoving(Right, *test_GameInfo);
  ck_assert_int_eq(0, result);
  result = checkCollisionForMoving(Down, *test_GameInfo);
  ck_assert_int_eq(0, result);
  test_GameInfo->field[0][0] = 0;
  getGameInfo_t(true);
}
END_TEST

START_TEST(tetris_checkCollisionForMoving_DownRight_Block) {
  GameInfo_t* test_GameInfo = getGameInfo_t(false);
  test_GameInfo->field[17][7] = 1;
  test_GameInfo->field[18][7] = 2;
  test_GameInfo->field[17][8] = 2;
  bool result = checkCollisionForMoving(Left, *test_GameInfo);
  ck_assert_int_eq(0, result);
  result = checkCollisionForMoving(Right, *test_GameInfo);
  ck_assert_int_eq(1, result);
  result = checkCollisionForMoving(Down, *test_GameInfo);
  ck_assert_int_eq(1, result);
  test_GameInfo->field[19][9] = 0;
  getGameInfo_t(true);
}
END_TEST

START_TEST(tetris_checkCollisionForMoving_DownLeft_Block) {
  GameInfo_t* test_GameInfo = getGameInfo_t(false);
  test_GameInfo->field[17][3] = 1;
  test_GameInfo->field[18][3] = 2;
  test_GameInfo->field[17][2] = 2;
  bool result = checkCollisionForMoving(Left, *test_GameInfo);
  ck_assert_int_eq(1, result);
  result = checkCollisionForMoving(Right, *test_GameInfo);
  ck_assert_int_eq(0, result);
  result = checkCollisionForMoving(Down, *test_GameInfo);
  ck_assert_int_eq(1, result);
  test_GameInfo->field[19][0] = 0;
  getGameInfo_t(true);
}
END_TEST

START_TEST(tetris_checkCollisionForMoving_UpRight_Block) {
  GameInfo_t* test_GameInfo = getGameInfo_t(false);
  test_GameInfo->field[2][7] = 1;
  test_GameInfo->field[1][7] = 2;
  test_GameInfo->field[2][8] = 2;
  bool result = checkCollisionForMoving(Left, *test_GameInfo);
  ck_assert_int_eq(0, result);
  result = checkCollisionForMoving(Right, *test_GameInfo);
  ck_assert_int_eq(1, result);
  result = checkCollisionForMoving(Down, *test_GameInfo);
  ck_assert_int_eq(0, result);
  test_GameInfo->field[0][9] = 0;
  getGameInfo_t(true);
}
END_TEST

START_TEST(tetris_checkCollisionForMoving_UpLeft_Block) {
  GameInfo_t* test_GameInfo = getGameInfo_t(false);
  test_GameInfo->field[2][2] = 1;
  test_GameInfo->field[1][2] = 2;
  test_GameInfo->field[2][1] = 2;
  bool result = checkCollisionForMoving(Left, *test_GameInfo);
  ck_assert_int_eq(1, result);
  result = checkCollisionForMoving(Right, *test_GameInfo);
  ck_assert_int_eq(0, result);
  result = checkCollisionForMoving(Down, *test_GameInfo);
  ck_assert_int_eq(0, result);
  test_GameInfo->field[0][0] = 0;
  getGameInfo_t(true);
}
END_TEST

START_TEST(tetris_checkCollisionForMoving_Middle) {
  GameInfo_t* test_GameInfo = getGameInfo_t(false);
  test_GameInfo->field[10][5] = 1;
  bool result = checkCollisionForMoving(Left, *test_GameInfo);
  ck_assert_int_eq(0, result);
  result = checkCollisionForMoving(Right, *test_GameInfo);
  ck_assert_int_eq(0, result);
  result = checkCollisionForMoving(Down, *test_GameInfo);
  ck_assert_int_eq(0, result);
  test_GameInfo->field[10][5] = 0;
  getGameInfo_t(true);
}
END_TEST

START_TEST(tetris_fsmField_with_zero_speed) {
  GameInfo_t* test_GameInfo = getGameInfo_t(false);
  field_fsm test_field_fsm = fsmField(test_GameInfo);
  ck_assert_int_eq(Spawn, test_field_fsm);
  ck_assert_int_eq(1, test_GameInfo->level);
  ck_assert_int_eq(0, test_GameInfo->score);
  ck_assert_int_eq(0, test_GameInfo->speed);
  FigureInfo* test_FigureInfo = getFigureInfo(*test_GameInfo, 0);
  ck_assert_int_ne(0, test_FigureInfo->next_type_figure);
  if (test_FigureInfo->next_type_figure == 2 ||
      test_FigureInfo->next_type_figure == 6) {
    ck_assert_int_eq(3, test_FigureInfo->next_type_position);
  } else
    ck_assert_int_eq(0, test_FigureInfo->next_type_position);
  ck_assert_int_eq(1, test_FigureInfo->moving);
  ck_assert_int_eq(0, test_FigureInfo->coor_x);
  ck_assert_int_eq(0, test_FigureInfo->coor_y);
  ck_assert_int_eq(0, test_FigureInfo->type_figure);
  ck_assert_int_eq(0, test_FigureInfo->type_position);
  getGameInfo_t(true);
}
END_TEST

START_TEST(tetris_fsmField_with_speed) {
  GameInfo_t* test_GameInfo = getGameInfo_t(false);
  test_GameInfo->speed = 1;
  field_fsm test_field_fsm = fsmField(test_GameInfo);
  ck_assert_int_eq(Spawn, test_field_fsm);
  ck_assert_int_eq(1, test_GameInfo->level);
  ck_assert_int_eq(0, test_GameInfo->score);
  ck_assert_int_eq(10, test_GameInfo->speed);
  FigureInfo* test_FigureInfo = getFigureInfo(*test_GameInfo, 0);
  ck_assert_int_ne(0, test_FigureInfo->next_type_figure);
  if (test_FigureInfo->next_type_figure == 2 ||
      test_FigureInfo->next_type_figure == 6) {
    ck_assert_int_eq(3, test_FigureInfo->next_type_position);
  } else
    ck_assert_int_eq(0, test_FigureInfo->next_type_position);
  ck_assert_int_eq(1, test_FigureInfo->moving);
  ck_assert_int_eq(0, test_FigureInfo->coor_x);
  ck_assert_int_eq(0, test_FigureInfo->coor_y);
  if (test_FigureInfo->type_figure == 2 || test_FigureInfo->type_figure == 6) {
    ck_assert_int_eq(3, test_FigureInfo->type_position);
  } else
    ck_assert_int_eq(0, test_FigureInfo->type_position);
  getGameInfo_t(true);
}
END_TEST

START_TEST(tetris_fsmField) {
  GameInfo_t* test_GameInfo = getGameInfo_t(false);
  field_fsm test_field_fsm = 0;
  for (int i = 0; i < 9000; i++) {
    test_field_fsm = fsmField(test_GameInfo);
  }
  FigureInfo* test_FigureInfo = getFigureInfo(*test_GameInfo, 0);
  ck_assert_int_eq(0, test_GameInfo->score);
  if (test_GameInfo->speed != -1) {
    ck_assert_int_eq(0, test_GameInfo->speed);
  } else
    ck_assert_int_eq(-1, test_GameInfo->speed);
  ck_assert_int_ne(0, test_FigureInfo->next_type_figure);
  if (test_FigureInfo->next_type_figure == 2 ||
      test_FigureInfo->next_type_figure == 6) {
    ck_assert_int_eq(3, test_FigureInfo->next_type_position);
  } else
    ck_assert_int_eq(0, test_FigureInfo->next_type_position);
  ck_assert_int_ne(0, test_FigureInfo->type_figure);
  if (test_FigureInfo->type_figure == 2 || test_FigureInfo->type_figure == 6) {
    ck_assert_int_eq(3, test_FigureInfo->type_position);
  } else
    ck_assert_int_eq(0, test_FigureInfo->type_position);
  getGameInfo_t(true);
}
END_TEST

START_TEST(tetris_fsmSpawn_true) {
  GameInfo_t* test_GameInfo = getGameInfo_t(false);
  nextTetromino(test_GameInfo, FIGURE_1);
  getFigureInfo(*test_GameInfo, 1);
  field_fsm test_field_fsm = 0;
  test_field_fsm = fsmSpawn(test_GameInfo);
  int count_block = 0;
  for (int j = 0; j < 10; j++) {
    if (test_GameInfo->field[0][j] == 1) count_block++;
  }
  ck_assert_int_ne(0, count_block);
  ck_assert_int_eq(Shift, test_field_fsm);
  getGameInfo_t(true);
}
END_TEST

START_TEST(tetris_fsmSpawn_false) {
  GameInfo_t* test_GameInfo = getGameInfo_t(false);
  nextTetromino(test_GameInfo, FIGURE_1);
  getFigureInfo(*test_GameInfo, 1);
  field_fsm test_field_fsm = 0;
  for (int j = 0; j < 10; j++) {
    test_GameInfo->field[0][j] = 2;
  }
  test_field_fsm = fsmSpawn(test_GameInfo);
  int count_block = 0;
  for (int j = 0; j < 10; j++) {
    if (test_GameInfo->field[0][j] == 1) count_block++;
  }
  ck_assert_int_eq(0, count_block);
  ck_assert_int_ne(Shift, test_field_fsm);
  ck_assert_int_eq(Game_Over, test_field_fsm);
  getGameInfo_t(true);
}
END_TEST

START_TEST(tetris_fsmShift) {
  GameInfo_t* test_GameInfo = getGameInfo_t(false);
  FigureInfo* test_FigureInfo = getFigureInfo(*test_GameInfo, 1);
  field_fsm test_field_fsm = 0;
  test_GameInfo->field[0][1] = 1;
  test_GameInfo->field[0][2] = 1;
  test_GameInfo->field[0][3] = 1;
  test_GameInfo->field[0][4] = 1;
  test_FigureInfo->type_figure = 1;
  test_FigureInfo->type_position = 0;
  test_FigureInfo->moving = true;
  test_FigureInfo->coor_x = 1;
  test_FigureInfo->coor_y = -1;
  test_field_fsm = fsmShift(test_GameInfo, test_FigureInfo);
  for (int j = 1; j < 5; j++) {
    ck_assert_int_eq(0, test_GameInfo->field[0][j]);
    ck_assert_int_eq(1, test_GameInfo->field[1][j]);
  }
  ck_assert_int_eq(Shift, test_field_fsm);
  getGameInfo_t(true);
}
END_TEST

START_TEST(tetris_fsmShift_five_times) {
  GameInfo_t* test_GameInfo = getGameInfo_t(false);
  FigureInfo* test_FigureInfo = getFigureInfo(*test_GameInfo, 1);
  field_fsm test_field_fsm = 0;
  test_GameInfo->field[0][1] = 1;
  test_GameInfo->field[0][2] = 1;
  test_GameInfo->field[0][3] = 1;
  test_GameInfo->field[0][4] = 1;
  test_FigureInfo->type_figure = 1;
  test_FigureInfo->type_position = 0;
  test_FigureInfo->moving = true;
  test_FigureInfo->coor_x = 1;
  test_FigureInfo->coor_y = -1;
  for (int i = 1; i <= 5; i++) {
    test_field_fsm = fsmShift(test_GameInfo, test_FigureInfo);
    test_FigureInfo->coor_y++;
    for (int j = 1; j < 5; j++) {
      ck_assert_int_eq(0, test_GameInfo->field[i - 1][j]);
      ck_assert_int_eq(1, test_GameInfo->field[i][j]);
    }
    ck_assert_int_eq(Shift, test_field_fsm);
  }
  getGameInfo_t(true);
}
END_TEST

START_TEST(tetris_fsmShift_stop) {
  GameInfo_t* test_GameInfo = getGameInfo_t(false);
  FigureInfo* test_FigureInfo = getFigureInfo(*test_GameInfo, 1);
  field_fsm test_field_fsm = 0;
  test_GameInfo->field[0][1] = 1;
  test_GameInfo->field[0][2] = 1;
  test_GameInfo->field[0][3] = 1;
  test_GameInfo->field[0][4] = 1;
  test_GameInfo->field[1][4] = 2;
  test_FigureInfo->type_figure = 1;
  test_FigureInfo->type_position = 0;
  test_FigureInfo->moving = true;
  test_FigureInfo->coor_x = 1;
  test_FigureInfo->coor_y = -1;
  test_field_fsm = fsmShift(test_GameInfo, test_FigureInfo);
  for (int j = 1; j < 5; j++) {
    ck_assert_int_eq(1, test_GameInfo->field[0][j]);
  }
  ck_assert_int_eq(Attaching, test_field_fsm);
  getGameInfo_t(true);
}
END_TEST

START_TEST(tetristetris_fsmGameOver) {
  GameInfo_t* test_GameInfo = getGameInfo_t(false);
  test_GameInfo->score = 100;
  test_GameInfo->high_score = 0;
  int result = fsmGameOver(test_GameInfo);
  getGameInfo_t(false);
  ck_assert_int_eq(-1, test_GameInfo->speed);
  test_GameInfo->speed = 0;
  ck_assert_int_eq(Start, result);
  getGameInfo_t(true);
}
END_TEST

START_TEST(tetris_getFigureInfo_switch_position) {
  GameInfo_t* test_GameInfo = getGameInfo_t(false);
  FigureInfo* test_FigureInfo = getFigureInfo(*test_GameInfo, 1);

  for (int i = 1; i <= 7; i++) {
    test_FigureInfo = getFigureInfo(*test_GameInfo, (i + 1));
    ck_assert_int_eq(1, test_FigureInfo->moving);
    ck_assert_int_eq(i, test_FigureInfo->type_figure);
    if (test_FigureInfo->type_figure == 2 ||
        test_FigureInfo->type_figure == 6) {
      ck_assert_int_eq(3, test_FigureInfo->type_position);
    } else
      ck_assert_int_eq(0, test_FigureInfo->type_position);

    test_FigureInfo = getFigureInfo(*test_GameInfo, -1);
    ck_assert_int_eq(1, test_FigureInfo->moving);
    ck_assert_int_eq(i, test_FigureInfo->type_figure);
    if (test_FigureInfo->type_figure == 2 ||
        test_FigureInfo->type_figure == 6) {
      ck_assert_int_eq(0, test_FigureInfo->type_position);
    } else
      ck_assert_int_eq(1, test_FigureInfo->type_position);

    test_FigureInfo = getFigureInfo(*test_GameInfo, -1);
    ck_assert_int_eq(1, test_FigureInfo->moving);
    ck_assert_int_eq(i, test_FigureInfo->type_figure);
    if (test_FigureInfo->type_figure == 2 ||
        test_FigureInfo->type_figure == 6) {
      ck_assert_int_eq(1, test_FigureInfo->type_position);
    } else if (test_FigureInfo->type_figure == 3) {
      ck_assert_int_eq(2, test_FigureInfo->type_position);
    } else {
      ck_assert_int_eq(0, test_FigureInfo->type_position);
    }
  }
  getGameInfo_t(true);
}
END_TEST

START_TEST(tetris_getFigureInfo_change_moving) {
  GameInfo_t* test_GameInfo = getGameInfo_t(false);
  FigureInfo* test_FigureInfo = getFigureInfo(*test_GameInfo, 1);
  test_FigureInfo = getFigureInfo(*test_GameInfo, 1);
  ck_assert_int_eq(true, test_FigureInfo->moving);
  test_FigureInfo = getFigureInfo(*test_GameInfo, -2);
  ck_assert_int_eq(false, test_FigureInfo->moving);
}
END_TEST

START_TEST(tetris_fsmAttaching_for_zero_line) {
  GameInfo_t* test_GameInfo = getGameInfo_t(false);
  test_GameInfo->score = 0;

  ck_assert_int_eq(0, test_GameInfo->level);
  ck_assert_int_eq(0, test_GameInfo->pause);
  ck_assert_int_eq(0, test_GameInfo->score);
  ck_assert_int_eq(0, test_GameInfo->speed);

  field_fsm test_field_fsm = fsmAttaching(test_GameInfo);

  ck_assert_int_eq(Spawn, test_field_fsm);
  ck_assert_int_eq(1, test_GameInfo->level);
  ck_assert_int_eq(0, test_GameInfo->pause);
  ck_assert_int_eq(0, test_GameInfo->score);
  ck_assert_int_eq(0, test_GameInfo->speed);
  getGameInfo_t(true);
}

START_TEST(tetris_fsmAttaching_for_one_line) {
  GameInfo_t* test_GameInfo = getGameInfo_t(false);
  test_GameInfo->score = 0;
  for (int j = 0; j < 10; j++) {
    test_GameInfo->field[13][j] = 1;
  }

  ck_assert_int_eq(0, test_GameInfo->level);
  ck_assert_int_eq(0, test_GameInfo->pause);
  ck_assert_int_eq(0, test_GameInfo->score);
  ck_assert_int_eq(0, test_GameInfo->speed);

  field_fsm test_field_fsm = fsmAttaching(test_GameInfo);

  ck_assert_int_eq(Spawn, test_field_fsm);
  ck_assert_int_eq(1, test_GameInfo->level);
  ck_assert_int_eq(0, test_GameInfo->pause);
  ck_assert_int_eq(100, test_GameInfo->score);
  ck_assert_int_eq(0, test_GameInfo->speed);
  getGameInfo_t(true);
}

START_TEST(tetris_fsmAttaching_for_two_line) {
  GameInfo_t* test_GameInfo = getGameInfo_t(false);
  test_GameInfo->score = 0;
  for (int j = 0; j < 10; j++) {
    test_GameInfo->field[4][j] = 1;
    test_GameInfo->field[18][j] = 1;
  }

  ck_assert_int_eq(0, test_GameInfo->level);
  ck_assert_int_eq(0, test_GameInfo->pause);
  ck_assert_int_eq(0, test_GameInfo->score);
  ck_assert_int_eq(0, test_GameInfo->speed);

  field_fsm test_field_fsm = fsmAttaching(test_GameInfo);

  ck_assert_int_eq(Spawn, test_field_fsm);
  ck_assert_int_eq(1, test_GameInfo->level);
  ck_assert_int_eq(0, test_GameInfo->pause);
  ck_assert_int_eq(300, test_GameInfo->score);
  ck_assert_int_eq(0, test_GameInfo->speed);
  getGameInfo_t(true);
}

START_TEST(tetris_fsmAttaching_for_three_line) {
  GameInfo_t* test_GameInfo = getGameInfo_t(false);
  test_GameInfo->score = 0;
  for (int j = 0; j < 10; j++) {
    test_GameInfo->field[6][j] = 1;
    test_GameInfo->field[3][j] = 1;
    test_GameInfo->field[18][j] = 1;
  }

  ck_assert_int_eq(0, test_GameInfo->level);
  ck_assert_int_eq(0, test_GameInfo->pause);
  ck_assert_int_eq(0, test_GameInfo->score);
  ck_assert_int_eq(0, test_GameInfo->speed);

  field_fsm test_field_fsm = fsmAttaching(test_GameInfo);

  ck_assert_int_eq(Spawn, test_field_fsm);
  ck_assert_int_eq(2, test_GameInfo->level);
  ck_assert_int_eq(0, test_GameInfo->pause);
  ck_assert_int_eq(700, test_GameInfo->score);
  ck_assert_int_eq(0, test_GameInfo->speed);
  getGameInfo_t(true);
}

START_TEST(tetris_fsmAttaching_for_four_line) {
  GameInfo_t* test_GameInfo = getGameInfo_t(false);
  test_GameInfo->score = 0;
  for (int j = 0; j < 10; j++) {
    test_GameInfo->field[1][j] = 1;
    test_GameInfo->field[4][j] = 1;
    test_GameInfo->field[15][j] = 1;
    test_GameInfo->field[18][j] = 1;
  }

  ck_assert_int_eq(0, test_GameInfo->level);
  ck_assert_int_eq(0, test_GameInfo->pause);
  ck_assert_int_eq(0, test_GameInfo->score);
  ck_assert_int_eq(0, test_GameInfo->speed);

  field_fsm test_field_fsm = fsmAttaching(test_GameInfo);

  ck_assert_int_eq(Spawn, test_field_fsm);
  ck_assert_int_eq(3, test_GameInfo->level);
  ck_assert_int_eq(0, test_GameInfo->pause);
  ck_assert_int_eq(1500, test_GameInfo->score);
  ck_assert_int_eq(0, test_GameInfo->speed);
  getGameInfo_t(true);
}

START_TEST(tetris_rotate_I_figure) {
  GameInfo_t* test_GameInfo = getGameInfo_t(false);
  FigureInfo* test_FigureInfo = getFigureInfo(*test_GameInfo, FIGURE_1);
  test_FigureInfo = getFigureInfo(*test_GameInfo, FIGURE_1);

  test_FigureInfo->type_figure = 1;
  test_FigureInfo->type_position = 0;
  test_GameInfo->field[4][3] = 1;
  test_GameInfo->field[4][4] = 1;
  test_GameInfo->field[4][5] = 1;
  test_GameInfo->field[4][6] = 1;
  test_FigureInfo->coor_x = 3;
  test_FigureInfo->coor_y = 3;
  rotateTetramino(test_FigureInfo, test_GameInfo);
  ck_assert_int_eq(1, test_FigureInfo->type_figure);
  ck_assert_int_eq(1, test_FigureInfo->type_position);
  for (int i = 3; i < 7; i++) {
    ck_assert_int_eq(1, test_GameInfo->field[i][4]);
  }
  rotateTetramino(test_FigureInfo, test_GameInfo);
  ck_assert_int_eq(1, test_FigureInfo->type_figure);
  ck_assert_int_eq(0, test_FigureInfo->type_position);
  for (int j = 3; j < 7; j++) {
    ck_assert_int_eq(1, test_GameInfo->field[4][j]);
  }
  test_GameInfo->field[4][3] = 0;
  test_GameInfo->field[4][4] = 0;
  test_GameInfo->field[4][5] = 0;
  test_GameInfo->field[4][6] = 0;
  getGameInfo_t(true);
}
END_TEST

START_TEST(tetris_rotate_G_figure) {
  GameInfo_t* test_GameInfo = getGameInfo_t(false);
  FigureInfo* test_FigureInfo = getFigureInfo(*test_GameInfo, FIGURE_1);

  test_FigureInfo->type_figure = 2;
  test_FigureInfo->type_position = 3;
  test_GameInfo->field[4][3] = 1;
  test_GameInfo->field[4][4] = 1;
  test_GameInfo->field[5][3] = 1;
  test_GameInfo->field[6][3] = 1;
  test_FigureInfo->coor_x = 2;
  test_FigureInfo->coor_y = 4;
  rotateTetramino(test_FigureInfo, test_GameInfo);
  ck_assert_int_eq(2, test_FigureInfo->type_figure);
  ck_assert_int_eq(0, test_FigureInfo->type_position);

  ck_assert_int_eq(1, test_GameInfo->field[5][2]);
  ck_assert_int_eq(1, test_GameInfo->field[5][3]);
  ck_assert_int_eq(1, test_GameInfo->field[5][4]);
  ck_assert_int_eq(1, test_GameInfo->field[6][4]);
  test_GameInfo->field[5][2] = 0;
  test_GameInfo->field[5][3] = 0;
  test_GameInfo->field[5][4] = 0;
  test_GameInfo->field[6][4] = 0;
  getGameInfo_t(true);
}
END_TEST

START_TEST(tetris_rotate_L_figure) {
  GameInfo_t* test_GameInfo = getGameInfo_t(false);
  FigureInfo* test_FigureInfo = getFigureInfo(*test_GameInfo, FIGURE_1);

  test_FigureInfo->type_figure = 3;
  test_FigureInfo->type_position = 0;
  test_GameInfo->field[6][2] = 1;
  test_GameInfo->field[4][3] = 1;
  test_GameInfo->field[5][3] = 1;
  test_GameInfo->field[6][3] = 1;
  test_FigureInfo->coor_x = 2;
  test_FigureInfo->coor_y = 4;
  rotateTetramino(test_FigureInfo, test_GameInfo);
  ck_assert_int_eq(3, test_FigureInfo->type_figure);
  ck_assert_int_eq(1, test_FigureInfo->type_position);

  ck_assert_int_eq(1, test_GameInfo->field[4][2]);
  ck_assert_int_eq(1, test_GameInfo->field[5][2]);
  ck_assert_int_eq(1, test_GameInfo->field[5][3]);
  ck_assert_int_eq(1, test_GameInfo->field[5][4]);
  test_GameInfo->field[4][2] = 0;
  test_GameInfo->field[5][2] = 0;
  test_GameInfo->field[5][3] = 0;
  test_GameInfo->field[5][4] = 0;
  getGameInfo_t(true);
}
END_TEST

START_TEST(tetris_rotate_T_figure) {
  GameInfo_t* test_GameInfo = getGameInfo_t(false);
  FigureInfo* test_FigureInfo = getFigureInfo(*test_GameInfo, FIGURE_1);

  test_FigureInfo->type_figure = 6;
  test_FigureInfo->type_position = 3;
  test_GameInfo->field[5][2] = 1;
  test_GameInfo->field[5][3] = 1;
  test_GameInfo->field[5][4] = 1;
  test_GameInfo->field[4][3] = 1;
  test_FigureInfo->coor_x = 2;
  test_FigureInfo->coor_y = 4;
  rotateTetramino(test_FigureInfo, test_GameInfo);
  ck_assert_int_eq(6, test_FigureInfo->type_figure);
  ck_assert_int_eq(0, test_FigureInfo->type_position);

  ck_assert_int_eq(1, test_GameInfo->field[4][3]);
  ck_assert_int_eq(1, test_GameInfo->field[5][3]);
  ck_assert_int_eq(1, test_GameInfo->field[6][3]);
  ck_assert_int_eq(1, test_GameInfo->field[5][4]);
  test_GameInfo->field[4][3] = 0;
  test_GameInfo->field[5][3] = 0;
  test_GameInfo->field[6][3] = 0;
  test_GameInfo->field[5][4] = 0;
  getGameInfo_t(true);
}
END_TEST

START_TEST(tetris_rotate_S_figure) {
  GameInfo_t* test_GameInfo = getGameInfo_t(false);
  FigureInfo* test_FigureInfo = getFigureInfo(*test_GameInfo, FIGURE_1);

  test_FigureInfo->type_figure = 5;
  test_FigureInfo->type_position = 0;
  test_GameInfo->field[4][2] = 1;
  test_GameInfo->field[5][2] = 1;
  test_GameInfo->field[5][3] = 1;
  test_GameInfo->field[6][3] = 1;
  test_FigureInfo->coor_x = 2;
  test_FigureInfo->coor_y = 3;
  rotateTetramino(test_FigureInfo, test_GameInfo);
  ck_assert_int_eq(5, test_FigureInfo->type_figure);
  ck_assert_int_eq(1, test_FigureInfo->type_position);

  ck_assert_int_eq(1, test_GameInfo->field[4][4]);
  ck_assert_int_eq(1, test_GameInfo->field[4][3]);
  ck_assert_int_eq(1, test_GameInfo->field[5][3]);
  ck_assert_int_eq(1, test_GameInfo->field[5][2]);

  rotateTetramino(test_FigureInfo, test_GameInfo);
  ck_assert_int_eq(5, test_FigureInfo->type_figure);
  ck_assert_int_eq(0, test_FigureInfo->type_position);

  ck_assert_int_eq(1, test_GameInfo->field[3][3]);
  ck_assert_int_eq(1, test_GameInfo->field[4][3]);
  ck_assert_int_eq(1, test_GameInfo->field[4][4]);
  ck_assert_int_eq(1, test_GameInfo->field[5][4]);
  test_GameInfo->field[3][4] = 0;
  test_GameInfo->field[4][3] = 0;
  test_GameInfo->field[4][3] = 0;
  test_GameInfo->field[5][3] = 0;
  getGameInfo_t(true);
}
END_TEST

START_TEST(tetris_rotate_Z_figure) {
  GameInfo_t* test_GameInfo = getGameInfo_t(false);
  FigureInfo* test_FigureInfo = getFigureInfo(*test_GameInfo, FIGURE_1);

  test_FigureInfo->type_figure = 7;
  test_FigureInfo->type_position = 0;
  test_GameInfo->field[4][2] = 1;
  test_GameInfo->field[4][3] = 1;
  test_GameInfo->field[5][3] = 1;
  test_GameInfo->field[5][4] = 1;
  test_FigureInfo->coor_x = 2;
  test_FigureInfo->coor_y = 3;
  rotateTetramino(test_FigureInfo, test_GameInfo);
  ck_assert_int_eq(7, test_FigureInfo->type_figure);
  ck_assert_int_eq(1, test_FigureInfo->type_position);

  ck_assert_int_eq(1, test_GameInfo->field[3][4]);
  ck_assert_int_eq(1, test_GameInfo->field[4][4]);
  ck_assert_int_eq(1, test_GameInfo->field[4][3]);
  ck_assert_int_eq(1, test_GameInfo->field[5][3]);

  rotateTetramino(test_FigureInfo, test_GameInfo);
  ck_assert_int_eq(7, test_FigureInfo->type_figure);
  ck_assert_int_eq(0, test_FigureInfo->type_position);

  ck_assert_int_eq(1, test_GameInfo->field[4][2]);
  ck_assert_int_eq(1, test_GameInfo->field[4][3]);
  ck_assert_int_eq(1, test_GameInfo->field[5][3]);
  ck_assert_int_eq(1, test_GameInfo->field[5][4]);
  test_GameInfo->field[4][2] = 0;
  test_GameInfo->field[4][3] = 0;
  test_GameInfo->field[5][3] = 0;
  test_GameInfo->field[5][4] = 0;
  getGameInfo_t(true);
}
END_TEST

START_TEST(tetris_edgeForRotate) {
  GameInfo_t* test_GameInfo = getGameInfo_t(false);
  FigureInfo* test_FigureInfo = getFigureInfo(*test_GameInfo, FIGURE_1);

  test_FigureInfo->type_figure = 1;
  test_FigureInfo->coor_x = 0;
  test_FigureInfo->coor_y = 4;
  bool result = checkEdgeForRotate(*test_FigureInfo);
  ck_assert_int_eq(0, result);
  test_FigureInfo->coor_x = -1;
  result = checkEdgeForRotate(*test_FigureInfo);
  ck_assert_int_eq(1, result);
  test_FigureInfo->coor_x = 8;
  result = checkEdgeForRotate(*test_FigureInfo);
  ck_assert_int_eq(1, result);

  test_FigureInfo->type_figure = 3;
  test_FigureInfo->coor_x = 0;
  test_FigureInfo->coor_y = 4;
  result = checkEdgeForRotate(*test_FigureInfo);
  ck_assert_int_eq(0, result);
  test_FigureInfo->coor_x = -1;
  result = checkEdgeForRotate(*test_FigureInfo);
  ck_assert_int_eq(1, result);
  test_FigureInfo->coor_x = 8;
  result = checkEdgeForRotate(*test_FigureInfo);
  ck_assert_int_eq(1, result);
  getGameInfo_t(true);
}
END_TEST

START_TEST(tetris_collisionForRotate_I_figure) {
  GameInfo_t* test_GameInfo = getGameInfo_t(false);
  FigureInfo* test_FigureInfo = getFigureInfo(*test_GameInfo, FIGURE_1);

  test_FigureInfo->type_figure = 1;
  test_FigureInfo->coor_x = 0;
  test_FigureInfo->coor_y = 4;
  // Сама фигура на поле
  test_GameInfo->field[4][1] = 1;
  test_GameInfo->field[5][1] = 1;
  test_GameInfo->field[6][1] = 1;
  test_GameInfo->field[7][1] = 1;
  // Преграда для поворота
  test_GameInfo->field[5][3] = 2;
  test_GameInfo->field[6][3] = 2;
  test_GameInfo->field[7][3] = 2;
  bool result = checkCollisionForRotate(*test_FigureInfo, *test_GameInfo);
  ck_assert_int_eq(1, result);
  getGameInfo_t(true);
}
END_TEST

START_TEST(tetris_collisionForRotate_T_figure) {
  GameInfo_t* test_GameInfo = getGameInfo_t(false);
  FigureInfo* test_FigureInfo = getFigureInfo(*test_GameInfo, FIGURE_1);

  test_FigureInfo->type_figure = 6;
  test_FigureInfo->coor_x = 7;
  test_FigureInfo->coor_y = 10;
  // Сама фигура на поле
  test_GameInfo->field[10][8] = 1;
  test_GameInfo->field[11][8] = 1;
  test_GameInfo->field[12][8] = 1;
  test_GameInfo->field[11][9] = 1;
  // Преграда для поворота
  test_GameInfo->field[10][7] = 2;
  test_GameInfo->field[11][7] = 2;
  bool result = checkCollisionForRotate(*test_FigureInfo, *test_GameInfo);
  ck_assert_int_eq(1, result);
  getGameInfo_t(true);
}
END_TEST

START_TEST(tetris_collisionForRotate_S_figure) {
  GameInfo_t* test_GameInfo = getGameInfo_t(false);
  FigureInfo* test_FigureInfo = getFigureInfo(*test_GameInfo, FIGURE_1);

  test_FigureInfo->type_figure = 5;
  test_FigureInfo->coor_x = 2;
  test_FigureInfo->coor_y = 14;
  // Сама фигура на поле
  test_GameInfo->field[14][3] = 1;
  test_GameInfo->field[15][3] = 1;
  test_GameInfo->field[15][4] = 1;
  test_GameInfo->field[16][4] = 1;
  // Преграда для поворота
  test_GameInfo->field[13][2] = 2;
  test_GameInfo->field[14][2] = 2;
  bool result = checkCollisionForRotate(*test_FigureInfo, *test_GameInfo);
  ck_assert_int_eq(1, result);
  getGameInfo_t(true);
}
END_TEST

START_TEST(tetris_getGameInfo_t_off) {
  GameInfo_t* test_GameInfo = getGameInfo_t(false);
  test_GameInfo->score = 1;
  test_GameInfo->high_score = 0;
  test_GameInfo = getGameInfo_t(true);

  ck_assert_ptr_ne(NULL, test_GameInfo);
  ck_assert_ptr_eq(NULL, test_GameInfo->field);
  ck_assert_ptr_eq(NULL, test_GameInfo->next);
  ck_assert_int_eq(0, test_GameInfo->high_score);
  ck_assert_int_eq(0, test_GameInfo->score);
  ck_assert_int_eq(0, test_GameInfo->pause);
  ck_assert_int_eq(0, test_GameInfo->level);
  ck_assert_int_eq(0, test_GameInfo->speed);
}
END_TEST

Suite* tetris_test_suite(void) {
  Suite* c = suite_create("test");
  TCase* tc = tcase_create("->");

  tcase_add_test(tc, tetris_getGameInfo_t_first);
  tcase_add_test(tc, tetris_getGameInfo_t);
  tcase_add_test(tc, tetris_getGameInfo_t_off);
  tcase_add_test(tc, tetris_checkCollisionForMoving_Empty);
  tcase_add_test(tc, tetris_checkCollisionForMoving_DownRight_Edge);
  tcase_add_test(tc, tetris_checkCollisionForMoving_DownLeft_Edge);
  tcase_add_test(tc, tetris_checkCollisionForMoving_UpRight_Edge);
  tcase_add_test(tc, tetris_checkCollisionForMoving_UpLeft_Edge);
  tcase_add_test(tc, tetris_checkCollisionForMoving_DownRight_Block);
  tcase_add_test(tc, tetris_checkCollisionForMoving_DownLeft_Block);
  tcase_add_test(tc, tetris_checkCollisionForMoving_UpRight_Block);
  tcase_add_test(tc, tetris_checkCollisionForMoving_UpLeft_Block);
  tcase_add_test(tc, tetris_checkCollisionForMoving_Middle);
  tcase_add_test(tc, tetris_fsmField_with_zero_speed);
  tcase_add_test(tc, tetris_fsmField_with_speed);
  tcase_add_test(tc, tetris_fsmField);
  tcase_add_test(tc, tetris_fsmSpawn_true);
  tcase_add_test(tc, tetris_fsmSpawn_false);
  tcase_add_test(tc, tetris_fsmShift);
  tcase_add_test(tc, tetris_fsmShift_five_times);
  tcase_add_test(tc, tetris_fsmShift_stop);
  tcase_add_test(tc, tetristetris_fsmGameOver);
  tcase_add_test(tc, tetris_getFigureInfo_switch_position);
  tcase_add_test(tc, tetris_getFigureInfo_change_moving);
  tcase_add_test(tc, tetris_fsmAttaching_for_zero_line);
  tcase_add_test(tc, tetris_fsmAttaching_for_one_line);
  tcase_add_test(tc, tetris_fsmAttaching_for_two_line);
  tcase_add_test(tc, tetris_fsmAttaching_for_three_line);
  tcase_add_test(tc, tetris_fsmAttaching_for_four_line);
  tcase_add_test(tc, tetris_rotate_I_figure);
  tcase_add_test(tc, tetris_rotate_G_figure);
  tcase_add_test(tc, tetris_rotate_L_figure);
  tcase_add_test(tc, tetris_rotate_T_figure);
  tcase_add_test(tc, tetris_rotate_S_figure);
  tcase_add_test(tc, tetris_rotate_Z_figure);
  tcase_add_test(tc, tetris_edgeForRotate);
  tcase_add_test(tc, tetris_collisionForRotate_I_figure);
  tcase_add_test(tc, tetris_collisionForRotate_T_figure);
  tcase_add_test(tc, tetris_collisionForRotate_S_figure);
  getGameInfo_t(true);

  suite_add_tcase(c, tc);
  return c;
}