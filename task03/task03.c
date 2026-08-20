#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define SIZE 100

/**
 * splitting each line of file with a delimeter, either by space or a newline
 * returning pointer to the array of strings (in this case commands)
 */
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

/**
 * reading and parsing file containting commands line by line
 * duplicates each line into memory (due to strtok) and splits using char **split function
 */
int parse_commands(FILE *fptr, char **args[], char *raw_lines[]){
    int i = 0; // counting arguments
    char buffer[SIZE];

    while(i < SIZE && fgets(buffer, sizeof(buffer), fptr) != NULL){
        char *buffer_clone = strdup(buffer);
        if(buffer_clone == NULL){
            fprintf(stderr, "Memory allocation failed\n");
            return i;
        }

        raw_lines[i] = buffer_clone; // saving original pointer for freeing 
        args[i] = split(buffer_clone, " \n");
        i++;
    }

    return i;
}

/**
 * executing sequence of commands chained together with pipes
 * routing std input/output with file descriptors
 * so the output of the command N becomes the input of the command N + 1 
 */
void process_pipeline(char **args[], int i){
    int fd[2]; // file descriptor, fd[0] read end, fd[1] write end

    // tracks the read end of the pipe from the previous command
    // starts at 0 (stdin) so the first command reads from the terminal/file
    int in_fd = 0;
    for(int c = 0; c < i; c++){
        if(c < i - 1) // creating new pipe for all commands except the last
            pipe(fd);

        pid_t pid = my_fork();
        if(pid == 0){
            // dealing with the input
            if(in_fd){
                dup2(in_fd, 0); 
                close(in_fd);
            }

            // dealing with the output
            if(c < i - 1){ // if theres a next command, pipe stdout 
                dup2(fd[1], 1);
                close(fd[1]);
                close(fd[0]); // child not reading from its own output pipe
            }
            execvp(args[c][0], args[c]);

            print_command_error(args[c]);
            exit(1);

        } else if(pid > 0){
            if(c < i - 1)
                close(fd[1]); // close write end of the new pipe

            if(in_fd != 0){
                close(in_fd); // close the read end of the old pipe that current child just consumed
            }
        }

        // save the read end of the current pipe for the next iteration
        if(c < i - 1)
            in_fd = fd[0];
    }

    for(int j = 0; j < i; j++)
        wait(NULL);
}

// deallocates memory used for command tokens and buffers
void free_commands(char **args[], char *raw_lines[], int count) {
    for (int j = 0; j < count; j++) {
        free(raw_lines[j]); // buffer
        free(args[j]);      // token pointer array
    }
}


int main(){
    char **args[SIZE] = {}; // to store commands line by line
    char *raw_lines[SIZE] = {};
    FILE *fptr = fopen("commands.txt", "r");

    if(fptr == NULL){
        fprintf(stderr, "Opening commands.txt failed\n");
        return 1;
    }

    int i = parse_commands(fptr, args, raw_lines); // final count of arguments
    fclose(fptr);

    if(i > 0){
        process_pipeline(args, i);
    }

    free_commands(args, raw_lines, i);

    return 0;
}