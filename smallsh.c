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
#include "input_processor.h"

/*
    main smallsh.c driver and input handler functions
*/

// foreground global switch initialized
int foregroundOnly = 0;

/* frees struct member memory */
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

/* For testing command struct */
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