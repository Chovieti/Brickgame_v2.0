#include "snake.h"

namespace s21 {
    // Snake_Model
    Snake_Model::Snake_Model(): speed_boost_(false) {
        game_info.field = new int*[HEIGHT];
        game_info.next = new int*[NEXT_SIZE];
        for (int i = 0; i < HEIGHT; i++) {
            game_info.field[i] = new int[WEIGHT]();
            if (i < NEXT_SIZE) game_info.next[i] = new int[NEXT_SIZE]();
        }
    }

    Snake_Model::~Snake_Model() {
        if (game_info.score >= game_info.high_score) saveScore();
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

    void Snake_Model::userInput(UserAction_t action, bool hold) {
        speed_boost_ = false;
        if (action == Up && hold == true && (snake_info.real_direction == RIGHT || snake_info.real_direction == LEFT)) {
            snake_info.next_direction = UP;
        }
        if (action == Right && hold == true && (snake_info.real_direction == UP || snake_info.real_direction == DOWN)) {
            snake_info.next_direction = RIGHT;
        }
        if (action == Down && hold == true && (snake_info.real_direction == RIGHT || snake_info.real_direction == LEFT)) {
            snake_info.next_direction = DOWN;
        }
        if (action == Left && hold == true && (snake_info.real_direction == UP || snake_info.real_direction == DOWN)) {
            snake_info.next_direction = LEFT;
        }
        if (action == Action && hold == true && game_info.pause == 0) {
            speed_boost_ = true;
        }
        if (action == Pause && hold == true) {
            if (game_info.pause == 0 && game_info.speed > 0)
                game_info.pause = 1;
            else
                game_info.pause = 0;
        }
        if (action == Start && hold == true) {
            game_info.speed = 1;
        }
        if (action == Terminate && hold == true) {
            for (int i = 0; i < HEIGHT; i++) {
                delete[] game_info.field[i];
                game_info.field[i] = nullptr;
                if (i < NEXT_SIZE) {
                    delete[] game_info.next[i];
                    game_info.next[i] = nullptr;
                }
            }
            delete[] game_info.field;
            game_info.field = nullptr;
            delete[] game_info.next;
            game_info.next = nullptr;
        }
    }

    GameInfo_t Snake_Model::updateCurrentState() {
        // Просто игровой цикл без разделения
        using namespace std::chrono;
        using namespace std::chrono_literals;
        if (game_info.speed <= 100) {
            static auto start_time = steady_clock::now();
            constexpr int BASE_DELAY_100TH = 120;

            auto current_time = steady_clock::now();

            auto elapsed_time = duration_cast<milliseconds>(current_time - start_time);
            const int threshold = (BASE_DELAY_100TH - game_info.speed) * 10;
            if (speed_boost_) elapsed_time *= 2;
            

            static field_fsm state_game = Start_Game;
            if (game_info.speed == 1) {
                state_game = Start_Game;
                game_info.pause = 0;
            }

            if (((elapsed_time.count() >= threshold && state_game == Movement) || state_game != Movement)
            && game_info.pause == 0 && game_info.speed > 0) {
                start_time = current_time;
                state_game = Snake_Model::FSMField();
            }
        }
        
        return game_info;
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

    field_fsm Snake_Model::FSMField() {
        static field_fsm game_state = Start_Game;
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
        return game_state;
    }

    field_fsm Snake_Model::FSMStartGame() {
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
            SegmentCoor tmp_seg = {2+i, 3};
            snake_info.body.push_front(tmp_seg);
        }
        snake_info.real_direction = snake_info.next_direction = RIGHT;
        addSnakeOnField();
        // Установка уровня и счета
        game_info.score = 0;
        game_info.level = 1;
        return Spawn;
    }

    field_fsm Snake_Model::FSMSpawn(std::mt19937& generator) {
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
        /* Нужно посмотреть координаты головы, направление. Затем проверить есть ли впереди преграда,
        если есть, то происходит столкновение, если нет, то движется. При движении добавляется элемент в начало
        тела и удаляется из конца. */
        SegmentCoor head = snake_info.getHead();
        bool collision = false;
        field_fsm state = Movement;
        snake_info.real_direction = snake_info.next_direction;
        // Проверка столкновений
        // С границами
        if ((snake_info.real_direction == UP && head.y == 0) ||
        (snake_info.real_direction == RIGHT && head.x == 9) ||
        (snake_info.real_direction == DOWN && head.y == 19) ||
        (snake_info.real_direction == LEFT && head.x == 0)) collision = true;
        // С собственным телом
        if (!collision && ((snake_info.real_direction == UP && game_info.field[head.y-1][head.x] == 1) ||
        (snake_info.real_direction == RIGHT && game_info.field[head.y][head.x+1] == 1) ||
        (snake_info.real_direction == DOWN && game_info.field[head.y+1][head.x] == 1) ||
        (snake_info.real_direction == LEFT && game_info.field[head.y][head.x-1] == 1)))
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
            if (!game_info.field[head.y][head.x]) snake_info.body.pop_back();
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
        }
        else state = Game_Over;
        if (snake_info.body.size() == 200) state = Win;
        return state;
    }

    field_fsm Snake_Model::FSMWin() {
        game_info.speed = 200;
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
        std::ifstream file_score("score.txt");
        std::string buffer;
        std::getline(file_score, buffer);
        game_info.high_score = std::stoi(buffer);
    }

    void Snake_Model::saveScore() {
        std::ofstream file_score("score.txt");
        file_score << game_info.score;
    }
    

    // Snake_Controller
    Snake_Controller *snake_controll() {
        static Snake_Controller snake_control;
        return &snake_control;
    }
    void userInput(UserAction_t action, bool hold) {
        Snake_Controller *snake_control = snake_controll();
        snake_control->userInput(action, hold);
    }
    GameInfo_t updateCurrentState() {
        Snake_Controller *snake_control = snake_controll();
        return snake_control->updateCurrentState();
    }

}