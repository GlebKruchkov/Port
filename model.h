#ifndef _model_h
#define _model_h

#include "ross.h"

#define MAX_BOXES 8
#define MAX_ROBOTS 15
#define MAX_CONVEYORS 150
#define MEM_POOL_SIZE (512 * 1024 * 1024)

#define MAX_FILES 14

#define MAX_RACKS 5

#define MAX_VERTEXES 49

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
static const int high_border = 51;

static int is_reverse = 0;
static int glb_time = 0;
static int event_id = 1;
static int control_id = 1;
static int cur_boxes = 0;
static int palet_type = 1;
static int rec_id = 1;

FILE *file;
FILE *f;
FILE *f_dep;
FILE *temp_txt;
FILE *bots_starting_positions;
FILE *paleta;
FILE *csv_file;

FILE *test;
FILE *robots_positions;
FILE *control_system_log;

const static double g_robot_calc_time = 0.001;
static const int threshold = (int)((MAX_BOXES * MAX_CONVEYORS) / (high_border - low_border));

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
    int queue[MAX_ROBOTS];
} cell;

typedef struct
{
    int row;
    int col;
    int cur_task;
    int has_box;
    int reserved_channel;
    int low_SKU;

    int kill;
    cell cur_cell;
    cell goal_cell;

    int pre_reserved;

    int cur_time;
    int goal_time;
    int cur_box;
    int tmp_fl;

    int prev_vertex;
    int prev_box_type;
    int prev_channel;
    int prev_tr_id;
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

    message messages[MAX_ROBOTS];

    char cur_order[50];

    char files[15][1024];
    int cur_file;

    int direction_graph[MAX_VERTEXES];

    char vertexes[MAX_VERTEXES + 1][5];
    int box_data[MAX_ROBOTS + 1][2];
    int kill_prog;

    robot robots[MAX_ROBOTS];
    cell cells[MAX_VERTEXES];

    
    int b_w[high_border - low_border + 1];
    box_pair box_width[high_border - low_border + 1];
    int conveyor_width[MAX_CONVEYORS];
    int cnt_boxes_type[high_border - low_border + 1];
    int cnt_boxes_type_const[high_border - low_border + 1];
    struct _conveyor conveyor[MAX_CONVEYORS];

    int used[MAX_ROBOTS + 1];
    int nt_used[MAX_ROBOTS + 1];
    int boxes_to_deliver;
    int type_to_add;
};

struct _Store Store;
struct _best_box best_box;

typedef struct {
  int got_msgs_TAKE_IN;
  int got_msgs_TAKE_OUT;
  int got_msgs_REVERSE;
  double value;
} state;

extern unsigned int setting_1;

extern tw_lptype model_lps[];

extern void model_init(state *s, tw_lp *lp);
extern void model_event(state *s, tw_bf *bf, message *in_msg, tw_lp *lp);
extern void model_event_reverse(state *s, tw_bf *bf, message *in_msg, tw_lp *lp);
extern void model_final(state *s, tw_lp *lp);
extern tw_peid model_map(tw_lpid gid);

extern int callback(void *NotUsed, int argc, char **argv, char **azColName);
extern int insert_data(sqlite3 **db1, int type, int row, int col, int width);
extern int find_data(sqlite3 **db1, int type);
extern int find_data_by_width(sqlite3 **db1, int type);
extern int Add_Box(sqlite3 **db1, int type, int process);
extern void Swap_Boxes(sqlite3 **db1, int row, int col1, int col2);
extern int Reverse(sqlite3 **db1, int row, int col, int *time, int *l_id, int process);
extern int Remove_Boxes(sqlite3 **db, int type, int *time, int *l_id, int process);
extern void Init_Commands(int *event_id, int *rec_id, int *time, const char *filename);
extern bool Check(int process);
extern int compare(const void *a, const void *b);
extern void Send_Event(int process, message_type command, tw_lp *lp, tw_lpid *self);
extern void Print_Channel(int col, FILE *log_file);
extern void write_csv(const char *filename, sqlite3 *db);
extern void add_to_queue(int robot_id, int next_vert);
extern void del_from_queue(int robot_id);
extern int next_vertex(int cur_vertex, int cur_goal);
extern int GeTrIdFromBot(int curr_cell);
#endif
