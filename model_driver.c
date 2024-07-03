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

void model_event (state *s, tw_bf *bf, message *in_msg, tw_lp *lp) {
  int self = lp->gid;
  *(int *) bf = (int) 0;
  SWAP(&(s->value), &(in_msg->contents));
  struct timeval currentTime;

  int c = -1000000;

  switch (s->type) {
    case COMMAND_CENTER:
      printf("%ld ", lp->gid);
      for (int cnt = 0; cnt < 1; ++cnt) {
        tw_event *e = tw_event_new(2, 1, lp);
        message *msg = tw_event_data(e);
        //# randomly choose message type
        double random = tw_rand_unif(lp->rng);
        msg->contents = tw_rand_unif(lp->rng);
        msg->sender = self;
        tw_event_send(e);
      }
      for (int j = 0; j < 1000000000; ++j) {
        ++c;
      }
      printf("%d\n", lp->gid);
      // printf("%d ", lp->gid);
      // // printf("%s\n", "COMMAND_CENTER");
      // gettimeofday(&currentTime, NULL);

      // printf("%ld, %ld\n", currentTime.tv_sec, currentTime.tv_usec);

      for (int i = 0; i < MAX_CONVEYORS / 10; ++i) {
        if (Store.cnt_boxes_type[i] < 20) {
          Add_Boxes(i);
        }
      }
      break;
    case CONVEYOR:
      printf("%d ", lp->gid);
      for (int cnt = 0; cnt < 1; ++cnt) {
        tw_event *e = tw_event_new(2, 1, lp);
        message *msg = tw_event_data(e);
        //# randomly choose message type
        double random = tw_rand_unif(lp->rng);
        msg->contents = tw_rand_unif(lp->rng);
        msg->sender = self;
        tw_event_send(e);
      }
      for (int j = 0; j < 1000000000; ++j) {
        ++c;
      }
      printf("%d\n", lp->gid);
      // for (int j = 0; j < 100000000; ++j) {
      //   ++c;
      // }
      
      // printf("%s\n", "COMMAND_CENTER");
      // gettimeofday(&currentTime, NULL);

      // printf("%ld, %ld\n", currentTime.tv_sec, currentTime.tv_usec);

      for (int i = 0; i < MAX_CONVEYORS / 10; ++i) {
        if (Store.cnt_boxes_type[i] < 20) {
          Add_Boxes(i);
        }
      }
      // struct timespec ts;
      // clock_gettime(CLOCK_REALTIME, &ts);
    
      // // Преобразуем время в макросекунды
      // long long microseconds = (long long)ts.tv_sec * 1000000 + (long long)ts.tv_nsec / 1000;
      
      // // Выводим время на консоль
      // printf("Текущее время в макросекундах: %lld\n", microseconds);


      // printf("%d\n", lp->gid);
      // struct timeval currentTime;
      // gettimeofday(&currentTime, NULL);
      // printf("%ld, %ld\n", currentTime.tv_sec, currentTime.tv_usec);
      // printf("%s\n", "CONVEYOR");
      break;
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
