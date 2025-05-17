#include "../../headers/tetris.h"

/**
 * @brief Сохранение результата в текстовый файл.
 * @param new_score Новый рекорд который будет сохранен.
 */
void saveScore(int new_score) {
  FILE *score_file = fopen("score.txt", "w");
  fprintf(score_file, "%d", new_score);
  fclose(score_file);
}

/**
 * @brief Считывание сохраненного результата из текстового файла.
 * @return Возвращает сохраненный результат.
 */
int readScore() {
  int high_score = 0;
  char buffer[15] = {0};
  FILE *score_file = fopen("score.txt", "r");
  if (score_file == NULL) {
    score_file = fopen("score.txt", "w");
    if (score_file == NULL) {
      high_score = -1;
    } else {
      fprintf(score_file, "0");
    }
  } else {
    if (fgets(buffer, sizeof(buffer), score_file) != NULL) {
      high_score = atoi(buffer);
    }
  }
  fclose(score_file);
  return high_score;
}