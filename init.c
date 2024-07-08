#include "model.h"
#include "sqlite3_commands.c"

// void RobotsInit()
// {
//     for (int i = 0; i < MAX_ROBOTS; ++i) {
//         Robots.elem[i].free = 1;
//     }
//     Robots.N = MAX_ROBOTS;
// }

void ConveyorsInit()
{
    fprintf(f, "startDepalletize\n"); 
    char *err_msg = 0;
    int rc = sqlite3_open("/Users/glebkruckov/Documents/Работа/Port/port-model/ross-sqlite.db", &db);
    // char *sql_del = "DROP TABLE IF EXISTS Warehouse";
    // char *sql = "CREATE TABLE Warehouse(Type INTEGER, Row INTEGER, Column INTEGER)";
    // sqlite3_exec(db, sql_del, 0, 0, &err_msg);
    // sqlite3_exec(db, sql, 0, 0, &err_msg);

    int change_tmp = 0;

    for (int col = 0; col < MAX_BOXES; ++col) {
        for (int row = 0; row < MAX_CONVEYORS; ++row) {
            Store.conveyor[row].max_length = MAX_BOXES;
            int current_SKU = low_border + (change_tmp) % (high_border - low_border + 1);
            box b;
            b.empty = 0;
            b.SKU = current_SKU;
            Store.conveyor[row].boxes[col] = b;
            change_tmp++;
            Store.conveyor[row].current_length++;
            Store.cnt_boxes_type[current_SKU]++;
            // insert_data(db, current_SKU, row, col);
        }
        // printf("%d\n", Store.cnt_boxes_type[1001]);
    }

    // for (int i = 0; i < MAX_CONVEYORS; ++i) {
    //     Store.conveyor[i].max_length = MAX_BOXES;
    //     Store.conveyor[i].current_length = 0;
    //     for (int j = 0; j < MAX_BOXES; ++j) {
    //         box b;
    //         b.empty = 0;
    //         b.SKU = low_border + (i / 10) % high_border;
    //         Store.conveyor[i].boxes[j] = b;
    //     }
    // }
    fprintf(f, "finishDepalletize\n");
    fprintf(f, "------------------------------------------\n");
    fprintf(f, "startPalletize\n");
    
    for (int i = 0; i < high_border - low_border; ++i) {
        // printf("%d\n", i);
        Store.cnt_boxes_type[i] = 80;
    }

    for (int i = 0; i < 9; ++i) {
        Store.box_data[i][0] = -1;
        Store.box_data[i][1] = 0;
    }
    find_data(db, 1001);
    // insert_data(db, type, row, col);
}

void InitROSS() {
// {
//   srand(time(NULL));
    // RobotsInit();
    ConveyorsInit();

}