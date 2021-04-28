#! /usr/bin/env bash
gcc -std=gnu99 -o smallsh -Wall -g main.c smallsh.c commands.c childpid_functions.c
valgrind --leak-check=yes --show-reachable=yes --track-origins=yes ./smallsh
