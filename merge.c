#include <string.h>
#include "list.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct __element {
    char *value;
    struct list_head list;
} list_ele_t;

static struct list_head *get_middle(struct list_head *list)
{
    struct list_head *fast = list->next, *slow;
    list_for_each (slow, list) {
        if (fast->next == list || fast->next->next == list)
            break;
        fast = fast->next->next;
    }
    return slow;
}

static void list_merge(struct list_head *lhs,
                       struct list_head *rhs,
                       struct list_head *head)
{
    INIT_LIST_HEAD(head);
    
    while (!list_empty(lhs) && !list_empty(rhs)) {
        char *lv = list_entry(lhs->next, list_ele_t, list)->value;
        char *rv = list_entry(rhs->next, list_ele_t, list)->value;
        struct list_head *tmp = strcmp(lv, rv) <= 0 ? lhs->next : rhs->next;
        list_del(tmp);
        list_add_tail(tmp, head);
    }
    list_splice_tail(list_empty(lhs) ? rhs : lhs, head);
}

void list_merge_sort(struct list_head *head)
{
    if (list_empty(head) || list_is_singular(head))
        return;

    struct list_head left;
    struct list_head sorted;
    INIT_LIST_HEAD(&left);
    list_cut_position(&left, head, get_middle(head));
    list_merge_sort(&left);
    list_merge_sort(head);
    list_merge(&left, head, &sorted);
    INIT_LIST_HEAD(head);
    list_splice_tail(&sorted, head);
}

static bool validate(struct list_head *head, FILE *fp)
{
    struct list_head *node;
    list_for_each (node, head) {
        fprintf(fp ,"%s",list_entry(node, list_ele_t, list)->value);
        if (node->next == head)
            break;
        if (strcmp(list_entry(node, list_ele_t, list)->value,
                   list_entry(node->next, list_ele_t, list)->value) > 0)
            return false;
    }
    return true;
}

bool insert_head(char *s, struct list_head *head)
{
    list_ele_t *newh = malloc(sizeof(list_ele_t));
    if (!newh)
        return false;

    char *new_value = strdup(s);
    if (!new_value) {
        free(newh);
        return false;
    }

    newh->value = new_value;
    list_add_tail(&newh->list, head);

    return true;
}

void list_free(struct list_head * head) {
    for (struct list_head *node = (head)->next; node != (head);){
        struct list_head *tmp = node;
        node = node->next;
        list_del(tmp);
        list_ele_t *entry = list_entry(tmp, list_ele_t, list);
        free(entry->value);
        free(entry);
    }
}

int main(void)
{
    FILE *fp = fopen("cities.txt", "r");
    FILE *fw = fopen("check.txt", "w");
    if (!fp) {
        perror("failed to open cities.txt");
        exit(EXIT_FAILURE);
    }

    struct list_head list;
    INIT_LIST_HEAD(&list);

    char buf[256];
    while (fgets(buf, 256, fp))
        insert_head(buf, &list);
    fclose(fp);

    list_merge_sort(&list);
    assert(validate(&list, fw));
    fclose(fw);
    list_free(&list);

    return 0;
}
