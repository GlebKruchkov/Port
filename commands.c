#include "model.h"

//extern sqlite3 *db;

int callback(void *NotUsed, int argc, char **argv, char **azColName) {
    if (atoi(argv[1]) > best_box.row) {
        int flag = 1;
        for (int i = 0; i < MAX_ROBOTS; ++i) {
            if (Store.robots[i].reserved_channel == atoi(argv[2])) {
                flag = 0;
            }
        }
        if (flag == 1) {
            best_box.row = atoi(argv[1]);
            best_box.column = atoi(argv[2]);
        }
    }
    return 0;
}

int callback_by_width(void *NotUsed, int argc, char **argv, char **azColName) {
    if (atoi(argv[1]) > best_box.row) {
        int flag = 1;
        for (int i = 0; i < MAX_ROBOTS; ++i) {
            if (Store.robots[i].reserved_channel == atoi(argv[2])) {
                flag = 0;
            }
        }
        if (flag == 1) {
            best_box.row = atoi(argv[1]);
            best_box.column = atoi(argv[2]);
        }
    }
    return 0;
}

int compare(const void *a, const void *b) {
    const box_pair *boxA = (const box_pair *)a;
    const box_pair *boxB = (const box_pair *)b;
    return boxA->width - boxB->width;
}

int insert_data(sqlite3 **db1, int type, int row, int col, int width) {
    struct sqlite3 * db = (struct sqlite3 *) *db1;
    char *err_msg = 0;
    char sql[100];
    sprintf(sql, "INSERT INTO Warehouse(Type, Row, Column, Width, Channel_Width) VALUES (%d, %d, %d, %d, %d)", type, row, col, width, Store.conveyor_width[col]);
    sqlite3_exec(db, sql, 0, 0, &err_msg);
    return 0;
}

int find_data(sqlite3 **db1, int type) {
    struct sqlite3 * db = (struct sqlite3 *) *db1;
    best_box.row = -1;
    best_box.column = -1;
    char *err_msg = 0;
    char sql[100];
    sprintf(sql, "SELECT * FROM Warehouse WHERE Type = %d", type);
    sqlite3_exec(db, sql, callback, 0, &err_msg);
    return 0;
}

int find_data_by_width(sqlite3 **db1, int type) {
    struct sqlite3 * db = (struct sqlite3 *) *db1;
    best_box.row = -1;
    best_box.column = -1;
    char *err_msg = 0;
    char sql[100];
    // printf("%d\n", 1);
    // printf("%d %d\n", type, Store.b_w[type]);
    sprintf(sql, "SELECT * FROM Warehouse WHERE Channel_Width >= %d AND Type = %d", Store.b_w[type], -1);
    // exit(0);
    sqlite3_exec(db, sql, callback_by_width, 0, &err_msg);
    return 0;
}

int Add_Box(sqlite3 **db1, int type, int process) {
    struct sqlite3 * db = (struct sqlite3 *) *db1;
    // find_data_by_width(db, type);
    int col = Store.robots[process - 1].col;
    int r = Store.robots[process - 1].row;
    char *err_msg = 0;
    char sql[100];
    sprintf(sql, "DELETE FROM Warehouse WHERE Type = %d AND Row = %d AND Column = %d", -1, r, col);
    sqlite3_exec(db, sql, 0, 0, &err_msg);

    Store.conveyor[col].boxes[r].SKU = type;
    Store.conveyor[col].boxes[r].empty = 0;
    Store.conveyor[col].boxes[r].width = Store.b_w[type];
    Store.cnt_boxes_type[type]++;

    insert_data(db1, type, r, col, Store.b_w[type]);
}

