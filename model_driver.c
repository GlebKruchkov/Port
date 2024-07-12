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
  bool flag = 0;
  if (self == 0) {
    for (int i = 0; i < high_border - low_border; ++i) {
      if (Store.cnt_boxes_type[i] < (int)(Store.cnt_boxes_type_const[i] * 2 / 3)) {
        flag = true;
      }
    }
    if (flag) {
      tw_event *e1 = tw_event_new(1, 0, lp);
      message *msg1 = tw_event_data(e1);
      msg1->type = TAKE_IN;
      msg1->contents = tw_rand_unif(lp->rng);
      msg1->sender = self;
      tw_event_send(e1);
      for (int process = 2; process < 10; ++process) {
        if (Store.box_data[process][1] == 0) {
          if (Check(process)) {
            tw_event *e = tw_event_new(process, 0, lp);
            message *msg = tw_event_data(e);
            msg->type = TAKE_OUT;
            msg->contents = tw_rand_unif(lp->rng);
            msg->sender = self;
            tw_event_send(e);
          };
        }
      }
    } else {
      for (int process = 1; process < 10; ++process) {
        if (Store.box_data[process][1] == 0) {
          if (Check(process)) {
            tw_event *e = tw_event_new(process, 0, lp);
            message *msg = tw_event_data(e);
            msg->type = TAKE_OUT;
            msg->contents = tw_rand_unif(lp->rng);
            msg->sender = self;
            tw_event_send(e);
          };
        }
      }
    }
  }
}

