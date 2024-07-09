//The C driver file for a ROSS model
//This file includes:
// - an initialization function for each LP type
// - a forward event function for each LP type
// - a reverse event function for each LP type
// - a finalization function for each LP type

//Includes
#include <stdio.h>

#include "ross.h"
#include "model.h"
#include <stdio.h>
#include <time.h>
#include "extras.c"
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
  int glb_time = 0;
  *(int *) bf = (int) 0;
  SWAP(&(s->value), &(in_msg->contents));
  bool flag = false;
  struct timeval currentTime;
  if (self == 0) {
    // printf("%d\n", Store.cnt_boxes_type[5898]);
    // printf("\n%s\n", "brain");
    for (int i = 0; i < high_border - low_border; ++i) {
      if (Store.cnt_boxes_type[i] < 18) {
        //printf("%d\n", i);
        //printf("%d\n", Store.cnt_boxes_type[i]);
        // printf("%d %d %d \n", self, i, Store.cnt_boxes_type[i]);
        flag = true;
      }
    }
    if (flag) {
      tw_event *e1 = tw_event_new(1, glb_time, lp);
      message *msg1 = tw_event_data(e1);
      msg1->type = TAKE_IN;
      msg1->contents = tw_rand_unif(lp->rng);
      msg1->sender = self;
      tw_event_send(e1);
      for (int process = 2; process < 10; ++process) {
        if (Check(process)) {
          tw_event *e = tw_event_new(process, glb_time, lp);
          message *msg = tw_event_data(e);
          msg->type = TAKE_OUT;
          msg->contents = tw_rand_unif(lp->rng);
          msg->sender = self;
          tw_event_send(e);
        };
      }
    } else {
      for (int process = 1; process < 10; ++process) {
        if (Check(process)) {
          tw_event *e = tw_event_new(process, glb_time, lp);
          message *msg = tw_event_data(e);
          msg->type = TAKE_OUT;
          msg->contents = tw_rand_unif(lp->rng);
          msg->sender = self;
          tw_event_send(e);
        };
      }
    }
  } else {
    switch (in_msg->type)
    {
      case TAKE_IN:
        fprintf(f, "------------------------------------------\n");
        fprintf(f, "startDepalletize\n"); 
        // printf("%d\n", Store.cnt_boxes_type[1001]);
        for (int i = 0; i < high_border - low_border; ++i) {
          if (Store.cnt_boxes_type[i] < 18) {
            for (int q = 0; q < 20 - Store.cnt_boxes_type[i]; ++q) {
              int channel = Add_Box(i);
              glb_time += 8;
              fprintf(f, "movebox%dchannel%d %d\n", i, channel, glb_time);
            }
          }
        }
        fprintf(f, "finishDepalletize\n");
        fprintf(f, "------------------------------------------\n");

        glb_time += 1;
        tw_event *e1 = tw_event_new(0, glb_time, lp);
        message *msg1 = tw_event_data(e1);
        msg1->type = TAKE_OUT;
        msg1->contents = tw_rand_unif(lp->rng);
        msg1->sender = self;
        tw_event_send(e1);
        break;
      case TAKE_OUT:
        // printf("%d\n", Store.cnt_boxes_type[1001]);
        // printf("%s", "");
        // printf("%d\n", Store.cnt_boxes_type[1001]);
        for (int q = 0; q < Store.box_data[self - 1][1]; ++q) {
          Remove_Boxes(Store.box_data[self - 1][0]);
          fprintf(f, "take out box type %d\n", Store.box_data[self - 1][0]);
        }
        glb_time += 1;
        // printf("%d %d\n", Store.box_data[self - 1][0], Store.cnt_boxes_type[Store.box_data[self - 1][0]]);
        tw_event *e = tw_event_new(0, glb_time, lp);
        message *msg = tw_event_data(e);
        msg->type = TAKE_OUT;
        msg->contents = tw_rand_unif(lp->rng);
        msg->sender = self;
        tw_event_send(e);
        
        break;
      default:
        printf("\n%s\n", "micropenis");
        break;
    }
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