void Swap_Boxes(sqlite3 **db1, int col, int row1, int row2) {
    struct sqlite3 * db = (struct sqlite3 *) *db1;
    bool empty_tmp = Store.conveyor[col].boxes[row1].empty;
    int SKU_tmp = Store.conveyor[col].boxes[row1].SKU;
    int width_tmp =  Store.conveyor[col].boxes[row1].width;
    char *err_msg = 0;
    char sql[100];
    
    sprintf(sql, "DELETE FROM Warehouse WHERE Type = %d AND Row = %d AND Column = %d", Store.conveyor[col].boxes[row1].SKU, row1, col);
    sqlite3_exec(db, sql, 0, 0, &err_msg);
    char sql2[100];
    sprintf(sql2, "DELETE FROM Warehouse WHERE Type = %d AND Row = %d AND Column = %d", Store.conveyor[col].boxes[row2].SKU, row2, col);
    sqlite3_exec(db, sql2, 0, 0, &err_msg);

    Store.conveyor[col].boxes[row1].empty = Store.conveyor[col].boxes[row2].empty;
    Store.conveyor[col].boxes[row1].SKU = Store.conveyor[col].boxes[row2].SKU;
    Store.conveyor[col].boxes[row1].width = Store.conveyor[col].boxes[row2].width;
    Store.conveyor[col].boxes[row2].empty = empty_tmp;
    Store.conveyor[col].boxes[row2].SKU = SKU_tmp;
    Store.conveyor[col].boxes[row2].width = width_tmp;

    insert_data(db1, Store.conveyor[col].boxes[row2].SKU, row2, col, Store.conveyor[col].boxes[row2].width);
    insert_data(db1, Store.conveyor[col].boxes[row1].SKU, row1, col, Store.conveyor[col].boxes[row1].width);
}

int Reverse(sqlite3 **db1, int col, int row, int *time, int *l_id, int process) {
    //fprintf(f, "%*d   %*d   getbox%*d   shiftbox%*d    channelwidth%*d    putbox%*d  channel%*d         ", 4, *l_id, 4, *time, 5, Store.conveyor[col].boxes[7].SKU, 6, Store.conveyor[col].boxes[7].SKU, 2, Store.conveyor_width[col], 2, Store.conveyor[col].boxes[7].SKU, 2, col);
    *l_id += 1;
    for (int i = MAX_BOXES - 1; i >= 1; --i) {
        if (Store.conveyor[col].boxes[i - 1].empty == 0) {
            Swap_Boxes(db1, col, i, i - 1);
        }
    }
    Store.robots[process - 1].row += 1;
    return 0;
}

int Remove_Boxes(sqlite3 **db1, int type, int *time, int *l_id, int process) {
    int col = Store.robots[process - 1].col;
    int row = Store.robots[process - 1].row;
    struct sqlite3 * db = (struct sqlite3 *) *db1;

    char *err_msg = 0;
    char sql[100];
    sprintf(sql, "DELETE FROM Warehouse WHERE Type = %d AND Row = %d AND Column = %d", type, 7, col);
    sqlite3_exec(db, sql, 0, 0, &err_msg);

    insert_data(db1, -1, MAX_BOXES - 1, col, -1);

    Store.conveyor[col].boxes[MAX_BOXES - 1].SKU = -1;
    Store.conveyor[col].boxes[MAX_BOXES - 1].empty = 1;
    Store.conveyor[col].boxes[MAX_BOXES - 1].width = -1;

    for (int i = MAX_BOXES - 1; i >= 1; --i) {
        Swap_Boxes(db1, col, i, i - 1);
    }
    Store.cnt_boxes_type[type]--;

    return col;
}

void Init_Commands(int *event_id, int *rec_id, int *glb_time, const char *filename) {
    FILE *file1 = fopen(filename, "r");
    int req_num = 0;
    Store.request.total = 0;
    Store.request.curr = 0;

    char line[1024];
    char *fields[10];

    fgets(line, sizeof(line), file1);

    char *temp_line = strtok(line, ",");

    strncpy(Store.cur_order, temp_line, sizeof(Store.cur_order) - 1);

    // Store.cur_order = strtok(line, ",");
    fprintf(paleta, "%*d %*d %*s %*s", 6, *rec_id, 6, *glb_time, 18, "startPalletize", 22, strtok(line, ","));
    fprintf(paleta, "\n");
    (*rec_id) += 1;

    fprintf(f, "%*d %*d       startPalletize %s", 6, *event_id, 6, *glb_time, strtok(line, ","));
    (*event_id) += 1;
    fgets(line, sizeof(line), file1);

    while (fgets(line, sizeof(line), file1)) {
        fields[0] = strtok(line, ",");
        for (int i = 1; i < 10; i++) {
          fields[i] = strtok(NULL, ",");
        }
        int SKU =  atoi(fields[0]) - 1000;
        int quantity = atoi(fields[1]);
        while (quantity != 0) {
          Store.request.requests[req_num][0] = SKU;
          Store.request.requests[req_num][1] = 1;
          quantity--;
          Store.request.total++;
          req_num++;
        }
    }
}

