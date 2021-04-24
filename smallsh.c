#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "command_struct.h"

/*
    main smallsh.c driver and input handler functions
*/

// frees struct member memory
void cleanupStruct(struct userCommand* currStruct)
{
    free(currStruct->command);
    currStruct->command = 0;
    free(currStruct->args);
    currStruct->args = 0;
    free(currStruct->inputFile);
    currStruct->inputFile = 0;
    free(currStruct->outputFile);
    currStruct->outputFile = 0;
    currStruct->backgroundBool = 0;
}

/* parses user input and stores elements in struct */
struct userCommand* parseUserInput(char* input)
{
    printf("%s\n", input);
    // handle blank line
    if (strcmp(input, "\0") == 0)
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

    // //initialize args string array
    currCommand->args = calloc(strlen("\0") + 1, sizeof(char));
    strcpy(currCommand->args, "\0");

    // start from next argument
    token = strtok_r(NULL, " ", &saveptr);

    while (token != NULL)
    {
         // Parse arguments
        if(strcmp(token, "<") != 0 && strcmp(token, ">") != 0 && strcmp(token, "&") != 0)
        {
            // concatenate argument values to args string
            currCommand->args = realloc(currCommand->args, sizeof(currCommand->args) + sizeof(token) + 1);
            strcat(currCommand->args, token);
            strcat(currCommand->args, " ");
        }

        // Parse input redirect filename
        if (strcmp(token, "<") == 0)
        {
            // skip input character
            token = strtok_r(NULL, " ", &saveptr);
            currCommand->inputFile = calloc(strlen(token) + 1, sizeof(char));
            strcpy(currCommand->inputFile, token);
        }

        // Parse output redirect filename
        if (strcmp(token, ">") == 0)
        {
            // skip input character
            token = strtok_r(NULL, " ", &saveptr);
            currCommand->outputFile = calloc(strlen(token) + 1, sizeof(char));
            strcpy(currCommand->outputFile, token);
        }

        // Check background argument and set
        if (strcmp(token, "&") == 0)
        {
            currCommand->backgroundBool = 1;
        }
        else
        {
            currCommand->backgroundBool = 0;
        }

        // iterate to next argument
        token = strtok_r(NULL, " ", &saveptr);
    }
    return currCommand;
}

/* grabs user input from stdin */
char* getUserInput()
{
    char *input = calloc(2048, sizeof(char));
    fgets(input, 2048, stdin);

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

/* handles user input and runs correct command subroutine */
void commandHandler(struct userCommand* currCommand)
{
    // handle blank line
    if (currCommand != NULL)
    {
        printf("Command: %s, Arguments: %s, Input: %s, Output: %s, Background Bool: %d\n", currCommand->command, currCommand->args,
     currCommand->inputFile, currCommand->outputFile, currCommand->backgroundBool);
    }
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
        commandHandler(currCommand);

        // cleanup up the trash
        if (currCommand != NULL)
        {
            cleanupStruct(currCommand);
            free(currCommand);
        }
        free(userInput);
    }
}