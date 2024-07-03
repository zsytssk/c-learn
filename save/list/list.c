#include <stdio.h>
#include <stdlib.h>

typedef struct node
{
    int val;
    struct node *next;
} node_t;

node_t *init_list(int val)
{
    node_t *head = NULL;
    head = (node_t *)malloc(sizeof(node_t));

    head->val = val;
    head->next = NULL;
}

int free_list(node_t *head)
{

    if (head->next != NULL)
    {
        free_list(head->next);
    }

    free(head);
}

void print_list(node_t *head)
{
    node_t *cur = head;

    printf("list: ");
    while (cur != NULL)
    {
        printf("%d ", cur->val);
        cur = cur->next;
    }

    printf("\n");
}

void push(node_t *head, int val)
{
    node_t *cur = head;

    while (cur->next != NULL)
    {
        cur = cur->next;
    }

    cur->next = init_list(val);
}

void unshift(node_t **head, int val)
{
    node_t *new_head = init_list(val);
    new_head->next = (*head);
    *head = new_head;
}

int shift(node_t **head)
{
    if (*head == NULL)
    {
        return -1;
    }
    node_t *free_head = *head;
    *head = (*head)->next;
    int val = (free_head)->val;
    free(free_head);

    return val;
}

int pop(node_t *head)
{
    if (head == NULL)
    {
        return -1;
    }

    if (head->next == NULL)
    {
        int val = head->val;
        free(head);
        return val;
    }

    node_t *before_last = head;
    while (before_last->next->next != NULL)
    {
        before_last = before_last->next;
    }

    node_t *last = before_last->next;
    before_last->next = NULL;
    int val = last->val;
    free(last);
    return val;
}

int remove_by_index(node_t **head, int index)
{

    if (index == 0)
    {
        return shift(head);
    }
    node_t *before = *head;
    int cur_index = 0;

    while (1)
    {
        if (cur_index == index - 1 || before->next == NULL)
        {
            break;
        }
        cur_index++;
        before = before->next;
    }
    if (before->next == NULL)
    {
        return -1;
    }

    node_t *remove_item = before->next;
    before->next = before->next->next;
    int val = remove_item->val;
    free(remove_item);
    return val;
}

int remove_by_value(node_t **head, int val)
{
    node_t *cur = *head;
    int index = 0;

    if (cur->val == val)
    {
        return pop(head);
    }

    while (1)
    {
        if (cur->next == NULL || cur->next->val == val)
        {
            break;
        }
        cur = cur->next;
        index++;
    }
    if (cur->next == NULL)
    {
        return -1;
    }

    node_t *remove_item = cur->next;
    int res = remove_item->val;
    cur->next = cur->next->next;
    free(remove_item);

    return res;
}