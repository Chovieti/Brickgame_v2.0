#include "../brick_game/snake/snake.h"

#include <gtest/gtest.h>

#include <chrono>
#include <thread>

namespace s21 {

class SnakeTester {
 public:
  static SnakeModel::SnakeInfo GetSnakeInfo(const SnakeModel &sm) {
    return sm.snake_info_;
  };

  static void SetGameSpeed(SnakeModel &sm, int speed) {
    sm.game_info_.speed = speed;
  }

  static int ForReadScoreTest(SnakeModel &sm) {
    sm.ReadScore();
    return sm.game_info_.high_score;
  }

  static void ForceFsmField(SnakeModel &sm) {
    sm.game_info_.speed = kSpeedMult;
    sm.FsmField();
  }

  static void ForceFsmStartGame(SnakeModel &sm) {
    sm.game_info_.speed = kSpeedMult;
    sm.FsmStartGame();
  }

  static FieldState ForceFsmMovement(SnakeModel &sm) {
    return sm.FsmMovement();
  }

  static void MoveAndEat(SnakeModel &sm) {
    SnakeModel::SegmentCoor head = sm.snake_info_.GetHead();
    SnakeDirection direction = sm.snake_info_.next_direction;
    if (direction == SnakeDirection::kUp) {
      sm.game_info_.field[head.y + 1][head.x] = kApplePoint;
    } else if (direction == SnakeDirection::kRight) {
      sm.game_info_.field[head.y][head.x + 1] = kApplePoint;
    } else if (direction == SnakeDirection::kDown) {
      sm.game_info_.field[head.y - 1][head.x] = kApplePoint;
    } else if (direction == SnakeDirection::kLeft) {
      sm.game_info_.field[head.y][head.x - 1] = kApplePoint;
    }
    sm.FsmMovement();
  }

  static void LevelUp(SnakeModel &sm) {
    sm.game_info_.score = 4;
    SnakeModel::SegmentCoor head = sm.snake_info_.GetHead();
    SnakeDirection direction = sm.snake_info_.next_direction;
    if (direction == SnakeDirection::kUp) {
      sm.game_info_.field[head.y + 1][head.x] = kApplePoint;
    } else if (direction == SnakeDirection::kRight) {
      sm.game_info_.field[head.y][head.x + 1] = kApplePoint;
    } else if (direction == SnakeDirection::kDown) {
      sm.game_info_.field[head.y - 1][head.x] = kApplePoint;
    } else if (direction == SnakeDirection::kLeft) {
      sm.game_info_.field[head.y][head.x - 1] = kApplePoint;
    }
    sm.FsmMovement();
  }

  static void SpawnSnakeEdgeRightUp(SnakeModel &sm) {
    for (int i = 0; i < 4; ++i) {
      SnakeModel::SegmentCoor tmp_seg = {6 + i, 0};
      sm.snake_info_.body.push_front(tmp_seg);
    }
    sm.snake_info_.real_direction = sm.snake_info_.next_direction =
        SnakeDirection::kRight;
    sm.AddSnakeOnField();
  }

  static void SpawnSnakeEdgeLeftDown(SnakeModel &sm) {
    for (int i = 0; i < 4; ++i) {
      SnakeModel::SegmentCoor tmp_seg = {3 - i, 19};
      sm.snake_info_.body.push_front(tmp_seg);
    }
    sm.snake_info_.real_direction = sm.snake_info_.next_direction =
        SnakeDirection::kRight;
    sm.AddSnakeOnField();
  }

  static void SetGameState(SnakeModel &sm, FieldState state) {
    sm.game_state_ = state;
  }

  static FieldState GetGameState(SnakeModel &sm) { return sm.game_state_; }

  static void FillField(SnakeModel &sm) {
    for (int i = kStartPoint; i < kHeight; ++i) {
      for (int j = kStartPoint; j < kWidth; ++j) {
        sm.game_info_.field[i][j] = 1;
        SnakeModel::SegmentCoor tmp_seg{j, i};
        sm.snake_info_.body.push_front(tmp_seg);
      }
    }
  }

