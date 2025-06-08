#ifndef BRICKGAME_V2_0_BRICK_GAME_SNAKE_SNAKE_H
#define BRICKGAME_V2_0_BRICK_GAME_SNAKE_SNAKE_H

#include <deque>
#include <random>

#include "../../lib_struct.h"

namespace s21 {

inline constexpr int kStartPoint = 0;
inline constexpr int kHeight = 20;
inline constexpr int kWidth = 10;
inline constexpr int kNextSize = 4;

inline constexpr int kSpeedMult = 10;
inline constexpr int kScoreForLevelUp = 5;

inline constexpr int kMaxLevel = 10;
inline constexpr int kMaxSpeed = 100;

inline constexpr int kSpeedForStart = 1;
inline constexpr int kWinSpeed = 200;
inline constexpr int kGameOverSpeed = -1;

inline constexpr int kBaseGameDelay = 120;

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
  SnakeDirection GetDirection() const;
  void SetSpeedBoost() { snake_info_.speed_boost = true; }
  void SetGameSpeed() { game_info_.speed = kSpeedForStart; }
  int GetGameSpeed() const { return game_info_.speed; }
  void SetGamePause(int set) { game_info_.pause = set; }
  int GetGamePause() const { return game_info_.pause; }
  // Очистка матриц
  void ClearMatrix();

 private:
  struct SegmentCoor {
    int x;
    int y;
  };

  struct SnakeInfo_t {
    std::deque<SegmentCoor> body;
    SnakeDirection real_direction;
    SnakeDirection next_direction;
    bool speed_boost = false;
    SegmentCoor GetHead() { return body.front(); }
    SegmentCoor GetTail() { return body.back(); }
  };

  GameInfo_t game_info_;
  SnakeInfo_t snake_info_;
  FieldState game_state_;

  // Методы для расчета времени
  int CalculateUpdateThreshold() const;
  bool IsMovementState() const;
  bool IsActive() const;
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

  FieldState GetGameState() const { return game_state_; };
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