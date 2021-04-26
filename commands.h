// Function declarations for built-in command function

void exit_cmd();
void cd_cmd(struct userCommand *currCommand);
void status_cmd(char *status);
void system_cmd(struct userCommand *currCommand, char *status);