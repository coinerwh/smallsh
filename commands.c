#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "command_struct.h"
#include "smallsh.h"
#include "childpid_functions.h"
#include "signal_handlers.h"

/*
    Command functions
*/

/* kills all processes and jobs that smallsh has stated, frees memory, and exits stage left */
void exit_cmd(char *status, struct userCommand *currCommand, char *userInput, struct childpidStruct *childPids)
{
    // clean up mess before leaving
    if (currCommand != NULL)
        {
            cleanupStruct(currCommand);
            free(currCommand);
        }
    free(userInput);
    free(status);

    // kill all leftover background pids
    for (int i=0; i<childPids->num+1; i++)
    {
        kill(childPids->pidArray[i], SIGTERM);
    }

    free(childPids->pidArray);
    free(childPids);

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

void inputOutputRedirect(char* inputFile, char* outputFile, int backgroundBool)
{
    // check for input redirection
    if (inputFile != NULL)
    {
        // open input file
        int sourceFD = open(inputFile, O_RDONLY);
        if (sourceFD == -1)
        {
            perror("source open()");
            exit(EXIT_FAILURE);
        }
        int result = dup2(sourceFD, 0);
        if (result == -1)
        {
            perror("source dup2()");
            exit(EXIT_FAILURE);
        }
    }

    // check for output redirection
    if (outputFile != NULL)
    {
        // open output file
        int targetFD = open(outputFile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (targetFD == -1)
        {
            perror("target open()");
            exit(EXIT_FAILURE);
        }
        int result = dup2(targetFD, 1);
        if (result == -1)
        {
            perror("target dup2()");
            exit(EXIT_FAILURE);
        }
    }
    // set to null if input or output not set
    if (backgroundBool)
    {
        if (inputFile == NULL)
        {
            // open input file
            int sourceFD = open("/dev/null",0);
            if (sourceFD == -1)
            {
                perror("source open()");
                exit(EXIT_FAILURE);
            }
            int result = dup2(sourceFD, 0);
            if (result == -1)
            {
                perror("source dup2()");
                exit(EXIT_FAILURE);
            }
        }
        if (outputFile == NULL)
        {
            // open output file
            int targetFD = open("/dev/null", O_WRONLY);
            if (targetFD == -1)
            {
                perror("target open()");
                exit(EXIT_FAILURE);
            }
            int result = dup2(targetFD, 1);
            if (result == -1)
            {
                perror("target dup2()");
                exit(EXIT_FAILURE);
            }
        }
    }
}

/* handles any non-builtin commands and executes via child process */
void system_cmd(struct userCommand *currCommand, char *status, struct childpidStruct *childPids)
{
    pid_t spawnPid = -5;
    int childStatus;

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
            // input and output redirection
            inputOutputRedirect(currCommand->inputFile, currCommand->outputFile, currCommand->backgroundBool);

            // background process signal handling
            if (currCommand->backgroundBool && foregroundOnly == 0)
            {
                signalBackgroundSetup();
            }

            // foreground process signal handling
            else
            {
                signalForegroundSetup();
            }
            // executes command provided by input and catches any errors
            execvp(currCommand->args[0], currCommand->args);
            perror("Error");
            fflush(stderr);
            exit(EXIT_FAILURE);
            break;
        // parent shell process
        default:
            // background & set and child process running in background
            if (currCommand->backgroundBool && foregroundOnly == 0)
            {
                printf("background pid is %d\n", spawnPid);
                fflush(stdout);
                add_pid(childPids, spawnPid);
                break;
            }
            else 
            {
                spawnPid = waitpid(spawnPid, &childStatus, 0);
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
                    printf("terminanated by signal %d\n", childStatus);
                    fflush(stdout);
                    sprintf(status, "terminated by signal %d", childStatus);
                }
                break;
            }
    } 
}