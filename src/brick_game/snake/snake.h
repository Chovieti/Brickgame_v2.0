#ifndef BRICKGAME_V2_0_BRICK_GAME_SNAKE_SNAKE_H
#define BRICKGAME_V2_0_BRICK_GAME_SNAKE_SNAKE_H

#include <chrono>
#include <deque>
#include <random>

#include "../../inline.h"
#include "../../lib_struct.h"

namespace s21 {

/// @name Константы игры "Змейка"
/// @{
inline constexpr int kSpeedMult = 10;  ///< Множитель скорости игры
inline constexpr int kScoreForLevelUp =
    5;  ///< Очки для перехода на новый уровень
inline constexpr int kMaxLevel = 10;  ///< Максимальный уровень игры
inline constexpr int kEmptyPoint = 0;  ///< Пустая клетка на поле
inline constexpr int kSnakePoint = 1;  ///< Клетка с частью змейки
inline constexpr int kApplePoint = 2;  ///< Клетка с яблоком (еда)
/// @}

/**
 * @brief Направления движения змейки
 */
enum class SnakeDirection {
  kUp,     ///< Движение вверх
  kRight,  ///< Движение вправо
  kDown,   ///< Движение вниз
  kLeft    ///< Движение влево
};

/**
 * @brief Состояния конечного автомата игры
 */
enum class FieldState {
  kStartGame,  ///< Начало новой игры
  kSpawn,      ///< Появление нового яблока
  kMovement,   ///< Движение змейки
  kWin,        ///< Победа (вся карта заполнена)
  kGameOver    ///< Поражение (столкновение)
};

/**
 * @brief Класс модели игры "Змейка"
 *
 * Отвечает за игровую логику, состояние поля и змейки,
 * реализацию конечного автомата игры.
 */
class SnakeModel {
 public:
  /**
   * @brief Конструктор модели
   *
   * Инициализирует игровое поле и состояние игры.
   */
  SnakeModel();

  /**
   * @brief Деструктор модели
   *
   * Сохраняет рекорд и освобождает ресурсы.
   */
  ~SnakeModel();

  /// @name Методы для контроллера
  /// @{

  /**
   * @brief Проверяет необходимость обновления состояния
   * @param elapsed_time Время, прошедшее с последнего обновления
   * @return true если требуется обновление, иначе false
   */
  bool ShouldUpdate(std::chrono::milliseconds elapsed_time) const;

  /**
   * @brief Обновляет игровое состояние
   * @return Актуальная игровая информация
   */
  GameInfo_t UpdateInfo();

  /**
   * @brief Возвращает текущее игровое состояние
   * @return Структура с игровой информацией
   */
  GameInfo_t GetGameInfo() const { return game_info_; };

  /// @}

  /// @name Методы управления игрой
  /// @{

  /**
   * @brief Устанавливает направление движения змейки
   * @param direction Новое направление движения
   */
  void SetDirection(SnakeDirection direction);

  /**
   * @brief Активирует временное ускорение змеи
   */
  void SetSpeedBoost();

  /**
   * @brief Устанавливает скорость для запуска новой игры
   */
  void SetGameSpeed();

  /**
   * @brief Переключает состояние паузы
   */
  void SetGamePause();

  /// @}

  /**
   * @brief Освобождает ресурсы игрового поля
   */
  void ClearMatrix();

  /**
   * @brief Структура координат сегмента змейки
   */
  struct SegmentCoor {
    int x;  ///< X-координата сегмента
    int y;  ///< Y-координата сегмента
  };

  /**
   * @brief Структура данных о состоянии змейки
   */
  struct SnakeInfo {
    std::deque<SegmentCoor> body;  ///< Сегменты тела змейки
    SnakeDirection real_direction;  ///< Текущее направление движения
    SnakeDirection next_direction;  ///< Запланированное направление
    bool speed_boost = false;  ///< Флаг временного ускорения

    /**
     * @brief Возвращает координаты головы змейки
     * @return Координаты головы
     */
    SegmentCoor GetHead() const { return body.front(); }

    /**
     * @brief Возвращает координаты хвоста змейки
     * @return Координаты хвоста
     */
    SegmentCoor GetTail() const { return body.back(); }
  };

