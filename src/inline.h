#ifndef BRICKGAME_V2_0_INLINE_H
#define BRICKGAME_V2_0_INLINE_H

namespace s21 {
/// @name Общие константы
/// @{
inline constexpr int kSpeedForStart = 1;  ///< Скорость для старта новой игры
inline constexpr int kGameOverSpeed = -1;  ///< Скорость при проигрыше
inline constexpr int kMaxSpeed = 100;  ///< Максимальная скорость в игре
inline constexpr int kWinSpeed = 200;  ///< Скорость при победе
inline constexpr int kBaseGameDelay =
    120;  ///< Базовая задержка между тиками игры
inline constexpr int kHeight = 20;  ///< Размер игрового поля по высоте
inline constexpr int kWidth = 10;  ///< Размер игрового поля по ширине
inline constexpr int kNextSize = 4;    ///< Размер поля NEXT
inline constexpr int kStartPoint = 0;  ///< Стартовая точка
/// @}
}  // namespace s21

#endif  // BRICKGAME_V2_0_INLINE_H