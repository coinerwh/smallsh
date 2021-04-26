#! /usr/bin/env bash
gcc -std=gnu99 -o smallsh -Wall main.c smallsh.c builtin_cmd.c