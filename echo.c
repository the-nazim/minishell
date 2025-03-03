#include "main.h"

int echo_spcl(char *input)
{
    if(strcmp(input, "echo $?")==0)
        printf("%d\n", status);

    else if(strcmp(input,"echo $$")==0)
        printf("%d\n", getpid());

    else if(strcmp(input, "echo $SHELL")==0)
        system("pwd");

}