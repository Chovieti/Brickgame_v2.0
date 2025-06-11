#ifndef BRICKGAME_V2_0_BRICK_GAME_SNAKE_SNAKE_H
#define BRICKGAME_V2_0_BRICK_GAME_SNAKE_SNAKE_H

#include <chrono>
#include <deque>
#include <random>

#include "../../inline.h"
#include "../../lib_struct.h"

namespace s21 {

inline constexpr int kSpeedMult = 10;
inline constexpr int kScoreForLevelUp = 5;
inline constexpr int kMaxLevel = 10;
inline constexpr int kEmptyPoint = 0;
inline constexpr int kSnakePoint = 1;
inline constexpr int kApplePoint = 2;

enum class SnakeDirection { kUp, kRight, kDown, kLeft };

enum class FieldState { kStartGame, kSpawn, kMovement, kWin, kGameOver };

class SnakeModel {
 public:
  SnakeModel();
  ~SnakeModel();

  // Методы для контроллера
  bool ShouldUpdate(std::chrono::milliseconds elapsed_time) const;
  GameInfo_t UpdateInfo();
  GameInfo_t GetGameInfo() const { return game_info_; };
  // Методы для доступа к модели
  void SetDirection(SnakeDirection direction);
  void SetSpeedBoost();
  void SetGameSpeed();
  void SetGamePause();
  // Очистка матриц
  void ClearMatrix();

  struct SegmentCoor {
    int x;
    int y;
  };

  struct SnakeInfo {
    std::deque<SegmentCoor> body;
    SnakeDirection real_direction;
    SnakeDirection next_direction;
    bool speed_boost = false;
    SegmentCoor GetHead() const { return body.front(); }
    SegmentCoor GetTail() const { return body.back(); }
  };

 private:
  GameInfo_t game_info_;
  SnakeInfo snake_info_;
  FieldState game_state_;
  friend class SnakeTester;

  // Методы для расчета времени
  int CalculateUpdateThreshold() const;
  bool IsMovementState() const;
  bool IsActive() const;
  // Проверка смены направлений
  bool CanChangeDirection(SnakeDirection direction);
  // Методы FSM
  void FsmField();
  FieldState FsmStartGame();
  FieldState FsmSpawn(std::mt19937& generator);
  FieldState FsmMovement();
  FieldState FsmWin();
  FieldState FsmGameOver();
  // Добавление и удаление змейки на поле
  void AddSnakeOnField();
  void RemoveSnakeOnField();
  void SpawnNewGameSnake();
  // Проверка на столкновение
  bool CheckCollision(SegmentCoor head);
  // Сохранение и чтение рекорда
  void ReadScore();
  void SaveScore();
};

class SnakeController {
 public:
  SnakeController() = default;
  ~SnakeController() = default;

  void userInput(UserAction_t action, bool hold);

  GameInfo_t updateCurrentState();

 private:
  SnakeModel snake_model_for_controller;
};

namespace SnakeAdapter {
static SnakeController controller;

void userInput(UserAction_t action, bool hold);

GameInfo_t updateCurrentState();
}  // namespace SnakeAdapter

}  // namespace s21

#endif  // BRICKGAME_V2_0_BRICK_GAME_SNAKE_SNAKE_H