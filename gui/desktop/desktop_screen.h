#ifndef BRICKGAME_V2_0_GUI_DESKTOP_DESKTOP_SCREEN_H
#define BRICKGAME_V2_0_GUI_DESKTOP_DESKTOP_SCREEN_H

#include <QApplication>
#include <QTimer>
#include <QWidget>

#include "../../lib_struct.h"

namespace s21 {

inline constexpr int kBorderOffset = 10;
inline constexpr int kPixelScale = 20;
inline constexpr int kNextBorderOffsetX = 255;
inline constexpr int kNextBorderOffsetY = 55;
inline constexpr int kStatFieldSize = 150;
inline constexpr int kStatTextStartX = 230;
inline constexpr int kStatTextStartY = 200;
inline constexpr int kStatTextIndetY = 36;
inline constexpr int kEnterTextStartX = 65;
inline constexpr int kEnterTextStartY = 210;
inline constexpr int kSharedScreenStartX = 30;
inline constexpr int kSharedScreenSizeX = 160;
inline constexpr int kLoseScreenStartY = 110;
inline constexpr int kLoseScreenSizeY = 200;
inline constexpr int kLoseTextStartX = 65;
inline constexpr int kLoseTextIndetX = 5;
inline constexpr int kLoseTextStartY = 160;
inline constexpr int kLoseTextIndentY = 40;
inline constexpr int kWinScreenStartY = 130;
inline constexpr int kWinScreenSizeY = 80;
inline constexpr int kWinTextStartX = 70;
inline constexpr int kWinTextStartY = 170;

class GameWidget : public QWidget {
  Q_OBJECT

 public:
  explicit GameWidget(void (*userInput)(UserAction_t, bool),
                      GameInfo_t (*updateCurrentState)(),
                      QWidget* parent = nullptr);

 protected:
  void paintEvent(QPaintEvent* event) override;
  void keyPressEvent(QKeyEvent* event) override;

 private slots:
  void GameLoop();

 private:
  GameInfo_t game_info_;
  QTimer game_timer_;
  void (*userInput)(UserAction_t, bool);
  GameInfo_t (*updateCurrentState)();

  void DrawBackground(QPainter& painter) const;
  void DrawGameBorders(QPainter& painter) const;
  void DrawNextPieceSection(QPainter& painter) const;
  void DrawStatsSection(QPainter& painter, double game_speed_metric) const;
  void DrawGameField(QPainter& painter) const;
  void DrawSpecialScreens(QPainter& painter) const;
};

}  // namespace s21

#endif  // BRICKGAME_V2_0_GUI_DESKTOP_DESKTOP_SCREEN_H