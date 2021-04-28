// declarations for childpidStruct functions

#include "childpid_struct.h"

void add_pid(struct childpidStruct *childPids, int pid);
void delete_pid(struct childpidStruct *childPids, int pid);
void check_pids(struct childpidStruct *childPids);
void printPids(struct childpidStruct *childPids);