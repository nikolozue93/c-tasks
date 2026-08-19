#include <stdio.h>
#include <stdlib.h> // getenv()
#include <unistd.h> // access(), X_OK
#include <string.h>

/**
 * searches directories in PATH for an executable command
 * on success it returns the corresponding path as a string
 * or NULL if not found or any other error occured
 */
char *find_in_path(const char *command, const char *path){ 
    char *path_clone = strdup(path); // as strtok modifies the string, for safety im duplicating PATH
    if(path_clone == NULL){
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }
    char *dir = strtok(path_clone, ":");
    char *result = NULL;

    while(dir != NULL){
        size_t total_len = strlen(dir) + 1 + strlen(command) + 1; // +1 for '/' and +1 for null terminator

        char *combined = malloc(total_len);
        if(combined == NULL){
            printf("Memory allocation failed\n");
            break;
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

/**
 * processes and prints command paths
 * handles both explicit (with '/') and PATH lookups
 * 
 * returns 0 on success, 1 on failure
 */
int process_command(const char *command, const char *path){
    if(strchr(command, '/') != NULL){
        if(access(command, X_OK) == 0){
            printf("%s\n", command);
            return 0; 
        } else {
            fprintf(stderr, "%s not found\n", command);
            return 1;
        }
    }

    char *found_path = find_in_path(command, path);
    if(found_path != NULL){
        printf("%s\n", found_path);
        free(found_path);
        return 0;
    } else {
        fprintf(stderr, "%s not found\n", command);
        return 1;
    }
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
        if(process_command(argv[i], path) != 0){
            has_error = 1;
        }
    }

    if(has_error) 
        return 1;

    return 0;
}