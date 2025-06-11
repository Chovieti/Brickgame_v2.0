#include "desktop_screen.h"

#include <QApplication>
#include <QKeyEvent>
#include <QPainter>
#include <QTimer>
#include <QWidget>

#include "../../inline.h"
#include "../../lib_struct.h"

namespace s21 {

GameWidget::GameWidget(void (*userInput)(UserAction_t, bool),
                       GameInfo_t (*updateCurrentState)(), QWidget* parent)
    : QWidget(parent),
      userInput(userInput),
      updateCurrentState(updateCurrentState),
      game_info_{},
      game_timer_{} {
  setFocusPolicy(Qt::StrongFocus);
  connect(&game_timer_, &QTimer::timeout, this, &GameWidget::GameLoop);
  game_timer_.start(50);
  game_info_.field = nullptr;
  game_info_.next = nullptr;
}

void GameWidget::paintEvent(QPaintEvent*) {
  if (!game_info_.field || !game_info_.next) return;

  static double game_speed_metric = 0.0;
  if (game_info_.speed >= kSpeedForStart && game_info_.speed <= kMaxSpeed) {
    game_speed_metric = 100.0 / (kBaseGameDelay - game_info_.speed);
  }

  QPainter painter(this);

  DrawBackground(painter);
  DrawGameBorders(painter);
  DrawNextPieceSection(painter);
  DrawStatsSection(painter, game_speed_metric);
  DrawGameField(painter);
  DrawSpecialScreens(painter);
}

void GameWidget::DrawBackground(QPainter& painter) const {
  painter.fillRect(rect(), Qt::black);
}

void GameWidget::DrawGameBorders(QPainter& painter) const {
  painter.setPen(QPen(Qt::white, 2));
  painter.setBrush(Qt::NoBrush);
  // Основное игровое поле
  painter.drawRect(kBorderOffset, kBorderOffset, kPixelScale * kWidth,
                   kPixelScale * kHeight);
  // Статистика (верхний блок)
  painter.drawRect((kPixelScale * kWidth + (2 * kBorderOffset)), kBorderOffset,
                   kStatFieldSize, kStatFieldSize);
  // Статистика (нижний блок)
  painter.drawRect((kPixelScale * kWidth + (2 * kBorderOffset)),
                   kStatFieldSize + kBorderOffset, kStatFieldSize,
                   kPixelScale * kHeight - kStatFieldSize);
}

void GameWidget::DrawNextPieceSection(QPainter& painter) const {
  painter.setFont(QFont("Verdana", 20));
  painter.drawText(270, 40, "NEXT");
  for (int y = kStartPoint; y < kNextSize; y++) {
    for (int x = kStartPoint; x < kNextSize; x++) {
      if (game_info_.next[y][x]) {
        painter.fillRect((x * kPixelScale) + kNextBorderOffsetX,
                         (y * kPixelScale) + kNextBorderOffsetY, kPixelScale,
                         kPixelScale, Qt::white);
      }
    }
  }
}

void GameWidget::DrawStatsSection(QPainter& painter,
                                  double game_speed_metric) const {
  painter.setFont(QFont("Verdana", 18));
  painter.drawText(kStatTextStartX, kStatTextStartY,
                   QString("Speed: %1").arg(game_speed_metric, 0, 'f', 4));
  painter.drawText(kStatTextStartX, kStatTextStartY + kStatTextIndetY,
                   QString("Level: %1").arg(game_info_.level));
  painter.drawText(kStatTextStartX, kStatTextStartY + kStatTextIndetY * 2,
                   QString("Score: %1").arg(game_info_.score));

  const int displayed_score = (game_info_.high_score > game_info_.score)
                                  ? game_info_.high_score
                                  : game_info_.score;
  painter.drawText(kStatTextStartX, kStatTextStartY + kStatTextIndetY * 3,
                   QString("High: %1").arg(displayed_score));

  const QString pause_status = game_info_.pause ? "Pause: ON" : "Pause: OFF";
  painter.drawText(kStatTextStartX, kStatTextStartY + kStatTextIndetY * 4,
                   pause_status);
}

void GameWidget::DrawGameField(QPainter& painter) const {
  for (int y = kStartPoint; y < kHeight; y++) {
    for (int x = kStartPoint; x < kWidth; x++) {
      if (game_info_.field[y][x]) {
        painter.fillRect((x * kPixelScale) + kBorderOffset,
                         (y * kPixelScale) + kBorderOffset, kPixelScale,
                         kPixelScale, Qt::white);
      }
    }
  }
}

void GameWidget::DrawSpecialScreens(QPainter& painter) const {
  if (game_info_.speed == 0) {
    painter.drawText(kEnterTextStartX, kEnterTextStartY,
                     QString("Press Enter"));
  } else if (game_info_.speed == kGameOverSpeed) {
    // Экран поражения
    painter.fillRect(kSharedScreenStartX, kLoseScreenStartY, kSharedScreenSizeX,
                     kLoseScreenSizeY, Qt::black);
    painter.drawRect(kSharedScreenStartX, kLoseScreenStartY, kSharedScreenSizeX,
                     kLoseScreenSizeY);
    painter.drawText(kLoseTextStartX + kLoseTextIndetX, kLoseTextStartY,
                     QString("You Lose"));
    painter.drawText(kLoseTextStartX - kLoseTextIndetX,
                     kLoseTextStartY + kLoseTextIndentY, QString("Your Score"));
    painter.drawText(kLoseTextStartX, kLoseTextStartY + kLoseTextIndentY * 2,
                     QString("%1").arg(game_info_.score, 8));
  } else if (game_info_.speed == kWinSpeed) {
    // Экран победы
    painter.fillRect(kSharedScreenStartX, kWinScreenStartY, kSharedScreenSizeX,
                     kWinScreenSizeY, Qt::black);
    painter.drawRect(kSharedScreenStartX, kWinScreenStartY, kSharedScreenSizeX,
                     kWinScreenSizeY);
    painter.drawText(kWinTextStartX, kWinTextStartY, QString("You Win!"));
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

void GameWidget::GameLoop() {
  if (!updateCurrentState) return;
  game_info_ = updateCurrentState();
  if (!game_info_.field || !game_info_.next) {
    game_timer_.stop();
    this->close();
    return;
  }
  update();
}

}  // namespace s21