#include "model.h"

int callback(void *NotUsed, int argc, char **argv, char **azColName) {
    if (atoi(argv[1]) > best_box.row) {
        best_box.row = atoi(argv[1]);
        best_box.column = atoi(argv[2]);
    }
    return 0;
}

int insert_data(sqlite3 *db, int type, int row, int col) {
    sqlite3_open("/Users/glebkruckov/Documents/Работа/Port/port-model/ross-sqlite.db", &db);
    char *err_msg = 0;
    char sql[100];
    sprintf(sql, "INSERT INTO Warehouse(Type, Row, Column) VALUES (%d, %d, %d)", type, row, col);
    sqlite3_exec(db, sql, 0, 0, &err_msg);
    return 0;
}

int find_data(sqlite3 *db, int type) {
    sqlite3_open("/Users/glebkruckov/Documents/Работа/Port/port-model/ross-sqlite.db", &db);
    best_box.row = -1;
    best_box.column = -1;
    char *err_msg = 0;
    char sql[100];
    sprintf(sql, "SELECT * FROM Warehouse WHERE Type = %d", type);
    sqlite3_exec(db, sql, callback, 0, &err_msg);
    return 0;
}

int Add_Box(int type) {
    find_data(db, -1);
    int col = best_box.column;
    int r = best_box.row;

    sqlite3_open("/Users/glebkruckov/Documents/Работа/Port/port-model/ross-sqlite.db", &db);
    char *err_msg = 0;
    char sql[100];
    sprintf(sql, "DELETE FROM Warehouse WHERE Type = %d AND Row = %d AND Column = %d", -1, r, col);
    sqlite3_exec(db, sql, 0, 0, &err_msg);

    Store.conveyor[col].boxes[r].SKU = type;
    Store.conveyor[col].boxes[r].empty = 0;
    Store.cnt_boxes_type[type]++;

    insert_data(db, type, r, col);
    return col;
}

void Swap_Boxes(int col, int row1, int row2) {
    sqlite3_open("/Users/glebkruckov/Documents/Работа/Port/port-model/ross-sqlite.db", &db);
    bool empty_tmp = Store.conveyor[col].boxes[row1].empty;
    int SKU_tmp = Store.conveyor[col].boxes[row1].SKU;
    char *err_msg = 0;
    char sql[100];
    
    sprintf(sql, "DELETE FROM Warehouse WHERE Type = %d AND Row = %d AND Column = %d", Store.conveyor[col].boxes[row1].SKU, row1, col);
    sqlite3_exec(db, sql, 0, 0, &err_msg);
    char sql2[100];
    sprintf(sql2, "DELETE FROM Warehouse WHERE Type = %d AND Row = %d AND Column = %d", Store.conveyor[col].boxes[row2].SKU, row2, col);
    sqlite3_exec(db, sql2, 0, 0, &err_msg);

    Store.conveyor[col].boxes[row1].empty = Store.conveyor[col].boxes[row2].empty;
    Store.conveyor[col].boxes[row1].SKU = Store.conveyor[col].boxes[row2].SKU;
    Store.conveyor[col].boxes[row2].empty = empty_tmp;
    Store.conveyor[col].boxes[row2].SKU = SKU_tmp;

    insert_data(db, Store.conveyor[col].boxes[row2].SKU, row2, col);
    insert_data(db, Store.conveyor[col].boxes[row1].SKU, row1, col);
}

void Reverse(int col, int row) {
    for (int step = 0; step < 7 - row; ++step) {
        for (int i = 7; i >= 1; --i) {
            if (Store.conveyor[col].boxes[i - 1].empty == 0) {
                Swap_Boxes(col, i, i - 1);
            }
        }
    }
}

int Remove_Boxes(int type) {
    sqlite3_open("/Users/glebkruckov/Documents/Работа/Port/port-model/ross-sqlite.db", &db);
    find_data(db, type);
    int col = best_box.column;
    int row = best_box.row;

    if (row != 7) {
        Reverse(col, row);
    }

    char *err_msg = 0;
    char sql[100];
    sprintf(sql, "DELETE FROM Warehouse WHERE Type = %d AND Row = %d AND Column = %d", type, 7, col);
    sqlite3_exec(db, sql, 0, 0, &err_msg);

    insert_data(db, -1, 7, col);

    Store.conveyor[col].boxes[7].SKU = -1;
    Store.conveyor[col].boxes[7].empty = 1;
    for (int i = 7; i >= 1; --i) {
        Swap_Boxes(col, i, i - 1);
    }
    Store.cnt_boxes_type[type]--;

    for (int i = 0; i < 8; ++i) {
        if (Store.conveyor[col].boxes[i].SKU == -1) {
            fprintf(f, "/%s", "-");
        } else {
            fprintf(f, "/%d", Store.conveyor[col].boxes[i].SKU);
        }
    }
    fprintf(f, "\n");

    return col;
}
 
 
bool Check(int process) { 
    char line[1024];
    char *fields[10];

    if (fgets(line, sizeof(line), file)) {
        fields[0] = strtok(line, ",");
        for (int i = 1; i < 10; i++) {
            fields[i] = strtok(NULL, ",");
        }
        int SKU =  atoi(fields[0]);
        //printf("%s %d ", "SKU", SKU);
        int quantity = atoi(fields[1]);
        //printf("%s %d\n", "quantity", quantity);
        int length = atoi(fields[2]);
        Store.box_data[process - 1][0] = SKU;
        Store.box_data[process - 1][1] = quantity;
        return true;
    } else {
        return false;
    }
}
