#include "model.h"

int CellIdFromName(char* name) {
  int code = (int)(name[5] - 'A');
  if (code == 0) {
    return (int)(name[7] - '0') - 1;
  }
  if (code % 2 == 0) {
    return (int)(name[7] - '0') + 6 + (code - 1) * 6;
  }
  return 7 + code * 6 - (int)(name[7] - '0');
}

void init_graph() {
    for (int i = 0; i < 49; ++i) {
        Store.direction_graph[i] = -1;
    }

    Store.direction_graph[6] = 7;

    Store.direction_graph[7] = 18;



    Store.direction_graph[18] = 19;

    Store.direction_graph[19] = 30;

    Store.direction_graph[30] = 31;

    Store.direction_graph[31] = 42;

    Store.direction_graph[42] = 43;

    Store.direction_graph[48] = 37;

    Store.direction_graph[37] = 36;

    Store.direction_graph[36] = 25;

    Store.direction_graph[25] = 24;

    Store.direction_graph[24] = 13;

    Store.direction_graph[13] = 12;

    Store.direction_graph[12] = 0;
}

void InitVertexNames(char ch, int from, int to) {
    if (from < to) {
        int ind = 1;
        while (from - 1 <= to) {
            char str1[5];
            str1[0] = ch;
            str1[1] = '\0';
            char str2[3];
            sprintf(str2, "%d", ind); 
            strcat(str1, str2);
            strcpy(Store.vertexes[from - 1], str1);
            from++;
            ind++;
        }
    } else {
        int ind = 1;
        while (from > to) {
            char str1[5]; 
            str1[0] = ch;
            str1[1] = '\0';
            char str2[3];
            sprintf(str2, "%d", ind); 
            strcat(str1, str2);
            strcpy(Store.vertexes[from - 1], str1); 
            from--;
            ind++;
        }
    }
}

