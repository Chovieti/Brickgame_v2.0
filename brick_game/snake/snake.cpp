#include "snake.h"

#include <chrono>
#include <deque>
#include <fstream>
#include <random>
#include <sstream>

#include "../../lib_struct.h"

namespace s21 {

SnakeModel::SnakeModel()
    : game_info_{}, snake_info_{}, game_state_(FieldState::kStartGame) {
  game_info_.field = new int *[kHeight];
  game_info_.next = new int *[kNextSize];
  for (int i = kStartPoint; i < kHeight; ++i) {
    game_info_.field[i] = new int[kWidth]();
  }
  for (int i = kStartPoint; i < kNextSize; ++i) {
    game_info_.next[i] = new int[kNextSize]();
  }
}

SnakeModel::~SnakeModel() {
  if (game_info_.score > game_info_.high_score) SaveScore();
  ClearMatrix();
}

bool SnakeModel::ShouldUpdate(std::chrono::milliseconds elapsed_time) const {
  if (!IsActive()) return false;
  if (snake_info_.speed_boost || !IsMovementState() ||
      game_info_.speed == kSpeedForStart)
    return true;
  return elapsed_time.count() >= CalculateUpdateThreshold();
}

bool SnakeModel::IsActive() const {
  return game_info_.speed >= kSpeedForStart && game_info_.speed <= kMaxSpeed &&
         !game_info_.pause;
}

bool SnakeModel::IsMovementState() const {
  return game_state_ == FieldState::kMovement;
}

int SnakeModel::CalculateUpdateThreshold() const {
  return (kBaseGameDelay - game_info_.speed) * kSpeedMult;
}

GameInfo_t SnakeModel::UpdateInfo() {
  if (game_info_.field && game_info_.next) {
    SnakeModel::FsmField();
  }
  return game_info_;
}

void SnakeModel::SetDirection(SnakeDirection direction) {
  if (SnakeModel::CanChangeDirection(direction))
    snake_info_.next_direction = direction;
}

bool SnakeModel::CanChangeDirection(SnakeDirection direction) {
  return (direction == SnakeDirection::kUp &&
          snake_info_.real_direction != SnakeDirection::kDown) ||
         (direction == SnakeDirection::kRight &&
          snake_info_.real_direction != SnakeDirection::kLeft) ||
         (direction == SnakeDirection::kDown &&
          snake_info_.real_direction != SnakeDirection::kUp) ||
         (direction == SnakeDirection::kLeft &&
          snake_info_.real_direction != SnakeDirection::kRight);
}

SnakeDirection SnakeModel::GetDirection() const {
  return snake_info_.real_direction;
}

void SnakeModel::SetSpeedBoost() {
  if (!game_info_.pause) snake_info_.speed_boost = true;
}

void SnakeModel::SetGameSpeed() {
  if (game_info_.pause || game_info_.speed < kSpeedForStart ||
      game_info_.speed > kMaxSpeed) {
    game_info_.speed = kSpeedForStart;
    game_info_.pause = 0;
  }
}

void SnakeModel::SetGamePause() {
  if (!game_info_.pause && game_info_.speed >= kSpeedForStart) {
    game_info_.pause = 1;
  } else {
    game_info_.pause = 0;
  }
}

void SnakeModel::ClearMatrix() {
  if (game_info_.field) {
    for (int i = kStartPoint; i < kHeight; ++i) {
      delete[] game_info_.field[i];
    }
    delete[] game_info_.field;
    game_info_.field = nullptr;
  }
  if (game_info_.next) {
    for (int i = kStartPoint; i < kNextSize; ++i) {
      delete[] game_info_.next[i];
    }
    delete[] game_info_.next;
    game_info_.next = nullptr;
  }
}

void SnakeModel::AddSnakeOnField() {
  for (auto it = snake_info_.body.cbegin(); it != snake_info_.body.cend();
       ++it) {
    game_info_.field[it->y][it->x] = kSnakePoint;
  }
}

void SnakeModel::RemoveSnakeOnField() {
  for (auto it = snake_info_.body.cbegin(); it != snake_info_.body.cend();
       ++it) {
    game_info_.field[it->y][it->x] = kEmptyPoint;
  }
}

void SnakeModel::SpawnNewGameSnake() {
  for (int i = kStartPoint; i < kHeight; ++i) {
    for (int j = kStartPoint; j < kWidth; ++j) {
      game_info_.field[i][j] = kEmptyPoint;
    }
  }
  snake_info_.body.clear();
  for (int i = 0; i < 4; ++i) {
    SegmentCoor tmp_seg = {2 + i, 3};
    snake_info_.body.push_front(tmp_seg);
  }
  snake_info_.real_direction = snake_info_.next_direction =
      SnakeDirection::kRight;
  AddSnakeOnField();
}

void SnakeModel::FsmField() {
  static std::mt19937 gen(std::random_device{}());
  if (snake_info_.speed_boost) snake_info_.speed_boost = false;
  if (game_info_.speed == kSpeedForStart) {
    game_state_ = FieldState::kStartGame;
    game_info_.speed = kSpeedMult;
  }
  switch (game_state_) {
    case FieldState::kStartGame:
      game_state_ = FsmStartGame();
      break;
    case FieldState::kSpawn:
      game_state_ = FsmSpawn(gen);
      break;
    case FieldState::kMovement:
      game_state_ = FsmMovement();
      break;
    case FieldState::kWin:
      game_state_ = FsmWin();
      break;
    case FieldState::kGameOver:
      game_state_ = FsmGameOver();
      break;
  }
}

FieldState SnakeModel::FsmStartGame() {
  if (game_info_.score > game_info_.high_score) SaveScore();
  ReadScore();
  SpawnNewGameSnake();
  game_info_.score = 0;
  game_info_.level = 1;
  return FieldState::kSpawn;
}

FieldState SnakeModel::FsmSpawn(std::mt19937 &generator) {
  std::uniform_int_distribution<> distr_x(kStartPoint, kWidth - 1);
  std::uniform_int_distribution<> distr_y(kStartPoint, kHeight - 1);
  int x = 0, y = 0;
  do {
    x = distr_x(generator);
    y = distr_y(generator);
  } while (game_info_.field[y][x] != kEmptyPoint);
  game_info_.field[y][x] = kApplePoint;
  return FieldState::kMovement;
}

FieldState SnakeModel::FsmMovement() {
  SegmentCoor head = snake_info_.GetHead();
  FieldState state = FieldState::kMovement;
  snake_info_.real_direction = snake_info_.next_direction;
  if (!CheckCollision(head)) {
    RemoveSnakeOnField();
    switch (snake_info_.real_direction) {
      case SnakeDirection::kUp:
        head.y = head.y - 1;
        break;
      case SnakeDirection::kRight:
        head.x = head.x + 1;
        break;
      case SnakeDirection::kDown:
        head.y = head.y + 1;
        break;
      case SnakeDirection::kLeft:
        head.x = head.x - 1;
        break;
    }
    if (!game_info_.field[head.y][head.x]) {
      snake_info_.body.pop_back();
    } else {
      ++game_info_.score;
      if (game_info_.level < kMaxLevel) {
        game_info_.level = (game_info_.score / kScoreForLevelUp) + 1;
        game_info_.speed = kSpeedMult * game_info_.level;
      }
      state = FieldState::kSpawn;
    }
    snake_info_.body.push_front(head);
    AddSnakeOnField();
  } else {
    state = FieldState::kGameOver;
  }
  if (snake_info_.body.size() == kHeight * kWidth) state = FieldState::kWin;
  return state;
}

bool SnakeModel::CheckCollision(SegmentCoor head) {
  SegmentCoor tail = snake_info_.GetTail();
  if ((snake_info_.real_direction == SnakeDirection::kUp &&
       head.y == kStartPoint) ||
      (snake_info_.real_direction == SnakeDirection::kRight &&
       head.x == kWidth - 1) ||
      (snake_info_.real_direction == SnakeDirection::kDown &&
       head.y == kHeight - 1) ||
      (snake_info_.real_direction == SnakeDirection::kLeft &&
       head.x == kStartPoint)) {
    return true;
  }
  // Проверка на столкновение с самой собой не учитывая хвост
  if ((snake_info_.real_direction == SnakeDirection::kUp &&
       game_info_.field[head.y - 1][head.x] == 1 &&
       !(tail.x == head.x && tail.y == head.y - 1)) ||
      (snake_info_.real_direction == SnakeDirection::kRight &&
       game_info_.field[head.y][head.x + 1] == 1 &&
       !(tail.x == head.x + 1 && tail.y == head.y)) ||
      (snake_info_.real_direction == SnakeDirection::kDown &&
       game_info_.field[head.y + 1][head.x] == 1 &&
       !(tail.x == head.x && tail.y == head.y + 1)) ||
      (snake_info_.real_direction == SnakeDirection::kLeft &&
       game_info_.field[head.y][head.x - 1] == 1 &&
       !(tail.x == head.x - 1 && tail.y == head.y))) {
    return true;
  }
  return false;
}

FieldState SnakeModel::FsmWin() {
  game_info_.speed = kWinSpeed;
  SaveScore();
  return FieldState::kStartGame;
}

FieldState SnakeModel::FsmGameOver() {
  if (game_info_.score >= game_info_.high_score) {
    SaveScore();
  }
  game_info_.speed = kGameOverSpeed;
  return FieldState::kStartGame;
}

void SnakeModel::ReadScore() {
  std::ifstream file_score("snake_score.txt");
  if (!file_score.is_open()) return;
  std::string buffer;
  if (std::getline(file_score, buffer)) {
    std::istringstream iss(buffer);
    int score = 0;

    if (iss >> score) game_info_.high_score = score;
  }
}

void SnakeModel::SaveScore() {
  std::ofstream file_score("snake_score.txt");
  if (file_score.is_open()) file_score << game_info_.score;
}

void SnakeController::userInput(UserAction_t action, bool hold) {
  if (action == Up && hold == true) {
    snake_model_for_controller.SetDirection(SnakeDirection::kUp);
  }
  if (action == Right && hold == true) {
    snake_model_for_controller.SetDirection(SnakeDirection::kRight);
  }
  if (action == Down && hold == true) {
    snake_model_for_controller.SetDirection(SnakeDirection::kDown);
  }
  if (action == Left && hold == true) {
    snake_model_for_controller.SetDirection(SnakeDirection::kLeft);
  }
  if (action == Action && hold == true) {
    snake_model_for_controller.SetSpeedBoost();
  }
  if (action == Pause && hold == true) {
    snake_model_for_controller.SetGamePause();
  }
  if (action == Start && hold == true) {
    snake_model_for_controller.SetGameSpeed();
  }
  if (action == Terminate && hold == true) {
    snake_model_for_controller.ClearMatrix();
  }
}

GameInfo_t SnakeController::updateCurrentState() {
  static auto start_time = std::chrono::steady_clock::now();
  auto current_time = std::chrono::steady_clock::now();
  auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(
      current_time - start_time);
  if (snake_model_for_controller.ShouldUpdate(elapsed_time)) {
    start_time = current_time;
    return snake_model_for_controller.UpdateInfo();
  }
  return snake_model_for_controller.GetGameInfo();
}

namespace SnakeAdapter {

void userInput(UserAction_t action, bool hold) {
  controller.userInput(action, hold);
}

GameInfo_t updateCurrentState() { return controller.updateCurrentState(); }

}  // namespace SnakeAdapter

}  // namespace s21