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
    char *sql = "CREATE TABLE Warehouse(Type INTEGER, Row INTEGER, Column INTEGER)";
    sqlite3_exec(db, sql, 0, 0, &err_msg);
    for (int i = 0; i < MAX_CONVEYORS; ++i) {
        Store.conveyor[i].max_length = MAX_BOXES;
        Store.conveyor[i].current_length = 0;
        for (int j = 0; j < MAX_BOXES; ++j) {
            box b;
            b.empty = 0;
            b.SKU = 1001 + i / 10;
            Store.conveyor[i].boxes[j] = b;
        }
    }
    fprintf(f, "finishDepalletize\n");
    fprintf(f, "------------------------------------------\n");
    fprintf(f, "startPalletize\n");
    
    for (int i = 0; i < MAX_CONVEYORS / 10; ++i) {
        // printf("%d\n", i);
        Store.cnt_boxes_type[i] = 80;
    }
    for (int i = 0; i < 9; ++i) {
        Store.box_data[i][0] = -1;
        Store.box_data[i][1] = 0;
    }
    insert_data(db);
}

void InitROSS() {
// {
// 	srand(time(NULL));
    // RobotsInit();
    ConveyorsInit();

}