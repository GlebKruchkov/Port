#include "model.h"

void RobotsInit()
{
    for (int i = 0; i < MAX_ROBOTS; ++i) {
        Robots.elem[i].free = 1;
    }
    Robots.N = MAX_ROBOTS;
}

void ConveyorsInit()
{
    for (int i = 0; i < MAX_CONVEYORS; ++i) {
        Conveyors.elem[i].length = 5;
        Conveyors.elem[i].width = 5;
    }
    Conveyors.N = MAX_CONVEYORS;
}

void InitROSS()
{
	srand(time(NULL));
	
    RobotsInit();
    ConveyorsInit();

}