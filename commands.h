// Function declarations for built-in command function

#include "childpid_struct.h"


void exit_cmd(char *status, struct userCommand *currCommand, char *userInput, struct childpidStruct *childPids);
void cd_cmd(struct userCommand *currCommand);
void status_cmd(char *status);
void inputOutputRedirect(char* inputFile, char* outputFile, int backgroundBool);
void system_cmd(struct userCommand *currCommand, char *status, struct childpidStruct *childPids);