# Smallsh

Linux shell with built-in commands. All other commands run as child processes using fork() and exec(). Includes signal handlers, variable expansion, input and output redirection, background argument handling, and process status

## Features

1. Built-in 'cd' command. Takes one argument for both absolute and relative paths. If no argument provided, changes to directory specified in the HOME environment variable
2. Built-in 'status' command. Returns the exit status or terminating signal of the last foreground process ran by smallsh shell
3. Built-in 'exit' command. Exits shell. When command is run, the shell will kill all other processes or jobs that the shell has started before terminating itself
4. Variable $$ expansion. Replaces any instance of '$$' in a command with the process ID of smallsh itself
5. Handles both blank lines and '#' comment lines
6. All other commands are executing as child processes using fork() and exec()
7. Input & output redirection with '<' and '>'
8. Commands including '&' at the end are ran as background processes, and the shell will not wait for the execution to complete. STDIN and STDOUT are redirected to /dev/null unless redirected
9. Inludes signal handlers for SIGINT (CTRL+C) and SIGTSTP (CTRL+Z)

## Compilation and execution

To compile via shell script:
./smallshscript.sh

To compile with command:
gcc -std=gnu99 -o smallsh main.c smallsh.c commands.c childpid_functions.c signal_handlers.c input_processor.c

To execute:
./smallsh
