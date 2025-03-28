#include "main.h"

void pipe_handler(char *input_string)
{
    char *commands[50];
    char *args[50][50];
    int cmd_count = 0;
    
    // Parse the input string to separate commands by pipes
    char *token = strtok(input_string, "|");
    while (token != NULL && cmd_count < 50) {
        commands[cmd_count++] = token;
        token = strtok(NULL, "|");
    }
    
    // Parse each command to separate command and arguments
    for (int i = 0; i < cmd_count; i++) {
        int arg_count = 0;
        char *cmd_token = strtok(commands[i], " \t");
        
        while (cmd_token != NULL && arg_count < 50) {
            args[i][arg_count++] = cmd_token;
            cmd_token = strtok(NULL, " \t");
        }
        args[i][arg_count] = NULL; // NULL-terminate the argument list
    }
    
    int num_pipes = cmd_count - 1;
    int pipes[50][2];
    
    // Create all necessary pipes
    for (int i = 0; i < num_pipes; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            return;
        }
    }
    
    // Execute commands
    for (int i = 0; i < cmd_count; i++) {
        int pid = fork();
        
        if (pid == -1) {
            perror("fork");
            return;
        }
        
        if (pid == 0) { // Child process
            // Set up input redirection (read from previous pipe)
            if (i > 0) {
                if (dup2(pipes[i-1][0], STDIN_FILENO) == -1) {
                    perror("dup2 input");
                    exit(EXIT_FAILURE);
                }
            }
            
            // Set up output redirection (write to next pipe)
            if (i < num_pipes) {
                if (dup2(pipes[i][1], STDOUT_FILENO) == -1) {
                    perror("dup2 output");
                    exit(EXIT_FAILURE);
                }
            }
            
            // Close all pipe file descriptors
            for (int j = 0; j < num_pipes; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }
            
            // Execute the command
            if (execvp(args[i][0], args[i]) == -1) {
                perror("execvp");
                exit(EXIT_FAILURE);
            }
        }
    }
    
    // Parent process: close all pipe file descriptors
    for (int i = 0; i < num_pipes; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
    
    // Wait for all child processes to complete
    for (int i = 0; i < cmd_count; i++) {
        wait(NULL);
    }
}