void model_event (state *s, tw_bf *bf, message *in_msg, tw_lp *lp) {
  // exit(0);
  int self = lp->gid;
  *(int *) bf = (int) 0;
  SWAP(&(s->value), &(in_msg->contents));
  bool flag = false;
  struct timeval currentTime;
  int cur_time = glb_time;
  //printf("%d\n", self);
  if (self == 0) {
    for (int i = 0; i < high_border - low_border; ++i) {
      if (Store.cnt_boxes_type[i] < (int)(Store.cnt_boxes_type_const[i] * 2 / 3)) {
        flag = true;
      }
    }
    if (flag) {
      Send_Event(TAKE_IN, lp, &(lp->gid));
      bool flag1 = 0;
      for (int process = 2; process < 10; ++process) {
        if (Store.box_data[process][1] != 0) {
          flag1 = 1;
        }
      }
      if (!flag1) {
        for (int i = 0; i < 10; ++i) {
          if (Store.arr_time[i] > glb_time) {
            glb_time = Store.arr_time[i];
          }
        }
        for (int process = 2; process < 10; ++process) {
          if (Store.box_data[process][1] != 0) {
            tw_event *e = tw_event_new(process, 0, lp);
            message *msg = tw_event_data(e);
            msg->type = TAKE_OUT;
            msg->contents = tw_rand_unif(lp->rng);
            msg->sender = self;
            tw_event_send(e);
          } else {
            if (Check(process)) {
              tw_event *e = tw_event_new(process, 0, lp);
              message *msg = tw_event_data(e);
              msg->type = TAKE_OUT;
              msg->contents = tw_rand_unif(lp->rng);
              msg->sender = self;
              tw_event_send(e);
            };
          }
        }
      }
    } else {
      bool flag1 = 0;
      for (int process = 2; process < 10; ++process) {
        if (Store.box_data[process][1] != 0) {
          flag1 = 1;
        }
      }
      if (!flag1) {
        for (int i = 0; i < 10; ++i) {
          if (Store.arr_time[i] > glb_time) {
            glb_time = Store.arr_time[i];
          }
        }
        for (int process = 1; process < 10; ++process) {
          if (Store.box_data[process][1] != 0) {
            tw_event *e = tw_event_new(process, 0, lp);
            message *msg = tw_event_data(e);
            msg->type = TAKE_OUT;
            msg->contents = tw_rand_unif(lp->rng);
            msg->sender = self;
            tw_event_send(e);
          } else {
            if (Check(process)) {
              tw_event *e = tw_event_new(process, 0, lp);
              message *msg = tw_event_data(e);
              msg->type = TAKE_OUT;
              msg->contents = tw_rand_unif(lp->rng);
              msg->sender = self;
              tw_event_send(e);
            };
          }
        }
      }
    }
  } else {
    //printf("%d\n", self);
    switch (in_msg->type)
    {
      case TAKE_IN: 
        for (int i = 0; i < high_border - low_border + 1; ++i) {
          if (Store.cnt_boxes_type[i] < (int)(Store.cnt_boxes_type_const[i] * 2 / 3)) {
            fprintf(f, "------------------------------------------------------------------------------------\n");
            fprintf(f, "startDepalletize\n");
            while (Store.cnt_boxes_type[i] < Store.cnt_boxes_type_const[i]) {
              int channel = Add_Box(&(Store.db), i);
              cur_time += 8;
              // fprintf(f, "movebox%dchannel%d %d %d\n", i, channel, cur_time, self);
              fprintf(f, "%*d   %*d   moveinbox%*d   channel%*d   process%*d   boxwidth%*d    channelwidth%*d   ", 4, log_id, 4, cur_time, 5, i, 6, channel, 2, self, 2, Store.b_w[i], 2, Store.conveyor_width[channel]);
              for (int i = 0; i < 8; ++i) {
                if (Store.conveyor[channel].boxes[i].SKU == -1) {
                    fprintf(f, "| - ");
                } else {
                    fprintf(f, "|%*d", 3, Store.conveyor[channel].boxes[i].SKU);
                }
              }
              fprintf(f, "|\n");
              log_id++;
            }
            fprintf(f, "finishDepalletize\n");
            fprintf(f, "------------------------------------------------------------------------------------\n");
          }
        }

        tw_event *e1 = tw_event_new(0, 0, lp);
        message *msg1 = tw_event_data(e1);
        msg1->type = TAKE_OUT;
        msg1->contents = tw_rand_unif(lp->rng);
        msg1->sender = self;
        tw_event_send(e1);
        break;
      case TAKE_OUT:
        if (Store.box_data[self][0] > high_border || Store.box_data[self][0] < low_border) {
            fprintf(f, "no-boxes-for-SKU %d\n", Store.box_data[self][0]);
            Store.box_data[self][1] = 0;
            tw_event *e = tw_event_new(0, 0, lp);
            message *msg = tw_event_data(e);
            msg->type = TAKE_OUT;
            msg->contents = tw_rand_unif(lp->rng);
            msg->sender = self;
            tw_event_send(e);
            break;
        } else {
          for (int q = 0; q < Store.box_data[self][1]; ++q) {
            int channel = Remove_Boxes(&(Store.db), Store.box_data[self][0]);
            cur_time += 8;
            fprintf(f, "%*d   %*d   moveoutbox%*d   channel%*d   process%*d    boxwidth%*d    channelwidth%*d   ", 4, log_id, 4, cur_time, 5, Store.box_data[self][0], 6, channel, 2, self, 2, Store.b_w[Store.box_data[self][0]], 2, Store.conveyor_width[channel]);
            // if (is_reverse != 0) {
            //   fprintf(f, "reverse box%*d  ", 4, best_box.row);
            // }
            // printf("%d\n", is_reverse);
            for (int i = 0; i < 8; ++i) {
              if (Store.conveyor[channel].boxes[i].SKU == -1) {
                  fprintf(f, "| - ");
              } else {
                  fprintf(f, "|%*d", 3, Store.conveyor[channel].boxes[i].SKU);
              }
            }
            fprintf(f, "|\n");
            log_id++;
          }
          Store.box_data[self][1] = 0;
          tw_event *e = tw_event_new(0, 0, lp);
          message *msg = tw_event_data(e);
          msg->type = TAKE_OUT;
          msg->contents = tw_rand_unif(lp->rng);
          msg->sender = self;
          tw_event_send(e);
          break;
        }
      default:
        printf("\n%s\n", "No message");
        break;
    }
    Store.arr_time[self] = cur_time;
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
