#include "main.h"

char *get_cmd(char *input_string) 
{
    static char cmd[25];
    int i=0;
    while(input_string[i] != '\0')
    {
        if(input_string[i] != ' ')
            cmd[i] = input_string[i];
        else
            break;
        i++;
    }
    cmd[i] = '\0';
    return cmd;
}

int check_cmd(char *command)
{
    int fd = open("External_commands.txt", O_RDONLY);
    char ch, file_cmd[25];
    int i=0;
    while(read(fd, &ch, 1))
    {
        if(ch != '\n')
            file_cmd[i++] = ch;
        else
        {
            file_cmd[i++] = '\0';
            if(strcmp(command, file_cmd)==0)
                return EXTERNAL;
            else
            {
                i=0;
                memset(file_cmd,0,25);
            }
        }
    }

    char *builtins[] = {"echo", "printf", "read", "cd", "pwd", "pushd", "popd", "dirs", "let", "eval",
        "set", "unset", "export", "declare", "typeset", "readonly", "getopts", "source",
        "exit", "exec", "shopt", "caller", "true", "type", "hash", "bind", "help", NULL};
    
    for(int i=0; builtins[i]!= NULL;i++)
    {
        if(strcmp(command, builtins[i])==0)
            return INTERNAL;
    }

    return NO_COMMAND;
}