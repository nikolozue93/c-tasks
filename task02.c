#include <stdio.h>
#include <stdlib.h> // getenv()
#include <unistd.h> // stat() and access()
#include <string.h>


char *find_in_path(const char *command, const char *path){ 
    char *path_clone = strdup(path);
    if(path_clone == NULL){
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }
    char *dir = strtok(path_clone, ":");
    char *result = NULL;

    while(dir != NULL){
        size_t total_len = strlen(dir) + 1 + strlen(command) + 1;

        char *combined = malloc(total_len);
        if(combined == NULL){
            printf("Memory allocation failed\n");
            return NULL;
        }

        snprintf(combined, total_len, "%s/%s", dir, command);

        if(access(combined, X_OK) == 0){
            result = combined; // saving here to free up the path_clone in the end and not directly returning
            break;
        }
        free(combined);

        dir = strtok(NULL, ":");
    }
    free(path_clone);

    return result;
}



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

        char *found_path = find_in_path(command, path);
        if(found_path != NULL){
            printf("%s\n", found_path);
            found = 1;
            free(found_path);
        }

        if(!found){
            fprintf(stderr, "%s not found\n", command);
            has_error = 1;
        }
    }

    if(has_error) 
        return 1;

    return 0;
}