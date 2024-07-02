#define MAX_BOXES 50
#define MAX_ROBOTS 50
#define MAX_CONVEYORS 50

// #include "ross.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include <assert.h>
#include <math.h>
#include <time.h>

typedef enum
{
    TAKE,
    PUT_BACK,
    PUT_FIN
} message_type;

struct _robot
{
    int free;
};

struct _robots
{
    struct _robot elem[MAX_ROBOTS];
    int N;
};

struct _robots Robots;

struct _conveyor
{
    int width;
    int length;
    int boxes[MAX_BOXES];
};

struct _conveyors
{
    struct _conveyor elem[MAX_CONVEYORS];
    int N;
};

struct _conveyors Conveyors;





