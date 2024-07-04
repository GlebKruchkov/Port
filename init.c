#include "model.h"

// void RobotsInit()
// {
//     for (int i = 0; i < MAX_ROBOTS; ++i) {
//         Robots.elem[i].free = 1;
//     }
//     Robots.N = MAX_ROBOTS;
// }

void ConveyorsInit()
{
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
    fprintf(f, "The warehouse is palletized\n");
    fprintf(f, "------------------------------------------\n");
    
    for (int i = 0; i < MAX_CONVEYORS / 10; ++i) {
        // printf("%d\n", i);
        Store.cnt_boxes_type[i] = 80;
    }
    for (int i = 0; i < 9; ++i) {
        Store.box_data[i][0] = -1;
        Store.box_data[i][1] = 0;
    }
}

void InitROSS() {
// {
// 	srand(time(NULL));
    // RobotsInit();
    ConveyorsInit();

}