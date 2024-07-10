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
    int tempor = 1;

    for (int row = 0; row < MAX_BOXES; ++row) {
        for (int col = 0; col < MAX_CONVEYORS; ++col) {
            box b;
            b.empty = 1;
            b.SKU = -1;
            Store.conveyor[col].boxes[row] = b;
        }
    }
    int id = 1;
    for (int row = MAX_BOXES - 1; row >= 0; --row) {
        for (int col = 0; col < MAX_CONVEYORS; ++col) {
            Store.conveyor[col].max_length = MAX_BOXES;
            int current_SKU = (low_border + (change_tmp) % (high_border - low_border + 1)) - 1;
            Store.conveyor[col].boxes[row].SKU = current_SKU;
            Store.conveyor[col].boxes[row].empty = 0;
            change_tmp++;
            Store.conveyor[col].current_length++;
            Store.cnt_boxes_type[current_SKU]++;
            insert_data(db, current_SKU, row, col);
            fprintf(f_dep, "%*d   %*d   movebox%*d   channel%*d    ", 4, id, 4, glb_time, 5, current_SKU, 6, col);
            ++id;
            for (int i = 0; i < MAX_BOXES; ++i) {
                if (Store.conveyor[col].boxes[i].empty) {
                    fprintf(f_dep, "| - ");
                } else {
                    fprintf(f_dep, "|%*d", 3, Store.conveyor[col].boxes[i].SKU);
                }
            }
            fprintf(f_dep, "|\n");
            if (tempor % 10 == 0) {
                glb_time += 8;
            }
            tempor += 1;
        }
    }

    for (int i = 0; i < 4; ++i) {
        printf("%d\n", Store.cnt_boxes_type[i]);
    }
    fprintf(f, "finishDepalletize\n");
    fprintf(f, "------------------------------------------------------------------------------------\n");
    fprintf(f, "startPalletize #1\n");
    for (int i = 0; i < 10; ++i) {
        Store.box_data[i][0] = -1;
        Store.box_data[i][1] = 0;
        Store.arr_time[i] = 0;
    }
}

void InitROSS() {
    ConveyorsInit();

}