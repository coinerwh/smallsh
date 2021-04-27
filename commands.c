#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include "command_struct.h"
#include "smallsh.h"

/*
    Command functions
*/

/* kills all processes and jobs that smallsh has stated, frees memory, and exits stage left */
void exit_cmd(char *status, struct userCommand *currCommand, char *userInput)
{
    // clean up mess before leaving
    if (currCommand != NULL)
        {
            cleanupStruct(currCommand);
            free(currCommand);
        }
    free(userInput);
    free(status);

    // that's all folks
    exit(EXIT_SUCCESS);
}

/* changes current directory to either HOME path if no argument provided or path provided by user */
void cd_cmd(struct userCommand *currCommand)
{
    // set current working directory to directory specific by HOME env variable
    if (currCommand->args[1] == 0)
    {
        char *homeDir = getenv("HOME");
        chdir(homeDir);
    }
    // set current working directory to user defined absolute or relative path
    else
    {
        // grabs directory argument
        char *cd_dir = currCommand->args[1];
        if(chdir(cd_dir))
        {
            printf("Directory does not exist\n");
            fflush(stdout);
        }
    }
    
}

/* prints exit status or terminating signal of last foreground process ran by shell */
void status_cmd(char *status)
{
    printf("%s\n", status);
    fflush(stdout);
}

/* handles any non-builtin commands and executes via child process */
void system_cmd(struct userCommand *currCommand, char *status)
{
    pid_t spawnPid = -5;
    int childStatus;
    int childPid = -1;

    // forking child process to spin off system command execution
    spawnPid = fork();
    switch (spawnPid)
    {
        // if fork and creation of child process fails
        case -1:
            printf("fork() failed\n");
            fflush(stdout);
            memset(status, 0, strlen(status));
            strcpy(status, "exit status 1");
            break;
        // child process
        case 0:
            // printf("I am a child. My pid = %d\n", getpid());
            // executes command provided by input and catches any errors
            execvp(currCommand->args[0], currCommand->args);
            perror("Error");
            exit(EXIT_FAILURE);
            break;
        // parent shell process
        default:
            childPid = waitpid(childPid, &childStatus, 0);
            // exited normally with exit status
            if(WIFEXITED(childStatus))
            {
                // grab status value
                childStatus = WEXITSTATUS(childStatus);

                // reset string and set new status
                memset(status, 0, strlen(status));
                sprintf(status, "exit status %d", childStatus);
            }
            // exited abnormally with signal
            else
            {
                childStatus = WTERMSIG(childStatus);

                // reset string and set new status
                memset(status, 0, strlen(status));
                sprintf(status, "terminated by signal %d", childStatus);
            }
            break;
    } 
}