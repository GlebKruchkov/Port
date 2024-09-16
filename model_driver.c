//The C driver file for a ROSS model
//This file includes:
// - an initialization function for each LP type
// - a forward event function for each LP type
// - a reverse event function for each LP type
// - a finalization function for each LP type

//Includes

#include "model.h"
#include <time.h>
#include "sqlite3.h"

//Helper Functions
void SWAP (double *a, double *b) {
  double tmp = *a;
  *a = *b;
  *b = tmp;
}

void model_init (state *s, tw_lp *lp) {
  int self = lp->gid;
  tw_event *e = tw_event_new(0, 1, lp);
  message *msg = tw_event_data(e);
  msg->type = TAKE_OUT;
  msg->contents = tw_rand_unif(lp->rng);
  msg->sender = self;
  tw_event_send(e);
  if (self == 0) {
    printf("%s\n", "COMMAND_CENTER is initialized");
  } else {
    printf("%s ", "CONVEYOR");
    printf("%d ", self);
    printf("%s\n", " is initialized");
  }
}

void model_event (state *s, tw_bf *bf, message *in_msg, tw_lp *lp) {
  int self = lp->gid;
  *(int *) bf = (int) 0;
  SWAP(&(s->value), &(in_msg->contents));

  Store.kill_prog = 0;
  int ok_take_in = 1;
  for (int i = 0; i < MAX_ROBOTS; ++i) {
    if (Store.robots[i].kill == 1) {
      Store.kill_prog = 1;
      ok_take_in = 0;
    }
  }

  if (self == 0 && Store.kill_prog == 0) {
    int not_do = 0;
    for (int i = 1; i < MAX_ROBOTS + 1; ++i) {
      if (Store.used[i] == 1) {
        not_do = 1;
      }
    }
    if (not_do == 0) {
      glb_time += 1;
      if (Store.boxes_to_deliver <= 0) {
        for (int i = 1; i < high_border - low_border + 1; ++i) {
          if (ok_take_in && Store.cnt_boxes_type[i] < 10) {
            Store.boxes_to_deliver = threshold - Store.cnt_boxes_type[i];
            Store.type_to_add = i;
            //printf("%d %d\n", Store.type_to_add, Store.boxes_to_deliver);
            
  
            for (int cur_robot = 0; cur_robot < MAX_ROBOTS; ++cur_robot) {
              if (Store.robots[i - 1].cur_task != 3 && (Store.robots[cur_robot].cur_task == 1 || Store.boxes_to_deliver >= cur_robot + 1) && Store.robots[cur_robot].cur_task != 2) {
                //printf("1");
                Store.nt_used[cur_robot + 1] = 1;
                Store.used[cur_robot + 1] = 1;
                Store.robots[cur_robot].cur_task = 1;
                Store.messages[cur_robot].type = TAKE_IN;
                if (Store.robots[cur_robot].has_box == -1) {
                  srand(time(NULL));
                  Store.robots[cur_robot].goal_cell.id = 46 + (int)(rand() % 2);
                }
              }
            }
            //printf("\n");
  

            // printf("%d %d\n", Store.type_to_add, Store.boxes_to_deliver);

            
            fprintf(paleta, "%*d %*d %*s %*d %*d", 6, rec_id, 6, glb_time, 18, "StartDepalletize", 21, palet_type, 12, Store.type_to_add);
            fprintf(paleta, "\n");
            // palet_type += 1;
            rec_id++;
            fprintf(f, "%*d %*d     startDepalletize %d\n", 6, event_id, 6, glb_time, Store.type_to_add);
            fprintf(control_system_log, "%*d %*d     startDepalletize %d\n", 6, control_id, 6, glb_time, Store.type_to_add);
            control_id += 1;
            event_id += 1;
            break;
          }
        }
      } else {

        //printf("%d\n", Store.boxes_to_deliver);

        for (int cur_robot = 0; cur_robot < MAX_ROBOTS; ++cur_robot) {
          if (Store.robots[cur_robot].cur_task != 3 && ok_take_in && (Store.robots[cur_robot].cur_task == 1 || Store.boxes_to_deliver >= cur_robot + 1) && Store.robots[cur_robot].cur_task != 2) {
            Store.nt_used[cur_robot + 1] = 1;
            Store.used[cur_robot + 1] = 1;
            Store.robots[cur_robot].cur_task = 1;
            Store.messages[cur_robot].type = TAKE_IN;
            if (Store.robots[cur_robot].has_box == -1) {
              srand(time(NULL));
              Store.robots[cur_robot].goal_cell.id = 46 + (int)(rand() % 2);
            }
          }
        }
      }
      // printf("%d\n", cur_boxes);

      for (int process = 1; process < MAX_ROBOTS + 1; ++process) {
        if (Store.robots[process - 1].cur_task != 3 && cur_boxes >= 1190 && Store.robots[process - 1].cur_task != 1) {
          Store.nt_used[process] = 1;
          if (Store.robots[process - 1].col != -1 && Store.robots[process - 1].row != -1) {
            Store.robots[process - 1].cur_task = 2;
            if (Store.robots[process - 1].row != 7) {
              Store.used[process] = 1;
              Store.messages[process - 1].type = REVERSE;
            } else {
              Store.used[process] = 1;
              Store.messages[process - 1].type = TAKE_OUT;
            }
          } else {
            if (Check(process)) {
              Store.robots[process - 1].cur_task = 2;
              find_data(&(Store.db), Store.box_data[process][0]);
              Store.robots[process - 1].goal_cell.id = (int)(best_box.column / 10) + 7 + (int)(best_box.column / 50) * 7;
              Store.robots[process - 1].reserved_channel = best_box.column;

              Store.robots[process - 1].col = best_box.column;
              Store.robots[process - 1].row = best_box.row;

              if (Store.robots[process - 1].row != 7) {
                Store.used[process] = 1;
                Store.messages[process - 1].type = REVERSE;
              } else {
                Store.used[process] = 1;
                Store.messages[process - 1].type = TAKE_OUT;
              }
            } else {
              if (Store.cur_file > MAX_FILES) {
                Store.robots[process - 1].kill = 1;
              } else {
                if (Store.cur_file != 0) {
                  fprintf(paleta, "%*d %*d %*s %*s", 6, rec_id, 6, glb_time, 18, "finishPalletize", 21, Store.cur_order);
                  fprintf(paleta, "\n");
                  rec_id++;
                  fprintf(f, "%*d %*d      finishPalletize %s", 6, event_id, 6, glb_time, Store.cur_order);
                  event_id += 1;
                }
                Init_Commands(&(event_id), &(rec_id), &(glb_time), Store.files[Store.cur_file]);
                Store.cur_file += 1;
              }
            }
          }

        }
      }
      for (int i = 1; i < MAX_ROBOTS + 1; ++i) {
        if (Store.robots[i - 1].cur_task == -1 && cur_boxes < 1190 && ok_take_in && Store.nt_used[i] == 0) {
          if (Store.robots[i - 1].cur_task != 3) {
            if (Store.robots[i - 1].cur_cell.id == 44) {
              Store.robots[i - 1].goal_cell.id = 4;
            } else {  
              Store.robots[i - 1].goal_cell.id = 44;
            }
          }
          Store.robots[i - 1].cur_task = 3;
          Store.used[i] = 1;
          Store.messages[i - 1].type = GO;
        }
      }

      Send_Event(1, Store.messages[0].type, lp, &(lp->gid));

    }
  } else if (Store.kill_prog == 0) {
    Store.used[self] = 0;
    Store.nt_used[self] = 0;

    Store.robots[self - 1].cur_time += 1;

    switch (in_msg->type)
    {
      case TAKE_IN:
        if (Store.robots[self - 1].has_box == -1 && Store.robots[self - 1].cur_cell.id == Store.robots[self - 1].goal_cell.id) {
          if (Store.robots[self - 1].cur_time == 1) {
            add_to_queue(self - 1, Store.robots[self - 1].cur_cell.id + 1);
            del_from_queue(self - 1);
            Store.robots[self - 1].goal_time = 8;
            if (glb_time > 1) {
              fprintf(f, "%*d %*d %*d    finishMotion       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].prev_vertex], 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.robots[self - 1].prev_box_type, 4, Store.robots[self - 1].prev_channel, 2, Store.robots[self - 1].prev_tr_id);
              event_id += 1;
            }
            fprintf(f, "%*d %*d %*d     startMotion       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[Store.robots[self - 1].cur_cell.id + 1], 4, 0, 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
            fprintf(control_system_log, "%*d %*d %*d     startMotion       %*s     %*s     %*d    %*d   %*d\n", 6, control_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[Store.robots[self - 1].cur_cell.id + 1], 4, 0, 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
            event_id += 1;
            control_id += 1;
          }
          if (Store.robots[self - 1].cur_time >= Store.robots[self - 1].goal_time) {
            find_data_by_width(&(Store.db), Store.type_to_add);

            if (best_box.row == -1 || best_box.column == -1) {

              cur_boxes += 1;
              if (self - 1 == Store.cells[Store.robots[self - 1].cur_cell.id].queue[0]) {
                  Store.cells[Store.robots[self - 1].cur_cell.id].queue[0] = -1;
                  for (int i = 0; i < MAX_ROBOTS - 1; ++i) {
                      int temp = Store.cells[Store.robots[self - 1].cur_cell.id].queue[i];
                      Store.cells[Store.robots[self - 1].cur_cell.id].queue[i] =  Store.cells[Store.robots[self - 1].cur_cell.id].queue[i + 1];
                      Store.cells[Store.robots[self - 1].cur_cell.id].queue[i + 1] = temp;
                  }
              }
              
              if (self - 1 == Store.cells[Store.robots[self - 1].cur_cell.id + 1].queue[0]) {
                  Store.cells[Store.robots[self - 1].cur_cell.id + 1].queue[0] = -1;
                  for (int i = 0; i < MAX_ROBOTS - 1; ++i) {
                      int temp = Store.cells[Store.robots[self - 1].cur_cell.id + 1].queue[i];
                      Store.cells[Store.robots[self - 1].cur_cell.id + 1].queue[i] =  Store.cells[Store.robots[self - 1].cur_cell.id + 1].queue[i + 1];
                      Store.cells[Store.robots[self - 1].cur_cell.id + 1].queue[i + 1] = temp;
                  }
              }


              Store.robots[self - 1].cur_time = 0;
              Store.robots[self - 1].cur_task = -1;
              Store.robots[self - 1].col = -1;
              Store.robots[self - 1].row = -1;
              Store.robots[self - 1].reserved_channel = -1;
              Store.boxes_to_deliver--;
              Store.robots[self - 1].tmp_fl = 1;
              if (Store.boxes_to_deliver == 0) {
                fprintf(paleta, "%*d %*d %*s %*d %*d", 6, rec_id, 6, glb_time, 18, "finishDepalletize", 21, palet_type, 12, Store.type_to_add);
                fprintf(paleta, "\n");
                palet_type++;
                rec_id++;
                fprintf(f, "%*d %*d    finishDepalletize\n", 6, event_id, 6, glb_time);
                // Store.type_to_add = -1;
                event_id += 1;
              }

              if (self == MAX_ROBOTS) {
                Send_Event(0, TAKE_IN, lp, &(lp->gid));
              } else {
                Send_Event(self + 1, Store.messages[self].type, lp, &(lp->gid));
              }
              break;
            }

            Store.robots[self - 1].col = best_box.column;
            Store.robots[self - 1].row = best_box.row;

            Store.robots[self - 1].reserved_channel = best_box.column;

            if (Store.cells[Store.robots[self - 1].cur_cell.id + 1].queue[0] != -1 && Store.cells[Store.robots[self - 1].cur_cell.id + 1].queue[0] != self - 1) {
              if (self == MAX_ROBOTS) {
                Send_Event(0, TAKE_IN, lp, &(lp->gid));
              } else {
                Send_Event(self + 1, Store.messages[self].type, lp, &(lp->gid));
              }
              break;
            }

            Store.robots[self - 1].tmp_fl = 1;

            Store.robots[self - 1].cur_time = 0;

            Store.robots[self - 1].has_box = 1;
            Store.robots[self - 1].goal_cell.id = (((Store.robots[self - 1].col / 50) + 1) * 12) + (4 - ((Store.robots[self - 1].col - 50 * (Store.robots[self - 1].col / 50)) / 10)) + 1;
            fprintf(f, "%*d %*d %*d     movebox2bot       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[Store.robots[self - 1].cur_cell.id + 1], 4, Store.type_to_add, 4, 0, 2, GeTrIdFromBot(Store.robots[self - 1].cur_cell.id));
            fprintf(control_system_log, "%*d %*d %*d     movebox2bot       %*s     %*s     %*d    %*d   %*d\n", 6, control_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[Store.robots[self - 1].cur_cell.id + 1], 4, Store.type_to_add, 4, 0, 2, GeTrIdFromBot(Store.robots[self - 1].cur_cell.id));
            event_id += 1;
            control_id += 1;

            //fprintf(f, "%*d %*d %*d    finishMotion       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[Store.robots[self - 1].cur_cell.id + 1], 4, Store.type_to_add, 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
            Store.robots[self - 1].prev_box_type = Store.type_to_add;
            Store.robots[self - 1].prev_channel = Store.robots[self - 1].col % 10 + 1;
            Store.robots[self - 1].prev_tr_id = 0;
            Store.robots[self - 1].prev_vertex = Store.robots[self - 1].cur_cell.id;
            del_from_queue(self - 1);
            Store.robots[self - 1].cur_cell.id += 1;
            //event_id += 1;

          }


        } else if (Store.robots[self - 1].has_box == 1 && Store.robots[self - 1].cur_cell.id == Store.robots[self - 1].goal_cell.id) {
          Store.robots[self - 1].goal_time = 8;
          if (Store.robots[self - 1].cur_time == 1) {
            add_to_queue(self - 1, Store.robots[self - 1].cur_cell.id + 1);
            del_from_queue(self - 1);
            if (glb_time > 1) {
              fprintf(f, "%*d %*d %*d    finishMotion       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].prev_vertex], 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.robots[self - 1].prev_box_type, 4, Store.robots[self - 1].prev_channel, 2, Store.robots[self - 1].prev_tr_id);
              event_id += 1;
            }
            fprintf(f,"%*d %*d %*d     startMotion       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[Store.robots[self - 1].cur_cell.id + 1], 4, Store.type_to_add, 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
            fprintf(control_system_log,"%*d %*d %*d     startMotion       %*s     %*s     %*d    %*d   %*d\n", 6, control_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[Store.robots[self - 1].cur_cell.id + 1], 4, Store.type_to_add, 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
            event_id += 1;
            control_id += 1;
          }
          if (Store.robots[self - 1].cur_time >= Store.robots[self - 1].goal_time) {

            if (Store.cells[Store.robots[self - 1].cur_cell.id + 1].queue[0] != -1 && Store.cells[Store.robots[self - 1].cur_cell.id + 1].queue[0] != self - 1) {
              if (self == MAX_ROBOTS) {
                Send_Event(0, TAKE_IN, lp, &(lp->gid));
              } else {
                Send_Event(self + 1, Store.messages[self].type, lp, &(lp->gid));
              }
              break;
            }

            Store.robots[self - 1].tmp_fl = 1;

            Store.robots[self - 1].cur_time = 0;
            Add_Box(&(Store.db), Store.type_to_add, self);
            Store.boxes_to_deliver--;
            cur_boxes += 1;
            
            fprintf(f, "%*d %*d %*d movebox2channel       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[Store.robots[self - 1].cur_cell.id + 1], 4, Store.type_to_add, 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
            fprintf(control_system_log, "%*d %*d %*d movebox2channel       %*s     %*s     %*d    %*d   %*d\n", 6, control_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[Store.robots[self - 1].cur_cell.id + 1], 4, Store.type_to_add, 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
            event_id += 1;
            control_id += 1;
            
            //fprintf(f, "%*d %*d %*d    finishMotion       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[Store.robots[self - 1].cur_cell.id + 1], 4, 0, 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
            Store.robots[self - 1].prev_box_type = 0;
            Store.robots[self - 1].prev_channel = Store.robots[self - 1].col % 10 + 1;
            Store.robots[self - 1].prev_tr_id = 0;
            Store.robots[self - 1].prev_vertex = Store.robots[self - 1].cur_cell.id;
            
            //event_id += 1;
            Store.robots[self - 1].reserved_channel = -1;
            Store.robots[self - 1].has_box = -1;
            Store.robots[self - 1].col = -1;
            Store.robots[self - 1].row = -1;
            Store.robots[self - 1].cur_task = -1;
            if (Store.boxes_to_deliver == 0) {
              fprintf(paleta, "%*d %*d %*s %*d %*d", 6, rec_id, 6, glb_time, 18, "finishDepalletize", 21, palet_type, 12, Store.type_to_add);
              fprintf(paleta, "\n");
              rec_id++;
              palet_type++;
              fprintf(f, "%*d %*d    finishDepalletize\n", 6, event_id, 6, glb_time);
              // Store.type_to_add = -1;
              event_id += 1;
            }

            del_from_queue(self - 1);
            Store.robots[self - 1].cur_cell.id += 1;
          }

        } else {
          int next_vert = next_vertex(Store.robots[self - 1].cur_cell.id, Store.robots[self - 1].goal_cell.id);

          if (Store.direction_graph[Store.robots[self - 1].cur_cell.id] != -1 && Store.direction_graph[next_vert] != -1) { // если оба стыка
            Store.robots[self - 1].goal_time = 3;
          } else if (Store.direction_graph[Store.robots[self - 1].cur_cell.id] != -1 && (next_vert == 0 || next_vert == 43)) { // если стык и конечная
            Store.robots[self - 1].goal_time = 3;
          } else if (Store.robots[self - 1].cur_cell.id == 0) { // если в начале (паллета на загрузку)
            Store.robots[self - 1].goal_time = 3;
          } else if (Store.robots[self - 1].cur_cell.id == 1) { // если в начале (паллета на загрузку)
            Store.robots[self - 1].goal_time = 4;
          } else {
            Store.robots[self - 1].goal_time = 6;
          }

          if (Store.robots[self - 1].cur_time == 1) {
            add_to_queue(self - 1, next_vert);
            del_from_queue(self - 1);
            if (glb_time > 1) {
              fprintf(f, "%*d %*d %*d    finishMotion       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].prev_vertex], 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.robots[self - 1].prev_box_type, 4, Store.robots[self - 1].prev_channel, 2, Store.robots[self - 1].prev_tr_id);
              event_id += 1;
            }
            if (Store.robots[self - 1].has_box == 1) {
              fprintf(f, "%*d %*d %*d     startMotion       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[next_vert], 4, Store.type_to_add, 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
              fprintf(control_system_log, "%*d %*d %*d     startMotion       %*s     %*s     %*d    %*d   %*d\n", 6, control_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[next_vert], 4, Store.type_to_add, 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
            } else {
              fprintf(f, "%*d %*d %*d     startMotion       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[next_vert], 4, 0, 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
              fprintf(control_system_log, "%*d %*d %*d     startMotion       %*s     %*s     %*d    %*d   %*d\n", 6, control_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[next_vert], 4, 0, 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
            }
            event_id += 1;
            control_id += 1;
          }

          if (Store.robots[self - 1].cur_time >= Store.robots[self - 1].goal_time) {
            if (Store.cells[next_vert].queue[0] != -1 && Store.cells[next_vert].queue[0] != self - 1) {
              if (self == MAX_ROBOTS) {
                Send_Event(0, TAKE_IN, lp, &(lp->gid));
              } else {
                Send_Event(self + 1, Store.messages[self].type, lp, &(lp->gid));
              }
              break;
            }
            Store.robots[self - 1].tmp_fl = 1;

            Store.robots[self - 1].cur_time = 0;
            if (Store.robots[self - 1].has_box == 1) {
              //fprintf(f, "%*d %*d %*d    finishMotion       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[next_vert], 4, Store.type_to_add, 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
              Store.robots[self - 1].prev_box_type = Store.type_to_add;
              Store.robots[self - 1].prev_channel = Store.robots[self - 1].col % 10 + 1;
              Store.robots[self - 1].prev_tr_id = 0;
            } else {
              //fprintf(f, "%*d %*d %*d    finishMotion       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[next_vert],  4, 0, 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
              Store.robots[self - 1].prev_box_type = 0;
              Store.robots[self - 1].prev_channel = Store.robots[self - 1].col % 10 + 1;
              Store.robots[self - 1].prev_tr_id = 0;
            }
            Store.robots[self - 1].prev_vertex = Store.robots[self - 1].cur_cell.id;
            //event_id += 1;

            del_from_queue(self - 1);

            Store.robots[self - 1].cur_cell.id = next_vert;

          }

        }
        
        
        if (self == MAX_ROBOTS) {
          Send_Event(0, TAKE_IN, lp, &(lp->gid));
        } else {
          Send_Event(self + 1, Store.messages[self].type, lp, &(lp->gid));
        }
        break;

      case TAKE_OUT:

        if (Store.robots[self - 1].has_box == -1 && Store.robots[self - 1].cur_cell.id == Store.robots[self - 1].goal_cell.id) {
          
          Store.robots[self - 1].goal_time = 8;
          if (Store.robots[self - 1].cur_time == 1) {
            add_to_queue(self - 1, Store.robots[self - 1].cur_cell.id + 1);
            del_from_queue(self - 1);
            if (glb_time > 1) {
              fprintf(f, "%*d %*d %*d    finishMotion       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].prev_vertex], 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.robots[self - 1].prev_box_type, 4, Store.robots[self - 1].prev_channel, 2, Store.robots[self - 1].prev_tr_id);
              event_id += 1;
            }
            fprintf(f, "%*d %*d %*d     startMotion       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[Store.robots[self - 1].cur_cell.id + 1], 4, 0, 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
            fprintf(control_system_log, "%*d %*d %*d     startMotion       %*s     %*s     %*d    %*d   %*d\n", 6, control_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[Store.robots[self - 1].cur_cell.id + 1], 4, 0, 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
            event_id += 1;
            control_id += 1;
          }
          if (Store.robots[self - 1].cur_time >= Store.robots[self - 1].goal_time) {
            if (Store.cells[Store.robots[self - 1].cur_cell.id + 1].queue[0] != -1 && Store.cells[Store.robots[self - 1].cur_cell.id + 1].queue[0] != self - 1) {
              if (self == MAX_ROBOTS) {
                Send_Event(0, TAKE_OUT, lp, &(lp->gid));
              } else {
                Send_Event(self + 1, Store.messages[self].type, lp, &(lp->gid));
              }
              break;
            }

            Store.robots[self - 1].tmp_fl = 1;

            Store.robots[self - 1].cur_time = 0;
            Remove_Boxes(&(Store.db), Store.box_data[self][0], &(glb_time), &(event_id), self);
            Store.robots[self - 1].cur_box = Store.box_data[self][0];
            fprintf(f, "%*d %*d %*d     movebox2bot       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[Store.robots[self - 1].cur_cell.id + 1], 4, Store.box_data[self][0], 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
            fprintf(control_system_log, "%*d %*d %*d     movebox2bot       %*s     %*s     %*d    %*d   %*d\n", 6, control_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[Store.robots[self - 1].cur_cell.id + 1], 4, Store.box_data[self][0], 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
            event_id += 1;
            control_id += 1;

            Store.box_data[self][1] = 0;
            Store.robots[self - 1].has_box = 1;
            Store.robots[self - 1].reserved_channel = -1;
            srand(time(NULL));
            Store.robots[self - 1].goal_cell.id = (int)(rand() % 3);

            //fprintf(f, "%*d %*d %*d    finishMotion       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[Store.robots[self - 1].cur_cell.id + 1], 4, Store.box_data[self][0], 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
            Store.robots[self - 1].prev_box_type = Store.box_data[self][0];
            Store.robots[self - 1].prev_channel = Store.robots[self - 1].col % 10 + 1;
            Store.robots[self - 1].prev_tr_id = 0;
            Store.robots[self - 1].prev_vertex = Store.robots[self - 1].cur_cell.id;
            //event_id += 1;

            del_from_queue(self - 1);
            Store.robots[self - 1].cur_cell.id += 1;
          }

        } else if (Store.robots[self - 1].has_box == 1 && Store.robots[self - 1].cur_cell.id == Store.robots[self - 1].goal_cell.id) {
          
          Store.robots[self - 1].goal_time = 8;
          if (Store.robots[self - 1].cur_time == 1) {
            add_to_queue(self - 1, Store.robots[self - 1].cur_cell.id + 1);
            del_from_queue(self - 1);
            if (glb_time > 1) {
              fprintf(f, "%*d %*d %*d    finishMotion       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].prev_vertex], 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.robots[self - 1].prev_box_type, 4, Store.robots[self - 1].prev_channel, 2, Store.robots[self - 1].prev_tr_id);
              event_id += 1;
            }
            fprintf(f, "%*d %*d %*d     startMotion       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[Store.robots[self - 1].cur_cell.id + 1], 4, Store.box_data[self][0], 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
            fprintf(control_system_log, "%*d %*d %*d     startMotion       %*s     %*s     %*d    %*d   %*d\n", 6, control_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[Store.robots[self - 1].cur_cell.id + 1], 4, Store.box_data[self][0], 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
            event_id += 1;
            control_id += 1;
          }
          if (Store.robots[self - 1].cur_time >= Store.robots[self - 1].goal_time) {
            if (Store.cells[Store.robots[self - 1].cur_cell.id + 1].queue[0] != -1 && Store.cells[Store.robots[self - 1].cur_cell.id + 1].queue[0] != self - 1) {
              if (self == MAX_ROBOTS) {
                Send_Event(0, TAKE_OUT, lp, &(lp->gid));
              } else {
                Send_Event(self + 1, Store.messages[self].type, lp, &(lp->gid));
              }
              break;
            }
            Store.robots[self - 1].tmp_fl = 1;

            Store.robots[self - 1].cur_time = 0;
            Store.robots[self - 1].has_box = -1;
            Store.robots[self - 1].row = -1;
            Store.robots[self - 1].col = -1;
            Store.robots[self - 1].cur_task = -1;
            fprintf(f, "%*d %*d %*d      movebox2tr       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[Store.robots[self - 1].cur_cell.id + 1], 4, Store.box_data[self][0], 4, 0, 2, GeTrIdFromBot(Store.robots[self - 1].cur_cell.id));
            fprintf(control_system_log, "%*d %*d %*d      movebox2tr       %*s     %*s     %*d    %*d   %*d\n", 6, control_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[Store.robots[self - 1].cur_cell.id + 1], 4, Store.box_data[self][0], 4, 0, 2, GeTrIdFromBot(Store.robots[self - 1].cur_cell.id));
            event_id += 1;
            control_id += 1;
            //fprintf(f, "%*d %*d %*d    finishMotion       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[Store.robots[self - 1].cur_cell.id + 1], 4, 0, 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
            Store.robots[self - 1].prev_box_type = 0;
            Store.robots[self - 1].prev_channel = Store.robots[self - 1].col % 10 + 1;
            Store.robots[self - 1].prev_tr_id = 0;
            Store.robots[self - 1].prev_vertex = Store.robots[self - 1].cur_cell.id;
            //event_id += 1;

            del_from_queue(self - 1);
            Store.robots[self - 1].cur_cell.id += 1;
          }
        } else {
          
          int next_vert = next_vertex(Store.robots[self - 1].cur_cell.id, Store.robots[self - 1].goal_cell.id);

          if (Store.direction_graph[Store.robots[self - 1].cur_cell.id] != -1 && Store.direction_graph[next_vert] != -1) { // если оба стыка
            Store.robots[self - 1].goal_time = 3;
          } else if (Store.direction_graph[Store.robots[self - 1].cur_cell.id] != -1 && (next_vert == 0 || next_vert == 43)) { // если стык и конечная
            Store.robots[self - 1].goal_time = 3;
          } else if (Store.robots[self - 1].cur_cell.id == 0) { // если в начале (паллета на загрузку)
            Store.robots[self - 1].goal_time = 3;
          } else if (Store.robots[self - 1].cur_cell.id == 1) { // если в начале (паллета на загрузку)
            Store.robots[self - 1].goal_time = 4;
          } else {
            Store.robots[self - 1].goal_time = 6;
          }


          if (Store.robots[self - 1].cur_time == 1) {
            add_to_queue(self - 1, next_vert);
            del_from_queue(self - 1);
            if (glb_time > 1) {
              fprintf(f, "%*d %*d %*d    finishMotion       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].prev_vertex], 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.robots[self - 1].prev_box_type, 4, Store.robots[self - 1].prev_channel, 2, Store.robots[self - 1].prev_tr_id);
              event_id += 1;
            }
            if (Store.robots[self - 1].has_box == 1) {
              fprintf(f, "%*d %*d %*d     startMotion       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[next_vert], 4, Store.box_data[self][0], 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
              fprintf(control_system_log, "%*d %*d %*d     startMotion       %*s     %*s     %*d    %*d   %*d\n", 6, control_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[next_vert], 4, Store.box_data[self][0], 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
            } else {
              fprintf(f, "%*d %*d %*d     startMotion       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[next_vert], 4, 0, 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
              fprintf(control_system_log, "%*d %*d %*d     startMotion       %*s     %*s     %*d    %*d   %*d\n", 6, control_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[next_vert], 4, 0, 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
            }
            event_id += 1;
            control_id += 1;
          }

          if (Store.robots[self - 1].cur_time >= Store.robots[self - 1].goal_time) {
            if (Store.cells[next_vert].queue[0] != -1 && Store.cells[next_vert].queue[0] != self - 1) {
              if (self == MAX_ROBOTS) {
                Send_Event(0, TAKE_IN, lp, &(lp->gid));
              } else {
                Send_Event(self + 1, Store.messages[self].type, lp, &(lp->gid));
              }
              break;
            }
            
            Store.robots[self - 1].tmp_fl = 1;

            Store.robots[self - 1].cur_time = 0;
            if (Store.robots[self - 1].has_box == 1) {
              Store.robots[self - 1].prev_box_type = Store.box_data[self][0];
              Store.robots[self - 1].prev_channel = Store.robots[self - 1].col % 10 + 1;
              Store.robots[self - 1].prev_tr_id = 0;
              //fprintf(f, "%*d %*d %*d    finishMotion       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[next_vert],  4, Store.box_data[self][0], 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
            } else {
              Store.robots[self - 1].prev_box_type = 0;
              Store.robots[self - 1].prev_channel = Store.robots[self - 1].col % 10 + 1;
              Store.robots[self - 1].prev_tr_id = 0;
              //fprintf(f, "%*d %*d %*d    finishMotion       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[next_vert],  4, 0, 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
            }
            Store.robots[self - 1].prev_vertex = Store.robots[self - 1].cur_cell.id;
            //event_id += 1;
            
            del_from_queue(self - 1);
            Store.robots[self - 1].cur_cell.id = next_vert;
            
          }
        }
        
        if (self == MAX_ROBOTS) {
          Send_Event(0, TAKE_OUT, lp, &(lp->gid));
        } else {
          Send_Event(self + 1, Store.messages[self].type, lp, &(lp->gid));
        }
        break;
        
      case REVERSE:

        if (Store.robots[self - 1].has_box == -1 && Store.robots[self - 1].cur_cell.id == Store.robots[self - 1].goal_cell.id) {
          
          Store.robots[self - 1].goal_time = 8;
          if (Store.robots[self - 1].cur_time == 1) {
            add_to_queue(self - 1, Store.robots[self - 1].cur_cell.id + 1);
            del_from_queue(self - 1);
            if (glb_time > 1) {
              fprintf(f, "%*d %*d %*d    finishMotion       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].prev_vertex], 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.robots[self - 1].prev_box_type, 4, Store.robots[self - 1].prev_channel, 2, Store.robots[self - 1].prev_tr_id);
              event_id += 1;
            }

            fprintf(f, "%*d %*d %*d     startMotion       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[Store.robots[self - 1].cur_cell.id + 1], 4, 0, 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
            fprintf(control_system_log, "%*d %*d %*d     startMotion       %*s     %*s     %*d    %*d   %*d\n", 6, control_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[Store.robots[self - 1].cur_cell.id + 1], 4, 0, 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
            event_id += 1;
            control_id += 1;
          }
          if (Store.robots[self - 1].cur_time >= Store.robots[self - 1].goal_time) {
            if (Store.cells[Store.robots[self - 1].cur_cell.id + 1].queue[0] != -1 && Store.cells[Store.robots[self - 1].cur_cell.id + 1].queue[0] != self - 1) {
              if (self == MAX_ROBOTS) {
                Send_Event(0, REVERSE, lp, &(lp->gid));
              } else {
                Send_Event(self + 1, Store.messages[self].type, lp, &(lp->gid));
              }
              break;
            }

            Store.robots[self - 1].tmp_fl = 1;

            Store.robots[self - 1].cur_time = 0;
            Store.robots[self - 1].low_SKU = Store.conveyor[Store.robots[self - 1].col].boxes[7].SKU;

            Remove_Boxes(&(Store.db), Store.robots[self - 1].low_SKU, &(glb_time), &(event_id), self);
            Store.robots[self - 1].cur_box = Store.robots[self - 1].low_SKU;
            fprintf(f, "%*d %*d %*d     movebox2bot       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[Store.robots[self - 1].cur_cell.id + 1], 4, Store.robots[self - 1].low_SKU, 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
            fprintf(control_system_log, "%*d %*d %*d     movebox2bot       %*s     %*s     %*d    %*d   %*d\n", 6, control_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[Store.robots[self - 1].cur_cell.id + 1], 4, Store.robots[self - 1].low_SKU, 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
            event_id += 1;
            control_id += 1;

            Store.robots[self - 1].has_box = 1;
            Store.robots[self - 1].goal_cell.id = (((Store.robots[self - 1].col / 50) + 1) * 12) + (4 - ((Store.robots[self - 1].col - 50 * (Store.robots[self - 1].col / 50)) / 10)) + 1;

            //fprintf(f, "%*d %*d %*d    finishMotion       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[Store.robots[self - 1].cur_cell.id + 1], 4, Store.robots[self - 1].low_SKU, 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
            Store.robots[self - 1].prev_box_type = Store.robots[self - 1].low_SKU;
            Store.robots[self - 1].prev_channel = Store.robots[self - 1].col % 10 + 1;
            Store.robots[self - 1].prev_tr_id = 0;
            Store.robots[self - 1].prev_vertex = Store.robots[self - 1].cur_cell.id;
            del_from_queue(self - 1);
            Store.robots[self - 1].cur_cell.id += 1;
            //event_id += 1;
          }

        } else if (Store.robots[self - 1].has_box == 1 && Store.robots[self - 1].cur_cell.id == Store.robots[self - 1].goal_cell.id) {
          
          Store.robots[self - 1].goal_time = 8;
          if (Store.robots[self - 1].cur_time == 1) {
            add_to_queue(self - 1, Store.robots[self - 1].cur_cell.id + 1);
            del_from_queue(self - 1);
            if (glb_time > 1) {
              fprintf(f, "%*d %*d %*d    finishMotion       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].prev_vertex], 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.robots[self - 1].prev_box_type, 4, Store.robots[self - 1].prev_channel, 2, Store.robots[self - 1].prev_tr_id);
              event_id += 1;
            }
            fprintf(f, "%*d %*d %*d     startMotion       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[Store.robots[self - 1].cur_cell.id + 1], 4, Store.robots[self - 1].low_SKU, 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
            fprintf(control_system_log, "%*d %*d %*d     startMotion       %*s     %*s     %*d    %*d   %*d\n", 6, control_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[Store.robots[self - 1].cur_cell.id + 1], 4, Store.robots[self - 1].low_SKU, 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
            event_id += 1;
            control_id += 1;
          }
          if (Store.robots[self - 1].cur_time >= Store.robots[self - 1].goal_time) {

            if (Store.cells[Store.robots[self - 1].cur_cell.id + 1].queue[0] != -1 && Store.cells[Store.robots[self - 1].cur_cell.id + 1].queue[0] != self - 1) {
              if (self == MAX_ROBOTS) {
                Send_Event(0, REVERSE, lp, &(lp->gid));
              } else {
                Send_Event(self + 1, Store.messages[self].type, lp, &(lp->gid));
              }
              break;
            }

            Store.robots[self - 1].tmp_fl = 1;

            Store.robots[self - 1].cur_time = 0;

            int row_to_add = 0;
            while (Store.conveyor[Store.robots[self - 1].col].boxes[row_to_add].SKU == -1) {
              row_to_add += 1;
            }
            row_to_add -= 1;
            
            int save = Store.robots[self - 1].row;
            Store.robots[self - 1].row = row_to_add;

            // Print_Channel(Store.robots[self - 1].col, f);
            Add_Box(&(Store.db), Store.robots[self - 1].low_SKU, self);
            fprintf(f, "%*d %*d %*d movebox2channel       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[Store.robots[self - 1].cur_cell.id + 1], 4, Store.robots[self - 1].low_SKU, 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
            fprintf(control_system_log, "%*d %*d %*d movebox2channel       %*s     %*s     %*d    %*d   %*d\n", 6, control_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[Store.robots[self - 1].cur_cell.id + 1], 4, Store.robots[self - 1].low_SKU, 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
            event_id += 1;
            control_id += 1;
            //Print_Channel(Store.robots[self - 1].col, f);
            Store.robots[self - 1].low_SKU = -1;

            Store.robots[self - 1].row = save + 1;

            Store.robots[self - 1].has_box = -1;

            //fprintf(f, "%*d %*d %*d    finishMotion       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[Store.robots[self - 1].cur_cell.id + 1], 4, 0, 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
            Store.robots[self - 1].prev_box_type = 0;
            Store.robots[self - 1].prev_channel = Store.robots[self - 1].col % 10 + 1;
            Store.robots[self - 1].prev_tr_id = 0;
            Store.robots[self - 1].prev_vertex = Store.robots[self - 1].cur_cell.id;
            del_from_queue(self - 1);
            Store.robots[self - 1].cur_cell.id += 1;
            //event_id += 1;
          }
        } else {
          
          int next_vert = next_vertex(Store.robots[self - 1].cur_cell.id, Store.robots[self - 1].goal_cell.id);

          if (Store.direction_graph[Store.robots[self - 1].cur_cell.id] != -1 && Store.direction_graph[next_vert] != -1) { // если оба стыка
            Store.robots[self - 1].goal_time = 3;
          } else if (Store.direction_graph[Store.robots[self - 1].cur_cell.id] != -1 && (next_vert == 0 || next_vert == 43)) { // если стык и конечная
            Store.robots[self - 1].goal_time = 3;
          } else if (Store.robots[self - 1].cur_cell.id == 0) { // если в начале (паллета на загрузку)
            Store.robots[self - 1].goal_time = 3;
          } else if (Store.robots[self - 1].cur_cell.id == 1) { // если в начале (паллета на загрузку)
            Store.robots[self - 1].goal_time = 4;
          } else {
            Store.robots[self - 1].goal_time = 6;
          }

          if (Store.robots[self - 1].cur_time == 1) {
            if (glb_time > 1) {
              fprintf(f, "%*d %*d %*d    finishMotion       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].prev_vertex], 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.robots[self - 1].prev_box_type, 4, Store.robots[self - 1].prev_channel, 2, Store.robots[self - 1].prev_tr_id);
              event_id += 1;
            }
            add_to_queue(self - 1, next_vert);
            del_from_queue(self - 1);
            if (Store.robots[self - 1].has_box == 1) {
              fprintf(f, "%*d %*d %*d     startMotion       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[next_vert], 4, Store.robots[self - 1].low_SKU, 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
              fprintf(control_system_log, "%*d %*d %*d     startMotion       %*s     %*s     %*d    %*d   %*d\n", 6, control_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[next_vert], 4, Store.robots[self - 1].low_SKU, 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
            } else {
              fprintf(f, "%*d %*d %*d     startMotion       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[next_vert], 4, 0, 4,Store.robots[self - 1].col % 10 + 1, 2, 0);
              fprintf(control_system_log, "%*d %*d %*d     startMotion       %*s     %*s     %*d    %*d   %*d\n", 6, control_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[next_vert], 4, 0, 4,Store.robots[self - 1].col % 10 + 1, 2, 0);
            }
            event_id += 1;
            control_id += 1;
          }

          if (Store.robots[self - 1].cur_time >= Store.robots[self - 1].goal_time) {
            if (Store.cells[next_vert].queue[0] != -1 && Store.cells[next_vert].queue[0] != self - 1) {
              if (self == MAX_ROBOTS) {
                Send_Event(0, TAKE_IN, lp, &(lp->gid));
              } else {
                Send_Event(self + 1, Store.messages[self].type, lp, &(lp->gid));
              }
              break;
            }
            Store.robots[self - 1].tmp_fl = 1;

            Store.robots[self - 1].cur_time = 0;

            if (Store.robots[self - 1].has_box == 1) {
              //fprintf(f, "%*d %*d %*d    finishMotion       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[next_vert], 4, Store.robots[self - 1].low_SKU, 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
              //event_id += 1;
              Store.robots[self - 1].prev_box_type = Store.robots[self - 1].low_SKU;
              Store.robots[self - 1].prev_channel = Store.robots[self - 1].col % 10 + 1;
              Store.robots[self - 1].prev_tr_id = 0;
            } else {
              Store.robots[self - 1].prev_box_type = 0;
              Store.robots[self - 1].prev_channel = Store.robots[self - 1].col % 10 + 1;
              Store.robots[self - 1].prev_tr_id = 0;
              //fprintf(f, "%*d %*d %*d    finishMotion       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[next_vert], 4, 0, 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
              //event_id += 1;
            }
            Store.robots[self - 1].prev_vertex = Store.robots[self - 1].cur_cell.id;
            
            del_from_queue(self - 1);
            Store.robots[self - 1].cur_cell.id = next_vert;
          }
        }
        
        if (self == MAX_ROBOTS) {
          Send_Event(0, REVERSE, lp, &(lp->gid));
        } else {
          Send_Event(self + 1, Store.messages[self].type, lp, &(lp->gid));
        }
        break;
      case GO:
        if (cur_boxes >= 1190) {
          break;
        }

        if (Store.robots[self - 1].cur_cell.id == Store.robots[self - 1].goal_cell.id) {

          if (Store.robots[self - 1].cur_time == 1) {
            Store.robots[self - 1].goal_time = 6;
            add_to_queue(self - 1, Store.robots[self - 1].cur_cell.id + 1);
            del_from_queue(self - 1);
            if (glb_time > 1) {
              fprintf(f, "%*d %*d %*d    finishMotion       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].prev_vertex], 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.robots[self - 1].prev_box_type, 4, Store.robots[self - 1].prev_channel, 2, Store.robots[self - 1].prev_tr_id);
              event_id += 1;
            }
            fprintf(f, "%*d %*d %*d     startMotion       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[Store.robots[self - 1].cur_cell.id + 1], 4, 0, 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
            fprintf(control_system_log, "%*d %*d %*d     startMotion       %*s     %*s     %*d    %*d   %*d\n", 6, control_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[Store.robots[self - 1].cur_cell.id + 1], 4, 0, 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
            event_id += 1;
            control_id += 1;
          }

          if (Store.robots[self - 1].cur_time >= Store.robots[self - 1].goal_time) {

            if (Store.cells[Store.robots[self - 1].cur_cell.id + 1].queue[0] != -1 && Store.cells[Store.robots[self - 1].cur_cell.id + 1].queue[0] != self - 1) {
              if (self == MAX_ROBOTS) {
                Send_Event(0, GO, lp, &(lp->gid));
              } else {
                Send_Event(self + 1, Store.messages[self].type, lp, &(lp->gid));
              }
              break;
            }

            Store.robots[self - 1].tmp_fl = 1;

            Store.robots[self - 1].cur_time = 0;
            Store.robots[self - 1].cur_task = -1;
            //fprintf(f, "%*d %*d %*d    finishMotion       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[Store.robots[self - 1].cur_cell.id + 1], 4, 0, 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
            Store.robots[self - 1].prev_box_type = 0;
            Store.robots[self - 1].prev_channel = Store.robots[self - 1].col % 10 + 1;
            Store.robots[self - 1].prev_tr_id = 0;
            Store.robots[self - 1].prev_vertex = Store.robots[self - 1].cur_cell.id;
            del_from_queue(self - 1);
            Store.robots[self - 1].cur_cell.id += 1;
            //event_id += 1;
          }
        } else {
          int next_vert = next_vertex(Store.robots[self - 1].cur_cell.id, Store.robots[self - 1].goal_cell.id);
          
          if (Store.direction_graph[Store.robots[self - 1].cur_cell.id] != -1 && Store.direction_graph[next_vert] != -1) { // если оба стыка
            Store.robots[self - 1].goal_time = 3;
          } else if (Store.direction_graph[Store.robots[self - 1].cur_cell.id] != -1 && (next_vert == 0 || next_vert == 43)) { // если стык и конечная
            Store.robots[self - 1].goal_time = 3;
          } else if (Store.robots[self - 1].cur_cell.id == 0) { // если в начале (паллета на загрузку)
            Store.robots[self - 1].goal_time = 3;
          } else if (Store.robots[self - 1].cur_cell.id == 1) { // если в начале (паллета на загрузку)
            Store.robots[self - 1].goal_time = 4;
          } else {
            Store.robots[self - 1].goal_time = 6;
          }
          if (Store.robots[self - 1].cur_time == 1) {
            add_to_queue(self - 1, next_vert);
            del_from_queue(self - 1);
            if (glb_time > 1) {
              fprintf(f, "%*d %*d %*d    finishMotion       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].prev_vertex], 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.robots[self - 1].prev_box_type, 4, Store.robots[self - 1].prev_channel, 2, Store.robots[self - 1].prev_tr_id);
              event_id += 1;
            }
            if (Store.robots[self - 1].has_box == 1) {
              fprintf(f, "%*d %*d %*d     startMotion       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[next_vert], 4, 0, 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
              fprintf(control_system_log, "%*d %*d %*d     startMotion       %*s     %*s     %*d    %*d   %*d\n", 6, control_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[next_vert], 4, 0, 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
            } else {
              fprintf(f, "%*d %*d %*d     startMotion       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[next_vert], 4, 0, 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
              fprintf(control_system_log, "%*d %*d %*d     startMotion       %*s     %*s     %*d    %*d   %*d\n", 6, control_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[next_vert], 4, 0, 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
            }
            control_id += 1;
            event_id += 1;
          }

          if (Store.robots[self - 1].cur_time >= Store.robots[self - 1].goal_time) {
            if (Store.cells[next_vert].queue[0] != -1 && Store.cells[next_vert].queue[0] != self - 1) {
              if (self == MAX_ROBOTS) {
                Send_Event(0, GO, lp, &(lp->gid));
              } else {
                Send_Event(self + 1, Store.messages[self].type, lp, &(lp->gid));
              }
              break;
            }
            Store.robots[self - 1].tmp_fl = 1;

            Store.robots[self - 1].cur_time = 0;

            // if (Store.robots[self - 1].has_box == 1) {
            //   Store.robots[self - 1].prev_box_type = 0;
            //   Store.robots[self - 1].prev_channel = Store.robots[self - 1].col % 10 + 1;
            //   Store.robots[self - 1].prev_tr_id = 0;
            //   // fprintf(f, "%*d %*d %*d    finishMotion       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[next_vert], 4, 0, 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
            //   // event_id += 1;
            // } else {
            //   Store.robots[self - 1].prev_box_type = 0;
            //   Store.robots[self - 1].prev_channel = Store.robots[self - 1].col % 10 + 1;
            //   Store.robots[self - 1].prev_tr_id = 0;
            //   // fprintf(f, "%*d %*d %*d    finishMotion       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[next_vert], 4, 0, 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
            //   // event_id += 1;
            // }
            Store.robots[self - 1].prev_box_type = 0;
            Store.robots[self - 1].prev_channel = Store.robots[self - 1].col % 10 + 1;
            Store.robots[self - 1].prev_tr_id = 0;
            Store.robots[self - 1].prev_vertex = Store.robots[self - 1].cur_cell.id;
            
            del_from_queue(self - 1);
            Store.robots[self - 1].cur_cell.id = next_vert;
          }
        }
      
        if (self == MAX_ROBOTS) {
          Send_Event(0, GO, lp, &(lp->gid));
        } else {
          Send_Event(self + 1, Store.messages[self].type, lp, &(lp->gid));
        }
        break;

      default:
        printf("\n%s\n", "No message");
        break;
    }
  }
}

//Reverse Event Handler
void model_event_reverse (state *s, tw_bf *bf, message *in_msg, tw_lp *lp) {
  return;
}

//report any final statistics for this LP
void model_final (state *s, tw_lp *lp) {
  if (lp->gid == 0) {
    write_csv("Store.csv", Store.db);
    fprintf(paleta, "%*d %*d %*s %*s", 6, rec_id, 6, glb_time, 18, "finishPalletize", 22, Store.cur_order);
    fprintf(paleta, "\n");
    rec_id++;
    fprintf(f, "%*d %*d      finishPalletize %s", 6, event_id, 6, glb_time, Store.cur_order);
  }
  return;
}
