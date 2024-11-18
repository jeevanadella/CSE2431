/*
Name: Sreenivas Jeevan Nadella
How to compile: run the command make all
How to run: run the command ./mysh
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>

#define MAX_LINE 80 /* 80 chars per line, per command, should be enough. */
#define HISTORY_SIZE 10 /* Max 10 commands in history */

char history[HISTORY_SIZE][MAX_LINE]; /* History buffer to store commands */
int history_count = 0; /* Total number of commands entered */

/* Signal handler for SIGINT */
void handle_SIGINT() {
    write(STDOUT_FILENO, "\nCommand History:\n", 18);
    int start = (history_count > HISTORY_SIZE) ? (history_count - HISTORY_SIZE) : 0;
    for (int i = start; i < history_count; i++) {
        printf("%d. %s\n", i + 1, history[i % HISTORY_SIZE]);
    }
    printf("COMMAND->");
    fflush(stdout);
}

/* Add command to history */
void add_to_history(char inputBuffer[]) {
    if (history_count < HISTORY_SIZE) {
        strncpy(history[history_count], inputBuffer, MAX_LINE);
        history[history_count][strcspn(history[history_count], "\n")] = '\0'; // Remove newline
    } else {
        // Shift history to make space for new command
        for (int i = 1; i < HISTORY_SIZE; i++) {
            strncpy(history[i - 1], history[i], MAX_LINE);
        }
        strncpy(history[HISTORY_SIZE - 1], inputBuffer, MAX_LINE);
        history[HISTORY_SIZE - 1][strcspn(history[HISTORY_SIZE - 1], "\n")] = '\0'; // Remove newline
    }
    history_count++;
}

/* Tokenize a command into arguments */
void tokenize_command(char *command, char *args[]) {
    int i = 0;
    char *token = strtok(command, " ");
    while (token != NULL) {
        args[i++] = token;
        token = strtok(NULL, " ");
    }
    args[i] = NULL; // Null-terminate the argument list
}

/* Execute the most recent command */
void repeat_last_command(char *inputBuffer, char *args[]) {
    if (history_count == 0) {
        printf("No commands in history.\n");
        return;
    }
    // Use the last command in history
    strncpy(inputBuffer, history[(history_count - 1) % HISTORY_SIZE], MAX_LINE);
    inputBuffer[MAX_LINE - 1] = '\0'; // Ensure null-termination
    printf("Repeating command: %s\n", inputBuffer); /* Echo last command */
    
    // Tokenize the fetched command
    tokenize_command(inputBuffer, args);  // Tokenize to prepare for execution

    // Now execute the command
    pid_t pid = fork();
    if (pid < 0) {
        printf("Fork failed.");
        exit(-1);
    } else if (pid == 0) {
        // Child process will invoke execvp()
        if (execvp(args[0], args) < 0) {
            printf("Command %s not found.\n", args[0]);
            exit(-1);
        }
    } else {
        // Parent waits if not background
        waitpid(pid, NULL, 0);
    }
}

/* Execute the most recent command starting with a specific character */
int find_and_prepare_from_history(char first_letter, char *inputBuffer, char *args[]) {
    int found = 0; // To track if we found a command

    // Check the most recent commands in history for one that starts with the specified letter
    for (int i = history_count - 1; i >= 0; i--) {
        // Adjust for the circular buffer
        int index = i % HISTORY_SIZE;
        if (history[index][0] == first_letter) {
            // Copy the found command into inputBuffer
            strncpy(inputBuffer, history[index], MAX_LINE);
            inputBuffer[MAX_LINE - 1] = '\0'; // Ensure null-termination
            
            // Tokenize the found command and prepare it for execution
            tokenize_command(inputBuffer, args);
            found = 1; // Command found

            // Execute the command
            pid_t pid = fork();
            if (pid < 0) {
                printf("Fork failed.");
                exit(-1);
            } else if (pid == 0) {
                // Child process will invoke execvp()
                if (execvp(args[0], args) < 0) {
                    printf("Command %s not found.\n", args[0]);
                    exit(-1);
                }
            } else {
                // Parent waits if not background
                waitpid(pid, NULL, 0);
            }

            printf("Repeating command: %s\n", inputBuffer); // Print the command being repeated
            break; // Exit loop after finding the most recent match
        }
    }

    return found; // Return whether a command was found
}

