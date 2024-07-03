#include <stdio.h>
#include <stdlib.h>
#include "list.c"

void change_p(node_t **head);

int main()
{
    node_t *head = init_list(1);
    // push(head, 2);
    // push(head, 3);
    // push(head, 4);

    // unshift(&head, 10);
    // unshift(&head, 20);
    // int a = shift(&head);
    // printf("%d,%p\n", a, head);

    // int last1 = pop(head);
    // printf("%d\n", last1);
    // int last2 = pop(head);
    // printf("%d\n", last2);

    int remove_item = remove_by_index(&head, 1);
    printf("%d\n", remove_item);

    print_list(head);

    // free_list(head);
    return 0;
}