#include "main.h"

int sig_caught = 0;

void sigint_handler(int signum)
{
    if(child_pid != 0)
        kill(child_pid, SIGINT);
    else
    {
        printf("\n%s ", global_prompt);
        fflush(stdout);
    }
}

void sigtstp_handler(int signum)
{
    if(child_pid != 0)
    {
        kill(child_pid, SIGSTOP);
        printf("\nProcess Stpped\n");
        sig_caught = 1;
        fflush(stdout);
    }
    else
    {
        printf("\n%s ", global_prompt);
        fflush(stdout);
    }
}