#include "model.h"

//extern sqlite3 *db;

int callback(void *NotUsed, int argc, char **argv, char **azColName) {
    if (atoi(argv[1]) > best_box.row) {
        best_box.row = atoi(argv[1]);
        best_box.column = atoi(argv[2]);
    }
    return 0;
}

int callback_by_width(void *NotUsed, int argc, char **argv, char **azColName) {
    // for (int i = 0; i < argc; i++) {
    //     printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    // }
    // printf("\n");
    if (atoi(argv[1]) > best_box.row) {
        best_box.row = atoi(argv[1]);
        best_box.column = atoi(argv[2]);
    }
    return 0;
}

int compare(const void *a, const void *b) {
    const box_pair *boxA = (const box_pair *)a;
    const box_pair *boxB = (const box_pair *)b;
    return boxA->width - boxB->width;
}

int insert_data(sqlite3 **db1, int type, int row, int col, int width) {
    struct sqlite3 * db = (struct sqlite3 *) *db1;
    char *err_msg = 0;
    char sql[100];
    sprintf(sql, "INSERT INTO Warehouse(Type, Row, Column, Width, Channel_Width) VALUES (%d, %d, %d, %d, %d)", type, row, col, width, Store.conveyor_width[col]);
    sqlite3_exec(db, sql, 0, 0, &err_msg);
    return 0;
}

int find_data(sqlite3 **db1, int type) {
    struct sqlite3 * db = (struct sqlite3 *) *db1;
    best_box.row = -1;
    best_box.column = -1;
    char *err_msg = 0;
    char sql[100];
    sprintf(sql, "SELECT * FROM Warehouse WHERE Type = %d", type);
    sqlite3_exec(db, sql, callback, 0, &err_msg);
    return 0;
}

int find_data_by_width(sqlite3 **db1, int type) {
    struct sqlite3 * db = (struct sqlite3 *) *db1;
    best_box.row = -1;
    best_box.column = -1;
    char *err_msg = 0;
    char sql[100];
    // printf("%d\n", 1);
    // printf("%d %d\n", type, Store.b_w[type]);
    sprintf(sql, "SELECT * FROM Warehouse WHERE Channel_Width >= %d AND Type = %d", Store.b_w[type], -1);
    // exit(0);
    sqlite3_exec(db, sql, callback_by_width, 0, &err_msg);
    return 0;
}

int Add_Box(sqlite3 **db1, int type) {
    struct sqlite3 * db = (struct sqlite3 *) *db1;
    find_data_by_width(db1, type);
    int col = best_box.column;
    int r = best_box.row;
    char *err_msg = 0;
    char sql[100];
    sprintf(sql, "DELETE FROM Warehouse WHERE Type = %d AND Row = %d AND Column = %d", -1, r, col);
    sqlite3_exec(db, sql, 0, 0, &err_msg);

    Store.conveyor[col].boxes[r].SKU = type;
    Store.conveyor[col].boxes[r].empty = 0;
    Store.conveyor[col].boxes[r].width = Store.b_w[type];
    Store.cnt_boxes_type[type]++;

    insert_data(db1, type, r, col, Store.b_w[type]);
    return col;
}

void Swap_Boxes(sqlite3 **db1, int col, int row1, int row2) {
    struct sqlite3 * db = (struct sqlite3 *) *db1;
    bool empty_tmp = Store.conveyor[col].boxes[row1].empty;
    int SKU_tmp = Store.conveyor[col].boxes[row1].SKU;
    int width_tmp =  Store.conveyor[col].boxes[row1].width;
    char *err_msg = 0;
    char sql[100];
    
    sprintf(sql, "DELETE FROM Warehouse WHERE Type = %d AND Row = %d AND Column = %d", Store.conveyor[col].boxes[row1].SKU, row1, col);
    sqlite3_exec(db, sql, 0, 0, &err_msg);
    char sql2[100];
    sprintf(sql2, "DELETE FROM Warehouse WHERE Type = %d AND Row = %d AND Column = %d", Store.conveyor[col].boxes[row2].SKU, row2, col);
    sqlite3_exec(db, sql2, 0, 0, &err_msg);

    Store.conveyor[col].boxes[row1].empty = Store.conveyor[col].boxes[row2].empty;
    Store.conveyor[col].boxes[row1].SKU = Store.conveyor[col].boxes[row2].SKU;
    Store.conveyor[col].boxes[row1].width = Store.conveyor[col].boxes[row2].width;
    Store.conveyor[col].boxes[row2].empty = empty_tmp;
    Store.conveyor[col].boxes[row2].SKU = SKU_tmp;
    Store.conveyor[col].boxes[row2].width = width_tmp;

    insert_data(db1, Store.conveyor[col].boxes[row2].SKU, row2, col, Store.conveyor[col].boxes[row2].width);
    insert_data(db1, Store.conveyor[col].boxes[row1].SKU, row1, col, Store.conveyor[col].boxes[row1].width);
}

void Reverse(sqlite3 **db1, int col, int row) {
    for (int step = 0; step < 7 - row; ++step) {
        for (int i = 7; i >= 1; --i) {
            if (Store.conveyor[col].boxes[i - 1].empty == 0) {
                Swap_Boxes(db1, col, i, i - 1);
            }
        }
        fprintf(f, "reverse%*d ", 4, best_box.column);
    }
    fprintf(f, "\n");
}

int Remove_Boxes(sqlite3 **db1, int type) {
    struct sqlite3 * db = (struct sqlite3 *) *db1;
    find_data(db1, type);
    int col = best_box.column;
    int row = best_box.row;

    if (row != 7) {
        Reverse(db1, col, row);
    }

    char *err_msg = 0;
    char sql[100];
    sprintf(sql, "DELETE FROM Warehouse WHERE Type = %d AND Row = %d AND Column = %d", type, 7, col);
    sqlite3_exec(db, sql, 0, 0, &err_msg);

    insert_data(db1, -1, 7, col, -1);

    Store.conveyor[col].boxes[7].SKU = -1;
    Store.conveyor[col].boxes[7].empty = 1;
    Store.conveyor[col].boxes[7].width = -1;

    for (int i = 7; i >= 1; --i) {
        Swap_Boxes(db1, col, i, i - 1);
    }
    Store.cnt_boxes_type[type]--;

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
        int quantity = atoi(fields[1]);
        int length = atoi(fields[2]);
        Store.box_data[process][0] = SKU;
        Store.box_data[process][1] = quantity;
        return true;
    } else {
        return false;
    }
}
