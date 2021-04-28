#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "childpid_struct.h"

/*
    Function for adding, removing, and checking child_struct
*/

void add_pid(struct childpidStruct *childPids, int pid)
{
    int *size = childPids->arraySize;
    int *num = childPids->num;
    int i;
    
    // append child pid to array
    childpids->pidArray[*num++] = pid;

    // check if array is full. If so, double size
    if (*num == *size)
    {
        *size *= 2;
        childPids->pidArray = realloc(childPids->pidArray, *size);
    }
}


// partially inspired by remove element from array algo found here: https://codeforwin.org/2015/07/c-program-to-delete-element-from-array.html
void delete_pid(struct childpidStruct *childPids, int pid)
{
    int *size = childPids->arraySize
    int *num = childPids->num;
    int *arr = childPids->pidArray;
    // holds index of child pid to delete
    int pos;

    // find pid in array and set pos to index of found pid
    for (i=0; i<num+1; i++)
    {
        if (arr[i] == pid)
        {
            pos = i;
            break;
        }
    }

    // copy next element value to current element
    for (i=pos; i<num+1; i++)
    {
        arr[i] = arr[i+1];
    }

    // decrement last position num of array to account for pid removed
    *num--;
}