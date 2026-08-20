#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define SIZE 100

char **split(char *s, char *delim) {
    char **buffer = malloc(SIZE * sizeof(char *));
    if (buffer == NULL) return NULL;

    // initializing all to NULL for execvp
    for (int i = 0; i < SIZE; i++) buffer[i] = NULL; 

    char *word = strtok(s, delim);
    for(int i = 0; word != NULL && i < SIZE; i++, word = strtok(NULL, delim)){
        buffer[i] = word;
    }

    return buffer;
}

pid_t my_fork(){ 
    const pid_t pid = fork();

    if(pid == -1){
        perror("fork failed");
        exit(1);
    }
    return pid;
}

void print_command_error(char **command){
    char **p = command;
    fprintf(stderr, "Command \"");
    while (*p != NULL){
        fprintf(stderr, "%s", *p);
        
        if(*(p + 1) != NULL) {
            fprintf(stderr, " ");
        }

        p++;
    }
    fprintf(stderr, "\" failed\n");
}

int parse_commands(FILE *fptr, char **args[]){
    int i = 0; // counting arguments
    char buffer[SIZE];

    while(fgets(buffer, sizeof(buffer), fptr) != NULL){
        char *buffer_clone = strdup(buffer);
        if(buffer_clone == NULL){
            fprintf(stderr, "Memory allocation failed\n");
        }

        args[i] = split(buffer_clone, " \n");
        i++;
    }

    return i;
}

void process_pipeline(char **args[], int i){
    int fd[2];
    int in_fd = 0;
    for(int c = 0; c < i; c++){
        if(c < i - 1)
            pipe(fd);

        pid_t pid = my_fork();
        if(pid == 0){
            if(in_fd){
                dup2(in_fd, 0); 
                close(in_fd);
            }

            if(c < i - 1){
                dup2(fd[1], 1);
                close(fd[1]);
                close(fd[0]);
            }
            execvp(args[c][0], args[c]);

            print_command_error(args[c]);
            exit(1);

        } else if(pid > 0){
            if(c < i - 1)
                close(fd[1]);

            if(in_fd != 0){
                close(in_fd);
            }
        }

        if(c < i - 1)
            in_fd = fd[0];
    }

    for(int j = 0; j < i; j++)
        wait(NULL);
}


int main(){
    char **args[10] = {}; // to store commands line by line
    FILE *fptr = fopen("commands.txt", "r");

    if(fptr == NULL){
        fprintf(stderr, "Opening commands.txt failed\n");
        return 1;
    }

    int i = parse_commands(fptr, args); // final count of arguments

    if(i > 0){
        process_pipeline(args, i);
    }

    return 0;
}