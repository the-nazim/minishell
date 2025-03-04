#include "main.h"

void execute_internal_cmd( char *input)
{
    if(strcmp(input, "exit")==0)
        exit(0);

    else if(strncmp(input, "cd", 2)==0)
    {
        //refer chdir() function
        char *dir = input+2;

        while(*dir == ' ')
            dir++;

        if(chdir(dir) == 0)
        {
            char cwd[200];
            if (getcwd(cwd, sizeof(cwd)) != NULL) 
                printf("%s\n", cwd);
        }
        else
            printf("No such file or directory\n");
    }

    else if(strcmp(input, "pwd")==0)
        system("pwd");
}