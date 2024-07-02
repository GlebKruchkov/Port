#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>


int main() {
    FILE *fp;
    int i = 0;
    DIR *dir;
    struct dirent *entry;
    dir = opendir("/Users/glebkruckov/Documents/Работа/Port/TEST1-SIMSIM");
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            char full_path[255];
            sprintf(full_path, "%s/%s", "/Users/glebkruckov/Documents/Работа/Port/TEST1-SIMSIM", entry->d_name);
            char line[1024];
            char *fields[10];
            FILE *file = fopen(full_path, "r");
            fgets(line, sizeof(line), file);

            while (fgets(line, sizeof(line), file)) {
                fields[0] = strtok(line, ",");
                for (i = 1; i < 10; i++) {
                    fields[i] = strtok(NULL, ",");
                }
                int SKU =  atoi(fields[0]);
                int quantity = atoi(fields[1]);
                int length = atoi(fields[2]);
                printf("%d ", SKU);
                printf("%d\n", quantity);
            }
            fclose(file);
        }
    }
    closedir(dir);
    return EXIT_SUCCESS;
}