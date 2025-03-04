#include "main.h"

void insert_into_list(Slist **head, int num, int pid, char *cmd, char *status)
{
    Slist *new_node = malloc(sizeof(Slist));

    if(new_node == NULL) {
        printf("Error: Memory allocation failed\n");
        return;
    }

    new_node->num = num;
    new_node->pid = pid;
    new_node->status = strdup(status);
    new_node->cmd = strdup(cmd);
    new_node->next = *head;
    *head = new_node;

    printf("[%d] %d %s %s\n", num, pid, status, cmd);
}

// Find a job by process ID
Slist* find_job_by_pid(Slist *head, int pid)
{
    Slist *current = head;
    while (current != NULL) {
        if (current->pid == pid) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

// Find a job by job number
Slist* find_job_by_num(Slist *head, int num)
{
    Slist *current = head;
    while (current != NULL) {
        if (current->num == num) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

// Print all jobs
void print_jobs(Slist *head)
{
    Slist *current = head;
    if (current == NULL) {
        printf("No jobs\n");
        return;
    }
    
    while (current != NULL) {
        printf("[%d] %d %s %s\n", current->num, current->pid, current->status, current->cmd);
        current = current->next;
    }
}

// Remove a job from the list
void remove_job(Slist **head, int pid)
{
    Slist *current = *head;
    Slist *prev = NULL;
    
    // If head node itself holds the pid to be deleted
    if (current != NULL && current->pid == pid) {
        *head = current->next;   // Changed head
        free(current->cmd);      // Free memory
        free(current->status);   // Free memory
        free(current);           // Free memory
        return;
    }
    
    // Search for the pid to be deleted
    while (current != NULL && current->pid != pid) {
        prev = current;
        current = current->next;
    }
    
    // If pid was not present in linked list
    if (current == NULL) return;
    
    // Unlink the node from linked list
    prev->next = current->next;
    
    // Free memory
    free(current->cmd);
    free(current->status);
    free(current);
}

// Get the last (highest) job number
int get_last_job_num(Slist *head)
{
    if (head == NULL) return 0;
    
    int max_num = 0;
    Slist *current = head;
    
    while (current != NULL) {
        if (current->num > max_num) {
            max_num = current->num;
        }
        current = current->next;
    }
    
    return max_num;
}

// Get the pid of the most recently added job
int get_last_job_pid(Slist *head)
{
    if (head == NULL) return 0;
    return head->pid;
}
