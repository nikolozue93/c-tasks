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

pid_t my_fork(){ // maybe need to change to pid_t instead of int
    const pid_t pid = fork();

    if(pid == -1){
        perror("fork failed");
        exit(1);
    }
    return pid;
}

int main(){
    FILE *fptr;
    char **args[10] = {}; // to store commands line by line
    char buffer[SIZE]; 

    fptr = fopen("commands.txt", "r");

    if(fptr == NULL){
        fprintf(stderr, "Opening commands.txt failed\n");
        return 1;
    }

    //char **p = args;
    int i = 0; // counting arguments
    while(fgets(buffer, sizeof(buffer), fptr) != NULL){
        char *buffer_clone = strdup(buffer);
        if(buffer_clone == NULL){
            fprintf(stderr, "Memory allocation failed\n");
        }

        args[i] = split(buffer_clone, " \n");
        i++;
    }

    // for(int c = 0; c < i; c++){
    //     for(int w = 0; args[c][w] != NULL; w++){
    //         printf("%s ", args[c][w]);
    //     }
    //     printf("\n");
    // }

    //pid_t pid = fork();
    // if(pid == 0){
    //     execvp(args[0][0], args[0]);
    //     perror("exec failed"); 
    //     exit(1);
    // } else if(pid > 1){
    //     wait(NULL);
    // } 


    // int fd[2];
    // pipe(fd);
    // pid_t pid0 = fork();
    // if(pid0 == 0){
    //     dup2(fd[1], 1);
    //     close(fd[0]);
    //     close(fd[1]);
    //     execvp(args[0][0], args[0]);
    // }

    // pid_t pid1 = fork();
    // if(pid1 == 0){
    //     dup2(fd[0], 0);
    //     close(fd[0]);
    //     close(fd[1]);
    //     execvp(args[1][0], args[1]);
    // } 

    // close(fd[0]); close(fd[1]);
    // wait(NULL); wait(NULL);


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
            
            fprintf(stderr, "Command \"");
            for (int w = 0; args[c][w] != NULL; w++) {
                fprintf(stderr, "%s", args[c][w]);

                if (args[c][w + 1] != NULL) {
                    fprintf(stderr, " ");
                }
            }
            fprintf(stderr, "\" failed\n");
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

    return 0;
}