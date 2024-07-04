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
#include "commands.c"
#include <time.h>
#include "extras.c"

int glb_time = 0;

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
  bool flag = false;
  if (self == 0) {
    // printf("\n%s\n", "brain");
    for (int i = 0; i < MAX_CONVEYORS / 10; ++i) {
      if (Store.cnt_boxes_type[i] < 30) {
        flag = true;
      }
    }
    if (flag) {
      tw_event *e1 = tw_event_new(1, 1, lp);
      message *msg1 = tw_event_data(e1);
      msg1->type = TAKE_IN;
      msg1->contents = tw_rand_unif(lp->rng);
      msg1->sender = self;
      tw_event_send(e1);
      for (int process = 2; process < 10; ++process) {
        Check(process);
        tw_event *e = tw_event_new(process, 1, lp);
        message *msg = tw_event_data(e);
        msg->type = TAKE_OUT;
        msg->contents = tw_rand_unif(lp->rng);
        msg->sender = self;
        tw_event_send(e);
      }
    } else {
      for (int process = 1; process < 10; ++process) {
        Check(process);
        tw_event *e = tw_event_new(process, 1, lp);
        message *msg = tw_event_data(e);
        msg->type = TAKE_OUT;
        msg->contents = tw_rand_unif(lp->rng);
        msg->sender = self;
        tw_event_send(e);
      }
    }
  } else {
    switch (in_msg->type)
    {
      case TAKE_IN:
        for (int i = 0; i < MAX_CONVEYORS / 10; ++i) {
          //printf("%d\n", Store.cnt_boxes_type[0]);
          if (Store.cnt_boxes_type[i] < 30) {
            if (i < 5900) {
              printf("%d %d\n", i, Store.cnt_boxes_type[i]);
            }
            
            Add_Boxes(i);
          }
        }
        tw_event *e1 = tw_event_new(0, 1, lp);
        message *msg1 = tw_event_data(e1);
        msg1->type = TAKE_OUT;
        msg1->contents = tw_rand_unif(lp->rng);
        msg1->sender = self;
        tw_event_send(e1);
        break;
      case TAKE_OUT:
        Remove_Boxes(Store.box_data[self - 1][0], Store.box_data[self - 1][1]);
        tw_event *e = tw_event_new(0, 1, lp);
        message *msg = tw_event_data(e);
        msg->type = TAKE_IN;
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
