#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "command_struct.h"

/*
    Function for handling user input, including tokenizing and inserting into command struct
*/


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
    // partially inspired by stackoverflow post here: https://stackoverflow.com/questions/2015901/inserting-char-string-into-another-char-string
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
            // free(expToken);
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
    while (token != NULL && strcmp(token, "<") != 0 && strcmp(token, ">") != 0)
    {
        // create argument string
        token = pidVarExpansion(token);
        currArg = calloc(strlen(token) + 1, sizeof(char));
        strcpy(currArg, token);

        // iterate to next argument
        token = strtok_r(NULL, " ", &saveptr);

        // check if & is last argument. If not, continue. If so, set backgroundBool but do not save to args array
        if (strcmp(currArg, "&") == 0 && token == NULL)
        {
            currCommand->backgroundBool = 1;
        }
        else
        {
            currCommand->backgroundBool = 0;
            // save current argument string to array of arguments
            argsArray[argsNum++] = currArg;
        }


        // check if array size needs to be increased
        if (argsNum == argsSize)
        {
            argsSize *= 2;
            argsArray = realloc(argsArray, argsSize * sizeof(char*));
            // set all new elements to 0 to bound args
            memset(argsArray+(argsSize / 2), 0, sizeof(char*) * (argsSize - (argsSize / 2)));
        }
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

    // // Check background argument and set
    if (token != NULL && strcmp(token, "&") == 0)
    {
        currCommand->backgroundBool = 1;
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