bool Check(int process) {
    if (Store.request.curr == Store.request.total) {
        return false;
    } else {
        int SKU = Store.request.requests[Store.request.curr][0];
        int quantity = Store.request.requests[Store.request.curr][1];
        Store.box_data[process][0] = SKU;
        Store.box_data[process][1] = quantity;
        Store.request.curr++;
        return true;
    }
}

void Send_Event(int process, message_type command, tw_lp *lp, tw_lpid *self) {
    tw_event *e = tw_event_new(process, 0.001, lp);
    message *msg = tw_event_data(e);
    msg->type = command;
    msg->contents = tw_rand_unif(lp->rng);
    msg->sender = self;
    tw_event_send(e);
}

void Print_Channel(int col, FILE *log_file) {
    for (int i = 0; i < MAX_BOXES; ++i) {
        if (Store.conveyor[col].boxes[i].empty) {
            fprintf(log_file, "|   -   ");
        } else {
            fprintf(log_file, "|%*d %*d", 3, Store.conveyor[col].boxes[i].SKU, 3, Store.conveyor[col].boxes[i].width);
        }
    }
    fprintf(log_file, "|\n");
}

void write_csv(const char *filename, sqlite3 *db) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT * FROM Warehouse"; // Замените your_table на имя вашей таблицы

    // Подготовка запроса
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Ошибка при подготовке SQL запроса: %s\n", sqlite3_errmsg(db));
        return;
    }

    if (!csv_file) {
        fprintf(stderr, "Не удалось открыть файл для записи: %s\n", filename);
        sqlite3_finalize(stmt);
        return;
    }

    // Запись заголовков
    int num_columns = sqlite3_column_count(stmt);
    for (int i = 0; i < num_columns; i++) {
        printf("%s\n", sqlite3_column_name(stmt, i));
        fprintf(csv_file, "%s", sqlite3_column_name(stmt, i));
        if (i < num_columns - 1) {
            fprintf(csv_file, ",");
        }
    }
    fprintf(csv_file, "\n");

    // Запись данных
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        for (int i = 0; i < num_columns; i++) {
            const char *value = (const char *)sqlite3_column_text(stmt, i);
            int to_print = 0;
            if (i == 2) {
                to_print = atoi(value);
                fprintf(csv_file, "%d", to_print % 10 + 1);
            } else {
                if (value) {
                    fprintf(csv_file, "%s", value);
                }
            }
            if (i < num_columns - 1) {
                fprintf(csv_file, ",");
            }
        }
        fprintf(csv_file, "\n");
    }

    fprintf(robots_positions, "BotID");
    fprintf(robots_positions, ", ");
    fprintf(robots_positions, "BotNode");
    fprintf(robots_positions, ", ");
    fprintf(robots_positions, "BoxTypeID");
    fprintf(robots_positions, "\n");
    for (int i = 0; i < MAX_ROBOTS; ++i) {
        fprintf(robots_positions, "%d", i);
        fprintf(robots_positions, ",     ");
        fprintf(robots_positions, "%s", Store.vertexes[Store.robots[i].cur_cell.id]);
        fprintf(robots_positions, ",      ");
        fprintf(robots_positions, "%d", Store.robots[i].cur_box);
        fprintf(robots_positions, "\n");    
    }

    fclose(csv_file);
    sqlite3_finalize(stmt);
}

