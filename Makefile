CC = clang
FLAGS_CPP = -Wall -Wextra -std=c++20
FLAGS_C = -Wall -Wextra -std=c11
CURSES_FLAG =  -lncurses
TEST_DIR = tests
TEST_RUNNER = tests/build/test_runner
#  brickgame.cpp
SRC_CONSOLE = gui/cli/user_screen.c
SRC_TETRIS = brick_game/tetris/tetris.c

DIST_DIR = dist
DIST_NAME = brick_game_v1.0.tar.gz
DIST_FILES = $(SRC_TETRIS) $(SRC_CONSOLE) headers Makefile Doxyfile

BUILD_DIR = build
OUTPUT_DIR = tests/report
TBD = tests/build

LINK_TEST_LIBS = -lcheck -lm -lsubunit
VALGRIND = valgrind --tool=memcheck --leak-check=full ./$(TEST_RUNNER)

UNAME := $(shell uname)
ifeq ($(UNAME), Darwin)
	LINK_TEST_LIBS = -lcheck -lm
	VALGRIND = echo "valgrind is not supported on MacOS"
	CLANG_FORMAT = clang_format
endif

# TODO: dvi

all: play

play: install
	$(BUILD_DIR)/BrickGameV2.0

install: uninstall tetris.a console.a
	mkdir -p $(BUILD_DIR)
	cd build && cmake ..
	$(MAKE) -C build
# Вариант до cmake
# $(CC) $(FLAGS_CPP) console.a tetris.a  $(CURSES_FLAG) -lc++ -o $(BUILD_DIR)/brick_game_v2.0

uninstall: 
	rm -rf $(BUILD_DIR)
	rm -rf score.txt score.db

dvi:
	rm -rf doxygen
	doxygen && open doxygen/html/index.html

dist: clean
	mkdir -p $(DIST_DIR)
	tar -czf $(DIST_DIR)/$(DIST_NAME) $(DIST_FILES)
	echo "Archive created!"

tetris.a: $(SRC_TETRIS)
	$(CC) $(FLAGS_C) -c $(SRC_TETRIS)
	ar rcs $@ *.o
	rm -rf *.o

console.a: $(SRC_CONSOLE)
	$(CC) $(FLAGS_C) -c $(SRC_CONSOLE)
	ar rcs $@ *.o
	rm -rf *.o
# TODO
gcov_report_common:
	mkdir -p tests/build
	$(CC) --coverage -fPIC -O0 brick_game/tetris/tetris.c $(TEST_DIR)/*.c -o $(TBD)/test_cov_runner $(LINK_TEST_LIBS)
	./$(TBD)/test_cov_runner
# TODO
gcov_report: gcov_report_common
	@mkdir -p $(OUTPUT_DIR)
	gcovr -e 'tests/*' --html $(OUTPUT_DIR)/gcov_report.html
# TODO
lcov_report: gcov_report_common
	rm -rf  $(TBD)/test_cov_runner-test.gcda
	rm -rf  $(TBD)/test_cov_runner-tetris_test.gcda
	@mkdir -p $(OUTPUT_DIR)
	@lcov --capture --directory . --output-file $(OUTPUT_DIR)/coverage.info
	@genhtml $(OUTPUT_DIR)/coverage.info --output-directory $(OUTPUT_DIR)
# TODO
gcov_report_console: gcov_report_common
	gcov -H $(TBD)/test_cov_runner-tetris.gcno

clean: uninstall
	rm -rf doxygen
	rm -rf *.o *.a
	rm -rf $(TBD) tests/report $(DIST_DIR)

.PHONY: test
test: $(TEST_RUNNER)
	CK_FORK=yes ./$(TEST_RUNNER)

tests/build/test_runner: brick_game.a $(TEST_DIR)/*.c
	mkdir -p tests/build
	$(CC) $(TEST_DIR)/*.c brick_game.a $(LINK_TEST_LIBS) $(FLAGS_C) -o $@

.PHONY: check_valgrind
check_valgrind: $(TEST_RUNNER)
	CK_FORK=no $(VALGRIND)