#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "smallsh.h"

/*
    Functions for handling SIGINT and SIGTSTP signals

    SIGINT
    Ignored by shell and background processes but will terminate any child processes running in foreground

    SIGTSTP
    All foreground and background child processes will ignore SIGTSTP
    Will place shell in state where no background processes can be ran. & operator ignored
    If user send SIGTSTP again, shell returns back to normal condition where & is honored

    Much of this module is inspired by the Signal Handling API Exploration as part of the OSU CS344 course 
*/ 

void SIGINT_handler(int signo)
{
    // Initialize SIGINT_action struct as empty
    
    char* message = "Haha you can't interrupt ME!\n";
    write(STDOUT_FILENO, message, 30);
}

void SIGTSTP_handler(int signo)
{
    // setting program to foreground only
    if (foregroundOnly == 0)
    {
        foregroundOnly = 1;
        char* message = "Entering foreground-only mode (& is now ignored)\n";
        write(STDOUT_FILENO, message, 50);
    }
    else
    {
        foregroundOnly = 0;
        char* message = "Exiting foreground-only mode\n";
        write(STDOUT_FILENO, message, 30);
    }
    
}

void SIGINT_child_handler(int signo)
{
    char* message = "terminated by signal 2\n";
    write(STDOUT_FILENO, message, 24);
}

void shellSignalSetup()
{
    // Initialize signal structs
    struct sigaction SIGINT_action = {{0}};
    struct sigaction SIGTSTP_action = {{0}};

    // Register signal handlers
    SIGINT_action.sa_handler = SIG_IGN;
    SIGTSTP_action.sa_handler = SIGTSTP_handler;

    // block all catchable signals 
    sigfillset(&SIGINT_action.sa_mask);
    sigfillset(&SIGTSTP_action.sa_mask);

    // No flags set
	SIGINT_action.sa_flags = 0;
    SIGTSTP_action.sa_flags = SA_RESTART;

    // Standup signal handlers
    sigaction(SIGINT, &SIGINT_action, NULL);
    sigaction(SIGTSTP, &SIGTSTP_action, NULL);
}

void signalForegroundSetup()
{
    // Initialize signal structs
    struct sigaction SIGINT_action = {{0}};
    struct sigaction SIGTSTP_action = {{0}};

    // Register signal handlers
    SIGINT_action.sa_handler = SIG_DFL;
    SIGTSTP_action.sa_handler = SIG_IGN;

    // block all catchable signals 
    sigfillset(&SIGINT_action.sa_mask);
    sigfillset(&SIGTSTP_action.sa_mask);

    // No flags set
	SIGINT_action.sa_flags = 0;
    SIGTSTP_action.sa_flags = 0;

    // Standup signal handlers
    sigaction(SIGINT, &SIGINT_action, NULL);
    sigaction(SIGTSTP, &SIGTSTP_action, NULL);
}

void signalBackgroundSetup()
{
    // Initialize signal structs
    struct sigaction SIGINT_action = {{0}};
    struct sigaction SIGTSTP_action = {{0}};

    // Register signal handlers
    SIGINT_action.sa_handler = SIG_IGN;
    SIGTSTP_action.sa_handler = SIG_IGN;

    // set up sa_masks
    sigfillset(&SIGINT_action.sa_mask);
    sigfillset(&SIGTSTP_action.sa_mask);

    // No flags set
	SIGINT_action.sa_flags = 0;
    SIGTSTP_action.sa_flags = 0;

    // Standup signal handlers
    sigaction(SIGINT, &SIGINT_action, NULL);
    sigaction(SIGTSTP, &SIGTSTP_action, NULL);
}