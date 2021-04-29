#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "command_struct.h"
#include "commands.h"
#include "childpid_functions.h"
#include "signal_handlers.h"

/*
    main smallsh.c driver and input handler functions
*/

// foreground global switch initialized
int foregroundOnly = 0;

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
    if (currStruct->inputFile != NULL)
        free(currStruct->inputFile);
    if (currStruct->outputFile != NULL)
        free(currStruct->outputFile);
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

    // setup expToken temp token and expansion pointer
    char *tempToken = calloc(strlen(token) +  1, sizeof(char));
    strcpy(tempToken, token);
    char *expToken = calloc(strlen(token) + strlen(pids) +  1, sizeof(char));
    char *expansionPtr = strstr(tempToken, "$$");

    // expand token string and replace $$ with pid for every instance of $$
    while (expansionPtr != NULL)
    {
        // add character of token up to start of $$ to new token string
        strncat(expToken, tempToken, (int)(expansionPtr - tempToken));

        // concatenate pid
        strncat(expToken, pids, strlen(pids));

        // concatenate rest of token string starting after $$ characters
        strcat(expToken, tempToken + (int)(expansionPtr - tempToken) + strlen("$$"));

        // check for another expansion variable and expand token
        if ((expansionPtr = strstr(expToken, "$$")) != NULL)
        {
            tempToken = realloc(expToken, strlen(expToken));
            // strcpy(tempToken, expToken);
            free(expToken);
            expToken = calloc(strlen(tempToken) + strlen(pids), sizeof(char));
        }
    }
    free(tempToken);
    return expToken;
}

/* parses user input and stores elements in struct */
struct userCommand* parseUserInput(char* input)
{
    // handle blank line
    if (strcmp(input, "\0") == 0)
    {
        return NULL;
    }
    // allocate space for parsed user command struct
    struct userCommand *currCommand = malloc(sizeof(struct userCommand));

    // initialize pointers to null
    currCommand->inputFile = NULL;
    currCommand->outputFile = NULL;

    // track length of args array and current
    int argsSize = 4;
    int argsNum = 0;

    // initialize array of argument pointers and current argument string
    char **argsArray = calloc(argsSize, sizeof(char*));
    char *currArg;
    
    // reentry pointer for strtok_r
    char *saveptr;

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
            // set all new elements to 0 to bound args
            memset(argsArray+(argsSize / 2), 0, sizeof(char*) * (argsSize - (argsSize / 2)));
        }

        // iterate to next argument
        token = strtok_r(NULL, " ", &saveptr);
    }

    // set struct args pointer to array
    currCommand->args = argsArray;

    // handle input and output arguments
    while (token != NULL && (strcmp(token, "<") == 0 || strcmp(token, ">") == 0))
    {
        // Parse input redirect filename
        if (token != NULL && strcmp(token, "<") == 0)
        {
            // skip input character
            token = strtok_r(NULL, " ", &saveptr);
            token = pidVarExpansion(token);
            currCommand->inputFile = calloc(strlen(token) + 1, sizeof(char));
            strcpy(currCommand->inputFile, token);

            token = strtok_r(NULL, " ", &saveptr);
        }

        // Parse output redirect filename
        if (token != NULL && strcmp(token, ">") == 0)
        {
            // skip input character
            token = strtok_r(NULL, " ", &saveptr);
            token = pidVarExpansion(token);
            currCommand->outputFile = calloc(strlen(token) + 1, sizeof(char));
            strcpy(currCommand->outputFile, token);
            token = strtok_r(NULL, " ", &saveptr);
        }
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
void commandHandler(struct userCommand* currCommand, char *status, char *userInput, struct childpidStruct *childPids)
{
    // handle blank line
    if (currCommand != NULL)
    {   
        // handle comment line
        if (strncmp(currCommand->args[0], "#", 1) == 0)
        {
            return;
        }
        // handle exit command
        else if (strcmp(currCommand->args[0], "exit") == 0)
        {
            exit_cmd(status, currCommand, userInput, childPids);
        }
        // handle cd command
        else if (strcmp(currCommand->args[0], "cd") == 0)
        {
            cd_cmd(currCommand);
        }
        // handle status command
        else if (strcmp(currCommand->args[0], "status") == 0)
        {
            status_cmd(status);
        }
        // handle any other command
        else
        {
            system_cmd(currCommand, status, childPids);
        }
    }
}

void printCommandStruct(struct userCommand* currStruct)
{
    if (currStruct != NULL)
    {
        int i = 0;
        while(currStruct->args[i] != 0)
        {
            printf("Arg: %s\n", currStruct->args[i]);
            i++;
        }
        printf("Input: %s\n", currStruct->inputFile);
        printf("Output: %s\n", currStruct->outputFile);
        printf("Background: %d\n", currStruct->backgroundBool);
    }
}

/* main smallsh program driver */
void smallsh()
{
    //  User input setup variables
    char *userInput;

    // Standup signal handlers
    shellSignalSetup();

    // struct storing background child PIDs
    struct childpidStruct* childPids = malloc(sizeof(struct childpidStruct));
    childPids->arraySize = 4;
    childPids->num = 0;
    childPids->pidArray = malloc(childPids->arraySize * sizeof(int));

    // holds exit status or terminating signal of last foreground process
    char *status = calloc(100, sizeof(char));
    strcpy(status, "exit status 0");

    while(1)
    {
        // checks for any child processes that have terminated
        check_pids(childPids);
        // printPids(childPids);

        printf(": ");
        fflush(stdout);

        // get user command input
        userInput = getUserInput();

        // parse input and create parsed command struct
        struct userCommand *currCommand = parseUserInput(userInput);

        // printCommandStruct(currCommand);

        // handle input
        commandHandler(currCommand, status, userInput, childPids);

        // cleanup up the trash
        if (currCommand != NULL)
        {
            cleanupStruct(currCommand);
            free(currCommand);
        }
        free(userInput);
    }
}