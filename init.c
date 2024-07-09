#include "model.h"

void ConveyorsInit()
{
    fprintf(f, "startDepalletize\n"); 
    char *err_msg = 0;
    char *sql_del = "DROP TABLE IF EXISTS Warehouse";
    char *sql = "CREATE TABLE Warehouse(Type INTEGER, Row INTEGER, Column INTEGER)";
    sqlite3_exec(db, sql_del, 0, 0, &err_msg);
    sqlite3_exec(db, sql, 0, 0, &err_msg);

    int change_tmp = 0;
    for (int i = 0; i < 4; ++i) {
        Store.cnt_boxes_type[i] = 0;
    }

    for (int row = 0; row < MAX_BOXES; ++row) {
        for (int col = 0; col < MAX_CONVEYORS; ++col) {
            Store.conveyor[col].max_length = MAX_BOXES;
            int current_SKU = (low_border + (change_tmp) % (high_border - low_border + 1)) - 1;
            box b;
            b.empty = 0;
            b.SKU = current_SKU;
            Store.conveyor[col].boxes[row] = b;
            change_tmp++;
            Store.conveyor[col].current_length++;
            Store.cnt_boxes_type[current_SKU]++;
            insert_data(db, current_SKU, row, col);
        }
    }
    for (int i = 0; i < 4; ++i) {
        printf("%d\n", Store.cnt_boxes_type[i]);
    }
    fprintf(f, "finishDepalletize\n");
    fprintf(f, "------------------------------------------\n");
    fprintf(f, "startPalletize #1\n");
    for (int i = 0; i < 9; ++i) {
        Store.box_data[i][0] = -1;
        Store.box_data[i][1] = 0;
    }
}

void InitROSS() {
    ConveyorsInit();

}