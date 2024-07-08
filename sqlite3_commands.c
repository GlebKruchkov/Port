#include "model.h"

int callback(void *NotUsed, int argc, char **argv, char **azColName) {
    // printf("%d ", atoi(argv[1]));
    // printf("%d\n\n", atoi(argv[2]));
    if (atoi(argv[2]) < best_box.row) {
        best_box.row = atoi(argv[2]);
        best_box.column = atoi(argv[1]);
    }
    return 0;
}

int insert_data(sqlite3 *db, int type, int row, int col) {
    char *err_msg = 0;
    char sql[100];
    sprintf(sql, "INSERT INTO Warehouse(Type, Row, Column) VALUES (%d, %d, %d)", type, row, col);
    sqlite3_exec(db, sql, callback, 0, &err_msg);
    return 0;
}

int find_data(sqlite3 *db, int type) {
    char *err_msg = 0;
    char sql[100];
    sprintf(sql, "SELECT * FROM Warehouse WHERE Type = %d", type);
    sqlite3_exec(db, sql, callback, 0, &err_msg);
    return 0;
}