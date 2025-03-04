#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<stdio_ext.h>
#include<signal.h>
#include<sys/types.h>

#define EXTERNAL 1
#define INTERNAL 2
#define NO_COMMAND 3

#define MAX_JOBS 20

extern int status;
extern int child_pid;
extern char global_prompt[25];
extern int sig_caught;

typedef struct node {
    int num;       // Job number
    int pid;       // Process ID
    char *status;  // "Running", "Stopped"
    char *cmd;     // Command string
    struct node *next;
} Slist;

void scan_input(char *prompt, char *input_string);
char *get_cmd(char *input_string);
void execute_internal_cmd(char *input);
int check_cmd(char *command);
int echo_spcl(char * input);
void sigint_handler(int signum);
void sigtstp_handler(int signum);
void sigchld_handler(int signum);
char *get_command_name(int pid);
void insert_into_list(Slist **head, int num, int pid, char *cmd, char *status);
Slist* find_job_by_pid(Slist *head, int pid);
Slist* find_job_by_num(Slist *head, int num);
void print_jobs(Slist *head);
void remove_job(Slist **head, int pid);
int is_background_command(char *input);
int get_last_job_num(Slist *head);
int get_last_job_pid(Slist *head);
void bg_command(Slist **head, char *args);
void fg_command(Slist **head, char *args);
