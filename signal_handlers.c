#include "main.h"
#include <errno.h>

int sig_caught = 0;
Slist *job_list;

void sigint_handler(int signum)
{
    if(child_pid != 0)
    {
        kill(child_pid, SIGINT);
    }
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
        kill(child_pid, SIGTSTP);
        sig_caught = 1;
    }
    else
    {
        printf("\n%s ", global_prompt);
        fflush(stdout);
    }
}

// Handler for SIGCHLD (child process termination)
void sigchld_handler(int signum)
{
    int saved_errno = errno;
    int pid;
    int status;
    
    // Use waitpid with WNOHANG to avoid blocking
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        // Find the job in our list
        Slist *job = find_job_by_pid(job_list, pid);
        
        if (job != NULL) {
            if (WIFEXITED(status)) {
                printf("\n[%d] %d Done %s (exit status: %d)\n", 
                       job->num, job->pid, job->cmd, WEXITSTATUS(status));
                remove_job(&job_list, pid);
            } else if (WIFSIGNALED(status)) {
                printf("\n[%d] %d Terminated %s (signal: %d)\n", 
                       job->num, job->pid, job->cmd, WTERMSIG(status));
                remove_job(&job_list, pid);
            }
            
            // Print prompt again
            printf("%s ", global_prompt);
            fflush(stdout);
        }
    }
    
    errno = saved_errno;
}
