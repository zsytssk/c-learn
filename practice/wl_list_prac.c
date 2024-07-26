#include <stdio.h>
#include <stdlib.h>
#include <wayland-server-core.h>

struct my_struct
{
    int data;
    struct wl_list link;
};

int main()
{
    struct wl_list my_list;
    wl_list_init(&my_list);

    struct my_struct *item1 = malloc(sizeof(struct my_struct));
    item1->data = 10;
    wl_list_insert(&my_list, &item1->link);

    struct my_struct *item2 = malloc(sizeof(struct my_struct));
    item2->data = 20;
    wl_list_insert(&my_list, &item2->link);

    struct my_struct *item3 = malloc(sizeof(struct my_struct));
    item3->data = 30;
    wl_list_insert(&my_list, &item3->link);

    struct my_struct *item;
    wl_list_for_each(item, &my_list, link)
    {
        printf("Data: %d\n", item->data);
    }

    wl_list_remove(&item1->link);
    free(item1);

    wl_list_remove(&item2->link);
    free(item2);

    wl_list_remove(&item3->link);
    free(item3);

    if (wl_list_empty(&my_list))
    {
        printf("List is empty\n");
    }

    return 0;
}
