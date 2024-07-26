
#include <stddef.h>

/**
 * Iterates over a list.
 *
 * This macro expresses a for-each iterator for wl_list. Given a list and
 * wl_list link member name (often named `link` by convention), this macro
 * assigns each element in the list to \p pos, which can then be referenced in
 * a trailing code block. For example, given a wl_list of `struct message`
 * elements:
 *
 * \code
 * struct message {
 *         char *contents;
 *         wl_list link;
 * };
 *
 * struct wl_list *message_list;
 * // Assume message_list now "contains" many messages
 *
 * struct message *m;
 * wl_list_for_each(m, message_list, link) {
 *         do_something_with_message(m);
 * }
 * \endcode
 *
 * \param pos Cursor that each list element will be assigned to
 * \param head Head of the list to iterate over
 * \param member Name of the link member within the element struct
 *
 * \relates wl_list
 */
#define wl_list_for_each(pos, head, member)                \
    for (pos = wl_container_of((head)->next, pos, member); \
         &pos->member != (head);                           \
         pos = wl_container_of(pos->member.next, pos, member))

/**
 * Iterates over a list, safe against removal of the list element.
 *
 * \note Only removal of the current element, \p pos, is safe. Removing
 *       any other element during traversal may lead to a loop malfunction.
 *
 * \sa wl_list_for_each()
 *
 * \param pos Cursor that each list element will be assigned to
 * \param tmp Temporary pointer of the same type as \p pos
 * \param head Head of the list to iterate over
 * \param member Name of the link member within the element struct
 *
 * \relates wl_list
 */
#define wl_list_for_each_safe(pos, tmp, head, member)           \
    for (pos = wl_container_of((head)->next, pos, member),      \
        tmp = wl_container_of((pos)->member.next, tmp, member); \
         &pos->member != (head);                                \
         pos = tmp,                                             \
        tmp = wl_container_of(pos->member.next, tmp, member))

/**
 * Iterates backwards over a list.
 *
 * \sa wl_list_for_each()
 *
 * \param pos Cursor that each list element will be assigned to
 * \param head Head of the list to iterate over
 * \param member Name of the link member within the element struct
 *
 * \relates wl_list
 */
#define wl_list_for_each_reverse(pos, head, member)        \
    for (pos = wl_container_of((head)->prev, pos, member); \
         &pos->member != (head);                           \
         pos = wl_container_of(pos->member.prev, pos, member))

/**
 * Iterates backwards over a list, safe against removal of the list element.
 *
 * \note Only removal of the current element, \p pos, is safe. Removing
 *       any other element during traversal may lead to a loop malfunction.
 *
 * \sa wl_list_for_each()
 *
 * \param pos Cursor that each list element will be assigned to
 * \param tmp Temporary pointer of the same type as \p pos
 * \param head Head of the list to iterate over
 * \param member Name of the link member within the element struct
 *
 * \relates wl_list
 */
#define wl_list_for_each_reverse_safe(pos, tmp, head, member)   \
    for (pos = wl_container_of((head)->prev, pos, member),      \
        tmp = wl_container_of((pos)->member.prev, tmp, member); \
         &pos->member != (head);                                \
         pos = tmp,                                             \
        tmp = wl_container_of(pos->member.prev, tmp, member))

struct wl_list
{
    /** Previous list element */
    struct wl_list *prev;
    /** Next list element */
    struct wl_list *next;
};

void wl_list_init(struct wl_list *list)
{
    list->prev = list;
    list->next = list;
}

void wl_list_insert(struct wl_list *list, struct wl_list *elm)
{
    elm->prev = list;
    elm->next = list->next;
    list->next = elm;
    elm->next->prev = elm;
}

void wl_list_remove(struct wl_list *elm)
{
    elm->prev->next = elm->next;
    elm->next->prev = elm->prev;
    elm->next = NULL;
    elm->prev = NULL;
}

int wl_list_length(const struct wl_list *list)
{
    struct wl_list *e;
    int count;

    count = 0;
    e = list->next;
    while (e != list)
    {
        e = e->next;
        count++;
    }

    return count;
}

int wl_list_empty(const struct wl_list *list)
{
    return list->next == list;
}

void wl_list_insert_list(struct wl_list *list, struct wl_list *other)
{
    if (wl_list_empty(other))
        return;

    other->next->prev = list;
    other->prev->next = list->next;
    list->next->prev = other->prev;
    list->next = other->next;
}