void ConveyorsInit()
{
    char *err_msg = 0;
    char *sql_del = "DROP TABLE IF EXISTS Warehouse";
    char *sql = "CREATE TABLE Warehouse(Type INTEGER, Row INTEGER, Column INTEGER, Width INTEGER, Channel_Width INTEGER)";
    sqlite3_exec(Store.db, sql_del, 0, 0, &err_msg);
    sqlite3_exec(Store.db, sql, 0, 0, &err_msg);

    init_graph();

    InitVertexNames('A', 1, MAX_RACKS + 1);
    InitVertexNames('B', MAX_RACKS * 2 + 3, MAX_RACKS + 2);
    InitVertexNames('C', MAX_RACKS * 2 + 4, MAX_RACKS * 3 + 3);
    InitVertexNames('D', MAX_RACKS * 5, MAX_RACKS * 3 + 4);
    InitVertexNames('E', MAX_RACKS * 5 + 1, MAX_RACKS * 6);
    InitVertexNames('F', MAX_RACKS * 7 + 2, MAX_RACKS * 6 + 1);
    InitVertexNames('G', MAX_RACKS * 7 + 3, MAX_RACKS * 8 + 2);
    InitVertexNames('H', MAX_RACKS * 9 + 4, MAX_RACKS * 8 + 3);
    Store.vertexes[MAX_VERTEXES][0] = '#';

    Store.kill_prog = false;

    for (int i = 0; i < MAX_CONVEYORS; ++i) {
        Store.conveyor_width[i] = i % 5 + 1;
    }

    for (int i = 0; i < high_border - low_border + 1; ++i) {
        Store.b_w[i] = i % 5;
        // printf("%d\n", Store.b_w[i]);
        box_pair bp;
        bp.SKU = i;
        bp.width = i % 5;
        Store.box_width[i] = bp;
    }

    // Initialization of cells
    for (int i = 0; i < MAX_VERTEXES; ++i) {
        printf("%d\n", i);
        cell c_cell;
        for (int j = 0; j < MAX_ROBOTS; ++j) {
            c_cell.queue[j] = -1;
        }
        c_cell.id = i;
        c_cell.reserved = 0;
        Store.cells[i] = c_cell;
    }

    // Store.vertexes[0] = "B2";
    // Store.vertexes[1] = "B1";
    // Store.vertexes[2] = "A1";
    // Store.vertexes[3] = "A2";
    // Store.vertexes[4] = "A3";
    // Store.vertexes[5] = "B3";
    // Store.vertexes[6] = "C3";
    // Store.vertexes[7] = "C2";
    // Store.vertexes[8] = "C1";
    // Store.vertexes[9] = "D1";
    // Store.vertexes[10] = "D2";
    // Store.vertexes[11] = "D3";

    for (int i = 0; i < (MAX_RACKS + 1) * 4; ++i) {
        if (i == MAX_RACKS * 2 + 1) {
            Store.store_graph[i][0] = i + 1;
            Store.store_graph[i][1] = 0;     
        } else if (i == MAX_RACKS * 3 + 2) {
            Store.store_graph[i][0] = i + 1;
            Store.store_graph[i][1] = MAX_RACKS + 1;
        } else if (i == MAX_RACKS * 4 + 3) {
            Store.store_graph[i][0] = MAX_RACKS * 2 + 2;
            Store.store_graph[i][1] = -1;  
        } else {
            Store.store_graph[i][0] = i + 1;
            Store.store_graph[i][1] = -1;
        }
    }

    char line[256];
    char *fields[3];
    fgets(line, sizeof(line), bots_starting_positions);
    
    // Initialization of robots to take out
    // for (int i = 0; i < MAX_ROBOTS; ++i) {
    //     Store.messages[i].type = GO;
    //     robot bot;
    //     bot.tmp_fl = 1;
    //     bot.cur_task = -1;
    //     bot.cur_box = -1;
    //     bot.pre_reserved = -1;
    //     bot.low_SKU = -1;
    //     bot.col = -1;
    //     bot.row = -1;
    //     bot.kill = 0;
    //     bot.has_box = -1;
    //     bot.reserved_channel = -1;
    //     bot.cur_time = 0;
    //     bot.goal_time = 0;
    //     fgets(line, sizeof(line), bots_starting_positions);
    //     fields[0] = strtok(line, ",");
    //     fields[1] = strtok(NULL, ",");
        
    //     if (fields[1][5] == 'A') {
    //         bot.cur_cell = Store.cells[MAX_RACKS * 4 + 4 - (int)(fields[1][7] - '0')];
    //         bot.prev_cell = Store.cells[MAX_RACKS * 4 + 4 - (int)(fields[1][7] - '0')];
    //         Store.cells[MAX_RACKS * 4 + 4 - (int)(fields[1][7] - '0')].reserved = 1;
    //     } else if (fields[1][5] == 'D') {
    //         bot.cur_cell = Store.cells[(int)(fields[1][7] - '0') - 1];
    //         bot.prev_cell = Store.cells[(int)(fields[1][7] - '0') - 1];
    //         Store.cells[(int)(fields[1][7] - '0') - 1].reserved = 1;
    //     } else if (fields[1][5] == 'B') {
    //         bot.cur_cell = Store.cells[MAX_RACKS * 2 + 1 + (int)(fields[1][7] - '0')];
    //         bot.prev_cell = Store.cells[MAX_RACKS * 2 + 1 + (int)(fields[1][7] - '0')];
    //         Store.cells[MAX_RACKS * 2 + 1 + (int)(fields[1][7] - '0')].reserved = 1;
    //     } else if (fields[1][5] == 'C') {
    //         bot.cur_cell = Store.cells[MAX_RACKS * 2 + 2 - (int)(fields[1][7] - '0')];
    //         bot.prev_cell = Store.cells[MAX_RACKS * 2 + 2 - (int)(fields[1][7] - '0')];
    //         Store.cells[MAX_RACKS * 2 + 2 - (int)(fields[1][7] - '0')].reserved = 1;
    //     } else {
    //         printf("WRONG BOTS STARTING POZITION\n");
    //         exit(0);
    //     }
    //     Store.robots[i] = bot;
    // }
    
    for (int i = 0; i < MAX_ROBOTS; ++i) {

        fgets(line, sizeof(line), bots_starting_positions);
        fields[0] = strtok(line, ",");
        fields[1] = strtok(NULL, ",");

        Store.messages[i].type = GO;
        robot bot;
        bot.tmp_fl = 1;
        bot.cur_task = -1;
        bot.cur_box = -1;
        bot.pre_reserved = -1;
        bot.low_SKU = -1;
        bot.col = -1;
        bot.row = -1;
        bot.kill = 0;
        bot.has_box = -1;
        bot.reserved_channel = -1;
        bot.cur_time = 0;
        bot.goal_time = 0;
        bot.cur_cell = Store.cells[CellIdFromName(fields[1])];
        Store.robots[(int)(fields[0][0] - '0') - 1] = bot;    
    }

    
    int change_tmp = 0;
    for (int i = 0; i < 4; ++i) {
        Store.cnt_boxes_type[i] = 0;
        Store.cnt_boxes_type_const[i] = 0;
    }
    int tempor = 1;

    for (int row = 0; row < MAX_BOXES; ++row) {
        for (int col = 0; col < MAX_CONVEYORS; ++col) {
            box b;
            b.empty = 1;
            b.SKU = -1;
            b.width = -1;
            Store.conveyor[col].boxes[row] = b;
        }
    }
    int id = 1;
    for (int row = MAX_BOXES - 1; row >= 0; --row) {
        for (int col = 0; col < MAX_CONVEYORS; ++col) {
            int current_SKU = (low_border + (change_tmp) % (high_border - low_border));
            Store.conveyor[col].boxes[row].SKU = -1;
            Store.conveyor[col].boxes[row].width = -1;
            Store.conveyor[col].boxes[row].empty = 1;
            change_tmp++;
            
            insert_data(&(Store.db), -1, row, col, -1);
        }
    }
    
    for (int i = 0; i < 10; ++i) {
        Store.used[i] = 0;
    }
    Store.boxes_to_deliver = 0;
    Store.cur_file = 0;

    for (int i = 0; i < 7; ++i) {
        Store.box_data[i][0] = -1;
        Store.box_data[i][1] = 0;
        Store.arr_time[i] = 0;
    }
}

void InitROSS() {
    ConveyorsInit();

}