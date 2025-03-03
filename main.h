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

extern int status;
extern int child_pid;
extern char global_prompt[25];
extern int num;
extern int sig_caught;

typedef struct node {
    int num;
    char *status;
    char *cmd;
    struct node *next;
}Slist;

void scan_input(char *prompt, char *input_string);
char *get_cmd(char *input_string);
void execute_internal_cmd(char *input);
int check_cmd(char *command);
int echo_spcl(char * input);
void sigint_handler(int signum);
void sigtstp_handler(int signum);
char *get_command_name(int pid);
void insert_into_list(Slist **head, int num, char *input_string);
void print_list(Slist **head);
void pipe_handler(char *input_string);