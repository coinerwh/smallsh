// main smallsh function declarations

#include "childpid_struct.h"

struct userCommand* parseUserInput(char* input);
char* getUserInput();
void cleanupStruct(struct userCommand* currStruct);
void commandHandler(struct userCommand* currCommand, char *status, char *userInput, struct childpidStruct *childPids);
void printCommandStruct(struct userCommand* currCommand);
void smallsh();