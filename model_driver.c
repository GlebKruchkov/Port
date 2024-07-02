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

//Helper Functions
void SWAP (double *a, double *b) {
  double tmp = *a;
  *a = *b;
  *b = tmp;
}


//Init function
// - called once for each LP
// ! LP can only send messages to itself during init !
void model_init (state *s, tw_lp *lp) {
  int self = lp->gid;

  // init state data
  s->rcvd_count_H = 0;
  s->rcvd_count_G = 0;
  s->value = -1;

  // Init message to myself
  tw_event *e = tw_event_new(self, 1, lp);
  message *msg = tw_event_data(e);
  msg->type = REVERSE;
  msg->contents = tw_rand_unif(lp->rng);
  msg->sender = self;
  tw_event_send(e);

  // int num_lps_per_pe = 8; //n robots + command center
  // tw_define_lps(num_lps_per_pe, sizeof(message));
  // for (int i = 0; i < g_tw_nlp; ++i)
  //   tw_lp_settype(i, &model_lps[0]);

  if (self == 0) {
    s->type = COMMAND_CENTER;
    printf("%s\n", "COMMAND_CENTER is initialized");
  } else {
    s->type = CONVEYOR;
    printf("%s ", "CONVEYOR");
    printf("%d ", self);
    printf("%s\n", " is initialized");
  }

}

//Forward event handler
void model_event (state *s, tw_bf *bf, message *in_msg, tw_lp *lp) {
  int self = lp->gid;

  // initialize the bit field
  *(int *) bf = (int) 0;

  // update the current state
  // however, save the old value in the 'reverse' message
  SWAP(&(s->value), &(in_msg->contents));

  // handle the message
  switch (in_msg->type) {
    case TAKE_IN :
    {
      s->rcvd_count_H++;
      break;
    }
    case REVERSE :
    {
      s->rcvd_count_G++;
      break;
    }
    default :
      printf("Unhandeled forward message type %d\n", in_msg->type);
  }

  tw_event *e = tw_event_new(self, 1, lp);
  message *msg = tw_event_data(e);
  //# randomly choose message type
  double random = tw_rand_unif(lp->rng);
  if (random < 0.5) {
    msg->type = REVERSE;
  } else {
    msg->type = REVERSE;
  }
  msg->contents = tw_rand_unif(lp->rng);
  msg->sender = self;
  tw_event_send(e);
}

//Reverse Event Handler
void model_event_reverse (state *s, tw_bf *bf, message *in_msg, tw_lp *lp) {
  int self = lp->gid;

  // undo the state update using the value stored in the 'reverse' message
  SWAP(&(s->value), &(in_msg->contents));

  // handle the message
  switch (in_msg->type) {
    case TAKE_IN :
    {
      s->rcvd_count_H--;
      break;
    }
    case REVERSE :
    {
      s->rcvd_count_G--;
      break;
    }
    default :
      printf("Unhandeled reverse message type %d\n", in_msg->type);
  }

  // don't forget to undo all rng calls
  tw_rand_reverse_unif(lp->rng);
  tw_rand_reverse_unif(lp->rng);
}

//report any final statistics for this LP
void model_final (state *s, tw_lp *lp){
  int self = lp->gid;
  printf("%d handled %d REVERSE and %d REVERSE messages\n", self, s->rcvd_count_H, s->rcvd_count_G);
}
