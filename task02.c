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

    char *path_clone = strdup(path); 
    // maybe i need to check memory allocation for clone?
    char *dir = strtok(path_clone, ":");

    for(int i = 1; i < argc; i++){
        while(dir != NULL){
            char *command = argv[i];

            size_t total_len = sizeof(dir) + sizeof(command) + 1;
            char *combined = malloc(total_len);

            if(combined == NULL){
                printf("Memory allocation failed\n");
                return 1;
            }
            
            snprintf(combined, total_len, "%s%s", dir, command);

            // should i directly terminate as soon as i find non executable file
            if(access(combined, X_OK) != 0){ 
                fprintf(stderr, "%s not found\n", command);
                exit(1);
            }
            printf("%s\n", combined);

            dir = strtok(NULL, ":");
        }

        free(dir);
    }


    return 0;
}