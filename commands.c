#include "model.h" 
 
void Add_Boxes(int type) { 
 for (int i = type * 10; i <= type * 10 + 9; ++i) { 
  for (int j = 0; j < 8; ++j) { 
   // printf("%s\n", "SUUUUUKA"); 
   if (Store.conveyor[i].boxes[j].empty) { 
    Store.conveyor[i].boxes[j].SKU = type; 
    Store.cnt_boxes_type[type]++; 
    Store.conveyor[i].boxes[j].empty = 0; 
   } 
  } 
 } 
} 
 
void Remove_Boxes(int type, int cnt) { 
 for (int j = 0; j < cnt; ++j) { 
  if (!Store.conveyor[type * 10].boxes[j].empty) { 
   Store.conveyor[type * 10].boxes[j].empty = 1; 
   Store.cnt_boxes_type[type]--; 
  } 
 } 
} 
 
 
void Check() { 
	DIR *dir; //ИСПРАВИТЬ
	dir = opendir("/Users/glebkruckov/Documents/Работа/Port/port-model/TEST1-SIMSIM");
 	struct dirent *entry; 
    if ((entry = readdir(dir)) != NULL) { 
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) { 
            char full_path[255]; 
            sprintf(full_path, "%s/%s", "/Users/glebkruckov/Documents/Работа/Port/port-model/TEST1-SIMSIM", entry->d_name); 
            char line[1024]; 
            char *fields[10]; 
            FILE *file = fopen(full_path, "r"); 
            fgets(line, sizeof(line), file); 
 
            while (fgets(line, sizeof(line), file)) { 
                fields[0] = strtok(line, ","); 
                for (int i = 1; i < 10; i++) { 
                    fields[i] = strtok(NULL, ","); 
                } 
                int SKU =  atoi(fields[0]); 
                int quantity = atoi(fields[1]); 
                int length = atoi(fields[2]); 
    			Remove_Boxes(SKU, quantity); 
            } 
            fclose(file); 
        } 
    } 
    closedir(dir); 
}
