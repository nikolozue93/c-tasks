#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    int i = 0;
    while(fgets(buffer, sizeof(buffer), fptr) != NULL){
        char *buffer_clone = strdup(buffer);
        if(buffer_clone == NULL){
            fprintf(stderr, "Memory allocation failed\n");
        }

        args[i] = split(buffer_clone, " \n");
        i++;
    }

    for(int c = 0; c < i; c++){
        for(int w = 0; args[c][w] != NULL; w++){
            printf("%s ", args[c][w]);
        }
        printf("\n");
    }

    return 0;
}