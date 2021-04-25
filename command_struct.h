// User command struct to store parsed user input 

struct userCommand
{
    // Pointer to an array of command arguments
    char **args;
    // if input file redirect is defined
    char *inputFile;
    // if output file redirect is defined
    char *outputFile;
    // background argument
    int backgroundBool;
};