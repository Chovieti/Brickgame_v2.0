#include "desktop_screen.h"

// Кастомный виджет для отображения матрицы
// Конструктор виджета
GameWidget::GameWidget(void (*userInput)(UserAction_t, bool),
                       GameInfo_t (*updateCurrentState)(), QWidget* parent)
    : QWidget(parent),
      userInput(userInput),
      updateCurrentState(updateCurrentState) {
  setFocusPolicy(Qt::StrongFocus);
  connect(&gameTimer, &QTimer::timeout, this, &GameWidget::gameLoop);
  gameTimer.start(50);
  game_info.field = nullptr;
  game_info.next = nullptr;
}

// Переопределяем метод рисования виджета
void GameWidget::paintEvent(QPaintEvent*) {
  if (!game_info.field || !game_info.next) return;

  static double game_speed_metric = 0.0;
  if (game_info.speed > 0 && game_info.speed <= 100)
    game_speed_metric = 100 / (120.0 - game_info.speed);

  QPainter painter(this);  // Создаем объект для рисования

  drawBackground(painter);
  drawGameBorders(painter);
  drawNextPieceSection(painter);
  drawStatsSection(painter, game_speed_metric);
  drawGameField(painter);
  drawSpecialScreens(painter);
}

void GameWidget::drawBackground(QPainter& painter) {
  painter.fillRect(rect(), Qt::black);
}

void GameWidget::drawGameBorders(QPainter& painter) {
  painter.setPen(QPen(Qt::white, 2));
  painter.setBrush(Qt::NoBrush);
  // Основное игровое поле
  painter.drawRect(10, 10, 200, 400);
  // Статистика (верхний блок)
  painter.drawRect(220, 10, 150, 150);
  // Статистика (нижний блок)
  painter.drawRect(220, 160, 150, 250);
}

void GameWidget::drawNextPieceSection(QPainter& painter) {
  painter.setFont(QFont("Verdana", 20));
  painter.drawText(270, 40, "NEXT");
  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      if (game_info.next[y][x]) {
        painter.fillRect((x * 20) + 255, (y * 20) + 55, 20, 20, Qt::white);
      }
    }
  }
}

void GameWidget::drawStatsSection(QPainter& painter, double game_speed_metric) {
  painter.setFont(QFont("Verdana", 18));
  painter.drawText(230, 200,
                   QString("Speed: %1").arg(game_speed_metric, 0, 'f', 4));
  painter.drawText(230, 236, QString("Level: %1").arg(game_info.level));
  painter.drawText(230, 272, QString("Score: %1").arg(game_info.score));

  const int highScore = (game_info.high_score > game_info.score)
                            ? game_info.high_score
                            : game_info.score;
  painter.drawText(230, 308, QString("High: %1").arg(highScore));

  const QString pauseStatus = game_info.pause ? "Pause: ON" : "Pause: OFF";
  painter.drawText(230, 344, pauseStatus);
}

void GameWidget::drawGameField(QPainter& painter) {
  for (int y = 0; y < 20; y++) {
    for (int x = 0; x < 10; x++) {
      if (game_info.field[y][x]) {
        painter.fillRect((x * 20) + 10, (y * 20) + 10, 20, 20, Qt::white);
      }
    }
  }
}

void GameWidget::drawSpecialScreens(QPainter& painter) {
  if (game_info.speed == 0) {
    painter.drawText(65, 210, QString("Press Enter"));
  } else if (game_info.speed == -1) {
    // Экран поражения
    painter.fillRect(30, 110, 160, 200, Qt::black);
    painter.drawRect(30, 110, 160, 200);
    painter.drawText(70, 160, QString("You Lose"));
    painter.drawText(60, 200, QString("Your Score"));
    painter.drawText(65, 240, QString("%1").arg(game_info.score, 8));
  } else if (game_info.speed == 200) {
    // Экран победы
    painter.fillRect(30, 130, 160, 80, Qt::black);
    painter.drawRect(30, 130, 160, 80);
    painter.drawText(70, 170, QString("You Win!"));
  }
}

void GameWidget::keyPressEvent(QKeyEvent* event) {
  UserAction_t action;
  bool hold = false;
  switch (event->key()) {
    case Qt::Key_Return:
      action = Start;
      hold = true;
      break;
    case Qt::Key_P:
      action = Pause;
      hold = true;
      break;
    case Qt::Key_Q:
    case Qt::Key_Escape:
      action = Terminate;
      hold = true;
      break;
    case Qt::Key_A:
    case Qt::Key_Left:
      action = Left;
      hold = true;
      break;
    case Qt::Key_D:
    case Qt::Key_Right:
      action = Right;
      hold = true;
      break;
    case Qt::Key_W:
    case Qt::Key_Up:
      action = Up;
      hold = true;
      break;
    case Qt::Key_S:
    case Qt::Key_Down:
      action = Down;
      hold = true;
      break;
    case Qt::Key_Space:
      action = Action;
      hold = true;
      break;
  }

  userInput(action, hold);
  if (action == Terminate) QApplication::quit();
}

void GameWidget::gameLoop() {
  game_info = updateCurrentState();
  if (!game_info.field || !game_info.next) {
    gameTimer.stop();
    this->close();
    return;
  }
  update();
}