#include "main.h"

void insert_into_list(Slist **head, int num, char *input_string)
{
    Slist *new_node = malloc(sizeof(Slist));

    if(new_node == NULL)
        printf("Error\n");

    new_node->num = num;
    new_node->status = strdup("Stopped");
    new_node->cmd = strdup(input_string);
    new_node->next = *head;
    *head = new_node;

}

void print_list(Slist **head)
{
    Slist *current = *head;
    while(current) {
        printf("[%d]        %s      %s\n", current->num, current->status, current->cmd);
        current = current->next;
    }
}