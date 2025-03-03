#include "main.h"

int status;
int child_pid;
char global_prompt[25];

void scan_input(char *prompt, char *input_string) {
    strcpy(global_prompt, prompt);

    signal(SIGINT, sigint_handler);
    signal(SIGTSTP, sigtstp_handler);

    Slist *head = NULL;
    int num=0;
    
    int pipe=0; 

    if(prompt == NULL && input_string == NULL)
        printf("%s ", prompt);

    while(1){
        memset(input_string, 0, 25);
        printf("%s ", prompt);
        scanf("%[^\n]", input_string);
        getchar();

        if(strncmp("PS1=", input_string, 4)==0){
            if(input_string[4] != ' ')
            {
                strcpy(prompt, &input_string[4]);
                strcpy(global_prompt, prompt);
            }
        }

        char *cmd = get_cmd(input_string);
        int ret = check_cmd(cmd);

        if(strcmp(input_string, "exit")==0)
            exit(0);

        for(int i=0; input_string[i]!='\0'; i++)
        {
            if(input_string[i] == '|')
            {
                pipe = 1;
                break;
            }
        }

        if(ret == EXTERNAL)
        {
            if(pipe==1)
                pipe_handler(input_string);
            
            else
            {   
                char exec_cmd[25][25];
                char *argv[25]; 
                int j=0,k=0;

                for(int i=0; input_string[i]!='\0';i++)
                {
                    if(input_string[i] != ' ')
                    {
                        exec_cmd[j][k++] = input_string[i];
                    }
                    else
                    {
                        exec_cmd[j][k] = '\0';
                        argv[j] = exec_cmd[j];
                        j++;
                        k=0;
                    }
                }
        
                exec_cmd[j][k] = '\0';
                argv[j] = exec_cmd[j]; 
                j++;
                argv[j] = NULL; 

                int pid = fork();
                if(pid == 0)
                {
                    execvp(argv[0], argv);
                }
                else
                {
                    child_pid = pid;
                    waitpid(pid, &status, WUNTRACED);
                    
                    // If the child was stopped by a signal
                    if(WIFSTOPPED(status) || sig_caught == 1) {
                        num++;
                        insert_into_list(&head, num, input_string);
                        sig_caught = 0;
                    }
                    
                    child_pid = 0; 
                }
            }
        }
        else if(ret == INTERNAL)
        {
            execute_internal_cmd(input_string);
        }
        else
            echo_spcl(input_string);

        if(strcmp(input_string, "jobs")==0)
        {
            Slist *current = head;
            if(current == NULL) {
                printf("No jobs\n");
            }
            else
                print_list(&head);
        }

        memset(input_string, 0, 25);
    }
}