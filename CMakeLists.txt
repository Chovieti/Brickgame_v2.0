cmake_minimum_required(VERSION 3.5)
project(BrickGameV2.0)

# Настройка стандартов C и C++
set(CMAKE_C_STANDARD 11)      # C11
set(CMAKE_CXX_STANDARD 20)    # C++20
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

# Включение Qt (только для desktop-фронтенда)
find_package(Qt6 COMPONENTS Widgets REQUIRED)  # Для Qt6 (или Qt5)

# Включение ncurses (только для CLI-фронтенда)
find_package(Curses REQUIRED)

# Основная программа (C++)
add_executable(BrickGameV2.0 
    brickgame.cc
    brick_game/tetris/tetris.c
    brick_game/snake/snake.cpp
    gui/cli/user_screen.c
    gui/desktop/desktop_screen.cpp
)

add_compile_options(-Wall -Wextra)

target_include_directories(BrickGameV2.0 PRIVATE ${CURSES_INCLUDE_DIR})
# Линковка всех библиотек и зависимостей
target_link_libraries(BrickGameV2.0
    PRIVATE 
    # snake_lib       # Бэкенд Snake (C++)
    # tetris_lib      # Бэкенд Tetris (C)
    # cli_gui         # CLI-фронтенд (C) + ncurses
    # desktop_gui     # Desktop-фронтенд (C++) + Qt
    ${CURSES_LIBRARIES} # ncurses
    Qt6::Widgets    # Qt6 (или Qt5::Widgets)
)

# Для Qt6 нужно добавить AUTOMOC (если используются Q_OBJECT)
set_target_properties(BrickGameV2.0 PROPERTIES
    AUTOMOC ON
)