#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <time.h>
#define MT_SIZE 624
int mt[MT_SIZE];
int mti = 0;
uint32_t accum = 1;

typedef struct __node
{
    int value;
    struct __node *next;
} node_t;

static void list_display(node_t *list);

static inline void list_add_node_t(node_t **list, node_t *node_t)
{
    node_t->next = *list;
    *list = node_t;
}

static inline void list_concat(node_t **left, node_t *right)
{
    while (*left)
        left = &((*left)->next);
    *left = right;
}

node_t *get_list_tail(node_t **list)
{
    while ((*list))
    {
        if ((*list)->next == NULL)
            return (*list);
        list = &((*list)->next);
    }
    return (*list);
}

#define MAX_LEVELS 300

void quicksort_itr(node_t **list)
{
    if (!*list || !(*list)->next)
        return;

    node_t *stack[MAX_LEVELS];
    int stack_top = 0;
    stack[stack_top] = *list;
    node_t *result = NULL, *L = NULL, *R = NULL;

    while (stack_top >= 0)
    {
        L = stack[stack_top];
        R = get_list_tail(&L);
        if (L != R)
        {
            node_t *pivot = L;
            int value = pivot->value;
            node_t *p = pivot->next;
            pivot->next = NULL;

            node_t *left = NULL, *right = NULL;
            while (p)
            {
                node_t *n = p;
                p = p->next;
                list_add_node_t(n->value >= value ? &right : &left, n);
            }
            stack[stack_top++] = left;
            stack[stack_top++] = pivot;
            stack[stack_top] = right;
        }
        else
        {
            if (L)
                list_add_node_t(&result, L);
            stack_top--;
        }
    }
    *list = result;
}

static bool list_is_ordered(node_t *list)
{
    bool first = true;
    int value;
    while (list)
    {
        if (first)
        {
            value = list->value;
            first = false;
        }
        else
        {
            if (list->value < value)
                return false;
            value = list->value;
        }
        list = list->next;
    }
    return true;
}

static void list_display(node_t *list)
{
    printf("%s IN ORDER : ", list_is_ordered(list) ? "   " : "NOT");
    while (list)
    {
        printf("%d ", list->value);
        list = list->next;
    }
    printf("\n");
}

static void list_make_node_t(node_t **list, uint32_t n)
{
    node_t *nnode = malloc(sizeof(node_t));
    nnode->next = *list;
    nnode->value = n;
    *list = nnode;
}

static void list_free(node_t **list)
{
    while ((*list)->next)
    {
        node_t *tmp = *list;
        list = &tmp->next;
        free(tmp);
    }
    free(*list);
}
// Mersenne twister
void init(int seed)
{
    mt[0] = seed;
    for (int i = 1; i < MT_SIZE; i++)
    {
        mt[i] = (uint32_t)1812433253 * (mt[i - 1] ^ mt[i - 1] >> 30) + i;
    }
}

uint32_t exract_random()
{
    if (mti == 0)
    { //twist
        for (int i = 0; i < MT_SIZE; i++)
        {
            int y = (uint32_t)(mt[i] & 0x80000000) + (mt[(i + 1) % 624] & 0x7fffffff);
            mt[i] = (y >> 1) ^ mt[(i + 397) % 624];
            if (y % 2 != 0)
                mt[i] = mt[i] ^ 0x9908b0df;
        }
    }
    int res = mt[mti];
    res = res ^ res >> 11;
    res = res ^ res << 7 & 2636928640;
    res = res ^ res << 15 & 4022730752;
    res = res ^ res >> 18;
    mti = (mti + 1) % 624;
    return (uint32_t)res;
}

void init_cryptMT()
{
    for (int i = 0; i < 64; i++)
    {
        accum *= (exract_random() | 0x1);
    }
}

uint32_t getCryptMT()
{
    uint32_t num = 0;
    for (int i = 0; i < 4; i++)
    {
        accum *= (exract_random() | 0x1);
        num = num << 8;
        num |= (accum >> 24);
    }
    return num;
}

int main(int argc, char **argv)
{
    size_t count = 1000;
    struct timespec start, end;
    init(time(NULL));
    FILE *fp = fopen("list_itr.txt", "w");
    for (int i = 0; i < 1000; i++)
    {
        node_t *list = NULL;
        for (int j = 0; j < count; j++)
            list_make_node_t(&list, getCryptMT() % 1024);

        // list_display(list);
        clock_gettime(CLOCK_MONOTONIC, &start);
        quicksort_itr(&list);
        clock_gettime(CLOCK_MONOTONIC, &end);
        // list_display(list);

        if (!list_is_ordered(list))
        {
            return EXIT_FAILURE;
        }
        fprintf(fp, "%d %ld\n", i, end.tv_nsec - start.tv_nsec);
        list_free(&list);
    }
    fclose(fp);
    return EXIT_SUCCESS;
}
