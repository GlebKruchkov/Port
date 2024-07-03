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


//Init function
// - called once for each LP
// ! LP can only send messages to itself during init !
void model_init (state *s, tw_lp *lp) {
  int self = lp->gid;

  SendMessage(0, lp, 1, COMMAND_CENTER);

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
  bool flag = false;

  switch (s->type) {
    case COMMAND_CENTER:
      // printf("%ld ", lp->gid);
      // for (int cnt = 0; cnt < 1; ++cnt) {
      //   tw_event *e = tw_event_new(2, 1, lp);
      //   message *msg = tw_event_data(e);
      //   //# randomly choose message type
      //   double random = tw_rand_unif(lp->rng);
      //   msg->contents = tw_rand_unif(lp->rng);
      //   msg->sender = self;
      //   tw_event_send(e);
      // }

      // Check();
      // for (int i = 0; i < MAX_CONVEYORS / 10; ++i) {
      //     if (Store.cnt_boxes_type[i] < 99) {
      //       // for (int j = 1; j < 10; ++j) {
      //         // printf("\n\n%s\n\n", "TVAAAAAAAARIIIIII");
      //       flag = true;
      //       SendMessage(2, lp, glb_time, TAKE_IN);
      //       // }
      //     }
      // }
      for (int k = 0; k < 100000000; ++k) {

      }
      for (int j = 1; j < 10; ++j) {
        printf("\n%s\n", "comand center");
        SendMessage(j, lp, 1, CONVEYOR);
      }
      break;
    case CONVEYOR:
  
      // for (int cnt = 0; cnt < 1; ++cnt) {
      //   tw_event *e = tw_event_new(2, 1, lp);
      //   message *msg = tw_event_data(e);
      //   //# randomly choose message type
      //   double random = tw_rand_unif(lp->rng);
      //   msg->contents = tw_rand_unif(lp->rng);
      //   msg->sender = self;
      //   tw_event_send(e);
      // }



      switch (in_msg->type)
      {
        case TAKE_IN:
          printf("%s\n", "take in");
          for (int i = 0; i < MAX_CONVEYORS / 10; ++i) {
            if (Store.cnt_boxes_type[i] < 20) {
              Add_Boxes(i);
            }
          }
          break;

        case TAKE_OUT:
          printf("\n%s\n", "take out");
          break;

        default:
          printf("%d: SAM huita %d\n", self, in_msg->type);
          printf("\n\n%ld\n\n", lp->gid);
          assert(false);
      }

      for (int k = 0; k < 100000000; ++k) {

      }

      SendMessage(0, lp, 1, COMMAND_CENTER); // TODO 


      break;

      // printf("%d\n", lp->gid);
      // for (int j = 0; j < 100000000; ++j) {
      //   ++c;
      // }
      
      // printf("%s\n", "COMMAND_CENTER");
      // gettimeofday(&currentTime, NULL);

      // printf("%ld, %ld\n", currentTime.tv_sec, currentTime.tv_usec);

      // for (int i = 0; i < MAX_CONVEYORS / 10; ++i) {
      //   if (Store.cnt_boxes_type[i] < 20) {
      //     Add_Boxes(i);
      //   }
      // }
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
      // break;
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
