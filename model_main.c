// aaaaaaaaaaa huilaaa aaa

//The C main file for a ROSS model
//This file includes:
// - definition of the LP types
// - command line argument setup
// - a main function

//includes
#include "ross.h"
#include "model.h"
#include "init.c"

// Define LP types
//   these are the functions called by ROSS for each LP
//   multiple sets can be defined (for multiple LP types)
tw_lptype model_lps[] = {
  {
    (init_f) model_init,
    (pre_run_f) NULL,
    (event_f) model_event,
    (revent_f) model_event_reverse,
    (commit_f) NULL,
    (final_f) model_final,
    (map_f) model_map,
    sizeof(state)
  },
  { 0 },
};

//Define command line arguments default values
unsigned int setting_1 = 0;

//add your command line opts
const tw_optdef model_opts[] = {
	TWOPT_GROUP("ROSS Model"),
	TWOPT_UINT("setting_1", setting_1, "first setting for this model"),
	TWOPT_END(),
};

void displayModelSettings()
{
  if (g_tw_mynode == 0)
  {
    for (int i = 0; i < 30; i++)
      printf("*");
    
    printf("\n");
    printf("Model Configuration:\n");
    printf("\t nnodes: %i\n", tw_nnodes());
    printf("\t g_tw_nlp: %llu\n", g_tw_nlp);

    for (int i = 0; i < 30; i++)
      printf("*");
    
    printf("\n");
  }
}

//for doxygen
#define model_main main

int model_main (int argc, char* argv[]) {
  file = fopen("/Users/glebkruckov/Documents/Работа/Port/port-model/TEST1-SIMSIM/order_2.csv", "r");
	InitROSS();
	int i, num_lps_per_pe;
    tw_opt_add(model_opts);
    tw_init(&argc, &argv);
    num_lps_per_pe = 10;
    tw_define_lps(num_lps_per_pe, sizeof(message));
  	// displayModelSettings();
    g_tw_lp_typemap = &model_map;
    for (int i = 0; i < g_tw_nlp; ++i)
    	tw_lp_settype(i, &model_lps[0]);
	
	// displayModelSettings();

	// Do some file I/O here? on a per-node (not per-LP) basis

	tw_run();

	tw_end();

	return 0;
}
