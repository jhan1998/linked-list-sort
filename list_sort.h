#ifndef _LINUX_LIST_SORT_H
#define _LINUX_LIST_SORT_H
#include "list.h"

typedef int __attribute__((nonnull(2,3))) (*cmp_func)(void *,
		struct list_head const *, struct list_head const *);

__attribute__((nonnull(2,3,4)))
static struct list_head *merge(void *priv, cmp_func cmp,
				struct list_head *a, struct list_head *b);

__attribute__((nonnull(2,3,4,5)))
static void merge_final(void *priv, cmp_func cmp, struct list_head *head,
			struct list_head *a, struct list_head *b);

__attribute__((nonnull(2,3)))
void list_sort(void *priv, struct list_head *head,
	       int (*cmp)(void *priv, struct list_head *a,
			  struct list_head *b));
#endif
