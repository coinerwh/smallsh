// main smallsh function declarations

#include "childpid_struct.h"
#include "command_struct.h"

void cleanupStruct(struct userCommand* currStruct);
void commandHandler(struct userCommand* currCommand, char *status, char *userInput, struct childpidStruct *childPids);
void printCommandStruct(struct userCommand* currCommand);
void smallsh();

// foreground switch declaration
#ifndef FOREGROUNDONLY
#define FOREGROUNDONLY
extern int foregroundOnly;
#endif