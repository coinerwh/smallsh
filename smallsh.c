#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "command_struct.h"

/*
    main smallsh.c driver function and user input functions
*/


/* parses user input and stores elements in struct */
struct userCommand* parseUserInput(char* input)
{
    // handle blank line
    if (strlen(input) == 0)
    {
        return NULL;
    }
    // allocate space for parsed user command struct
    struct userCommand *currCommand = malloc(sizeof(struct userCommand));
    
    // reentry pointer for strtok_r
    char *saveptr;

    // First token is command
    char *token = strtok_r(input, " ", &saveptr);
    currCommand->command = calloc(strlen(token) + 1, sizeof(char));
    strcpy(currCommand->command, token);
    printf("struct: %s\n", currCommand->command);

    printf("token: %s\n", token);


    // FIND WAY TO CHECK IF MORE ARGUMENTS THEN CHECK WHAT KIND TOMORROW
    // Parse arguments
    if(token != NULL && strcmp(token, "<") != 0 && strcmp(token, ">") != 0 && strcmp(token, "&") != 0)
    {
        token = strtok_r(input, " ", &saveptr);
        currCommand->args = calloc(strlen(token) + 1, sizeof(char));
        strcpy(currCommand->args, token);
        printf("struct: %s\n", currCommand->args);
    }

    // Parse input redirect filename
    if (token != NULL && strcmp(token, "<") == 0)
    {
        // skip input character
        token = strtok_r(input, " ", &saveptr);
        token = strtok_r(input, " ", &saveptr);
        currCommand->inputFile = calloc(strlen(token) + 1, sizeof(char));
        strcpy(currCommand->inputFile, token);
        printf("struct: %s\n", currCommand->inputFile);
    }

    // Parse output redirect filename
    if (token != NULL && strcmp(token, ">") ==0)
    {
        // skip input character
        token = strtok_r(input, " ", &saveptr);
        token = strtok_r(input, " ", &saveptr);
        currCommand->outputFile = calloc(strlen(token) + 1, sizeof(char));
        strcpy(currCommand->outputFile, token);
        printf("struct: %s\n", currCommand->outputFile);
    }

    // Check background argument and set
    if (token != NULL && strcmp(token, "&") == 0)
    {
        currCommand->backgroundBool = 1;
    }
    else
    {
        currCommand->backgroundBool = 0;
    }
    printf("Boo: %d\n", currCommand->backgroundBool);

    return currCommand;
}

/* grabs user input from stdin */
char* getUserInput()
{
    char *input = calloc(512, sizeof(char));
    fgets(input, 512, stdin);

    // remove newline from string
    char *pos;
    if ((pos=strchr(input, '\n')) != NULL)
        *pos = '\0';

    // handle input overflowing buffer
    else
    {
        printf("Input buffer overflow \n");
        fflush(stdout);
    }
    return input;
}

/* main smallsh program driver */
void smallsh()
{
    //  User input setup variables
    char *userInput;

    while(1)
    {
        printf(": ");
        fflush(stdout);

        // get user command input
        userInput = getUserInput();
        // printf("%s\n", userInput);

        // parse input and create parsed command struct
        struct userCommand *currCommand = parseUserInput(userInput);

        // handle input
        // inputHandler(currCommand);


        // cleanup up the trash
        free(userInput);
        free(currCommand);
    }
}