  static void Pre11LvlState(SnakeModel &sm) {
    sm.game_info_.score = 59;
    sm.game_info_.level = 10;
  }
};

// Тест конструктора, что он работает корректно, а обе матрицы и тело змейки
// пусты
TEST(S21SnakeModelTest, ConstrustorTest) {
  SnakeModel test_model{};
  GameInfo_t test_gi_t = test_model.GetGameInfo();
  for (int i = kStartPoint; i < kHeight; ++i) {
    for (int j = kStartPoint; j < kWidth; ++j) {
      EXPECT_EQ(kEmptyPoint, test_gi_t.field[i][j]);
    }
  }
  for (int i = kStartPoint; i < kNextSize; ++i) {
    for (int j = kStartPoint; j < kNextSize; ++j) {
      EXPECT_EQ(kEmptyPoint, test_gi_t.next[i][j]);
    }
  }
  SnakeModel::SnakeInfo test_si = SnakeTester::GetSnakeInfo(test_model);
  EXPECT_TRUE(test_si.body.empty());
}
// Тест, что ReadScore корректно считывает файл, так как при сборке
// предварительно будет удален файл с рекордом результат должен быть 0
TEST(S21SnakeModelTest, ReadScoreTest) {
  SnakeModel test_model{};
  EXPECT_EQ(SnakeTester::ForReadScoreTest(test_model), 0);
}
// Добавить тесты для движений в другие стороны кроме право
TEST(S21SnakeModelTest, SnakeSpawnAndMovementRightTest) {
  SnakeModel test_model{};
  SnakeTester::ForceFsmStartGame(test_model);
  GameInfo_t test_gi_t = test_model.GetGameInfo();
  SnakeModel::SnakeInfo test_si = SnakeTester::GetSnakeInfo(test_model);
  for (int i = 0; i < 4; i++) {
    EXPECT_EQ(kSnakePoint, test_gi_t.field[3][2 + i]);
  }
  EXPECT_EQ(4, test_si.body.size());
  EXPECT_TRUE(test_si.next_direction == SnakeDirection::kRight);
  EXPECT_TRUE(test_si.real_direction == SnakeDirection::kRight);
  SnakeTester::ForceFsmMovement(test_model);
  for (int i = 0; i < 4; i++) {
    EXPECT_EQ(kSnakePoint, test_gi_t.field[3][3 + i]);
  }
  EXPECT_EQ(4, test_si.body.size());
  EXPECT_TRUE(test_si.next_direction == SnakeDirection::kRight);
  EXPECT_TRUE(test_si.real_direction == SnakeDirection::kRight);
}

TEST(S21SnakeModelTest, SnakeEatingTest) {
  SnakeModel test_model{};
  SnakeTester::ForceFsmStartGame(test_model);
  GameInfo_t test_gi_t = test_model.GetGameInfo();
  SnakeModel::SnakeInfo test_si = SnakeTester::GetSnakeInfo(test_model);
  EXPECT_EQ(4, test_si.body.size());
  EXPECT_EQ(0, test_gi_t.score);
  SnakeTester::MoveAndEat(test_model);
  test_gi_t = test_model.GetGameInfo();
  test_si = SnakeTester::GetSnakeInfo(test_model);
  EXPECT_EQ(5, test_si.body.size());
  EXPECT_EQ(1, test_gi_t.score);
  SnakeTester::MoveAndEat(test_model);
  test_gi_t = test_model.GetGameInfo();
  test_si = SnakeTester::GetSnakeInfo(test_model);
  EXPECT_EQ(6, test_si.body.size());
  EXPECT_EQ(2, test_gi_t.score);
}

TEST(S21SnakeModelTest, LevelUpTest) {
  SnakeModel test_model{};
  SnakeTester::ForceFsmStartGame(test_model);
  GameInfo_t test_gi_t = test_model.GetGameInfo();
  EXPECT_EQ(0, test_gi_t.score);
  EXPECT_EQ(1, test_gi_t.level);
  EXPECT_EQ(10, test_gi_t.speed);
  SnakeTester::LevelUp(test_model);
  test_gi_t = test_model.GetGameInfo();
  EXPECT_EQ(5, test_gi_t.score);
  EXPECT_EQ(2, test_gi_t.level);
  EXPECT_EQ(20, test_gi_t.speed);
}

TEST(S21SnakeModelTest, 11LevelTest) {
  SnakeModel test_model{};
  SnakeTester::ForceFsmStartGame(test_model);
  SnakeTester::Pre11LvlState(test_model);
  EXPECT_EQ(59, test_model.GetGameInfo().score);
  EXPECT_EQ(10, test_model.GetGameInfo().level);
  SnakeTester::MoveAndEat(test_model);
  EXPECT_EQ(60, test_model.GetGameInfo().score);
  EXPECT_NE(11, test_model.GetGameInfo().level);
}

TEST(S21SnakeModelTest, CollisionEmptyTest) {
  SnakeModel test_model{};
  SnakeTester::ForceFsmStartGame(test_model);
  FieldState test_state = SnakeTester::ForceFsmMovement(test_model);
  EXPECT_TRUE(test_state == FieldState::kMovement);
}

TEST(S21SnakeModelTest, CollisionWithEdgeUpTest) {
  SnakeModel test_model{};
  SnakeTester::SpawnSnakeEdgeRightUp(test_model);
  test_model.SetDirection(SnakeDirection::kUp);
  FieldState test_state = SnakeTester::ForceFsmMovement(test_model);
  EXPECT_TRUE(test_state == FieldState::kGameOver);
}

TEST(S21SnakeModelTest, CollisionWithEdgeRightTest) {
  SnakeModel test_model{};
  SnakeTester::SpawnSnakeEdgeRightUp(test_model);
  test_model.SetDirection(SnakeDirection::kRight);
  FieldState test_state = SnakeTester::ForceFsmMovement(test_model);
  EXPECT_TRUE(test_state == FieldState::kGameOver);
}

TEST(S21SnakeModelTest, CollisionWithEdgeDownTest) {
  SnakeModel test_model{};
  SnakeTester::SpawnSnakeEdgeLeftDown(test_model);
  test_model.SetDirection(SnakeDirection::kDown);
  FieldState test_state = SnakeTester::ForceFsmMovement(test_model);
  EXPECT_TRUE(test_state == FieldState::kGameOver);
}

TEST(S21SnakeModelTest, CollisionWithEdgeLeftTest) {
  SnakeModel test_model{};
  SnakeTester::SpawnSnakeEdgeLeftDown(test_model);
  test_model.SetDirection(SnakeDirection::kLeft);
  FieldState test_state = SnakeTester::ForceFsmMovement(test_model);
  EXPECT_TRUE(test_state == FieldState::kGameOver);
}

TEST(S21SnakeModelTest, CollisionWithBodyTest) {
  SnakeModel test_model{};
  SnakeTester::ForceFsmStartGame(test_model);
  SnakeTester::MoveAndEat(test_model);

  test_model.SetDirection(SnakeDirection::kDown);
  SnakeTester::ForceFsmMovement(test_model);
  test_model.SetDirection(SnakeDirection::kLeft);
  SnakeTester::ForceFsmMovement(test_model);
  test_model.SetDirection(SnakeDirection::kUp);
  FieldState test_state = SnakeTester::ForceFsmMovement(test_model);
  EXPECT_TRUE(test_state == FieldState::kGameOver);
}

TEST(S21SnakeModelTest, CollisionWithTailTest) {
  SnakeModel test_model{};
  SnakeTester::ForceFsmStartGame(test_model);

  test_model.SetDirection(SnakeDirection::kDown);
  SnakeTester::ForceFsmMovement(test_model);
  test_model.SetDirection(SnakeDirection::kLeft);
  SnakeTester::ForceFsmMovement(test_model);
  test_model.SetDirection(SnakeDirection::kUp);
  FieldState test_state = SnakeTester::ForceFsmMovement(test_model);
  EXPECT_TRUE(test_state == FieldState::kMovement);
  test_model.SetDirection(SnakeDirection::kRight);
  test_state = SnakeTester::ForceFsmMovement(test_model);
  EXPECT_TRUE(test_state == FieldState::kMovement);
  test_model.SetDirection(SnakeDirection::kDown);
  test_state = SnakeTester::ForceFsmMovement(test_model);
  EXPECT_TRUE(test_state == FieldState::kMovement);
  test_model.SetDirection(SnakeDirection::kLeft);
  test_state = SnakeTester::ForceFsmMovement(test_model);
  EXPECT_TRUE(test_state == FieldState::kMovement);
  test_model.SetDirection(SnakeDirection::kUp);
  test_state = SnakeTester::ForceFsmMovement(test_model);
  EXPECT_TRUE(test_state == FieldState::kMovement);
}

TEST(S21SnakeModelTest, SnakeRotate180Test) {
  SnakeModel test_model{};
  SnakeTester::ForceFsmStartGame(test_model);

  // При движении вправо поворот на 180
  test_model.SetDirection(SnakeDirection::kLeft);
  SnakeModel::SnakeInfo test_si = SnakeTester::GetSnakeInfo(test_model);
  EXPECT_TRUE(test_si.next_direction != SnakeDirection::kLeft);
  SnakeTester::ForceFsmMovement(test_model);
  test_si = SnakeTester::GetSnakeInfo(test_model);
  EXPECT_TRUE(test_si.real_direction != SnakeDirection::kLeft);
  // При движении вниз поворт на 180
  test_model.SetDirection(SnakeDirection::kDown);
  SnakeTester::ForceFsmMovement(test_model);
  test_model.SetDirection(SnakeDirection::kUp);
  test_si = SnakeTester::GetSnakeInfo(test_model);
  EXPECT_TRUE(test_si.next_direction != SnakeDirection::kUp);
  SnakeTester::ForceFsmMovement(test_model);
  test_si = SnakeTester::GetSnakeInfo(test_model);
  EXPECT_TRUE(test_si.real_direction != SnakeDirection::kUp);
  // При движении влево поворт на 180
  test_model.SetDirection(SnakeDirection::kLeft);
  SnakeTester::ForceFsmMovement(test_model);
  test_model.SetDirection(SnakeDirection::kRight);
  test_si = SnakeTester::GetSnakeInfo(test_model);
  EXPECT_TRUE(test_si.next_direction != SnakeDirection::kRight);
  SnakeTester::ForceFsmMovement(test_model);
  test_si = SnakeTester::GetSnakeInfo(test_model);
  EXPECT_TRUE(test_si.real_direction != SnakeDirection::kRight);
  // При движении вверх поворт на 180
  test_model.SetDirection(SnakeDirection::kUp);
  SnakeTester::ForceFsmMovement(test_model);
  test_model.SetDirection(SnakeDirection::kDown);
  test_si = SnakeTester::GetSnakeInfo(test_model);
  EXPECT_TRUE(test_si.next_direction != SnakeDirection::kDown);
  SnakeTester::ForceFsmMovement(test_model);
  test_si = SnakeTester::GetSnakeInfo(test_model);
  EXPECT_TRUE(test_si.real_direction != SnakeDirection::kDown);
}

TEST(S21SnakeModelTest, SpeedBoostTest) {
  SnakeModel test_model{};
  SnakeTester::ForceFsmStartGame(test_model);
  test_model.SetSpeedBoost();
  SnakeModel::SnakeInfo test_si = SnakeTester::GetSnakeInfo(test_model);
  EXPECT_TRUE(test_si.speed_boost);
  SnakeTester::SetGameState(test_model, FieldState::kMovement);
  SnakeTester::ForceFsmField(test_model);
  test_si = SnakeTester::GetSnakeInfo(test_model);
  EXPECT_FALSE(test_si.speed_boost);
}

TEST(S21SnakeModelTest, SpeedBoostWithPauseTest) {
  SnakeModel test_model{};
  SnakeTester::ForceFsmStartGame(test_model);
  test_model.SetGamePause();
  test_model.SetSpeedBoost();
  SnakeModel::SnakeInfo test_si = SnakeTester::GetSnakeInfo(test_model);
  EXPECT_FALSE(test_si.speed_boost);
  SnakeTester::SetGameState(test_model, FieldState::kMovement);
  SnakeTester::ForceFsmField(test_model);
  test_si = SnakeTester::GetSnakeInfo(test_model);
  EXPECT_FALSE(test_si.speed_boost);
}

TEST(S21SnakeModelTest, WinTest) {
  SnakeModel test_model{};
  SnakeTester::FillField(test_model);
  SnakeTester::SetGameState(test_model, FieldState::kSpawn);
  SnakeTester::ForceFsmField(test_model);
  EXPECT_TRUE(SnakeTester::GetGameState(test_model) == FieldState::kWin);
  SnakeTester::ForceFsmField(test_model);
  EXPECT_TRUE(SnakeTester::GetGameState(test_model) == FieldState::kStartGame);
  GameInfo_t test_gi_t = test_model.GetGameInfo();
  EXPECT_EQ(200, test_gi_t.speed);
}

TEST(S21SnakeModelTest, GameOverTest) {
  SnakeModel test_model{};
  SnakeTester::SpawnSnakeEdgeRightUp(test_model);
  SnakeTester::SetGameState(test_model, FieldState::kMovement);
  SnakeTester::ForceFsmField(test_model);
  EXPECT_TRUE(SnakeTester::GetGameState(test_model) == FieldState::kGameOver);
  SnakeTester::ForceFsmField(test_model);
  EXPECT_TRUE(SnakeTester::GetGameState(test_model) == FieldState::kStartGame);
  EXPECT_EQ(-1, test_model.GetGameInfo().speed);
}

TEST(S21SnakeModelTest, FsmTest) {
  SnakeModel test_model{};
  test_model.SetGameSpeed();
  EXPECT_EQ(1, test_model.GetGameInfo().speed);
  EXPECT_TRUE(FieldState::kStartGame == SnakeTester::GetGameState(test_model));
  test_model.UpdateInfo();
  EXPECT_EQ(10, test_model.GetGameInfo().speed);
  EXPECT_TRUE(FieldState::kSpawn == SnakeTester::GetGameState(test_model));
  test_model.UpdateInfo();
  EXPECT_TRUE(FieldState::kMovement == SnakeTester::GetGameState(test_model));
  for (int i = 0; i < 100; ++i) test_model.UpdateInfo();
  EXPECT_EQ(-1, test_model.GetGameInfo().speed);
  EXPECT_TRUE(FieldState::kMovement == SnakeTester::GetGameState(test_model));
}

TEST(S21SnakeModelTest, SetGameSpeedAndPauseTest) {
  SnakeModel test_model{};
  EXPECT_EQ(0, test_model.GetGameInfo().pause);
  EXPECT_EQ(0, test_model.GetGameInfo().speed);
  test_model.SetGameSpeed();
  EXPECT_EQ(1, test_model.GetGameInfo().speed);
  test_model.SetGamePause();
  EXPECT_EQ(1, test_model.GetGameInfo().pause);
  test_model.SetGamePause();
  EXPECT_EQ(0, test_model.GetGameInfo().pause);
  test_model.SetGamePause();
  test_model.SetGameSpeed();
  EXPECT_EQ(1, test_model.GetGameInfo().speed);
  SnakeTester::FillField(test_model);
  SnakeTester::SetGameState(test_model, FieldState::kSpawn);
  SnakeTester::SetGameSpeed(test_model, 100);
  SnakeTester::ForceFsmField(test_model);
  EXPECT_TRUE(FieldState::kWin == SnakeTester::GetGameState(test_model));
  SnakeTester::ForceFsmField(test_model);
  EXPECT_EQ(200, test_model.GetGameInfo().speed);
  test_model.SetGameSpeed();
  EXPECT_EQ(1, test_model.GetGameInfo().speed);
}

TEST(S21SnakeControllerTest, updateCurrentStateTest) {
  SnakeController test_controller{};
  GameInfo_t test_gi_t = test_controller.updateCurrentState();
  EXPECT_EQ(0, test_gi_t.speed);
  test_gi_t = test_controller.updateCurrentState();
  test_controller.userInput(Start, true);
  test_gi_t = test_controller.updateCurrentState();
  EXPECT_EQ(10, test_gi_t.speed);
  std::this_thread::sleep_for(std::chrono::milliseconds(200));
  test_gi_t = test_controller.updateCurrentState();
  EXPECT_EQ(0, test_gi_t.pause);
  test_controller.userInput(Pause, true);
  test_gi_t = test_controller.updateCurrentState();
  EXPECT_EQ(1, test_gi_t.pause);
  test_controller.userInput(Pause, true);
  test_controller.userInput(Action, true);
  test_gi_t = test_controller.updateCurrentState();
  EXPECT_EQ(0, test_gi_t.pause);
  std::this_thread::sleep_for(std::chrono::milliseconds(250));
  test_gi_t = test_controller.updateCurrentState();
}

TEST(S21SnakeControllerTest, RoundSnakeTest) {
  SnakeController test_controller{};
  test_controller.userInput(Start, true);
  test_controller.updateCurrentState();
  std::this_thread::sleep_for(std::chrono::milliseconds(250));
  test_controller.updateCurrentState();
  std::this_thread::sleep_for(std::chrono::milliseconds(250));
  test_controller.userInput(Down, true);
  test_controller.updateCurrentState();
  std::this_thread::sleep_for(std::chrono::milliseconds(250));
  test_controller.userInput(Left, true);
  test_controller.updateCurrentState();
  std::this_thread::sleep_for(std::chrono::milliseconds(250));
  test_controller.userInput(Up, true);
  test_controller.updateCurrentState();
  std::this_thread::sleep_for(std::chrono::milliseconds(250));
  test_controller.userInput(Right, true);
  test_controller.updateCurrentState();
}

TEST(S21SnakeControllerTest, TerminateTest) {
  SnakeController test_controler{};
  GameInfo_t test_gi_t = test_controler.updateCurrentState();
  EXPECT_TRUE(test_gi_t.field != nullptr);
  EXPECT_TRUE(test_gi_t.next != nullptr);
  test_controler.userInput(Terminate, true);
  test_gi_t = test_controler.updateCurrentState();
  EXPECT_TRUE(test_gi_t.field == nullptr);
  EXPECT_TRUE(test_gi_t.next == nullptr);
}

TEST(S21SnakeAdapterTest, GeneralTest) {
  GameInfo_t test_gi_t = SnakeAdapter::updateCurrentState();
  EXPECT_TRUE(test_gi_t.field != nullptr);
  EXPECT_TRUE(test_gi_t.next != nullptr);
  SnakeAdapter::userInput(Terminate, true);
  test_gi_t = SnakeAdapter::updateCurrentState();
  EXPECT_TRUE(test_gi_t.field == nullptr);
  EXPECT_TRUE(test_gi_t.next == nullptr);
}
// TODO adapter tests

}  // namespace s21