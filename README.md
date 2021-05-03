# Smallsh

To compile via shell script:
./smallshscript.sh

To compile with command:
gcc -std=gnu99 -o smallsh -Wall main.c smallsh.c commands.c childpid_functions.c signal_handlers.c input_processor.c

To execute:
./smallsh
