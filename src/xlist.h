//
// Created by koushiro on 10/18/18.
//

// References from https://github.com/antirez/redis/blob/unstable/src/adlist.h

#pragma once

#include <stddef.h>

typedef struct xlist_node {
    struct xlist_node *prev;
    struct xlist_node *next;
    void *value;
} xlist_node;

typedef enum {
    FORWARD = 0,    // head --> tail
    BACKWARD = 1,   // tail --> head
} xlist_iter_direction;

typedef struct xlist_iter {
    struct xlist_node *node;
    xlist_iter_direction direction;
} xlist_iter;

typedef void*(*dup_func)(void *ptr);
typedef void(*free_func)(void *ptr);
typedef int(*match_func)(void *ptr, void *value);

typedef struct xlist {
    struct xlist_node *head;
    struct xlist_node *tail;
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

static inline void xlist_set_dup_func(xlist list, dup_func dup) { list.dup = dup; }
static inline void xlist_set_free_func(xlist list, free_func free) { list.free = free; }
static inline void xlist_set_match_func(xlist list, match_func match) { list.match = match; }

static inline dup_func xlist_get_dup_func(xlist list) { return list.dup; }
static inline free_func xlist_get_free_func(xlist list) { return list.free; }
static inline match_func xlist_get_match_func(xlist list) { return list.match; }

xlist* xlist_create(void);
void xlist_destroy(xlist *list);
void xlist_clear(xlist *list);

xlist* xlist_add_node_head(xlist *list, void *value);
xlist* xlist_add_node_tail(xlist *list, void *value);
xlist* xlist_insert_node(xlist *list, xlist_node *pos, int after, void *value);
void xlist_delete_node(xlist *list, xlist_node *node);
xlist_node* xlist_search_node(xlist *list, void *value);

xlist* xlist_dup(xlist *origin);
void xlist_join(xlist *list, xlist *other);

xlist_iter* xlist_iter_create(xlist *list, xlist_iter_direction direction);
void xlist_iter_destroy(xlist_iter *iter);
xlist_node* xlist_iter_next(xlist_iter *iter);
void xlist_iter_rewind_head(xlist *list, xlist_iter *iter);
void xlist_iter_rewind_tail(xlist *list, xlist_iter *iter);
