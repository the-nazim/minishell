#include "main.h"

char *get_cmd(char *input_string) 
{
    static char cmd[25];
    int i = 0;
    
    // Clear previous contents
    memset(cmd, 0, sizeof(cmd));
    
    while(input_string[i] != '\0' && input_string[i] != ' ' && i < sizeof(cmd) - 1)
    {
        cmd[i] = input_string[i];
        i++;
    }
    cmd[i] = '\0';
    return cmd;
}

int check_cmd(char *command)
{
    // Check builtin commands first
    char *builtins[] = {"echo", "printf", "read", "cd", "pwd", "pushd", "popd", "dirs", "let", "eval",
    "set", "unset", "export", "declare", "typeset", "readonly", "getopts", "source",
    "exit", "exec", "shopt", "caller", "true", "type", "hash", "bind", "help", "clear", NULL};

    for(int i = 0; builtins[i] != NULL; i++)
    {
        if(strcmp(command, builtins[i]) == 0)
            return INTERNAL;
    }

    // Open external commands file
    int fd = open("External_commands.txt", O_RDONLY);
    if(fd == -1)
    return NO_COMMAND;  // File doesn't exist or can't be opened

    char buffer[256];
    char file_cmd[256];
    int bytes_read;
    int cmd_index = 0;

    while((bytes_read = read(fd, buffer, sizeof(buffer))) > 0)
    {
        for(int i = 0; i < bytes_read; i++)
        {
            if(buffer[i] == '\n' || cmd_index >= sizeof(file_cmd) - 1)
            {
                file_cmd[cmd_index] = '\0';
                
                if(strcmp(command, file_cmd) == 0)
                {
                    close(fd);
                    return EXTERNAL;
                }
                
                // Reset for next line
                memset(file_cmd, 0, sizeof(file_cmd));
                cmd_index = 0;
            }
            else
            {
                file_cmd[cmd_index++] = buffer[i];
            }
        }
    }

    close(fd);
    return NO_COMMAND;
}