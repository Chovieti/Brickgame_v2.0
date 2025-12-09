CC = gcc
CPP = g++
FLAGS_CPP = -Wall -Wextra -std=c++20
FLAGS_C = -Wall -Wextra -std=c11

TEST_CPP_FLAGS = -lgtest -lgtest_main
TEST_C_FLAGS = -lcheck -lm -lsubunit

CURSES_FLAG =  -lncurses
TEST_DIR = tests
TEST_RUNNER = tests/build/test_runner

SRC_CONSOLE = gui/cli/user_screen.c
SRC_TETRIS = brick_game/tetris/tetris.c
SRC_DESKTOP = gui/desktop/desktop_screen.cc
SRC_SNAKE = brick_game/snake/snake.cc

DIST_DIR = dist
DIST_NAME = brick_game_v2.0.tar.gz
DIST_FILES = $(SRC_TETRIS) $(SRC_CONSOLE) $(SRC_SNAKE) $(SRC_DESKTOP) headers Makefile CMakeLists.txt Doxyfile brickgame.cc *.h *.png

BUILD_DIR = build
OUTPUT_GCOV_DIR = tests/report
TBD = tests/build


UNAME := $(shell uname)
ifeq ($(UNAME), Darwin)
	TEST_C_FLAGS = -lcheck -lm
endif

all: install

install: uninstall
	mkdir -p $(BUILD_DIR)
	cd build && cmake ..
	$(MAKE) -C build

play:  build/BrickGameV2.0 
	./build/BrickGameV2.0 

uninstall: 
	rm -rf $(BUILD_DIR)
	rm -rf *_score.txt

dvi:
	rm -rf doxygen
	doxygen && open doxygen/html/index.html

dist: clean
	mkdir -p $(DIST_DIR)
	tar -czf $(DIST_DIR)/$(DIST_NAME) $(DIST_FILES)
	echo "Archive created!"

.PHONY: test
test: $(TEST_RUNNER)
# CK_FORK=yes ./$(TEST_RUNNER)_tetris
	CK_FORK=yes ./$(TEST_RUNNER)_snake

tests/build/test_runner: snake.a $(TEST_DIR)/*.cc
#  tetris.a $(TEST_DIR)/*.c
	mkdir -p tests/build
#  $(CC) $(TEST_DIR)/*.c tetris.a $(TEST_C_FLAGS) $(FLAGS_C) -o $@_tetris
	$(CPP) $(TEST_DIR)/*.cc snake.a $(TEST_CPP_FLAGS)  $(FLAGS_CPP) -o $@_snake

tetris.a: $(SRC_TETRIS)
	$(CC) $(FLAGS_C) -c $(SRC_TETRIS)
	ar rcs $@ *.o
	rm -rf *.o

snake.a: $(SRC_SNAKE)
	$(CPP) $(FLAGS_CPP) -c $(SRC_SNAKE)
	ar rcs $@ *.o
	rm -rf *.o

gcov_report: gcov_report_common
	mkdir -p $(OUTPUT_GCOV_DIR)
	gcovr -r . \
		--exclude 'tests/build/' \
		--exclude 'tests/' \
		--exclude '.*/gtest/.*' \
		--html-details \
		--print-summary \
		--html-title "Snake Game Coverage Report" \
		--html-theme blue \
		--exclude-unreachable-branches \
		--exclude-throw-branches \
		--decisions \
		-o $(OUTPUT_GCOV_DIR)/gcovr_report.html

gcov_report_common: snake_gcov.a $(TEST_DIR)/*.cc 
# tetris_gcov.a $(TEST_DIR)/*.c
	mkdir -p $(TBD)
# $(CC) $(FLAGS_C) $(TEST_DIR)/*.c tetris_gcov.a -fprofile-arcs -ftest-coverage $(TEST_C_FLAGS) -lpthread -o tests/build/tetris_test
	$(CPP) $(FLAGS_CPP) $(TEST_DIR)/snake_test.cc snake_gcov.a -fprofile-arcs -ftest-coverage $(TEST_CPP_FLAGS) -lpthread -o tests/build/snake_test
# ./$(TBD)/tetris_test
	./$(TBD)/snake_test

tetris_gcov.a: $(SRC_TETRIS)
	$(CC) $(FLAGS_C) -fprofile-arcs -ftest-coverage -c $(SRC_TETRIS) 
	ar rcs $@ *.o
	rm -rf *.o

snake_gcov.a: $(SRC_SNAKE)
	$(CPP) $(FLAGS_CPP) -fprofile-arcs -ftest-coverage -c $(SRC_SNAKE)
	ar rcs $@ *.o
	rm -rf *.o

clean: uninstall
	rm -rf doxygen
	rm -rf *.o *.a
	rm -rf *.g*
	rm -rf $(TBD) tests/report $(DIST_DIR)