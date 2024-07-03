//The header file template for a ROSS model
//This file includes:
// - the state and message structs
// - extern'ed command line arguments
// - custom mapping function prototypes (if needed)
// - any other needed structs, enums, unions, or #defines

#ifndef _model_h
#define _model_h

#include "ross.h"

#define MAX_BOXES 8
// #define MAX_ROBOTS 50
#define MAX_CONVEYORS 60000

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
#include <string.h>
#include <dirent.h>

typedef struct
{
    bool empty;
    int SKU;
} box;

typedef enum
{
    TAKE_IN,
    TAKE_OUT,
    REVERSE
} message_type;

typedef struct
{
    message_type type;
    double contents;
    tw_lpid sender;
} message;

// struct _robot
// {
//     int free;
// };

// struct _robots
// {
//     struct _robot elem[MAX_ROBOTS];
//     int N;
// };

// struct _robots Robots;

struct _conveyor
{
    int max_length;
    int current_length;
    box boxes[MAX_BOXES];
};

struct _Store
{
    int cnt_boxes_type[MAX_CONVEYORS / 10];
    struct _conveyor conveyor[MAX_CONVEYORS];
    int N;
    bool full;
};

struct _Store Store;

typedef enum
{
    COMMAND_CENTER,
    CONVEYOR,
} lp_type;

//State struct
//   this defines the state of each LP
typedef struct {
  int got_msgs_TAKE_IN;
  int got_msgs_TAKE_OUT;
  int got_msgs_REVERSE;
  lp_type type;
  int rcvd_count_H; //УБРАТЬ НАХУЙ
  int rcvd_count_G; //УБРАТЬ НАХУЙ
  double value;
} state;


//Command Line Argument declarations
extern unsigned int setting_1;

//Global variables used by both main and driver
// - this defines the LP types
extern tw_lptype model_lps[];

//Function Declarations
// defined in model_driver.c:
extern void model_init(state *s, tw_lp *lp);
extern void model_event(state *s, tw_bf *bf, message *in_msg, tw_lp *lp);
extern void model_event_reverse(state *s, tw_bf *bf, message *in_msg, tw_lp *lp);
extern void model_final(state *s, tw_lp *lp);
// defined in model_map.c:
extern tw_peid model_map(tw_lpid gid);

/*
//Custom mapping prototypes
void model_cutom_mapping(void);
tw_lp * model_mapping_to_lp(tw_lpid lpid);
tw_peid model_map(tw_lpid gid);
*/

#endif
