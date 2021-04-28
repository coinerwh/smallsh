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
#include "childpid_struct.h"

/*
    Function for adding, removing, and checking child_struct
*/

void add_pid(struct childpidStruct *childPids, int pid)
{   
    // append child pid to array
    childPids->pidArray[childPids->num] = pid;
    childPids->num++;

    // check if array is full. If so, double size
    if (childPids->num == childPids->arraySize)
    {
        childPids->arraySize *= 2;
        printf("array size: %d\n", childPids->arraySize);
        fflush(stdout);
        childPids->pidArray = realloc(childPids->pidArray, childPids->arraySize * sizeof(int));
    }
}


// partially inspired by remove element from array algo found here: https://codeforwin.org/2015/07/c-program-to-delete-element-from-array.html
// returns T/F whether pid was found and deleted
void delete_pid(struct childpidStruct *childPids, int pid)
{
    // int *size = childPids->arraySize;
    // int *num = &childPids->num;
    int *arr = childPids->pidArray;
    // holds index of child pid to delete
    int pos = 0;
    int i;

    // find pid in array and set pos to index of found pid
    for (i=0; i<childPids->num; i++)
    {
        if (arr[i] == pid)
        {
            pos = i;
            break;
        }
    }

    // copy next element value to current element
    for (i=pos; i<childPids->num; i++)
    {
        arr[i] = arr[i+1];
    }

    // decrement last position num of array to account for pid removed
    childPids->num--;
}

// check if there are child pids that have terminated and prints their exit status
void check_pids(struct childpidStruct *childPids)
{
    int killedPid = -5;
    int childStatus;


    for (int i=0; i<childPids->num; i++)
    {
        killedPid= waitpid(childPids->pidArray[i], &childStatus, WNOHANG);
        // exited normally with exit status
        if (killedPid == 0)
        {
            continue;
        }
        if(WIFEXITED(childStatus))
        {
            // grab status value
            childStatus = WEXITSTATUS(childStatus);

            printf("background pid %d is done: exit value %d\n", killedPid, childStatus);
            fflush(stdout);
        }
        // exited abnormally with signal
        else
        {
            childStatus = WTERMSIG(childStatus);

            printf("background pid %d is done: terminated by signal %d\n", killedPid, childStatus);
            fflush(stdout);
        }
        delete_pid(childPids, killedPid);
    }
}

void printPids(struct childpidStruct *childPids)
{
    for (int i=0; i<childPids->num; i++)
    {
        printf("why are you here, %d\n", childPids->pidArray[i]);
    }
}