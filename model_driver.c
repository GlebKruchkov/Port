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
  fprintf(f, "%*d   moverobot   id %*d   from %*s   to %*s\n", 4, *time, 2, robot_id, 2, Store.vertexes[id_from], 2, Store.vertexes[id_to]);
}

void model_event (state *s, tw_bf *bf, message *in_msg, tw_lp *lp) {
  // for (int i = 0; i < 21; ++i) {
  //   fprintf(temp_txt, "%d ", Store.cnt_boxes_type[i]);
  // }
  // fprintf(temp_txt, "\n");
  // exit(0);
  int self = lp->gid;
  *(int *) bf = (int) 0;
  SWAP(&(s->value), &(in_msg->contents));
  bool flag = false;
  struct timeval currentTime;

  if (self == 0 && Store.kill_prog == false) {
    // for (int i = 0; i < 21; ++i) {
    //   printf("%d ", Store.cnt_boxes_type[i]);
    // }
    // printf("\n");
    int not_do = 0;
    for (int i = 1; i < 7; ++i) {
      if (Store.used[i] == 1) {
        not_do = 1;
      }
    }
    if (not_do == 0) {
      glb_time += 2;
      if (Store.boxes_to_deliver <= 0) {
        for (int i = 0; i < high_border - low_border + 1; ++i) {
          if (Store.cnt_boxes_type[i] < 15) {
            Store.boxes_to_deliver = threshold - Store.cnt_boxes_type[i];
            //printf("%d\n", Store.boxes_to_deliver);
            Store.used[1] = 1;
            Store.used[2] = 1;
            Store.used[3] = 1;
            Send_Event(1, TAKE_IN, lp, &(lp->gid));
            Send_Event(2, TAKE_IN, lp, &(lp->gid));
            Send_Event(3, TAKE_IN, lp, &(lp->gid));
            Store.type_to_add = i;
            fprintf(f, "------------------------------------------------------------------------------------\n");
            fprintf(f, "startDepalletize type %d\n", Store.type_to_add);
            break;
          }
        }
      } else {
          
          Store.used[1] = 1;
          Send_Event(1, TAKE_IN, lp, &(lp->gid));

          Store.used[2] = 1;
          Send_Event(2, TAKE_IN, lp, &(lp->gid));

          Store.used[3] = 1;
          Send_Event(3, TAKE_IN, lp, &(lp->gid));
      }

      if (Store.boxes_to_deliver <= 0) {
        Store.used[1] = 1;
        Send_Event(1, GO, lp, &(lp->gid));

        Store.used[2] = 1;
        Send_Event(2, GO, lp, &(lp->gid));

        Store.used[3] = 1;
        Send_Event(3, GO, lp, &(lp->gid));
      }

      for (int process = 4; process < 7; ++process) {
        if (Store.robots[process - 1].col != -1 && Store.robots[process - 1].row != -1) {
          if (Store.robots[process - 1].row != 7) {
            Store.used[process] = 1;
            Send_Event(process, REVERSE, lp, &(lp->gid));
          } else {
            
            Store.used[process] = 1;
            Send_Event(process, TAKE_OUT, lp, &(lp->gid));
          }
        } else {
          if (Check(process)) {
            find_data(&(Store.db), Store.box_data[process][0]);
            Store.robots[process - 1].reserved_channel = best_box.column;

            Store.robots[process - 1].col = best_box.column;
            Store.robots[process - 1].row = best_box.row;

            if (Store.robots[process - 1].row != 7) {
              Store.used[process] = 1;
              Send_Event(process, REVERSE, lp, &(lp->gid));
            } else {
              
              Store.used[process] = 1;
              Send_Event(process, TAKE_OUT, lp, &(lp->gid));
            }
          } else {
            Store.kill_prog = true;
          }
        }
      }
    }
  } else if (Store.kill_prog == false) {
    Store.used[self] = 0;
    //printf("%d\n", self);
    switch (in_msg->type)
    {
      case TAKE_IN:
        // printf("");
        for (int i = 0; i < 6; ++i) {
          fprintf(temp_txt, "bot%d cell%d ||| ", i, Store.robots[i].cur_cell.id);
        }
        fprintf(temp_txt, "\n");
        fprintf(temp_txt, "\n");

        if (Store.robots[self - 1].col == -1 && Store.robots[self - 1].cur_cell.id == 10 && Store.cells[11].reserved == 0) {
          find_data_by_width(&(Store.db), Store.type_to_add);

          Store.robots[self - 1].reserved_channel = -1;

          print_motion(Store.robots[self - 1].cur_cell.id, 11, self, &(glb_time));
          fprintf(f, "%*d   movebox2bot   SKU%*d   BotId%*d\n", 3, glb_time, 2, Store.type_to_add, 2, self);

          log_id++;

          Store.robots[self - 1].reserved_channel = best_box.column;
          Store.robots[self - 1].col = best_box.column;
          Store.robots[self - 1].row = best_box.row;
          Store.robots[self - 1].cur_cell.id = 11;
          Store.cells[11].reserved = 1;
          Store.cells[10].reserved = 0;
        } else {
          if (Store.robots[self - 1].cur_cell.id == 11) {
            if (Store.cells[6].reserved == 0) {
              print_motion(Store.robots[self - 1].cur_cell.id, 6, self, &(glb_time));
              Store.cells[11].reserved = 0;
              Store.robots[self - 1].cur_cell.id = 6;
              Store.cells[6].reserved = 1;
            }
          } else {
            if (Store.cells[Store.robots[self - 1].cur_cell.id + 1].reserved == 0) {
              print_motion(Store.robots[self - 1].cur_cell.id, Store.robots[self - 1].cur_cell.id + 1, self, &(glb_time));
              if (Store.robots[self - 1].col != -1 && Store.robots[self - 1].cur_cell.id == 6 && Store.robots[self - 1].col < 50) {
                int channel = Add_Box(&(Store.db), Store.type_to_add, self);
                Store.boxes_to_deliver -= 1;
                Store.robots[self - 1].col = -1;
                Store.robots[self - 1].reserved_channel = -1;
                fprintf(f, "%*d   movebox2channel   SKU%*d   BotId%*d   Channel%*d   ", 4, glb_time, 2, Store.type_to_add, 2, self, 3, channel);
                Print_Channel(channel, f);
                log_id++;
              } else if (Store.robots[self - 1].col != -1 && Store.robots[self - 1].cur_cell.id == 7 && Store.robots[self - 1].col >= 50) {
                int channel = Add_Box(&(Store.db), Store.type_to_add, self);
                Store.boxes_to_deliver -= 1;
                Store.robots[self - 1].col = -1;
                Store.robots[self - 1].reserved_channel = -1;
                fprintf(f, "%*d   movebox2channel   SKU%*d   BotId%*d   Channel%*d   ", 4, glb_time, 2, Store.type_to_add, 2, self, 3, channel);
                Print_Channel(channel, f);
                log_id++;
              }
              Store.cells[Store.robots[self - 1].cur_cell.id].reserved = 0;
              Store.robots[self - 1].cur_cell.id += 1;
              Store.cells[Store.robots[self - 1].cur_cell.id].reserved = 1;
            }
          }
        }
        
        Send_Event(0, TAKE_OUT, lp, &(lp->gid));
        break;
      case TAKE_OUT:
        for (int i = 0; i < 6; ++i) {
          fprintf(temp_txt, "bot%d cell%d ||| ", i, Store.robots[i].cur_cell.id);
        }
        fprintf(temp_txt, "\n");
        fprintf(temp_txt, "\n");

        if (Store.robots[self - 1].cur_cell.id == 1 && Store.cells[0].reserved == 0) {
          print_motion(1, 0, self, &(glb_time));
          if (Store.robots[self - 1].col >= 50 && Store.robots[self - 1].col != -1) {
            int channel = Remove_Boxes(&(Store.db), Store.box_data[self][0], &(glb_time), &(log_id), self);
            Store.robots[self - 1].reserved_channel = -1;
            fprintf(f, "%*d   movebox2bot   SKU%*d   BotId%*d   Channel%*d   ", 4, glb_time, 2, Store.box_data[self][0], 2, self, 3, channel);
            Print_Channel(channel, f);
            log_id++;
            Store.box_data[self][1] = 0;
            Store.robots[self - 1].cur_conv = -1;
            Store.robots[self - 1].has_box = 1;
          }

          Store.cells[Store.robots[self - 1].cur_cell.id].reserved = 0;
          Store.robots[self - 1].cur_cell.id -= 1;
          Store.cells[Store.robots[self - 1].cur_cell.id].reserved = 1;
        

        } else if (Store.robots[self - 1].cur_cell.id == 0 && Store.cells[5].reserved == 0) {
          print_motion(0, 5, self, &(glb_time));
          if (Store.robots[self - 1].col < 50 && Store.robots[self - 1].col != -1) {
            int channel = Remove_Boxes(&(Store.db), Store.box_data[self][0], &(glb_time), &(log_id), self);
            Store.robots[self - 1].reserved_channel = -1;
            fprintf(f, "%*d   movebox2bot   SKU%*d   BotId%*d   Channel%*d   ", 4, glb_time, 2, Store.box_data[self][0], 2, self, 3, channel);
            Print_Channel(channel, f);
            log_id++;
            Store.box_data[self][1] = 0;
            Store.robots[self - 1].cur_conv = -1;
            Store.robots[self - 1].has_box = 1;
          }

          Store.cells[Store.robots[self - 1].cur_cell.id].reserved = 0;
          Store.robots[self - 1].cur_cell.id = 5;
          Store.cells[Store.robots[self - 1].cur_cell.id].reserved = 1;
        

        } else if (Store.robots[self - 1].cur_cell.id == 3 && Store.cells[2].reserved == 0) {
          print_motion(3, 2, self, &(glb_time));
          if (Store.robots[self - 1].has_box == 1) {
            fprintf(f, "%*d   movebox2tr   SKU%*d   BotId%*d\n", 4, glb_time, 2, Store.box_data[self][0], 2, self);
            Store.robots[self - 1].has_box = 0;
            Store.robots[self - 1].cur_conv = -1;
            Store.robots[self - 1].row = -1;
            Store.robots[self - 1].col = -1;
          }

          Store.cells[Store.robots[self - 1].cur_cell.id].reserved = 0;
          Store.robots[self - 1].cur_cell.id -= 1;
          Store.cells[Store.robots[self - 1].cur_cell.id].reserved = 1;
        
        } else if (Store.robots[self - 1].cur_cell.id == 7 && Store.cells[8].reserved == 0) {
          print_motion(7, 8, self, &(glb_time));
          Store.cells[Store.robots[self - 1].cur_cell.id].reserved = 0;
          Store.robots[self - 1].cur_cell.id = 8;
          Store.cells[Store.robots[self - 1].cur_cell.id].reserved = 1;

        } else if (Store.robots[self - 1].cur_cell.id == 8 && Store.cells[1].reserved == 0) {
          print_motion(8, 1, self, &(glb_time));
          Store.cells[Store.robots[self - 1].cur_cell.id].reserved = 0;
          Store.robots[self - 1].cur_cell.id = 1;
          Store.cells[Store.robots[self - 1].cur_cell.id].reserved = 1;

        } else if (Store.robots[self - 1].cur_cell.id != 8 && Store.robots[self - 1].cur_cell.id != 7) {
          if (Store.robots[self - 1].cur_cell.id == 0 && Store.cells[5].reserved == 0) {
            print_motion(0, 5, self, &(glb_time));
            Store.cells[Store.robots[self - 1].cur_cell.id].reserved = 0;
            Store.robots[self - 1].cur_cell.id = 5;
            Store.cells[Store.robots[self - 1].cur_cell.id].reserved = 1;

          } else if (Store.robots[self - 1].cur_cell.id != 0 && Store.cells[Store.robots[self - 1].cur_cell.id - 1].reserved == 0) {
            print_motion(Store.robots[self - 1].cur_cell.id, Store.robots[self - 1].cur_cell.id - 1, self, &(glb_time));
            Store.cells[Store.robots[self - 1].cur_cell.id].reserved = 0;
            Store.robots[self - 1].cur_cell.id -= 1;
            Store.cells[Store.robots[self - 1].cur_cell.id].reserved = 1;

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
        //   int channel = Remove_Boxes(&(Store.db), Store.box_data[self][0], &(cur_time), &(log_id));
        //   cur_time += 8;
        //   fprintf(f, "%*d   %*d   moveoutbox%*d   channel%*d   process%*d    boxwidth%*d    channelwidth%*d   ", 4, log_id, 4, cur_time, 5, Store.box_data[self][0], 6, channel, 2, self, 2, Store.b_w[Store.box_data[self][0]], 2, Store.conveyor_width[channel]);
        //   Print_Channel(channel, f);
        //   log_id++;
          
        //   Store.box_data[self][1] = 0;
        //   Send_Event(0, TAKE_OUT, lp, &(lp->gid));
        //   break;
        // }
      case REVERSE:
        for (int i = 0; i < 6; ++i) {
          fprintf(temp_txt, "bot%d cell%d ||| ", i, Store.robots[i].cur_cell.id);
        }
        fprintf(temp_txt, "\n");
        fprintf(temp_txt, "\n");

        if (Store.robots[self - 1].cur_cell.id == 1 && Store.cells[0].reserved == 0) {
          print_motion(1, 0, self, &(glb_time));
          if (Store.robots[self - 1].col >= 50 && Store.robots[self - 1].col != -1) {     
            fprintf(f, "%*d   movebox2bot   SKU%*d   BotId%*d   Channel%*d\n", 4, glb_time, 2, Store.conveyor[Store.robots[self - 1].col].boxes[7].SKU, 2, self, 3, Store.robots[self - 1].col);
            Store.robots[self - 1].cur_conv = -1;
            Store.robots[self - 1].has_box = 1;
          }
          Store.cells[Store.robots[self - 1].cur_cell.id].reserved = 0;
          Store.robots[self - 1].cur_cell.id = 0;
          Store.cells[Store.robots[self - 1].cur_cell.id].reserved = 1;
        } else if (Store.robots[self - 1].cur_cell.id == 0 && Store.cells[5].reserved == 0) {
          print_motion(0, 5, self, &(glb_time));
          if (Store.robots[self - 1].col < 50 && Store.robots[self - 1].col != -1) {
            fprintf(f, "%*d   movebox2bot   SKU%*d   BotId%*d   Channel%*d\n", 4, glb_time, 2, Store.conveyor[Store.robots[self - 1].col].boxes[7].SKU, 2, self, 3, Store.robots[self - 1].col);
            Store.robots[self - 1].cur_conv = -1;
            Store.robots[self - 1].has_box = 1;
          }

          Store.cells[Store.robots[self - 1].cur_cell.id].reserved = 0;
          Store.robots[self - 1].cur_cell.id = 5;
          Store.cells[Store.robots[self - 1].cur_cell.id].reserved = 1;

        } else if (Store.robots[self - 1].cur_cell.id == 6 && Store.cells[7].reserved == 0) {
          print_motion(6, 7, self, &(glb_time));
          if (Store.robots[self - 1].col < 50 && Store.robots[self - 1].col != -1) {
            fprintf(f, "%*d   movebox2channel   SKU%*d   BotId%*d   Channel%*d   ", 4, glb_time, 2, Store.conveyor[Store.robots[self - 1].col].boxes[7].SKU, 2, self, 3, Store.robots[self - 1].col);
            Reverse(&(Store.db), Store.robots[self - 1].col, Store.robots[self - 1].row, &(glb_time), &(log_id), self);
            Print_Channel(Store.robots[self - 1].col, f);
            Store.robots[self - 1].reserved_channel = -1;
            Store.robots[self - 1].cur_conv = -1;
            Store.robots[self - 1].has_box = 0;

            Store.robots[self - 1].row = -1;
            Store.robots[self - 1].col = -1;
          }

          // Выгрузка в канал
          Store.cells[Store.robots[self - 1].cur_cell.id].reserved = 0;
          Store.robots[self - 1].cur_cell.id = 7;
          Store.cells[Store.robots[self - 1].cur_cell.id].reserved = 1;

        }  else if (Store.robots[self - 1].cur_cell.id == 7 && Store.cells[8].reserved == 0) {
          print_motion(7, 8, self, &(glb_time));
          if (Store.robots[self - 1].col >= 50 && Store.robots[self - 1].col != -1) {
            fprintf(f, "%*d   movebox2channel   SKU%*d   BotId%*d   Channel%*d   ", 4, glb_time, 2, Store.conveyor[Store.robots[self - 1].col].boxes[7].SKU, 2, self, 3, Store.robots[self - 1].col);
            Reverse(&(Store.db), Store.robots[self - 1].col, Store.robots[self - 1].row, &(glb_time), &(log_id), self);
            Print_Channel(Store.robots[self - 1].col, f);
            Store.robots[self - 1].reserved_channel = -1;
            Store.robots[self - 1].cur_conv = -1;
            Store.robots[self - 1].has_box = 0;

            Store.robots[self - 1].row = -1;
            Store.robots[self - 1].col = -1;
          }

          // Выгрузка в канал
      
          Store.cells[Store.robots[self - 1].cur_cell.id].reserved = 0;
          Store.robots[self - 1].cur_cell.id = 8;
          Store.cells[Store.robots[self - 1].cur_cell.id].reserved = 1;
        
          
        } else if (Store.robots[self - 1].cur_cell.id == 8 && Store.cells[1].reserved == 0) {
          print_motion(8, 1, self, &(glb_time));
          Store.cells[Store.robots[self - 1].cur_cell.id].reserved = 0;
          Store.robots[self - 1].cur_cell.id = 1;
          Store.cells[Store.robots[self - 1].cur_cell.id].reserved = 1;
          
        } else if (Store.robots[self - 1].cur_cell.id == 4 && Store.cells[3].reserved == 0) {
          print_motion(4, 3, self, &(glb_time));
          Store.cells[Store.robots[self - 1].cur_cell.id].reserved = 0;
          Store.robots[self - 1].cur_cell.id = 3;
          Store.cells[Store.robots[self - 1].cur_cell.id].reserved = 1;

        } else if (Store.robots[self - 1].cur_cell.id == 3 && Store.cells[2].reserved == 0) {
          print_motion(3, 2, self, &(glb_time));
          Store.cells[Store.robots[self - 1].cur_cell.id].reserved = 0;
          Store.robots[self - 1].cur_cell.id = 2;
          Store.cells[Store.robots[self - 1].cur_cell.id].reserved = 1;

        } else if (Store.robots[self - 1].cur_cell.id == 2 && Store.cells[1].reserved == 0) {
          print_motion(2, 1, self, &(glb_time));
          Store.cells[Store.robots[self - 1].cur_cell.id].reserved = 0;
          Store.robots[self - 1].cur_cell.id = 1;
          Store.cells[Store.robots[self - 1].cur_cell.id].reserved = 1;

        } else if (Store.robots[self - 1].cur_cell.id == 5 && Store.cells[6].reserved == 0) {
          print_motion(5, 6, self, &(glb_time));
          Store.cells[Store.robots[self - 1].cur_cell.id].reserved = 0;
          Store.robots[self - 1].cur_cell.id = 6;
          Store.cells[Store.robots[self - 1].cur_cell.id].reserved = 1;

        }
        Send_Event(0, REVERSE, lp, &(lp->gid));
        break;
      case GO:
        for (int i = 0; i < 6; ++i) {
          fprintf(temp_txt, "bot%d cell%d ||| ", i, Store.robots[i].cur_cell.id);
        }
        fprintf(temp_txt, "\n");
        fprintf(temp_txt, "\n");
        if (Store.robots[self - 1].cur_cell.id != 11) {
          print_motion(Store.robots[self - 1].cur_cell.id, Store.robots[self - 1].cur_cell.id + 1, self, &(glb_time));
          if (Store.cells[Store.robots[self - 1].cur_cell.id + 1].reserved == 0) {
            Store.cells[Store.robots[self - 1].cur_cell.id].reserved = 0;
            Store.robots[self - 1].cur_cell.id += 1;
            Store.cells[Store.robots[self - 1].cur_cell.id].reserved = 1;
          }
        }
        Send_Event(0, REVERSE, lp, &(lp->gid));
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
void model_final (state *s, tw_lp *lp){
  return;
}
