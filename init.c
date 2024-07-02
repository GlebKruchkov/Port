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
        Store.conveyor[i].max_length = 8;
        Store.conveyor[i].current_length = 0;
        for (int j = 0; j < MAX_BOXES; ++j) {
            box b;
            b.empty = 1;
            b.SKU = -1;
            Store.conveyor[i].boxes[j] = b;
        }
    }
}

void InitROSS()
{
	srand(time(NULL));
    // RobotsInit();
    ConveyorsInit();

}