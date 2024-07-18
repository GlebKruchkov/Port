#include "model.h"

void ConveyorsInit()
{
    fprintf(f, "startDepalletize\n"); 
    char *err_msg = 0;
    char *sql_del = "DROP TABLE IF EXISTS Warehouse";
    char *sql = "CREATE TABLE Warehouse(Type INTEGER, Row INTEGER, Column INTEGER, Width INTEGER, Channel_Width INTEGER)";
    sqlite3_exec(Store.db, sql_del, 0, 0, &err_msg);
    sqlite3_exec(Store.db, sql, 0, 0, &err_msg);

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

    // Initialization of robots to take in
    for (int i = 0; i < 3; ++i) {
        robot bot;
        bot.cur_cell = Store.cells[11 - i];
        Store.cells[11 - i].reserved = 1;
        bot.cur_task = -1;
        bot.cur_conv = -1;
        bot.col = -1;
        bot.row = -1;
        bot.has_box = -1;
        bot.reserved_channel = -1;

        Store.robots[i] = bot;
    }

    // Initialization of robots to take in
    for (int i = 3; i < 6; ++i) {
        robot bot;
        bot.cur_cell = Store.cells[6 - i];
        Store.cells[6 - i].reserved = 1;
        bot.cur_task = -1;
        bot.cur_conv = -1;
        bot.col = -1;
        bot.row = -1;
        bot.reserved_channel = -1;
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
            for (int con = 0; con < high_border - low_border + 1; ++con) {
                if (Store.box_width[con].width > cur_box_len && Store.box_width[con].width <= current_conv_len && Store.cnt_boxes_type[Store.box_width[con].SKU] < MAX_CONVEYORS * MAX_BOXES / (high_border - low_border) - 2) {
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
            if (tempor % 10 == 0) {
                glb_time += 8;
            }
            tempor += 1;
        }
    }
    for (int i = 0; i < 10; ++i) {
        Store.used[i] = 0;
    }
    Store.boxes_to_deliver = 0;

    fprintf(f, "finishDepalletize\n");
    fprintf(f, "------------------------------------------\n");
    fprintf(f, "startPalletize #1\n");
    for (int i = 0; i < 7; ++i) {
        Store.box_data[i][0] = -1;
        Store.box_data[i][1] = 0;
        Store.arr_time[i] = 0;
    }
}

void InitROSS() {
    ConveyorsInit();

}