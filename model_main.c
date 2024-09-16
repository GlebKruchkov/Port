//The C main file for a ROSS model
//This file includes:
// - definition of the LP types
// - command line argument setup
// - a main function

//includes
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
  bots_starting_positions = fopen("/Users/glebkruckov/Documents/Работа/Port/port-model/bots_starting_positions.csv", "r");

  f = fopen("/Users/glebkruckov/Documents/Работа/Port/port-model/full_actions_log.txt", "w");
  paleta = fopen("/Users/glebkruckov/Documents/Работа/Port/port-model/paletize_depaletize.txt", "w");
  file = fopen("/Users/glebkruckov/Documents/Работа/Port/port-model/TEST1-SIMSIM/small_test.csv", "r");
  f_dep = fopen("/Users/glebkruckov/Documents/Работа/Port/port-model/first_depalitization.txt", "w");
  temp_txt = fopen("/Users/glebkruckov/Documents/Работа/Port/port-model/temp_txt.txt", "w");
  csv_file = fopen("/Users/glebkruckov/Documents/Работа/Port/port-model/final_warehouse.csv", "w");
  test = fopen("/Users/glebkruckov/Documents/Работа/Port/port-model/test.txt", "w");
  robots_positions = fopen("/Users/glebkruckov/Documents/Работа/Port/port-model/robots_positions.csv", "w");
  control_system_log = fopen("/Users/glebkruckov/Documents/Работа/Port/port-model/control_system_log.txt", "w");
  const char *directory_path = "/Users/glebkruckov/Documents/Работа/Port/port-model/TEST3-SIMSIM";
  struct dirent *entry;
  DIR *dp = opendir(directory_path);


  while ((entry = readdir(dp))) {
    if (strstr(entry->d_name, ".csv") != NULL) {
      snprintf(Store.files[Store.cur_file], sizeof(Store.files[Store.cur_file]), "%s/%s", directory_path, entry->d_name);
      Store.cur_file += 1;
    }
  }
  Store.cur_file = 0;

  fprintf(paleta, " RecId   Time             Action              PalletID PalletTypeID\n");

  sqlite3_open("/Users/glebkruckov/Documents/Работа/Port/port-model/ross-sqlite.db", &Store.db);
  fprintf(f, "EventID  Time BotID        Command StartPoint EndPoint  BoxType Channel TrID\n");
  fprintf(control_system_log, "EventID  Time BotID        Command StartPoint EndPoint  BoxType Channel TrID\n");

  // f_dep = fopen("/home/sasha/Port/first_depalitization.txt", "w");
  // sqlite3_open("/home/sasha/Port/ross-sqlite.db", &Store.db);

  // f = fopen("/home/sasha/Port/log.txt", "w");
  // file = fopen("/home/sasha/Port/TEST1-SIMSIM/small_test.csv", "r");


	InitROSS();
	int i, num_lps_per_pe;
  tw_opt_add(model_opts);
  tw_init(&argc, &argv);
  num_lps_per_pe = MAX_ROBOTS + 1;
  // g_tw_events_per_pe = 100000;
  tw_define_lps(num_lps_per_pe, sizeof(message));
  // displayModelSettings();
  g_tw_lp_typemap = &model_map;
  for (int i = 0; i < g_tw_nlp; ++i)
    tw_lp_settype(i, &model_lps[0]);

	// displayModelSettings();

	// Do some file I/O here? on a per-node (not per-LP) basis

	tw_run();

	tw_end();
  fprintf(f, "------------------------------------------------------------------------------------\n");
  fprintf(f, "------------------------------------------------------------------------------------\n");
  fprintf(f, "------------------------------------------------------------------------------------\n");

  // for (int i = 0; i < MAX_VERTEXES; ++i) {
  //   fprintf(test, "CELL %s. QUEUE", Store.vertexes[i]);
  //   for (int j = 0; j < MAX_ROBOTS; ++j) {
  //     fprintf(test, "%d ", Store.cells[i].queue[j]);
  //   }
  //   fprintf(test, "\n");
  // }

	return 0;
}
