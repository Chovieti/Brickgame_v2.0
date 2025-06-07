#ifndef DESCTOP_SCREEN_H
#define DESCTOP_SCREEN_H

#include <QApplication>
#include <QKeyEvent>
#include <QPainter>
#include <QTimer>
#include <QWidget>

#include "../../lib_struct.h"

class GameWidget : public QWidget {
  Q_OBJECT

 public:
  explicit GameWidget(void (*userInput)(UserAction_t, bool),
                      GameInfo_t (*updateCurrentState)(),
                      QWidget *parent = nullptr);

 protected:
  void paintEvent(QPaintEvent *event) override;
  void keyPressEvent(QKeyEvent *event) override;

 private slots:
  void gameLoop();

 private:
  GameInfo_t game_info;
  QTimer gameTimer;
  void (*userInput)(UserAction_t, bool);
  GameInfo_t (*updateCurrentState)();

  void drawBackground(QPainter& painter);
  void drawGameBorders(QPainter& painter);
  void drawNextPieceSection(QPainter& painter);
  void drawStatsSection(QPainter& painter, double game_speed_metric);
  void drawGameField(QPainter& painter);
  void drawSpecialScreens(QPainter& painter);
  
};

#endif