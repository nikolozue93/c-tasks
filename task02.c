#include <stdio.h>
#include <stdlib.h> // getenv()
#include <unistd.h> // stat() and access()
#include <string.h>


int main(int argc, char *argv[]){
    if(argc < 2){
        fprintf(stderr, "Not enough arguments\n");
        return 1;
    }

    char *path = getenv("PATH");

    if(path == NULL){
        fprintf(stderr, "PATH variable not found\n");
        return 1;
    }

    int has_error = 0; // to track whether to return 1 or not

    for(int i = 1; i < argc; i++){
        int found = 0;
        char *command = argv[i];

        // potential improvement: extracting from full paths like /usr/bin/ls
        char *path_clone = strdup(path); 
        if(path_clone == NULL){
            fprintf(stderr, "Memory allocation failed\n");
            return 1;
        }
        char *dir = strtok(path_clone, ":");


        while(dir != NULL){
            // /bin + 1 for /
            size_t total_len = strlen(dir) + 1 + strlen(command) + 1;
            char *combined = malloc(total_len);

            if(combined == NULL){
                printf("Memory allocation failed\n");
                return 1;
            }
            
            snprintf(combined, total_len, "%s/%s", dir, command);

            if(access(combined, X_OK) == 0){
                printf("%s\n", combined);
                found = 1;
                free(combined);
                break;
            }
            free(combined);
            dir = strtok(NULL, ":");
        }
        free(path_clone);

        if(!found){
            fprintf(stderr, "%s not found\n", command);
            has_error = 1;
        }
    }

    if(has_error) 
        return 1;

    return 0;
}