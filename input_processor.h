// declarations for handling user input

#include "command_struct.h"

char *pidVarExpansion(char *token);
struct userCommand* parseUserInput(char* input);
char* getUserInput();