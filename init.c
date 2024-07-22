#include "model.h"

void ConveyorsInit()
{
    char *err_msg = 0;
    char *sql_del = "DROP TABLE IF EXISTS Warehouse";
    char *sql = "CREATE TABLE Warehouse(Type INTEGER, Row INTEGER, Column INTEGER, Width INTEGER, Channel_Width INTEGER)";
    sqlite3_exec(Store.db, sql_del, 0, 0, &err_msg);
    sqlite3_exec(Store.db, sql, 0, 0, &err_msg);

    int a_tmp = MAX_RACKS * 4 + 3;
    int b_tmp = MAX_RACKS * 2 + 2;
    int c_tmp = MAX_RACKS * 2 + 1;
    int ind = 1;
    while (ind - 1 <= MAX_RACKS) {
        char str1[5] = "D";
        char str2[3];
        sprintf(str2, "%d", ind);
        strcat(str1, str2);
        strcpy(Store.vertexes[ind - 1], str1);
        ind++;
    }
    ind = 1;
    while (c_tmp > MAX_RACKS) { 
        char str1[5] = "C";
        char str2[3];
        sprintf(str2, "%d", ind);
        strcat(str1, str2);
        strcpy(Store.vertexes[c_tmp], str1);
        ind++;
        c_tmp--;
    }
    ind = 1;
    while (b_tmp <= MAX_RACKS * 3 + 2) { 
        char str1[5] = "B";
        char str2[3];
        sprintf(str2, "%d", ind);
        strcat(str1, str2);
        strcpy(Store.vertexes[b_tmp], str1);
        ind++;
        b_tmp++;
    }
    ind = 1;
    while (a_tmp >= MAX_RACKS * 3 + 3) { 
        char str1[5] = "A";
        char str2[3];
        sprintf(str2, "%d", ind);
        strcat(str1, str2);
        strcpy(Store.vertexes[a_tmp], str1);
        ind++;
        a_tmp--;
    }

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
    for (int i = 0; i < MAX_CELLS; ++i) {
        cell c_cell;
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
    for (int i = 0; i < MAX_ROBOTS; ++i) {
        robot bot;
        bot.cur_task = -1;
        bot.low_SKU = -1;
        bot.col = -1;
        bot.row = -1;
        bot.kill = 0;
        bot.has_box = -1;
        bot.reserved_channel = -1;
        bot.cur_time = 0;
        bot.goal_time = 0;
        fgets(line, sizeof(line), bots_starting_positions);
        fields[0] = strtok(line, ",");
        fields[1] = strtok(NULL, ",");
        
        if (fields[1][5] == 'A') {
            bot.cur_cell = Store.cells[MAX_RACKS * 4 + 4 - (int)(fields[1][7] - '0')];
            Store.cells[MAX_RACKS * 4 + 4 - (int)(fields[1][7] - '0')].reserved = 1;
        } else if (fields[1][5] == 'D') {
            bot.cur_cell = Store.cells[(int)(fields[1][7] - '0') - 1];
            Store.cells[(int)(fields[1][7] - '0') - 1].reserved = 1;
        } else if (fields[1][5] == 'B') {
            bot.cur_cell = Store.cells[MAX_RACKS * 2 + 1 + (int)(fields[1][7] - '0')];
            Store.cells[MAX_RACKS * 2 + 1 + (int)(fields[1][7] - '0')].reserved = 1;
        } else if (fields[1][5] == 'C') {
            bot.cur_cell = Store.cells[MAX_RACKS * 2 + 2 - (int)(fields[1][7] - '0')];
            Store.cells[MAX_RACKS * 2 + 2 - (int)(fields[1][7] - '0')].reserved = 1;
        } else {
            printf("WRONG BOTS STARTING POZITION\n");
            exit(0);
        }
        Store.robots[i] = bot;
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
            Store.conveyor[col].max_length = MAX_BOXES;
            int current_SKU = (low_border + (change_tmp) % (high_border - low_border + 1)) - 1;


            int current_conv_len = Store.conveyor_width[col];
            int answer = -100;
            int cur_box_len = -1;
            for (int con = 1; con < high_border - low_border + 1; ++con) {
                if (Store.box_width[con].width > cur_box_len && Store.box_width[con].width <= current_conv_len && Store.cnt_boxes_type[Store.box_width[con].SKU] < MAX_CONVEYORS * MAX_BOXES / (high_border - low_border)) {
                    cur_box_len = Store.box_width[con].width;
                    answer = con;
                }
            }
            // printf("%d\n", answer);

            Store.conveyor[col].boxes[row].SKU = Store.box_width[answer].SKU;
            Store.conveyor[col].boxes[row].width = Store.box_width[answer].width;
            Store.conveyor[col].boxes[row].empty = 0;
            change_tmp++;
            Store.conveyor[col].current_length++;
            Store.cnt_boxes_type[Store.box_width[answer].SKU]++;
            Store.cnt_boxes_type_const[Store.box_width[answer].SKU]++;
            insert_data(&(Store.db), Store.box_width[answer].SKU, row, col, Store.box_width[answer].width);
            fprintf(f_dep, "%*d   %*d   movebox%*d   channel%*d    channellen%*d    ", 4, id, 4, glb_time, 5, Store.box_width[answer].SKU, 6, col, 4, current_conv_len);
            
            Print_Channel(col, f_dep);
            tempor += 1;
        }
    }
    for (int i = 0; i < 10; ++i) {
        Store.used[i] = 0;
    }
    Store.boxes_to_deliver = 0;

    fprintf(f, "------------------------------------------\n");
    fprintf(f, "%*d startPalletize #1\n", 4, glb_time);
    for (int i = 0; i < 7; ++i) {
        Store.box_data[i][0] = -1;
        Store.box_data[i][1] = 0;
        Store.arr_time[i] = 0;
    }
}

void InitROSS() {
    ConveyorsInit();

}