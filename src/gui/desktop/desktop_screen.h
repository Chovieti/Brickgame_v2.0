#ifndef BRICKGAME_V2_0_GUI_DESKTOP_DESKTOP_SCREEN_H
#define BRICKGAME_V2_0_GUI_DESKTOP_DESKTOP_SCREEN_H

#include <QApplication>
#include <QTimer>
#include <QWidget>

#include "../../lib_struct.h"

namespace s21 {

/// @name Константы для отрисовки игрового интерфейса
/// @{
inline constexpr int kBorderOffset = 10;  ///< Отступ границ игрового поля
inline constexpr int kPixelScale = 20;  ///< Масштаб пикселя (размер клетки)
inline constexpr int kNextBorderOffsetX =
    255;  ///< X-координата блока "Next Piece"
inline constexpr int kNextBorderOffsetY =
    55;  ///< Y-координата блока "Next Piece"
inline constexpr int kStatFieldSize = 150;  ///< Размер блока NEXT
inline constexpr int kStatTextStartX =
    230;  ///< X-координата текста игровой статистики
inline constexpr int kStatTextStartY =
    200;  ///< Y-координата текста игровой статистики
inline constexpr int kStatTextIndetY =
    36;  ///< Отступ по Y между строками статистики
inline constexpr int kEnterTextStartX =
    65;  ///< X-координата текста начала игры
inline constexpr int kEnterTextStartY =
    210;  ///< Y-координата текста начала игры
inline constexpr int kSharedScreenStartX =
    30;  ///< X-координата экрана конца игры
inline constexpr int kSharedScreenSizeX =
    160;  ///< Размер по X экрана конца игры
inline constexpr int kLoseScreenStartY =
    110;  ///< Y-координата экрана конца игры
inline constexpr int kLoseScreenSizeY = 200;  ///< Размер по Y экрана конца игры
inline constexpr int kLoseTextStartX =
    65;  ///< X-координата текста проигрыша игры
inline constexpr int kLoseTextIndetX =
    5;  ///< Смещение по X строк текста проигрыша
inline constexpr int kLoseTextStartY =
    160;  ///< Y-координата текста проигрыша игры
inline constexpr int kLoseTextIndentY =
    40;  ///< Смещение по Y строк текста проигрыша
inline constexpr int kWinScreenStartY = 130;  ///< Y-координата экрана победы
inline constexpr int kWinScreenSizeY = 80;  ///< Размер по Y экрана победы
inline constexpr int kWinTextStartX = 70;  ///< X-координата текста победы
inline constexpr int kWinTextStartY = 170;  ///< Y-координата текста победы
/// @}

/**
 * @brief Виджет игрового экрана для десктопной версии
 *
 * Класс отвечает за отрисовку всех элементов игры (игровое поле, статистика,
 * специальные экраны) и обработку пользовательского ввода.
 */
class GameWidget : public QWidget {
  Q_OBJECT

 public:
  /**
   * @brief Конструктор игрового виджета
   * @param userInput Callback-функция для обработки действий пользователя
   * @param updateCurrentState Callback-функция для обновления состояния игры
   * @param parent Родительский виджет (по умолчанию nullptr)
   */
  explicit GameWidget(void (*userInput)(UserAction_t, bool),
                      GameInfo_t (*updateCurrentState)(),
                      QWidget* parent = nullptr);

 protected:
  /**
   * @brief Отрисовывает все компоненты игры
   */
  void paintEvent(QPaintEvent* event) override;

  /**
   *  @brief Обрабатывает нажатия клавиш
   *  @param event Нажатие клавиши клавиатуры
   */
  void keyPressEvent(QKeyEvent* event) override;

 private slots:
  /// @brief Основной игровой цикл, обновляет состояние и перерисовыает экран
  /// (вызывается по таймеру)
  void GameLoop();

 private:
  GameInfo_t game_info_;  ///< Текущее состояние игры
  QTimer game_timer_;  ///< Таймер для обновления экрана
  void (*userInput)(UserAction_t,
                    bool);  ///< Callback для пользовательского ввода
  GameInfo_t (*updateCurrentState)();  ///< Callback для обновления состояния

  /// @name Приватные методы отрисовки
  /// @private
  /// @{

  /**
   * @brief Отрисовывает фоновое заполнение
   * @param painter Референс на QPainter
   */
  void DrawBackground(QPainter& painter) const;

  /**
   * @brief Рисует границы игровых областей
   * @param painter Референс на QPainter
   */
  void DrawGameBorders(QPainter& painter) const;

  /**
   * @brief Отрисовывает область "Следующая фигура"
   * @param painter Референс на QPainter
   */
  void DrawNextPieceSection(QPainter& painter) const;

  /**
   * @brief Отрисовывает блок статистики
   * @param painter Референс на QPainter
   * @param game_speed_metric Рассчитанная метрика скорости
   */
  void DrawStatsSection(QPainter& painter, double game_speed_metric) const;

  /**
   * @brief Отрисовывает основное игровое поле
   * @param painter Референс на QPainter
   */
  void DrawGameField(QPainter& painter) const;

  /**
   * @brief Отрисовывает спецэкраны (пауза, победа, поражение)
   * @param painter Референс на QPainter
   */
  void DrawSpecialScreens(QPainter& painter) const;

  /// @}
};

}  // namespace s21

#endif  // BRICKGAME_V2_0_GUI_DESKTOP_DESKTOP_SCREEN_H