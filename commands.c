#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "command_struct.h"

/*
    Command functions
*/

void exit_cmd()
{
    exit(EXIT_SUCCESS);
}

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

void status_cmd(char *status)
{
    printf("%s\n", status);
    fflush(stdout);
}

void system_cmd(struct userCommand *currCommand, char *status)
{
    pid_t spawnid = fork();
    switch (spawnid)
    {
        // if fork and creation of child proces fails
        case -1:
            printf("fork() failed\n");
            fflush(stdout);
            memset(status, 0, sizeof(status));
            strcpy(status, "Exit status 1");
            break;
        // child code
        case 0:

    } 
}