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
   // cleanup array of strings
   int i = 0;
    while(currStruct->args[i] != 0)
    {
        free(currStruct->args[i]);
        i++;
    }
    free(currStruct->args);
    free(currStruct->inputFile);
    currStruct->inputFile = 0;
    free(currStruct->outputFile);
    currStruct->outputFile = 0;
    currStruct->backgroundBool = 0;
}

/* checks if argument contains $$ process id variable and expands to include PID */
char *pidVarExpansion(char *token)
{
    // no expansion variable found
    if (strstr(token, "$$") == NULL)
    {
        return token;
    }

    // grab pid of current shell process and convert to string
    pid_t pid = getpid();
    char pids[50];
    sprintf(pids, "%d", pid);
    printf("pid: %s\n", pids);

    // setup expToken temp token and expansion pointer
    char *expansionPtr = strstr(token, "$$");
    char *tempToken = calloc(strlen(token) +  1, sizeof(char));
    strcpy(tempToken, token);
    char *expToken = calloc(strlen(token) + strlen(pids) +  1, sizeof(char));

    // expand token string and replace $$ with pid for every instance of $$
    while (expansionPtr != NULL)
    {
        // add character of token up to start of $$ to new token string
        strncpy(expToken, tempToken, (int)(expansionPtr - tempToken));

        // concatenate pid
        strcat(expToken, pids);

        // concatenate rest of token string starting after $$ characters
        printf("%s", expToken + 2);
        fflush(stdout);
        strcat(expToken, tempToken + 2);

        // check for another expansion variable and expand token
        if ((expansionPtr = strstr(expToken, "$$")) != NULL)
        {
            tempToken = realloc(expToken, strlen(expToken));

            expToken = calloc(strlen(expToken) + strlen(pids) + 1, sizeof(char));
        }
    }
    free(tempToken);
    printf("expanded: %s\n", expToken);
    return expToken;
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

    // track length of args array and current
    int argsSize = 4;
    int argsNum = 0;

    // initialize array of argument pointers and current argument string
    char **argsArray = calloc(argsSize, sizeof(char*));
    char *currArg;
    
    // reentry pointer for strtok_r
    char *saveptr;

    // dynamic memory allocation for PID variable token expansion
    // int memAllocBool = 0;

    // start from first argument
    char *token = strtok_r(input, " ", &saveptr);


    // Parse arguments
    while (token != NULL && strcmp(token, "<") != 0 && strcmp(token, ">") != 0 && strcmp(token, "&") != 0)
    {
        // create argument string
        token = pidVarExpansion(token);
        currArg = calloc(strlen(token) + 1, sizeof(char));
        strcpy(currArg, token);

        // save current argument string to array of arguments
        argsArray[argsNum++] = currArg;

        // check if array size needs to be increased
        if (argsNum == argsSize)
        {
            argsSize *= 2;
            argsArray = realloc(argsArray, argsSize * sizeof(char*));
        }

        // iterate to next argument
        token = strtok_r(NULL, " ", &saveptr);
    }

    // set struct args pointer to array
    currCommand->args = argsArray;

    // Parse input redirect filename
    if (token != NULL && strcmp(token, "<") == 0)
    {
        // skip input character
        token = strtok_r(NULL, " ", &saveptr);
        currCommand->inputFile = calloc(strlen(token) + 1, sizeof(char));
        strcpy(currCommand->inputFile, token);

        token = strtok_r(NULL, " ", &saveptr);
    }

    // Parse output redirect filename
    if (token != NULL && strcmp(token, ">") == 0)
    {
        // skip input character
        token = strtok_r(NULL, " ", &saveptr);
        currCommand->outputFile = calloc(strlen(token) + 1, sizeof(char));
        strcpy(currCommand->outputFile, token);
        token = strtok_r(NULL, " ", &saveptr);
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
        // handle comment line
        if (strncmp(currCommand->args[0], "#", 1) == 0)
        {
            printf("Comment\n");
        }
        
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

        // check for and perform variable expansion

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