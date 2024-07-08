#include "model.h"

int callback(void *NotUsed, int argc, char **argv, char **azColoName) {
    return 0;
}

int insert_data(sqlite3 *db) {
    char *err_msg = 0;
    char *sql = "INSERT INTO Warehouse(Type, Row, Column) VALUES (1001, 1, 2)";
    sqlite3_exec(db, sql, callback, 0, &err_msg);
    return 0;
}