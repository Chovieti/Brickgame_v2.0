#include "snake.h"

namespace s21 {
// Snake_Model
Snake_Model::Snake_Model() : game_state(Start_Game) {
  game_info.field = new int *[HEIGHT];
  game_info.next = new int *[NEXT_SIZE];
  for (int i = 0; i < HEIGHT; i++) {
    game_info.field[i] = new int[WEIGHT]();
    if (i < NEXT_SIZE) game_info.next[i] = new int[NEXT_SIZE]();
  }
}

Snake_Model::~Snake_Model() {
  if (game_info.score > game_info.high_score) saveScore();
  clearMatrix();
}

GameInfo_t Snake_Model::getGameInfo() { return game_info; }

GameInfo_t Snake_Model::updateInfo() {
  if (game_info.field && game_info.next) {
    snake_info.speed_boost_ = false;
    Snake_Model::FSMField();
  }
  return game_info;
}

void Snake_Model::setDirection(SnakeDirection direction) {
  snake_info.next_direction = direction;
}
SnakeDirection Snake_Model::getDirection() { return snake_info.real_direction; }
void Snake_Model::setSpeedBoost() { snake_info.speed_boost_ = true; }
bool Snake_Model::getSpeedBoost() { return snake_info.speed_boost_; }
void Snake_Model::setGameSpeed() { game_info.speed = 1; }
int Snake_Model::getGameSpeed() { return game_info.speed; }
void Snake_Model::setGamePause(int set) { game_info.pause = set; }
int Snake_Model::getGamePause() { return game_info.pause; }
field_fsm Snake_Model::getGameState() { return game_state; }

void Snake_Model::clearMatrix() {
  if (game_info.field) {
    for (int i = 0; i < HEIGHT; i++) {
      delete[] game_info.field[i];
      game_info.field[i] = nullptr;
    }
    delete[] game_info.field;
    game_info.field = nullptr;
  }
  if (game_info.next) {
    for (int i = 0; i < NEXT_SIZE; i++) {
      delete[] game_info.next[i];
      game_info.next[i] = nullptr;
    }
    delete[] game_info.next;
    game_info.next = nullptr;
  }
}

void Snake_Model::addSnakeOnField() {
  for (auto it = snake_info.body.cbegin(); it != snake_info.body.cend(); ++it) {
    game_info.field[it->y][it->x] = 1;
  }
}

void Snake_Model::removeSnakeOnField() {
  for (auto it = snake_info.body.cbegin(); it != snake_info.body.cend(); ++it) {
    game_info.field[it->y][it->x] = 0;
  }
}

void Snake_Model::FSMField() {
  static std::mt19937 gen(std::random_device{}());
  if (game_info.speed == 1) {
    game_state = Start_Game;
    game_info.speed = 10;
  }
  switch (game_state) {
    case Start_Game:
      game_state = FSMStartGame();
      break;
    case Spawn:
      game_state = FSMSpawn(gen);
      break;
    case Movement:
      game_state = FSMMovement();
      break;
    case Win:
      game_state = FSMWin();
      break;
    case Game_Over:
      game_state = FSMGameOver();
      break;
  }
}

field_fsm Snake_Model::FSMStartGame() {
  if (game_info.score > game_info.high_score) saveScore();
  // Чтение файла рекорда
  readScore();
  // Зануление матрицы поля
  for (int i = 0; i < HEIGHT; i++) {
    for (int j = 0; j < WEIGHT; j++) {
      game_info.field[i][j] = 0;
    }
  }
  // Очистка тела змейки и появление новой
  snake_info.body.clear();
  for (int i = 0; i < 4; i++) {
    SegmentCoor tmp_seg = {2 + i, 3};
    snake_info.body.push_front(tmp_seg);
  }
  snake_info.real_direction = snake_info.next_direction = RIGHT;
  addSnakeOnField();
  // Установка уровня и счета
  game_info.score = 0;
  game_info.level = 1;
  return Spawn;
}

field_fsm Snake_Model::FSMSpawn(std::mt19937 &generator) {
  std::uniform_int_distribution<> distr_x(0, 9);
  std::uniform_int_distribution<> distr_y(0, 19);
  int x, y;
  do {
    x = distr_x(generator);
    y = distr_y(generator);
  } while (game_info.field[y][x] != 0);
  game_info.field[y][x] = 2;
  return Movement;
}

field_fsm Snake_Model::FSMMovement() {
  /* Нужно посмотреть координаты головы, направление. Затем проверить есть ли
  впереди преграда, если есть, то происходит столкновение, если нет, то
  движется. При движении добавляется элемент в начало тела и удаляется из конца.
*/
  SegmentCoor head = snake_info.getHead();
  bool collision = false;
  field_fsm state = Movement;
  snake_info.real_direction = snake_info.next_direction;
  // Проверка столкновений
  // С границами
  if ((snake_info.real_direction == UP && head.y == 0) ||
      (snake_info.real_direction == RIGHT && head.x == 9) ||
      (snake_info.real_direction == DOWN && head.y == 19) ||
      (snake_info.real_direction == LEFT && head.x == 0))
    collision = true;
  // С собственным телом
  if (!collision && ((snake_info.real_direction == UP &&
                      game_info.field[head.y - 1][head.x] == 1) ||
                     (snake_info.real_direction == RIGHT &&
                      game_info.field[head.y][head.x + 1] == 1) ||
                     (snake_info.real_direction == DOWN &&
                      game_info.field[head.y + 1][head.x] == 1) ||
                     (snake_info.real_direction == LEFT &&
                      game_info.field[head.y][head.x - 1] == 1)))
    collision = true;

  // После проверки
  if (!collision) {
    removeSnakeOnField();
    switch (snake_info.real_direction) {
      case UP:
        head.y = head.y - 1;
        break;
      case RIGHT:
        head.x = head.x + 1;
        break;
      case DOWN:
        head.y = head.y + 1;
        break;
      case LEFT:
        head.x = head.x - 1;
        break;
    }
    if (!game_info.field[head.y][head.x])
      snake_info.body.pop_back();
    else {
      game_info.score++;
      if (game_info.level < 10) {
        game_info.level = (game_info.score / 5) + 1;
        game_info.speed = 10 * game_info.level;
      }
      state = Spawn;
    }
    snake_info.body.push_front(head);
    addSnakeOnField();
  } else
    state = Game_Over;
  if (snake_info.body.size() == 200) state = Win;
  return state;
}

field_fsm Snake_Model::FSMWin() {
  game_info.speed = 200;
  saveScore();
  return Start_Game;
}

field_fsm Snake_Model::FSMGameOver() {
  if (game_info.score >= game_info.high_score) {
    saveScore();
  }
  game_info.speed = -1;
  return Start_Game;
}

void Snake_Model::readScore() {
  std::ifstream file_score("snake_score.txt");
  std::string buffer;
  std::getline(file_score, buffer);
  if (!buffer.empty()) game_info.high_score = std::stoi(buffer);
}

void Snake_Model::saveScore() {
  std::ofstream file_score("snake_score.txt");
  file_score << game_info.score;
}

// Snake_Controller
void Snake_Controller::userInput(UserAction_t action, bool hold) {
  SnakeDirection realDirection = snake_model_for_controller.getDirection();
  if (action == Up && hold == true &&
      (realDirection == RIGHT || realDirection == LEFT)) {
    snake_model_for_controller.setDirection(UP);
  }
  if (action == Right && hold == true &&
      (realDirection == UP || realDirection == DOWN)) {
    snake_model_for_controller.setDirection(RIGHT);
  }
  if (action == Down && hold == true &&
      (realDirection == RIGHT || realDirection == LEFT)) {
    snake_model_for_controller.setDirection(DOWN);
  }
  if (action == Left && hold == true &&
      (realDirection == UP || realDirection == DOWN)) {
    snake_model_for_controller.setDirection(LEFT);
  }
  if (action == Action && hold == true &&
      snake_model_for_controller.getGamePause() == 0) {
    snake_model_for_controller.setSpeedBoost();
  }
  if (action == Pause && hold == true) {
    if (snake_model_for_controller.getGamePause() == 0 &&
        snake_model_for_controller.getGameSpeed() > 0)
      snake_model_for_controller.setGamePause(1);
    else
      snake_model_for_controller.setGamePause(0);
  }
  if (action == Start && hold == true &&
      (snake_model_for_controller.getGamePause() ||
       snake_model_for_controller.getGameSpeed() <= 0)) {
    snake_model_for_controller.setGameSpeed();
    snake_model_for_controller.setGamePause(0);
  }
  if (action == Terminate && hold == true) {
    snake_model_for_controller.clearMatrix();
  }
}

GameInfo_t Snake_Controller::updateCurrentState() {
  using namespace std::chrono;
  using namespace std::chrono_literals;
  GameInfo_t game_info = snake_model_for_controller.getGameInfo();
  int speed = snake_model_for_controller.getGameSpeed();
  if (game_info.speed <= 100) {
    static auto start_time = steady_clock::now();
    constexpr int BASE_DELAY_100TH = 120;

    auto current_time = steady_clock::now();

    auto elapsed_time = duration_cast<milliseconds>(current_time - start_time);
    const int threshold = (BASE_DELAY_100TH - speed) * 10;

    if ((((elapsed_time.count() >= threshold &&
           snake_model_for_controller.getGameState() == Movement) ||
          snake_model_for_controller.getGameState() != Movement) ||
         snake_model_for_controller.getSpeedBoost()) &&
        snake_model_for_controller.getGamePause() == 0 && speed > 0) {
      start_time = current_time;
      game_info = snake_model_for_controller.updateInfo();
    }
  }
  return game_info;
}

// SnakeAdapter
void SnakeAdapter::userInput(UserAction_t action, bool hold) {
  controller.userInput(action, hold);
}

GameInfo_t SnakeAdapter::updateCurrentState() {
  return controller.updateCurrentState();
}

}  // namespace s21