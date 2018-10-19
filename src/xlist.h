//
// Created by koushiro on 10/18/18.
//

// References from https://github.com/antirez/redis/blob/unstable/src/adlist.h

#pragma once

#include <stddef.h>

typedef struct list_node {
    struct list_node *prev;
    struct list_node *next;
    void *value;
} xlist_node;

typedef enum {
    BACKWARD = 0,
    FORWARD = 1,
} xlist_iter_direction;

typedef struct list_iter {
    struct list_node *next;
    xlist_iter_direction direction;
} xlist_iter;

typedef void*(*dup_func)(void *ptr);
typedef void(*free_func)(void *ptr);
typedef int(*match_func)(void *ptr, void *value);

typedef struct list {
    struct list_node *head;
    struct list_node *tail;
    size_t len;
    dup_func dup;
    free_func free;
    match_func match;
} xlist;

static inline xlist_node* xlist_first(xlist l) { return l.head; }
static inline xlist_node* xlist_last(xlist l) { return l.tail; }
static inline size_t xlist_len(xlist l) { return l.len; }

static inline xlist_node* xlist_node_prev(xlist_node *n) { return n->prev; }
static inline xlist_node* xlist_node_next(xlist_node *n) { return n->next; }
static inline void* xlist_node_value(xlist_node *n) { return n->value; }

static inline void xlist_set_dup_func(xlist l, dup_func dup) { l.dup = dup; }
static inline void xlist_set_free_func(xlist l, free_func free) { l.free = free; }
static inline void xlist_set_match_func(xlist l, match_func match) { l.match = match; }

static inline dup_func xlist_get_dup_func(xlist l) { return l.dup; }
static inline free_func xlist_get_free_func(xlist l) { return l.free; }
static inline match_func xlist_get_match_func(xlist l) { return l.match; }

xlist* xlist_create(void);
void xlist_release(xlist *list);
void xlist_clear(xlist *list);

xlist* xlist_add_node_head(xlist *list, void *value);
xlist* xlist_add_node_tail(xlist *list, void *value);
xlist* xlist_insert_node(xlist *list, xlist_node *pos, int after, void *value);
void xlist_delete_node(xlist *list, xlist_node *node);
xlist_node* xlist_search_node(xlist *list, void *value);

xlist_iter* xlist_iter_create(xlist *list, xlist_iter_direction direction);
xlist_node* xlist_iter_next(xlist_iter *iter);
void xlist_iter_release(xlist_iter *iter);
void xlist_iter_rewind_head(xlist *list, xlist_iter *iter);
void xlist_iter_rewind_tail(xlist *list, xlist_iter *iter);

xlist* xlist_duplicate(xlist *origin);
void xlist_join(xlist *list, xlist *other);

xlist_node* xlist_index(xlist *list, size_t index);