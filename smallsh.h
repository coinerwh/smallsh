// main smallsh function declarations

struct userCommand* parseUserInput(char* input);
char* getUserInput();
void cleanupStruct(struct userCommand* currStruct);
void commandHandler(struct userCommand* currCommand);
void smallsh();