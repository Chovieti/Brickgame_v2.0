#ifndef SNAKE_H
#define SNAKE_H
#define HEIGHT 20
#define WEIGHT 10
#define NEXT_SIZE 4

#include <chrono>
#include <deque>
#include <fstream>
#include <random>

#include "../../lib_struct.h"

namespace s21 {

typedef enum { UP, RIGHT, DOWN, LEFT } SnakeDirection;

typedef enum { Start_Game, Spawn, Movement, Win, Game_Over } field_fsm;

class Snake_Model {
 public:
  Snake_Model();
  ~Snake_Model();

  void userInput(UserAction_t action, bool hold);
  GameInfo_t updateCurrentState();

 private:
  struct SegmentCoor {
    int x;
    int y;
  };

  struct SnakeInfo_t {
    std::deque<SegmentCoor> body;
    SnakeDirection real_direction;
    SnakeDirection next_direction;
    bool speed_boost_ = false;
    SegmentCoor getHead() { return body.front(); }
  };

  GameInfo_t game_info;
  SnakeInfo_t snake_info;


  void addSnakeOnField();
  void removeSnakeOnField();

  field_fsm FSMField();
  field_fsm FSMStartGame();
  field_fsm FSMSpawn(std::mt19937& generator);
  field_fsm FSMMovement();
  field_fsm FSMWin();
  field_fsm FSMGameOver();
  void readScore();
  void saveScore();

  void clearMatrix();
};

class Snake_Controller {
 public:
  Snake_Controller() = default;
  ~Snake_Controller() = default;

  void userInput(UserAction_t action, bool hold) {
    snake_model_for_controller.userInput(action, hold);
  }

  GameInfo_t updateCurrentState() {
    return snake_model_for_controller.updateCurrentState();
  }

 private:
  Snake_Model snake_model_for_controller;
};

Snake_Controller* snake_controll();
void userInput(UserAction_t action, bool hold);
GameInfo_t updateCurrentState();

}  // namespace s21

#endif