/* Setup function */
void setup(char inputBuffer[], char *args[], int *background) {
    int length, /* # of characters in the command line */
        i,      /* loop index for accessing inputBuffer array */
        start,  /* index where beginning of next command parameter is */
        ct;     /* index of where to place the next parameter into args[] */

    ct = 0;

    /* read what the user enters on the command line */
    length = read(STDIN_FILENO, inputBuffer, MAX_LINE);  

    start = -1;
    if (length == 0)
        exit(0);            /* ^d was entered, end of user command stream */
    if (length < 0) {
        perror("error reading the command");
        exit(-1);           /* terminate with error code of -1 */
    }

    // Check if input starts with 'r'
    if (strncmp(inputBuffer, "r", 1) == 0) {
        // Case: just 'r' or 'r ' - repeat last command
        if (length == 1 || length == 2) {
            repeat_last_command(inputBuffer, args);
        } else {
            // 'r x' - get the character after 'r'
            char first_letter = inputBuffer[2];
            // Check if the character is not a space and is a valid letter
            if (first_letter != ' ' && isalpha(first_letter)) {
                // Proceed to find and prepare the command from history
                if (!find_and_prepare_from_history(first_letter, inputBuffer, args)) {
                    // Command was not found
                    printf("No command found starting with '%c'.\n", first_letter);
                }
            } else {
                printf("Invalid command format. Please use 'r' or 'r <letter>'.\n");
            }
        }
    } else {
        // Add to history if it's a regular command
        add_to_history(inputBuffer); /* Add the command to history */
    }
    
    /* examine every character in the inputBuffer */
    for (i = 0; i < length; i++) { 
        switch (inputBuffer[i]) {
        case ' ':
        case '\t':               /* argument separators */
            if (start != -1) {
                args[ct] = &inputBuffer[start];    /* set up pointer */
                ct++;
            }
            inputBuffer[i] = '\0'; /* add a null char; make a C string */
            start = -1;
            break;
        case '\n':                 /* should be the final char examined */
            if (start != -1) {
                args[ct] = &inputBuffer[start];     
                ct++;
            }
            inputBuffer[i] = '\0';
            args[ct] = NULL; /* no more arguments to this command */
            break;
        case '&':
            *background = 1;
            inputBuffer[i] = '\0';
            break;
        default:             /* some other character */
            if (start == -1)
                start = i;
        } 
    }    
    args[ct] = NULL; /* just in case the input line was > 80 */
}

int main(void) {
    char inputBuffer[MAX_LINE]; /* buffer to hold the command entered */
    int background;             /* equals 1 if a command is followed by '&' */
    char *args[MAX_LINE / 2 + 1]; /* command line (of 80) has max of 40 arguments */
    pid_t pid;
    int returnVal;

    /* Set up the signal handler for SIGINT */
    struct sigaction handler;
    handler.sa_handler = handle_SIGINT;
    handler.sa_flags = SA_RESTART; /* This flag ensures system calls are restarted */
    sigaction(SIGINT, &handler, NULL);

    while (1) {
    background = 0;
    printf("COMMAND->");
    fflush(stdout);
    setup(inputBuffer, args, &background); /* get next command */

    // Execute the command if args are set
    if (args[0] != NULL) {
        // (1) fork a child process using fork()
        pid = fork();
        if (pid < 0) {
            printf("Fork failed.");
            exit(-1);
        } else if (pid == 0) {
            // (2) the child process will invoke execvp()
            int status = execvp(args[0], args);
            if (status < 0) {
                printf("Command %s not found.\n", args[0]);
                exit(-1);
            }
        } else {
            // (3) if background == 0, the parent will wait, otherwise returns to the setup() function
            if (background == 0) {
                waitpid(pid, &returnVal, 0);
            }
        }
    }
}
    return 0;
}
