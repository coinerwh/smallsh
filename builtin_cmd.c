#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "command_struct.h"

/*
    Built-in command functions
*/

void exit_cmd()
{
    printf("I exited...not\n");
}

void cd_cmd(struct userCommand *currCommand)
{
    // set current working directory to directory specific by HOME env variable
    char curr_dir[100];
    if (currCommand->args[1] == 0)
    {
        getcwd(curr_dir, 100);
        printf("Curr: %s\n", curr_dir);
        char *homeDir = getenv("HOME");
        chdir(homeDir);
        getcwd(curr_dir, 100);
        printf("Curr: %s\n", curr_dir);
    }
    // set current working directory to user defined absolute or relative path
    else
    {
        // grabs directory argument
        getcwd(curr_dir, 100);
        printf("Curr: %s\n", curr_dir);
        char *cd_dir = currCommand->args[1];
        chdir(cd_dir);
        getcwd(curr_dir, 100);
        printf("Curr: %s\n", curr_dir);
    }
    
}

void status_cmd()
{
    
}