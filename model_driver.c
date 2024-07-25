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
  // for (int process = 4; process < 7; ++process) {
  //   if (Store.box_data[process][1] != 0) {
  //     Store.used[process] = 1;
  //     Send_Event(process, TAKE_OUT, lp, &(lp->gid));
  //   } else {
  //     if (Check(process)) {
  //       Store.used[process] = 1;
  //       Send_Event(process, TAKE_OUT, lp, &(lp->gid));
  //     };
  //   }
  // }
}

void print_motion(int id_from, int id_to, int robot_id, int *time) {
  fprintf(f, "%*d %*d   moverobot   id %*d   from %*s   to %*s\n", 6, event_id, 6, *time, 2, robot_id, 2, Store.vertexes[id_from], 2, Store.vertexes[id_to]);
  event_id += 1;
}

void model_event (state *s, tw_bf *bf, message *in_msg, tw_lp *lp) {
  int self = lp->gid;
  *(int *) bf = (int) 0;
  SWAP(&(s->value), &(in_msg->contents));

  Store.kill_prog = 1;
  int ok_take_in = 1;
  for (int i = 0; i < MAX_ROBOTS; ++i) {
    if (Store.robots[i].kill == 0) {
      Store.kill_prog = 0;
    } else {
      ok_take_in = 0;
    }
  }

  if (self == 0 && Store.kill_prog == 0) {
    int not_do = 0;
    for (int i = 1; i < 7; ++i) {
      if (Store.used[i] == 1) {
        not_do = 1;
      }
    }
    if (not_do == 0) {
      for (int i = 0; i < 6; ++i) {
        fprintf(test, "%d ", Store.robots[i].pre_reserved);
      }
      fprintf(test, "%d ", Store.boxes_to_deliver);
      fprintf(test, "\n");
      glb_time += 1;
      if (Store.boxes_to_deliver <= 0) {
        for (int i = 1; i < high_border - low_border + 1; ++i) {
          if (ok_take_in && Store.cnt_boxes_type[i] < 5) {
            Store.boxes_to_deliver = threshold - Store.cnt_boxes_type[i];
            //printf("%d %d\n", Store.boxes_to_deliver, Store.cnt_boxes_type[i]);
            //printf("%d\n", Store.boxes_to_deliver);
            if ((Store.robots[0].cur_task == 1 || Store.boxes_to_deliver >= 1) && Store.robots[0].cur_task != 2) {
              Store.nt_used[1] = 1;
              if (Store.robots[0].pre_reserved == -1 || (Store.robots[0].pre_reserved == 1 && Store.cells[Store.robots[0].cur_cell.id].reserved == 0)) {
                Store.used[1] = 1;
                Store.robots[0].cur_task = 1;
                Send_Event(1, TAKE_IN, lp, &(lp->gid));
                if (Store.robots[0].has_box == -1) {
                  Store.robots[0].goal_cell.id = MAX_RACKS - 1; 
                }
              }
            }

            if ((Store.robots[1].cur_task == 1 || Store.boxes_to_deliver >= 2) && Store.robots[1].cur_task != 2) {
              Store.nt_used[2] = 1;
              if (Store.robots[1].pre_reserved == -1 || (Store.robots[1].pre_reserved == 1 && Store.cells[Store.robots[1].cur_cell.id].reserved == 0)) {
                Store.used[2] = 1;
                Store.robots[1].cur_task = 1;
                Send_Event(2, TAKE_IN, lp, &(lp->gid));
                if (Store.robots[1].has_box == -1) {
                  Store.robots[1].goal_cell.id = MAX_RACKS - 1; 
                }
              }
            }
            if ((Store.robots[2].cur_task == 1 || Store.boxes_to_deliver >= 3) && Store.robots[2].cur_task != 2) {
              Store.nt_used[3] = 1;
              if (Store.robots[2].pre_reserved == -1 || (Store.robots[2].pre_reserved == 1 && Store.cells[Store.robots[2].cur_cell.id].reserved == 0)) {
                Store.used[3] = 1;
                Store.robots[2].cur_task = 1;
                Send_Event(3, TAKE_IN, lp, &(lp->gid));
                if (Store.robots[2].has_box == -1) {
                  Store.robots[2].goal_cell.id = MAX_RACKS - 1; 
                }
              }
            }
            if ((Store.robots[3].cur_task == 1 || Store.boxes_to_deliver >= 4) && Store.robots[3].cur_task != 2) {
              Store.nt_used[4] = 1;
              if (Store.robots[3].pre_reserved == -1 || (Store.robots[3].pre_reserved == 1 && Store.cells[Store.robots[3].cur_cell.id].reserved == 0)) {
                Store.used[4] = 1;
                Store.robots[3].cur_task = 1;
                Send_Event(4, TAKE_IN, lp, &(lp->gid));
                if (Store.robots[3].has_box == -1) {
                  Store.robots[3].goal_cell.id = MAX_RACKS - 1; 
                }
              }
            }
            if ((Store.robots[4].cur_task == 1 || Store.boxes_to_deliver >= 5) && Store.robots[4].cur_task != 2) {
              Store.nt_used[5] = 1;
              if (Store.robots[4].pre_reserved == -1 || (Store.robots[4].pre_reserved == 1 && Store.cells[Store.robots[4].cur_cell.id].reserved == 0)) {
                Store.used[5] = 1;
                Store.robots[4].cur_task = 1;
                Send_Event(5, TAKE_IN, lp, &(lp->gid));
                if (Store.robots[4].has_box == -1) {
                  Store.robots[4].goal_cell.id = MAX_RACKS - 1; 
                }
              }
            }
            if ((Store.robots[5].cur_task == 1 || Store.boxes_to_deliver >= 6) && Store.robots[5].cur_task != 2) {
              Store.nt_used[6] = 1;
              if (Store.robots[5].pre_reserved == -1 || (Store.robots[5].pre_reserved == 1 && Store.cells[Store.robots[5].cur_cell.id].reserved == 0)) {
                Store.used[6] = 1;
                Store.robots[5].cur_task = 1;
                Send_Event(6, TAKE_IN, lp, &(lp->gid));
                if (Store.robots[5].has_box == -1) {
                  Store.robots[5].goal_cell.id = MAX_RACKS - 1; 
                }
              }
            }
            Store.type_to_add = i;
            fprintf(f, "%*d %*d     startDepalletize\n", 6, event_id, 6, glb_time);
            event_id += 1;
            break;
          }
        }
      } else {

        //printf("%d\n", Store.boxes_to_deliver);

        if (ok_take_in && (Store.robots[0].cur_task == 1 || Store.boxes_to_deliver >= 1) && Store.robots[0].cur_task != 2) {
          Store.nt_used[1] = 1;
          if (Store.robots[0].pre_reserved == -1 || (Store.robots[0].pre_reserved == 1 && Store.cells[Store.robots[0].cur_cell.id].reserved == 0)) {
            Store.used[1] = 1;
            Store.robots[0].cur_task = 1;
            Send_Event(1, TAKE_IN, lp, &(lp->gid));
            if (Store.robots[0].has_box == -1) {
              Store.robots[0].goal_cell.id = MAX_RACKS - 1; 
            }
          }
        }
        if (ok_take_in && (Store.robots[1].cur_task == 1 || Store.boxes_to_deliver >= 2) && Store.robots[1].cur_task != 2) {
          Store.nt_used[2] = 1;
          if (Store.robots[1].pre_reserved == -1 || (Store.robots[1].pre_reserved == 1 && Store.cells[Store.robots[1].cur_cell.id].reserved == 0)) {
            Store.used[2] = 1;
            Store.robots[1].cur_task = 1;
            Send_Event(2, TAKE_IN, lp, &(lp->gid));
            if (Store.robots[1].has_box == -1) {
              Store.robots[1].goal_cell.id = MAX_RACKS - 1; 
            }
          }
        }
        if (ok_take_in && (Store.robots[2].cur_task == 1 || Store.boxes_to_deliver >= 3) && Store.robots[2].cur_task != 2) {
          Store.nt_used[3] = 1;
          if (Store.robots[2].pre_reserved == -1 || (Store.robots[2].pre_reserved == 1 && Store.cells[Store.robots[2].cur_cell.id].reserved == 0)) {
            Store.used[3] = 1;
            Store.robots[2].cur_task = 1;
            Send_Event(3, TAKE_IN, lp, &(lp->gid));
            if (Store.robots[2].has_box == -1) {
              Store.robots[2].goal_cell.id = MAX_RACKS - 1; 
            }
          }
        }
        if (ok_take_in && (Store.robots[3].cur_task == 1 || Store.boxes_to_deliver >= 4) && Store.robots[3].cur_task != 2) {
          Store.nt_used[4] = 1;
          if (Store.robots[3].pre_reserved == -1 || (Store.robots[3].pre_reserved == 1 && Store.cells[Store.robots[3].cur_cell.id].reserved == 0)) {
            Store.used[4] = 1;
            Store.robots[3].cur_task = 1;
            Send_Event(4, TAKE_IN, lp, &(lp->gid));
            if (Store.robots[3].has_box == -1) {
              Store.robots[3].goal_cell.id = MAX_RACKS - 1; 
            }
          }
        }
        if (ok_take_in && (Store.robots[4].cur_task == 1 || Store.boxes_to_deliver >= 5) && Store.robots[4].cur_task != 2) {
          Store.nt_used[5] = 1;
          if (Store.robots[4].pre_reserved == -1 || (Store.robots[4].pre_reserved == 1 && Store.cells[Store.robots[4].cur_cell.id].reserved == 0)) {
            Store.used[5] = 1;
            Store.robots[4].cur_task = 1;
            Send_Event(5, TAKE_IN, lp, &(lp->gid));
            if (Store.robots[4].has_box == -1) {
              Store.robots[4].goal_cell.id = MAX_RACKS - 1; 
            }
          }
        }
        if (ok_take_in && (Store.robots[5].cur_task == 1 || Store.boxes_to_deliver >= 6) && Store.robots[5].cur_task != 2) {
          Store.nt_used[6] = 1;
          if (Store.robots[5].pre_reserved == -1 || (Store.robots[5].pre_reserved == 1 && Store.cells[Store.robots[5].cur_cell.id].reserved == 0)) {
            Store.used[6] = 1;
            Store.robots[5].cur_task = 1;
            Send_Event(6, TAKE_IN, lp, &(lp->gid));
            if (Store.robots[5].has_box == -1) {
              Store.robots[5].goal_cell.id = MAX_RACKS - 1; 
            }
          }
        }
      }

      for (int process = 1; process < 4; ++process) {
        if (cur_boxes >= 390 && Store.robots[process - 1].cur_task != 1) {
          Store.nt_used[process] = 1;
          if (Store.robots[process - 1].col != -1 && Store.robots[process - 1].row != -1) {
            Store.robots[process - 1].cur_task = 2;
            if (Store.robots[process - 1].row != 7) {
              if (Store.robots[process - 1].pre_reserved == -1 || (Store.robots[process - 1].pre_reserved == 1 && Store.cells[Store.robots[process - 1].cur_cell.id].reserved == 0)) {
                Store.used[process] = 1;
                Send_Event(process, REVERSE, lp, &(lp->gid));
              }
            } else {
              if (Store.robots[process - 1].pre_reserved == -1 || (Store.robots[process - 1].pre_reserved == 1 && Store.cells[Store.robots[process - 1].cur_cell.id].reserved == 0)) {
                Store.used[process] = 1;
                Send_Event(process, TAKE_OUT, lp, &(lp->gid)); 
              }
            }
          } else {
            if (Check(process)) {
              Store.robots[process - 1].cur_task = 2;
              find_data(&(Store.db), Store.box_data[process][0]);
              Store.robots[process - 1].goal_cell.id = MAX_RACKS * 3 + 1 - (int)(best_box.column / 10);
              Store.robots[process - 1].reserved_channel = best_box.column;

              Store.robots[process - 1].col = best_box.column;
              Store.robots[process - 1].row = best_box.row;

              if (Store.robots[process - 1].row != 7) {
                if (Store.robots[process - 1].pre_reserved == -1 || (Store.robots[process - 1].pre_reserved == 1 && Store.cells[Store.robots[process - 1].cur_cell.id].reserved == 0)) {
                  Store.used[process] = 1;
                  Send_Event(process, REVERSE, lp, &(lp->gid));
                }
              } else {
                if (Store.robots[process - 1].pre_reserved == -1 || (Store.robots[process - 1].pre_reserved == 1 && Store.cells[Store.robots[process - 1].cur_cell.id].reserved == 0)) {
                  Store.used[process] = 1;
                  Send_Event(process, TAKE_OUT, lp, &(lp->gid));
                }
              }
            } else {
              if (Store.cur_file > MAX_FILES) {
                Store.robots[process - 1].kill = 1;
              } else {
                if (Store.cur_file != 0) {
                  fprintf(f, "%*d %*d      finishPalletize %s", 6, event_id, 6, glb_time, Store.cur_order);
                  event_id += 1;
                }
                Init_Commands(&(event_id), &(glb_time), Store.files[Store.cur_file]);
                Store.cur_file += 1;
              }
            }
          }

        }
      }

      for (int process = 4; process < 7; ++process) {
        if (cur_boxes >= 390 && Store.robots[process - 1].cur_task != 1) {
          Store.nt_used[process] = 1;
          if (Store.robots[process - 1].col != -1 && Store.robots[process - 1].row != -1) {

            Store.robots[process - 1].cur_task = 2;
            if (Store.robots[process - 1].row != 7) {
              if (Store.robots[process - 1].pre_reserved == -1 || (Store.robots[process - 1].pre_reserved == 1 && Store.cells[Store.robots[process - 1].cur_cell.id].reserved == 0)) {
                Store.used[process] = 1;
                Send_Event(process, REVERSE, lp, &(lp->gid));
              }
            } else {
              if (Store.robots[process - 1].pre_reserved == -1 || (Store.robots[process - 1].pre_reserved == 1 && Store.cells[Store.robots[process - 1].cur_cell.id].reserved == 0)) {
                Store.used[process] = 1;
                Send_Event(process, TAKE_OUT, lp, &(lp->gid));
              }
            }
          } else {
            if (Check(process)) {
              Store.robots[process - 1].cur_task = 2;
              find_data(&(Store.db), Store.box_data[process][0]);
              Store.robots[process - 1].goal_cell.id = MAX_RACKS * 3 + 1 - (int)(best_box.column / 10);

              Store.robots[process - 1].reserved_channel = best_box.column;

              Store.robots[process - 1].col = best_box.column;
              Store.robots[process - 1].row = best_box.row;

              if (Store.robots[process - 1].row != 7) {
                if (Store.robots[process - 1].pre_reserved == -1 || (Store.robots[process - 1].pre_reserved == 1 && Store.cells[Store.robots[process - 1].cur_cell.id].reserved == 0)) {
                  Store.used[process] = 1;
                  Send_Event(process, REVERSE, lp, &(lp->gid));
                }
              } else {
                if (Store.robots[process - 1].pre_reserved == -1 || (Store.robots[process - 1].pre_reserved == 1 && Store.cells[Store.robots[process - 1].cur_cell.id].reserved == 0)) {
                  Store.used[process] = 1;
                  Send_Event(process, TAKE_OUT, lp, &(lp->gid));
                }
              }
            } else {
              if (Store.cur_file > MAX_FILES) {
                Store.robots[process - 1].kill = 1;
              } else {
                if (Store.cur_file != 0) {
                  fprintf(f, "%*d %*d      finishPalletize %s", 6, event_id, 6, glb_time, Store.cur_order);
                  event_id += 1;
                }
                Init_Commands(&(event_id), &(glb_time), Store.files[Store.cur_file]);
                Store.cur_file += 1;
              }
            }
          }
        }
      }
      for (int i = 1; i < 7; ++i) {
        if (cur_boxes < 390 && ok_take_in && Store.nt_used[i] == 0) {
          Send_Event(i, GO, lp, &(lp->gid));
        }
      }
    }
  } else if (Store.kill_prog == 0) {
    Store.used[self] = 0;
    Store.nt_used[self] = 0;
    //printf("%d\n", self);
    Store.robots[self - 1].cur_time += 1;
    switch (in_msg->type)
    {
      case TAKE_IN:
        for (int i = 0; i < 6; ++i) {
          fprintf(temp_txt, "%d ||| ", Store.robots[i].cur_cell.id);
        }
        fprintf(temp_txt, "\n");
        fprintf(temp_txt, "\n");
        Store.cells[Store.robots[self - 1].cur_cell.id].reserved = 0;

        if (Store.robots[self - 1].has_box == -1 && Store.robots[self - 1].cur_cell.id == Store.robots[self - 1].goal_cell.id) {
          Store.robots[self - 1].goal_time = 8;
          find_data_by_width(&(Store.db), Store.type_to_add);
          if (best_box.row == -1 || best_box.column == -1) {
            Store.boxes_to_deliver--;
            break;
          }
          if (Store.robots[self - 1].cur_time == 1) {
            fprintf(f, "%*d %*d %*d     startMotion       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[Store.robots[self - 1].cur_cell.id + 1], 4, 0, 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
            event_id += 1;
          }
          if (Store.robots[self - 1].cur_time >= Store.robots[self - 1].goal_time) {

            Store.robots[self - 1].cur_time = 0;

            fprintf(paleta, "%*d AddBox       %*d\n", 4, glb_time, 4, Store.type_to_add);

            Store.robots[self - 1].reserved_channel = best_box.column;
            Store.robots[self - 1].has_box = 1;
            Store.robots[self - 1].goal_cell.id = MAX_RACKS + 1 + (int)(best_box.column / 10);
            Store.robots[self - 1].col = best_box.column;
            Store.robots[self - 1].row = best_box.row;

            fprintf(f, "%*d %*d %*d     movebox2bot       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[Store.robots[self - 1].cur_cell.id + 1], 4, Store.type_to_add, 4, 0, 2, 1);
            event_id += 1;

            if (Store.cells[Store.robots[self - 1].cur_cell.id + 1].reserved == 0) {
              Store.cells[Store.robots[self - 1].cur_cell.id + 1].reserved = 1;
              Store.robots[self - 1].pre_reserved = -1;
            } else {
              Store.robots[self - 1].pre_reserved = 1;
            }
            fprintf(f, "%*d %*d %*d    finishMotion       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[Store.robots[self - 1].cur_cell.id + 1], 4, Store.type_to_add, 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
            Store.robots[self - 1].cur_cell.id += 1;
            event_id += 1;

          }


        } else if (Store.robots[self - 1].has_box == 1 && Store.robots[self - 1].cur_cell.id == Store.robots[self - 1].goal_cell.id) {
          Store.robots[self - 1].goal_time = 8;
          if (Store.robots[self - 1].cur_time == 1) {
            fprintf(f,"%*d %*d %*d     startMotion       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[Store.robots[self - 1].cur_cell.id + 1], 4, Store.type_to_add, 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
            event_id += 1;
          }
          if (Store.robots[self - 1].cur_time >= Store.robots[self - 1].goal_time) {
            Store.robots[self - 1].cur_time = 0;
            Add_Box(&(Store.db), Store.type_to_add, self);
            Store.boxes_to_deliver--;
            cur_boxes += 1;
            
            fprintf(f, "%*d %*d %*d movebox2channel       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[Store.robots[self - 1].cur_cell.id + 1], 4, Store.type_to_add, 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
            event_id += 1;
            //Print_Channel(Store.robots[self - 1].col, f);
            fprintf(f, "%*d %*d %*d    finishMotion       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[Store.robots[self - 1].cur_cell.id + 1], 4, 0, 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
            Store.robots[self - 1].reserved_channel = -1;
            Store.robots[self - 1].has_box = -1;
            Store.robots[self - 1].col = -1;
            Store.robots[self - 1].row = -1;
            Store.robots[self - 1].cur_task = -1;
            if (Store.boxes_to_deliver == 0) {
              fprintf(f, "%*d %*d    finishDepalletize\n", 6, event_id, 6, glb_time);
              event_id += 1;
            }
            event_id += 1;

            if (Store.cells[Store.robots[self - 1].cur_cell.id + 1].reserved == 0) {
              Store.robots[self - 1].pre_reserved = -1;
              Store.cells[Store.robots[self - 1].cur_cell.id + 1].reserved = 1;
            } else {
              Store.robots[self - 1].pre_reserved = 1;
            }
            
            Store.robots[self - 1].cur_cell.id += 1;
          }

        } else {
          if (Store.robots[self - 1].cur_cell.id == MAX_RACKS * 2 + 1) {
            Store.robots[self - 1].goal_time = 3;
          } else if (Store.robots[self - 1].cur_cell.id == MAX_RACKS * 4 + 3) {
            Store.robots[self - 1].goal_time = 3;
          } else if (Store.robots[self - 1].cur_cell.id == MAX_RACKS) {
            Store.robots[self - 1].goal_time = 3;
          } else if (Store.robots[self - 1].cur_cell.id == MAX_RACKS * 3 + 2) {
            Store.robots[self - 1].goal_time = 7;
          } else {
            Store.robots[self - 1].goal_time = 6;
          }

          int future_id = 0;
          if (Store.robots[self - 1].cur_cell.id == MAX_RACKS * 3 + 2) {
            future_id = Store.store_graph[MAX_RACKS * 3 + 2][1];
          } else if (Store.robots[self - 1].cur_cell.id == MAX_RACKS * 2 + 1) {
            future_id = Store.store_graph[MAX_RACKS * 2 + 1][1];
          } else {
            future_id = Store.store_graph[Store.robots[self - 1].cur_cell.id][0];
          }

          if (Store.robots[self - 1].cur_time == 1) {
            if (Store.robots[self - 1].has_box == 1) {
              fprintf(f, "%*d %*d %*d     startMotion       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[future_id], 4, Store.type_to_add, 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
              event_id += 1;
            } else {
              fprintf(f, "%*d %*d %*d     startMotion       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[future_id], 4, 0, 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
              event_id += 1;
            }
          }

          if (Store.robots[self - 1].cur_time >= Store.robots[self - 1].goal_time) {
            Store.robots[self - 1].cur_time = 0;
            if (Store.robots[self - 1].has_box == 1) {
              fprintf(f, "%*d %*d %*d    finishMotion       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[future_id], 4, Store.type_to_add, 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
              event_id += 1;
            } else {
              fprintf(f, "%*d %*d %*d    finishMotion       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[future_id],  4, 0, 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
              event_id += 1;
            }
            if (Store.robots[self - 1].cur_cell.id == MAX_RACKS * 3 + 2) {

              Store.robots[self - 1].cur_cell.id = Store.store_graph[MAX_RACKS * 3 + 2][1];

              if (Store.cells[Store.robots[self - 1].cur_cell.id].reserved == 0) {
                Store.cells[Store.robots[self - 1].cur_cell.id].reserved = 1;
                Store.robots[self - 1].pre_reserved = -1;
              } else {
                Store.robots[self - 1].pre_reserved = 1;
              }

            } else if (Store.robots[self - 1].cur_cell.id == MAX_RACKS * 2 + 1) {

              Store.robots[self - 1].cur_cell.id = Store.store_graph[MAX_RACKS * 2 + 1][1];
              
              if (Store.cells[Store.robots[self - 1].cur_cell.id].reserved == 0) {
                Store.cells[Store.robots[self - 1].cur_cell.id].reserved = 1;
                Store.robots[self - 1].pre_reserved = -1;
              } else {
                Store.robots[self - 1].pre_reserved = 1;
              }

            } else {

              Store.robots[self - 1].cur_cell.id = Store.store_graph[Store.robots[self - 1].cur_cell.id][0];

              if (Store.cells[Store.robots[self - 1].cur_cell.id].reserved == 0) {
                Store.cells[Store.robots[self - 1].cur_cell.id].reserved = 1;
                Store.robots[self - 1].pre_reserved = -1;
              } else {
                Store.robots[self - 1].pre_reserved = 1;
              }

            }

          }

        }
        
        
        Send_Event(0, TAKE_IN, lp, &(lp->gid));
        break;

      case TAKE_OUT:
        for (int i = 0; i < 6; ++i) {
          fprintf(temp_txt, "%d %d %d||| ", Store.robots[i].goal_cell.id, Store.robots[i].cur_cell.id, Store.robots[i].has_box);
        }
        fprintf(temp_txt, "\n");
        fprintf(temp_txt, "\n");

        Store.cells[Store.robots[self - 1].cur_cell.id].reserved = 0;
        if (Store.robots[self - 1].has_box == -1 && Store.robots[self - 1].cur_cell.id == Store.robots[self - 1].goal_cell.id) {
          Store.robots[self - 1].goal_time = 8;
          if (Store.robots[self - 1].cur_time == 1) {
            fprintf(f, "%*d %*d %*d     startMotion       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[Store.robots[self - 1].cur_cell.id + 1], 4, 0, 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
            event_id += 1;
          }
          if (Store.robots[self - 1].cur_time >= Store.robots[self - 1].goal_time) {
            Store.robots[self - 1].cur_time = 0;
            Remove_Boxes(&(Store.db), Store.box_data[self][0], &(glb_time), &(event_id), self);
            Store.robots[self - 1].cur_box = Store.box_data[self][0];
            fprintf(f, "%*d %*d %*d     movebox2bot       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[Store.robots[self - 1].cur_cell.id + 1], 4, Store.box_data[self][0], 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
            event_id += 1;

            Store.box_data[self][1] = 0;
            Store.robots[self - 1].reserved_channel = -1;
            Store.robots[self - 1].has_box = 1;
            Store.robots[self - 1].goal_cell.id = MAX_RACKS * 4 + 2;


            fprintf(f, "%*d %*d %*d    finishMotion       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[Store.robots[self - 1].cur_cell.id + 1], 4, Store.box_data[self][0], 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
            event_id += 1;

            if (Store.cells[Store.robots[self - 1].cur_cell.id + 1].reserved == 0) {
              Store.robots[self - 1].pre_reserved = -1;
              Store.cells[Store.robots[self - 1].cur_cell.id + 1].reserved = 1;
            } else {
              Store.robots[self - 1].pre_reserved = 1;
            }

            Store.robots[self - 1].cur_cell.id += 1;
          }

        } else if (Store.robots[self - 1].has_box == 1 && Store.robots[self - 1].cur_cell.id == Store.robots[self - 1].goal_cell.id) {
          Store.robots[self - 1].goal_time = 8;
          if (Store.robots[self - 1].cur_time == 1) {
            fprintf(f, "%*d %*d %*d     startMotion       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[Store.robots[self - 1].cur_cell.id + 1], 4, Store.box_data[self][0], 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
            event_id += 1;
          }
          if (Store.robots[self - 1].cur_time >= Store.robots[self - 1].goal_time) {
            Store.robots[self - 1].cur_time = 0;
            Store.robots[self - 1].has_box = -1;
            Store.robots[self - 1].row = -1;
            Store.robots[self - 1].cur_task = -1;
            fprintf(f, "%*d %*d %*d      movebox2tr       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[Store.robots[self - 1].cur_cell.id + 1], 4, Store.box_data[self][0], 4, 0, 2, 2);
            event_id += 1;
            fprintf(f, "%*d %*d %*d    finishMotion       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[Store.robots[self - 1].cur_cell.id + 1], 4, 0, 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
            Store.robots[self - 1].col = -1;
            event_id += 1;
            fprintf(paleta, "%*d DeliveredBox %*d\n", 4, glb_time, 4, Store.box_data[self][0]);
            if (Store.cells[Store.robots[self - 1].cur_cell.id + 1].reserved == 0) {
              Store.robots[self - 1].pre_reserved = -1;
              Store.cells[Store.robots[self - 1].cur_cell.id + 1].reserved = 1;
            } else {
              Store.robots[self - 1].pre_reserved = 1;
            }
            Store.robots[self - 1].cur_cell.id += 1;
          }
        } else {
          if (Store.robots[self - 1].cur_cell.id == MAX_RACKS * 2 + 1) {
            Store.robots[self - 1].goal_time = 7;
          } else if (Store.robots[self - 1].cur_cell.id == MAX_RACKS * 4 + 3) {
            Store.robots[self - 1].goal_time = 3;
          } else if (Store.robots[self - 1].cur_cell.id == MAX_RACKS) {
            Store.robots[self - 1].goal_time = 3;
          } else if (Store.robots[self - 1].cur_cell.id == MAX_RACKS * 3 + 2) {
            Store.robots[self - 1].goal_time = 3;
          } else {
            Store.robots[self - 1].goal_time = 6;
          }

          int future_id = 0;
          if (Store.robots[self - 1].cur_cell.id == MAX_RACKS * 3 + 2) {
            future_id = Store.store_graph[MAX_RACKS * 3 + 2][0];
          } else if (Store.robots[self - 1].cur_cell.id == MAX_RACKS * 2 + 1) {
            future_id = Store.store_graph[MAX_RACKS * 2 + 1][0];
          } else {
            future_id = Store.store_graph[Store.robots[self - 1].cur_cell.id][0];
          }


          if (Store.robots[self - 1].cur_time == 1) {
            if (Store.robots[self - 1].has_box == 1) {
              fprintf(f, "%*d %*d %*d     startMotion       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[future_id], 4, Store.box_data[self][0], 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
              event_id += 1;
            } else {
              fprintf(f, "%*d %*d %*d     startMotion       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[future_id], 4, 0, 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
              event_id += 1;
            }
          
          }

          if (Store.robots[self - 1].cur_time >= Store.robots[self - 1].goal_time) {
            Store.robots[self - 1].cur_time = 0;
            if (Store.robots[self - 1].has_box == 1) {
              fprintf(f, "%*d %*d %*d    finishMotion       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[future_id],  4, Store.box_data[self][0], 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
              event_id += 1;
            } else {
              fprintf(f, "%*d %*d %*d    finishMotion       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[future_id],  4, 0, 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
              event_id += 1;
            }
            if (Store.robots[self - 1].cur_cell.id == MAX_RACKS * 3 + 2) {
              Store.robots[self - 1].cur_cell.id = Store.store_graph[MAX_RACKS * 3 + 2][0];

              if (Store.cells[Store.robots[self - 1].cur_cell.id].reserved == 0) {
                Store.robots[self - 1].pre_reserved = -1;
                Store.cells[Store.robots[self - 1].cur_cell.id].reserved = 1;
              } else {
                Store.robots[self - 1].pre_reserved = 1;
              }

            } else if (Store.robots[self - 1].cur_cell.id == MAX_RACKS * 2 + 1) {
              Store.robots[self - 1].cur_cell.id = Store.store_graph[MAX_RACKS * 2 + 1][0];
              
              if (Store.cells[Store.robots[self - 1].cur_cell.id].reserved == 0) {
                Store.cells[Store.robots[self - 1].cur_cell.id].reserved = 1;
                Store.robots[self - 1].pre_reserved = -1;
              } else {
                Store.robots[self - 1].pre_reserved = 1;
              }

            } else {
              Store.robots[self - 1].cur_cell.id = Store.store_graph[Store.robots[self - 1].cur_cell.id][0];

              if (Store.cells[Store.robots[self - 1].cur_cell.id].reserved == 0) {
                Store.robots[self - 1].pre_reserved = -1;
                Store.cells[Store.robots[self - 1].cur_cell.id].reserved = 1;
              } else {
                Store.robots[self - 1].pre_reserved = 1;
              }

            }
          }
        }
        
        Send_Event(0, TAKE_OUT, lp, &(lp->gid));
        break;

        // if (Store.box_data[self][0] > high_border || Store.box_data[self][0] < low_border) {
        //     fprintf(f, "no-boxes-for-SKU %d\n", Store.box_data[self][0]);
        //     Store.box_data[self][1] = 0;
        //     Send_Event(0, TAKE_OUT, lp, &(lp->gid));
        //     break;
        // } else {
        //   find_data(&(Store.db), Store.box_data[self][0]);
        //   int channel = Remove_Boxes(&(Store.db), Store.box_data[self][0], &(cur_time), &(event_id));
        //   cur_time += 8;
        //   fprintf(f, "%*d   %*d   moveoutbox%*d   channel%*d   process%*d    boxwidth%*d    channelwidth%*d   ", 4, event_id, 4, cur_time, 5, Store.box_data[self][0], 6, channel, 2, self, 2, Store.b_w[Store.box_data[self][0]], 2, Store.conveyor_width[channel]);
        //   Print_Channel(channel, f);
        //   event_id++;
          
        //   Store.box_data[self][1] = 0;
        //   Send_Event(0, TAKE_OUT, lp, &(lp->gid));
        //   break;
        // }
      case REVERSE:
        for (int i = 0; i < 6; ++i) {
          fprintf(temp_txt, "%d %d||| ", Store.robots[i].cur_cell.id, Store.robots[i].has_box);
        }
        fprintf(temp_txt, "\n");
        fprintf(temp_txt, "\n");

        Store.cells[Store.robots[self - 1].cur_cell.id].reserved = 0;
        if (Store.robots[self - 1].has_box == -1 && Store.robots[self - 1].cur_cell.id == Store.robots[self - 1].goal_cell.id) {
          Store.robots[self - 1].goal_time = 8;
          if (Store.robots[self - 1].cur_time == 1) {
            fprintf(f, "%*d %*d %*d     startMotion       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[Store.robots[self - 1].cur_cell.id + 1], 4, 0, 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
            event_id += 1;
          }
          if (Store.robots[self - 1].cur_time >= Store.robots[self - 1].goal_time) {
            Store.robots[self - 1].cur_time = 0;
            Store.robots[self - 1].low_SKU = Store.conveyor[Store.robots[self - 1].col].boxes[7].SKU;

            Remove_Boxes(&(Store.db), Store.robots[self - 1].low_SKU, &(glb_time), &(event_id), self);
            Store.robots[self - 1].cur_box = Store.robots[self - 1].low_SKU;
            fprintf(f, "%*d %*d %*d     movebox2bot       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[Store.robots[self - 1].cur_cell.id + 1], 4, Store.robots[self - 1].low_SKU, 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
            event_id += 1;

            Store.robots[self - 1].has_box = 1;
            Store.robots[self - 1].goal_cell.id = MAX_RACKS + 1 + (int)(Store.robots[self - 1].col / 10);

            if (Store.cells[Store.robots[self - 1].cur_cell.id + 1].reserved == 0) {
              Store.robots[self - 1].pre_reserved = -1;
              Store.cells[Store.robots[self - 1].cur_cell.id + 1].reserved = 1;
            } else {
              Store.robots[self - 1].pre_reserved = 1;
            }
            
            fprintf(f, "%*d %*d %*d    finishMotion       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[Store.robots[self - 1].cur_cell.id + 1], 4, Store.robots[self - 1].low_SKU, 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
            Store.robots[self - 1].cur_cell.id += 1;
            event_id += 1;
          }

        } else if (Store.robots[self - 1].has_box == 1 && Store.robots[self - 1].cur_cell.id == Store.robots[self - 1].goal_cell.id) {
          Store.robots[self - 1].goal_time = 8;
          if (Store.robots[self - 1].cur_time == 1) {
            fprintf(f, "%*d %*d %*d     startMotion       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[Store.robots[self - 1].cur_cell.id + 1], 4, Store.robots[self - 1].low_SKU, 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
            event_id += 1;
          }
          if (Store.robots[self - 1].cur_time >= Store.robots[self - 1].goal_time) {
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
            event_id += 1;
            //Print_Channel(Store.robots[self - 1].col, f);
            Store.robots[self - 1].low_SKU = -1;

            Store.robots[self - 1].row = save + 1;

            Store.robots[self - 1].has_box = -1;

            Store.robots[self - 1].goal_cell.id = MAX_RACKS * 3 + 1 - (int)(Store.robots[self - 1].col / 10);

            if (Store.cells[Store.robots[self - 1].cur_cell.id + 1].reserved == 0) {
              Store.robots[self - 1].pre_reserved = -1;
              Store.cells[Store.robots[self - 1].cur_cell.id + 1].reserved = 1;
            } else {
              Store.robots[self - 1].pre_reserved = 1;
            }
            fprintf(f, "%*d %*d %*d    finishMotion       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[Store.robots[self - 1].cur_cell.id + 1], 4, 0, 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
            Store.robots[self - 1].cur_cell.id += 1;
            event_id += 1;
          }
        } else {
          if (Store.robots[self - 1].cur_cell.id == MAX_RACKS * 2 + 1) {
            Store.robots[self - 1].goal_time = 7;
          } else if (Store.robots[self - 1].cur_cell.id == MAX_RACKS * 4 + 3) {
            Store.robots[self - 1].goal_time = 3;
          } else if (Store.robots[self - 1].cur_cell.id == MAX_RACKS) {
            Store.robots[self - 1].goal_time = 3;
          } else if (Store.robots[self - 1].cur_cell.id == MAX_RACKS * 3 + 2) {
            Store.robots[self - 1].goal_time = 7;
          } else {
            Store.robots[self - 1].goal_time = 6;
          }

          int future_id = 0;
          if (Store.robots[self - 1].cur_cell.id == MAX_RACKS * 3 + 2) {
            future_id = Store.store_graph[MAX_RACKS * 3 + 2][1];
          } else if (Store.robots[self - 1].cur_cell.id == MAX_RACKS * 2 + 1) {
            future_id = Store.store_graph[MAX_RACKS * 2 + 1][0];
          } else {
            future_id = Store.store_graph[Store.robots[self - 1].cur_cell.id][0];
          }

          if (Store.robots[self - 1].cur_time == 1) {
            if (Store.robots[self - 1].has_box == 1) {
              fprintf(f, "%*d %*d %*d     startMotion       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[future_id], 4, Store.robots[self - 1].low_SKU, 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
              event_id += 1;
            } else {
              fprintf(f, "%*d %*d %*d     startMotion       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[future_id], 4, 0, 4,Store.robots[self - 1].col % 10 + 1, 2, 0);
              event_id += 1;
            }
          }

          if (Store.robots[self - 1].cur_time >= Store.robots[self - 1].goal_time) {
            Store.robots[self - 1].cur_time = 0;
            if (Store.robots[self - 1].has_box == 1) {
              fprintf(f, "%*d %*d %*d    finishMotion       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[future_id], 4, Store.robots[self - 1].low_SKU, 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
              event_id += 1;
            } else {
              fprintf(f, "%*d %*d %*d    finishMotion       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[future_id], 4, 0, 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
              event_id += 1;
            }
            if (Store.robots[self - 1].cur_cell.id == MAX_RACKS * 3 + 2) {
              Store.robots[self - 1].cur_cell.id = Store.store_graph[MAX_RACKS * 3 + 2][1];

              if (Store.cells[Store.robots[self - 1].cur_cell.id].reserved == 0) {
                Store.robots[self - 1].pre_reserved = -1;
                Store.cells[Store.robots[self - 1].cur_cell.id].reserved = 1;
              } else {
                Store.robots[self - 1].pre_reserved = 1;
              }

            } else if (Store.robots[self - 1].cur_cell.id == MAX_RACKS * 2 + 1) {
              Store.robots[self - 1].cur_cell.id = Store.store_graph[MAX_RACKS * 2 + 1][0];
              
              if (Store.cells[Store.robots[self - 1].cur_cell.id].reserved == 0) {
                Store.cells[Store.robots[self - 1].cur_cell.id].reserved = 1;
                Store.robots[self - 1].pre_reserved = -1;
              } else {
                Store.robots[self - 1].pre_reserved = 1;
              }

            } else {
              Store.robots[self - 1].cur_cell.id = Store.store_graph[Store.robots[self - 1].cur_cell.id][0];

              if (Store.cells[Store.robots[self - 1].cur_cell.id].reserved == 0) {
                Store.cells[Store.robots[self - 1].cur_cell.id].reserved = 1;
                Store.robots[self - 1].pre_reserved = -1;
              } else {
                Store.robots[self - 1].pre_reserved = 1;
              }
            }
          }
        }
        
        Send_Event(0, REVERSE, lp, &(lp->gid));
        break;
      case GO:
        if (cur_boxes >= 400) {
          break;
        }
        Store.cells[Store.robots[self - 1].cur_cell.id].reserved = 0;
        if (Store.robots[self - 1].cur_cell.id == MAX_RACKS * 2 + 1) {
          Store.robots[self - 1].goal_time = 7;
        } else if (Store.robots[self - 1].cur_cell.id == MAX_RACKS * 4 + 3) {
          Store.robots[self - 1].goal_time = 3;
        } else if (Store.robots[self - 1].cur_cell.id == MAX_RACKS) {
          Store.robots[self - 1].goal_time = 3;
        } else if (Store.robots[self - 1].cur_cell.id == MAX_RACKS * 3 + 2) {
          Store.robots[self - 1].goal_time = 7;
        } else {
          Store.robots[self - 1].goal_time = 6;
        }

        int future_id = 0;
        if (Store.robots[self - 1].cur_cell.id == MAX_RACKS * 3 + 2) {
          future_id = Store.store_graph[MAX_RACKS * 3 + 2][1];
        } else if (Store.robots[self - 1].cur_cell.id == MAX_RACKS * 2 + 1) {
          future_id = Store.store_graph[MAX_RACKS * 2 + 1][0];
        } else {
          future_id = Store.store_graph[Store.robots[self - 1].cur_cell.id][0];
        }

        if (Store.robots[self - 1].cur_time == 1) {
          if (Store.robots[self - 1].has_box == 1) {
            fprintf(f, "%*d %*d %*d     startMotion       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[future_id], 4, Store.robots[self - 1].low_SKU, 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
            event_id += 1;
          } else {
            fprintf(f, "%*d %*d %*d     startMotion       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[future_id], 4, 0, 4,Store.robots[self - 1].col % 10 + 1, 2, 0);
            event_id += 1;
          }
        }

        if (Store.robots[self - 1].cur_time >= Store.robots[self - 1].goal_time) {
          Store.robots[self - 1].cur_time = 0;
          if (Store.robots[self - 1].has_box == 1) {
            fprintf(f, "%*d %*d %*d    finishMotion       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[future_id], 4, Store.robots[self - 1].low_SKU, 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
            event_id += 1;
          } else {
            fprintf(f, "%*d %*d %*d    finishMotion       %*s     %*s     %*d    %*d   %*d\n", 6, event_id, 6, glb_time, 4, self, 4, Store.vertexes[Store.robots[self - 1].cur_cell.id], 4, Store.vertexes[future_id], 4, 0, 4, Store.robots[self - 1].col % 10 + 1, 2, 0);
            event_id += 1;
          }
          if (Store.robots[self - 1].cur_cell.id == MAX_RACKS * 3 + 2) {
            Store.robots[self - 1].cur_cell.id = Store.store_graph[MAX_RACKS * 3 + 2][1];

            if (Store.cells[Store.robots[self - 1].cur_cell.id].reserved == 0) {
              Store.robots[self - 1].pre_reserved = -1;
              Store.cells[Store.robots[self - 1].cur_cell.id].reserved = 1;
            } else {
              Store.robots[self - 1].pre_reserved = 1;
            }

          } else if (Store.robots[self - 1].cur_cell.id == MAX_RACKS * 2 + 1) {
            Store.robots[self - 1].cur_cell.id = Store.store_graph[MAX_RACKS * 2 + 1][0];
            
            if (Store.cells[Store.robots[self - 1].cur_cell.id].reserved == 0) {
              Store.cells[Store.robots[self - 1].cur_cell.id].reserved = 1;
              Store.robots[self - 1].pre_reserved = -1;
            } else {
              Store.robots[self - 1].pre_reserved = 1;
            }

          } else {
            Store.robots[self - 1].cur_cell.id = Store.store_graph[Store.robots[self - 1].cur_cell.id][0];

            if (Store.cells[Store.robots[self - 1].cur_cell.id].reserved == 0) {
              Store.cells[Store.robots[self - 1].cur_cell.id].reserved = 1;
              Store.robots[self - 1].pre_reserved = -1;
            } else {
              Store.robots[self - 1].pre_reserved = 1;
            }
          }
        }
        Send_Event(0, TAKE_OUT, lp, &(lp->gid));
        break;
      default:
        printf("\n%s\n", "No message");
        break;
    }
    Store.arr_time[self] = glb_time;
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
    fprintf(f, "%*d %*d      finishPalletize %s", 6, event_id, 6, glb_time, Store.cur_order);
  }
  return;
}
