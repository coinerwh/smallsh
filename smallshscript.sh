#! /usr/bin/env bash
gcc -std=gnu99 -o smallsh -Wall main.c smallsh.c commands.c childpid_functions.c signal_handlers.c