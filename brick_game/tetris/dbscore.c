#include <sqlite3.h>

#include "../../headers/tetris.h"

/**
 * @brief Сохранение результата в базу данных.
 * @param new_score Новый рекорд который будет сохранен.
 */
void saveScore(int new_score) {
  sqlite3 *db;
  char *err_msg = 0;
  if (sqlite3_open("score.db", &db) == SQLITE_OK) {
    const char *sql_create_table =
        "CREATE TABLE IF NOT EXISTS Scores(Id INTEGER PRIMARY KEY, Score "
        "INTEGER);";
    if (sqlite3_exec(db, sql_create_table, 0, 0, &err_msg) == SQLITE_OK) {
      char sql_update[100];
      snprintf(sql_update, sizeof(sql_update),
               "UPDATE Scores SET Score = %d WHERE id = 1;", new_score);
      if (sqlite3_exec(db, sql_update, 0, 0, &err_msg) != SQLITE_OK) {
        sqlite3_free(err_msg);
      }
    } else {
      sqlite3_free(err_msg);
      sqlite3_close(db);
    }
  }

  sqlite3_close(db);
}

/**
 * @brief Считывание сохраненного результата из базы данных.
 * @return Возвращает сохраненный результат.
 */
int readScore() {
  sqlite3 *db;
  sqlite3_stmt *stmt;
  int high_score = 0;

  sqlite3_open("score.db", &db);
  const char *sql_select = "SELECT MAX(Score) FROM Scores;";
  if (sqlite3_prepare_v2(db, sql_select, -1, &stmt, 0) == SQLITE_OK) {
    if (sqlite3_step(stmt) == SQLITE_ROW) {
      high_score = sqlite3_column_int(stmt, 0);
    }
  }
  sqlite3_finalize(stmt);
  sqlite3_close(db);

  return high_score;
}