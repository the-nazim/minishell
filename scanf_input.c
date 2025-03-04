#include "main.h"

int status;
int child_pid = 0;
char global_prompt[25];
extern Slist *job_list;
int job_num = 0;

// Check if a command should run in background
int is_background_command(char *input)
{
    int len = strlen(input);
    if (len > 0 && input[len-1] == '&') {
        input[len-1] = '\0';  // Remove the & character
        return 1;
    }
    return 0;
}

// Handle bg command
void bg_command(Slist **head, char *args)
{
    int job_num = 0;
    if (args != NULL && *args != '\0') {
        job_num = atoi(args);
    }
    
    Slist *job;
    if (job_num > 0) {
        job = find_job_by_num(*head, job_num);
    } else {
        // Find the most recent stopped job
        job = *head;
        while (job != NULL) {
            if (strcmp(job->status, "Stopped") == 0) {
                break;
            }
            job = job->next;
        }
    }
    
    if (job == NULL) {
        printf("No such job\n");
        return;
    }
    
    if (strcmp(job->status, "Running") == 0) {
        printf("Job %d already in background\n", job->num);
        return;
    }
    
    // Continue the stopped process and move it to background
    kill(job->pid, SIGCONT);
    job->status = "Running";
    printf("[%d] %d continued in background %s\n", job->num, job->pid, job->cmd);
}

// Handle fg command
void fg_command(Slist **head, char *args)
{
    int job_num = 0;
    if (args != NULL && *args != '\0') {
        job_num = atoi(args);
    }
    
    Slist *job;
    if (job_num > 0) {
        job = find_job_by_num(*head, job_num);
    } else {
        // Get the most recent job (first in the list)
        job = *head;
    }
    
    if (job == NULL) {
        printf("No such job\n");
        return;
    }
    
    // Bring the job to foreground
    printf("%s\n", job->cmd);
    
    // Set as current child process
    child_pid = job->pid;
    
    // Continue the process if it was stopped
    if (strcmp(job->status, "Stopped") == 0) {
        kill(job->pid, SIGCONT);
    }
    
    // Wait for the process to complete or stop
    int wait_status;
    waitpid(job->pid, &wait_status, WUNTRACED);
    
    // Check if process stopped or completed
    if (WIFSTOPPED(wait_status)) {
        // Process was stopped, update status
        job->status = "Stopped";
        printf("\n[%d] %d stopped %s\n", job->num, job->pid, job->cmd);
    } else {
        // Process completed, remove from job list
        printf("\n[%d] %d completed %s\n", job->num, job->pid, job->cmd);
        remove_job(head, job->pid);
    }
    
    // Reset child_pid
    child_pid = 0;
}

void scan_input(char *prompt, char *input_string) {
    strcpy(global_prompt, prompt);
    
    // Set up signal handlers
    signal(SIGINT, sigint_handler);
    signal(SIGTSTP, sigtstp_handler);
    signal(SIGCHLD, sigchld_handler);
    
    if(prompt != NULL) {
        printf("%s ", prompt);
    }

    while(1) {
        // Clear input buffer
        memset(input_string, 0, 25);
        
        // Read input
        scanf("%[^\n]", input_string);
        getchar();  // Consume newline
        
        // Handle empty input
        if (strlen(input_string) == 0) {
            printf("%s ", prompt);
            continue;
        }
        
        // Handle PS1 change
        if(strncmp("PS1=", input_string, 4) == 0) {
            if(input_string[4] != ' ') {
                strcpy(prompt, &input_string[4]);
                strcpy(global_prompt, prompt);
            }
            continue;
        }

        // Exit command
        if(strcmp(input_string, "exit") == 0)
            exit(0);
            
        // Handle builtin job control commands
        if(strncmp(input_string, "jobs", 4) == 0) {
            print_jobs(job_list);
            printf("%s ", prompt);
            continue;
        }
        
        if(strncmp(input_string, "bg", 2) == 0) {
            bg_command(&job_list, input_string + 3);  // Skip "bg " to get arguments
            printf("%s ", prompt);
            continue;
        }
        
        if(strncmp(input_string, "fg", 2) == 0) {
            fg_command(&job_list, input_string + 3);  // Skip "fg " to get arguments
            printf("%s ", prompt);
            continue;
        }
        
        // Get command and check if it should run in background
        char *cmd = get_cmd(input_string);
        int ret = check_cmd(cmd);
        int bg = is_background_command(input_string);
            
        if(ret == EXTERNAL) {
            char exec_cmd[25][25];
            char *argv[25]; 
            int j = 0, k = 0;
            
            // Parse command and arguments
            for(int i = 0; input_string[i] != '\0'; i++) {
                if(input_string[i] != ' ') {
                    exec_cmd[j][k++] = input_string[i];
                } else {
                    exec_cmd[j][k] = '\0';
                    argv[j] = exec_cmd[j];
                    j++;
                    k = 0;
                }
            }
    
            exec_cmd[j][k] = '\0';
            argv[j] = exec_cmd[j]; 
            j++;
            argv[j] = NULL; 

            // Fork and execute
            int pid = fork();
            if(pid < 0) {
                perror("fork failed");
                continue;
            } else if(pid == 0) {
                // Child process
                // Reset signal handlers to default
                signal(SIGINT, SIG_DFL);
                signal(SIGTSTP, SIG_DFL);
                signal(SIGCHLD, SIG_DFL);
                
                // Execute the command
                execvp(argv[0], argv);
                perror("exec failed");
                exit(1);
            } else {
                // Parent process
                if (bg) {
                    // Background process - add to job list
                    job_num++;
                    insert_into_list(&job_list, job_num, pid, cmd, "Running");
                    
                    // Don't wait for it to complete
                    printf("%s ", prompt);
                } else {
                    // Foreground process
                    child_pid = pid;
                    
                    // Wait for it to complete or stop
                    waitpid(pid, &status, WUNTRACED);
                    
                    // Check if it was stopped
                    if(WIFSTOPPED(status) || sig_caught == 1) {
                        job_num++;
                        insert_into_list(&job_list, job_num, pid, cmd, "Stopped");
                        sig_caught = 0;
                    }
                    
                    child_pid = 0;
                    printf("%s ", prompt);
                }
            }
        } else if(ret == INTERNAL) {
            execute_internal_cmd(input_string);
            printf("%s ", prompt);
        } else {
            echo_spcl(input_string);
            printf("%s ", prompt);
        }
    }
}