int next_vertex(int cur_vertex, int cur_goal) {
    if (Store.vertexes[cur_goal][0] == Store.vertexes[cur_vertex][0]) {  // если нам нужно двигаться в текущей строке
        if (Store.vertexes[cur_vertex + 1][0] != Store.vertexes[cur_vertex][0]) { // если цель в обратном направлении 
            return Store.direction_graph[cur_vertex];
        } else {
            return cur_vertex + 1;
        }
    } else if (Store.vertexes[cur_goal][0] > Store.vertexes[cur_vertex][0]) { // если нам нужно двигаться вверх
        if (Store.direction_graph[cur_vertex] != -1 && Store.vertexes[Store.direction_graph[cur_vertex]][1] != '1') { // если мы на стыке и можно вверх
            return Store.direction_graph[cur_vertex];
        } else if (Store.direction_graph[cur_vertex] == -1) { // если мы не на стыке
            return cur_vertex + 1;
        } else { // иначе вниз
            return Store.direction_graph[cur_vertex];
        }
    } else {
        if (Store.direction_graph[cur_vertex] != -1 && Store.vertexes[Store.direction_graph[cur_vertex]][1] == '1') { // если мы на стыке и можно вниз
            return Store.direction_graph[cur_vertex];
        } else if (Store.direction_graph[cur_vertex] == -1) { // если мы не на стыке
            return cur_vertex + 1;
        } else { // иначе вверх
            return Store.direction_graph[cur_vertex];
        }
    }
}

void add_to_queue(int robot_id, int next_vert) {

    // if ((Store.robots[robot_id].goal_cell.id == Store.robots[robot_id].cur_cell.id) || (Store.cells[next_vert].queue[0] != -1)) {

        // fprintf(f, "BEFORE ADDING ID %d, CELL %s ", robot_id + 1, Store.vertexes[next_vert]);
        // for (int j = 0; j < MAX_ROBOTS; ++j) {
        //     fprintf(f, "%d ", Store.cells[next_vert].queue[j] + 1);
        // }
        // fprintf(f, "\n");

    for (int i = 0; i < MAX_ROBOTS; ++i) {
        if (Store.cells[next_vert].queue[i] == -1) {
            Store.cells[next_vert].queue[i] = robot_id;
            // fprintf(f, "AFTER ADDING ");
            // for (int j = 0; j < MAX_ROBOTS; ++j) {
            //     fprintf(f, "%d ", Store.cells[next_vert].queue[j] + 1);
            // }
            // fprintf(f, "\n");
            return;
        }
        }

    // }
}

void del_from_queue(int robot_id) {
    if (robot_id == Store.cells[Store.robots[robot_id].cur_cell.id].queue[0]) {

        // fprintf(f, "BEFORE DELETING ID %d, CELL %s ", robot_id + 1, Store.vertexes[Store.robots[robot_id].cur_cell.id]);
        // for (int j = 0; j < MAX_ROBOTS; ++j) {
        //     fprintf(f, "%d ", Store.cells[Store.robots[robot_id].cur_cell.id].queue[j] + 1);
        // }
        // fprintf(f, "\n");

        Store.cells[Store.robots[robot_id].cur_cell.id].queue[0] = -1;
        for (int i = 0; i < MAX_ROBOTS - 1; ++i) {
            int temp = Store.cells[Store.robots[robot_id].cur_cell.id].queue[i];
            Store.cells[Store.robots[robot_id].cur_cell.id].queue[i] =  Store.cells[Store.robots[robot_id].cur_cell.id].queue[i + 1];
            Store.cells[Store.robots[robot_id].cur_cell.id].queue[i + 1] = temp;
        }

        // fprintf(f, "AFTER DELETING ");
        // for (int j = 0; j < MAX_ROBOTS; ++j) {
        //     fprintf(f, "%d ", Store.cells[Store.robots[robot_id].cur_cell.id].queue[j] + 1);
        // }
        // fprintf(f, "\n");

    }
}

int GeTrIdFromBot(int curr_cell) {
  if (curr_cell < 3) {
    return curr_cell + 3;
  }
  if (curr_cell + 1 == MAX_VERTEXES - 2) {
    return 2;
  }
  if (curr_cell + 1 == MAX_VERTEXES - 1) {
    return 1;
  }
}

