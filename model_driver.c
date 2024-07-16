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
  for (int process = 4; process < 7; ++process) {
    if (Store.box_data[process][1] != 0) {
      Store.used[process] = 1;
      Send_Event(process, TAKE_OUT, lp, &(lp->gid));
    } else {
      if (Check(process)) {
        Store.used[process] = 1;
        Send_Event(process, TAKE_OUT, lp, &(lp->gid));
      };
    }
  }
}

void model_event (state *s, tw_bf *bf, message *in_msg, tw_lp *lp) {
  for (int i = 0; i < 21; ++i) {
    fprintf(temp_txt, "%d ", Store.cnt_boxes_type[i]);
  }
  fprintf(temp_txt, "\n");
  // exit(0);
  int self = lp->gid;
  *(int *) bf = (int) 0;
  SWAP(&(s->value), &(in_msg->contents));
  bool flag = false;
  struct timeval currentTime;
  int cur_time = glb_time;

  if (self == 0) {
    int not_do = 0;
    for (int i = 1; i < 7; ++i) {
      if (Store.used[i] == 1) {
        not_do = 1;
      }
    }
    if (not_do == 0) {
      if (Store.boxes_to_deliver <= 0) {
        for (int i = 0; i < high_border - low_border + 1; ++i) {
          if (Store.cnt_boxes_type[i] < 15) {

            fprintf(f, "------------------------------------------------------------------------------------\n");
            fprintf(f, "startDepalletize\n");
            Store.boxes_to_deliver = threshold - Store.cnt_boxes_type[i];
            Store.used[1] = 1;
            Store.used[2] = 1;
            Store.used[3] = 1;
            Send_Event(1, TAKE_IN, lp, &(lp->gid));
            Send_Event(2, TAKE_IN, lp, &(lp->gid));
            Send_Event(3, TAKE_IN, lp, &(lp->gid));
            Store.type_to_add = i;
            break;
          }
        }
      } else {
          Store.used[1] = 1;
          Store.used[2] = 1;
          Store.used[3] = 1;
          Send_Event(1, TAKE_IN, lp, &(lp->gid));
          Send_Event(2, TAKE_IN, lp, &(lp->gid));
          Send_Event(3, TAKE_IN, lp, &(lp->gid));
      }

      for (int process = 4; process < 7; ++process) {
        if (Check(process)) {
          find_data(&(Store.db), Store.box_data[process][0]);
          if (best_box.row != 7) {
            is_reverse = 1;
            Store.used[process] = 1;
            Send_Event(process, REVERSE, lp, &(lp->gid));
          } else {
            is_reverse = 0;
            Store.used[process] = 1;
            Send_Event(process, TAKE_OUT, lp, &(lp->gid));
          }
        };
      }
    }
  } else {
    Store.used[self] = 0;
    //printf("%d\n", self);
    switch (in_msg->type)
    {
      case TAKE_IN:
        printf("");
        Store.boxes_to_deliver -= 1;
        int channel = Add_Box(&(Store.db), Store.type_to_add);
        cur_time += 8;
        fprintf(f, "%*d   %*d   moveinbox%*d   channel%*d   process%*d   boxwidth%*d    channelwidth%*d   ", 4, log_id, 4, cur_time, 5, Store.type_to_add, 6, channel, 2, self, 2, Store.b_w[Store.type_to_add], 2, Store.conveyor_width[channel]);
        Print_Channel(channel, f);
        log_id++;
        

        Send_Event(0, TAKE_OUT, lp, &(lp->gid));
        break;
      case TAKE_OUT:

        if (Store.box_data[self][0] > high_border || Store.box_data[self][0] < low_border) {
            fprintf(f, "no-boxes-for-SKU %d\n", Store.box_data[self][0]);
            Store.box_data[self][1] = 0;
            Send_Event(0, TAKE_OUT, lp, &(lp->gid));
            break;
        } else {
          find_data(&(Store.db), Store.box_data[self][0]);
          int channel = Remove_Boxes(&(Store.db), Store.box_data[self][0], &(cur_time), &(log_id));
          cur_time += 8;
          fprintf(f, "%*d   %*d   moveoutbox%*d   channel%*d   process%*d    boxwidth%*d    channelwidth%*d   ", 4, log_id, 4, cur_time, 5, Store.box_data[self][0], 6, channel, 2, self, 2, Store.b_w[Store.box_data[self][0]], 2, Store.conveyor_width[channel]);
          Print_Channel(channel, f);
          log_id++;
          
          Store.box_data[self][1] = 0;
          Send_Event(0, TAKE_OUT, lp, &(lp->gid));
          break;
        }
      case REVERSE:
          find_data(&(Store.db), Store.box_data[self][0]);
          Reverse(&(Store.db), best_box.column, best_box.row, &(cur_time), &(log_id));
          Send_Event(0, REVERSE, lp, &(lp->gid));
          break;

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
