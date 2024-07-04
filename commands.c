#include "model.h" 
 
void Add_Boxes(int type) { 
 for (int i = type * 10; i <= type * 10 + 9; ++i) { 
  for (int j = 0; j < 8; ++j) { 
    //printf("%s\n", "SUUUUUKA"); 
   if (Store.conveyor[i].boxes[j].empty) { 
    //printf("%s\n", "SUUUUUKA");
    Store.conveyor[i].boxes[j].SKU = type; 
    Store.cnt_boxes_type[type]++; 
    Store.conveyor[i].boxes[j].empty = 0; 
   } 
  } 
 } 
} 
 
// void Remove_Boxes(int type, int cnt) { 
//  for (int j = 0; j < cnt; ++j) { 
//   if (!Store.conveyor[type * 10].boxes[j].empty) { 
//    Store.conveyor[type * 10].boxes[j].empty = 1; 
//    Store.cnt_boxes_type[type]--; 
//   } 
//  }
// }

void Remove_Boxes(int type, int cnt) { 
 //for (int j = 0; j < cnt; ++j) { 
 // if (!Store.conveyor[type * 10].boxes[j].empty) { 
 //  Store.conveyor[type * 10].boxes[j].empty = 1; 
 //  Store.cnt_boxes_type[type]--; 
 // } 
 //}
   int have_done = 0;
   bool break_flag = 0;
   for (int ind = type * 10; ind <= type * 10 + 9; ++ind) {
       for (int j = 0; j < 8; ++j) {
           if (!Store.conveyor[ind].boxes[7].empty) {
              have_done++;
              Store.cnt_boxes_type[type]--;
              for (int ch = 0; ch < 8; ++ch) {
                if (ch == 7) {
                    Store.conveyor[ind].boxes[7].empty = 1;
                    break;
                }
                if (!Store.conveyor[ind].boxes[ch].empty) {
                    Store.conveyor[ind].boxes[ch].empty = 1;
                    Store.conveyor[ind].boxes[7].empty = 0;
                    // printf("%s\n", "REMOVE BOX");
                    break;
                }

              }
              if (have_done == cnt) {
                break_flag = 1;
                break;
              }           
           } else {
            break;
           }
       }
       if (break_flag) {
            break;
       }
   }
}
 
 
void Check(int process) { 
    char line[1024];
    char *fields[10];

    if (fgets(line, sizeof(line), file)) {
        fields[0] = strtok(line, ",");
        for (int i = 1; i < 10; i++) {
            fields[i] = strtok(NULL, ",");
        }
        int SKU =  atoi(fields[0]);
        int quantity = atoi(fields[1]);
        int length = atoi(fields[2]);
        // printf("%d %d %d\n", SKU, quantity, process);
        Store.box_data[process - 1][0] = SKU;
        Store.box_data[process - 1][1] = quantity;
    }

}
