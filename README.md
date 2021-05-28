# smallsh

POSIX shell with built-in commands. All other commands run as child processes using fork() and exec(). Includes signal handlers, variable expansion, input and output redirection, background daemon argument handling, and process status

## Features

1. Built-in 'cd' command. Takes one argument for both absolute and relative paths. If no argument provided, changes to directory specified in the HOME environment variable
2. Built-in 'status' command. Returns the exit status or terminating signal of the last foreground process ran by smallsh shell
3. Built-in 'exit' command. Shell kills all other processes or jobs that the shell has started before terminating itself
4. Variable $$ expansion. Replaces any instance of '$$' in a command with the process ID of shell
5. Handles both blank lines and '#' comment lines
6. All other commands are executing as child processes using fork() and exec()
7. Bash-style input & output redirection with '<' and '>'
8. Commands including '&' at the end are ran as background processes, and the shell will not wait for the execution to complete. STDIN and STDOUT are redirected to /dev/null unless redirected
9. Inludes signal handlers for SIGINT (CTRL+C) and SIGTSTP (CTRL+Z)
  > * SIGINT is ignored by shell and all background processes
  > * SIGINT terminates all foreground child processes, printing out PID of the process and the signal that killed it
  > * All child processes, foreground and background, ignore SIGTSTP
  > * SIGTSTP flips shell into 'foreground only mode' where '&' is ignored until shell receives SIGTSTP again

## C/POSIX API Tools

- unistd POSIX OS API, including use of fork, exec, dup2 for redirection, chdir and other I/O primitives
- wait and waitpid system calls for child process handling
- POSIX signal handling with sigaction function and struct

## Compilation and execution

Compile via shell script:
./smallshscript.sh

Compile with command:
gcc -std=gnu99 -o smallsh main.c smallsh.c commands.c childpid_functions.c signal_handlers.c input_processor.c

Execute:
./smallsh
