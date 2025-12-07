cmake_minimum_required(VERSION 3.5)
project(BrickGameV2.0)

set(CMAKE_CXX_STANDARD 20)    # C++20
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 COMPONENTS Widgets REQUIRED)
find_package(Curses REQUIRED)

add_executable(BrickGameV2.0 
    brickgame.cc
    brick_game/tetris/tetris.c
    brick_game/snake/snake.cc
    gui/cli/user_screen.c
    gui/desktop/desktop_screen.cc
)

add_compile_options(-Wall -Wextra)

target_include_directories(BrickGameV2.0 PRIVATE ${CURSES_INCLUDE_DIR})
# Линковка всех библиотек и зависимостей
target_link_libraries(BrickGameV2.0
    PRIVATE 
    ${CURSES_LIBRARIES} # ncurses
    Qt6::Widgets    # Qt6 (или Qt5::Widgets)
)

set_target_properties(BrickGameV2.0 PROPERTIES
    AUTOMOC ON
)