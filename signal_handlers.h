// declarations for signal handling functions

void SIGINT_handler(int signo);
void SIGTSTP_handler(int signo);
void SIGINT_child_handler(int signo);
void shellSignalSetup();
void signalForegroundSetup();
void signalBackgroundSetup();