 private:
  GameInfo_t game_info_;  ///< Текущее состояние игры
  SnakeInfo snake_info_;  ///< Данные о змейке
  FieldState game_state_;  ///< Текущее состояние автомата игры
  friend class SnakeTester;  ///< Дружественный класс для тестирования

  /// @name Приватные вспомогательные методы
  /// @private
  /// @{

  /**
   * @brief Вычисляет порог обновления состояния
   * @return Время до следующего обновления (мс)
   */
  int CalculateUpdateThreshold() const;

  /**
   * @brief Проверяет состояние движения
   * @return true если текущее состояние - движение, иначе false
   */
  bool IsMovementState() const;

  /**
   * @brief Проверяет активность игры
   * @return true если игра активна, иначе false
   */
  bool IsActive() const;

  /// @}

  /// @name Методы конечного автомата
  /// @private
  /// @{

  /**
   * @brief Основной цикл конечного автомата
   */
  void FsmField();

  /**
   * @brief Обработка состояния начала игры
   * @return Следующее состояние автомата
   */
  FieldState FsmStartGame();

  /**
   * @brief Обработка состояния появления яблока
   * @param generator Генератор случайных чисел
   * @return Следующее состояние автомата
   */
  FieldState FsmSpawn(std::mt19937& generator);

  /**
   * @brief Обработка состояния движения
   * @return Следующее состояние автомата
   */
  FieldState FsmMovement();

  /**
   * @brief Обработка состояния победы
   * @return Следующее состояние автомата
   */
  FieldState FsmWin();

  /**
   * @brief Обработка состояния поражения
   * @return Следующее состояние автомата
   */
  FieldState FsmGameOver();

  /// @}

  /// @name Методы работы с игровым полем
  /// @private
  /// @{

  /**
   * @brief Добавляет змейку на игровое поле
   */
  void AddSnakeOnField();

  /**
   * @brief Удаляет змейку с игрового поля
   */
  void RemoveSnakeOnField();

  /**
   * @brief Инициализирует новую змейку в начале игры
   */
  void SpawnNewGameSnake();

  /**
   * @brief Проверяет возможность смены направления
   * @param direction Запрашиваемое направление
   * @return true если смена направления возможна, иначе false
   */
  bool CanChangeDirection(SnakeDirection direction);

  /**
   * @brief Проверяет столкновение головы змейки
   * @param head Координаты головы змейки
   * @return true если столкновение произошло, иначе false
   */
  bool CheckCollision(SegmentCoor head);

  /// @}

  /// @name Методы работы с рекордами
  /// @private
  /// @{

  /**
   * @brief Считывает рекорд из файла
   */
  void ReadScore();

  /**
   * @brief Сохраняет рекорд в файл
   */
  void SaveScore();

  /// @}
};

/**
 * @brief Контроллер игры "Змейка"
 *
 * Обрабатывает пользовательский ввод и обновляет состояние модели.
 */
class SnakeController {
 public:
  SnakeController() = default;
  ~SnakeController() = default;

  /**
   * @brief Обрабатывает действие пользователя
   * @param action Тип действия пользователя
   * @param hold Флаг удержания действия
   */
  void userInput(UserAction_t action, bool hold);

  /**
   * @brief Обновляет игровое состояние
   * @return Актуальная игровая информация
   */
  GameInfo_t updateCurrentState();

 private:
  SnakeModel snake_model_for_controller;  ///< Экземпляр модели игры
};

/**
 * @brief Адаптер для взаимодействия с игрой
 */
namespace SnakeAdapter {
static SnakeController controller;  ///< Контроллер для адаптера

/**
 * @brief Интерфейс обработки пользовательского ввода
 * @param action Тип действия пользователя
 * @param hold Флаг удержания действия
 */
void userInput(UserAction_t action, bool hold);

/**
 * @brief Интерфейс обновления игрового состояния
 * @return Актуальная игровая информация
 */
GameInfo_t updateCurrentState();
}  // namespace SnakeAdapter

}  // namespace s21

#endif  // BRICKGAME_V2_0_BRICK_GAME_SNAKE_SNAKE_H