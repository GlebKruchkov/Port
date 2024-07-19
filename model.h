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
#define MAX_CELLS 12
#define MAX_ROBOTS 6
// #define MAX_ROBOTS 50
#define MAX_CONVEYORS 100
#define MEM_POOL_SIZE (512 * 1024 * 1024)

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
#include "sqlite3.h"

static const int low_border = 1;
static const int high_border = 21;

// static const int threshold = 38;

static int is_reverse = 0;
static int glb_time = 0;
static int log_id = 1;
FILE *file;
FILE *f;
FILE *f_dep;
FILE *temp_txt;

const static double g_robot_calc_time = 0.001;
static const int threshold = (int)((MAX_BOXES * MAX_CONVEYORS) / (high_border - low_border + 1));

typedef struct
{
    bool reserved;
    bool empty;
    int SKU;
    int width;
} box;

struct _best_box
{
    int row;
    int column;
};

typedef enum
{
    TAKE_IN,
    TAKE_OUT,
    REVERSE,
    GO
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

typedef struct
{
    int width;
    int SKU;
} box_pair;

typedef struct
{
    int id;
    bool reserved;
} cell;

typedef struct
{
    int row;
    int col;
    int cur_task;
    int cur_conv;
    int has_box;
    int reserved_channel;
    cell cur_cell;
} robot;

typedef struct {
    int requests[3000][2];
    int total;
    int curr;
} file_requests;

struct _Store
{
    file_requests request;
    sqlite3 *db;
    int box_data[7][2];
    int arr_time[7];
    int graph[12];

    robot robots[MAX_ROBOTS];
    cell cells[MAX_CELLS];

    //int prev_commands[10][36]; // 1 + 7 * 5
    int times_to_inc;
    
    int b_w[high_border - low_border + 1];
    box_pair box_width[high_border - low_border + 1];
    int conveyor_width[MAX_CONVEYORS];
    int cnt_boxes_type[high_border - low_border + 1];
    int cnt_boxes_type_const[high_border - low_border + 1];
    struct _conveyor conveyor[MAX_CONVEYORS];

    int used[7];
    int boxes_to_deliver;
    int type_to_add;
    int N;
    bool full;
};

struct _Store Store;
struct _best_box best_box;

typedef struct {
  int got_msgs_TAKE_IN;
  int got_msgs_TAKE_OUT;
  int got_msgs_REVERSE;
//   lp_type type;
  double value;
} state;

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

extern int callback(void *NotUsed, int argc, char **argv, char **azColName);
extern int insert_data(sqlite3 **db1, int type, int row, int col, int width);
extern int find_data(sqlite3 **db1, int type);
extern int find_data_by_width(sqlite3 **db1, int type);
extern int Add_Box(sqlite3 **db1, int type, int process);
extern void Swap_Boxes(sqlite3 **db1, int row, int col1, int col2);
extern int Reverse(sqlite3 **db1, int row, int col, int *time, int *l_id, int process);
extern int Remove_Boxes(sqlite3 **db, int type, int *time, int *l_id, int process);
extern bool Check(int process);
extern int compare(const void *a, const void *b);
extern void Send_Event(int process, message_type command, tw_lp *lp, tw_lpid *self);
extern void Print_Channel(int col, FILE *log_file);
#endif
