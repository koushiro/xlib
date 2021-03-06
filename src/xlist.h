//
// Created by koushiro on 10/18/18.
//

// References from https://github.com/antirez/redis/blob/unstable/src/adlist.h

#pragma once

#include <stddef.h>

typedef struct xlist_node {
    struct xlist_node* prev;
    struct xlist_node* next;
    void* value;
} xlist_node;

typedef enum {
    FORWARD = 0,    // head --> tail
    BACKWARD = 1,   // head <-- tail
} xlist_iter_direction;

typedef struct xlist_iter {
    struct xlist_node* node;
    xlist_iter_direction direction;
} xlist_iter;

typedef void* (* xlist_node_value_dup_func)(void* ptr);
typedef void(* xlist_node_value_free_func)(void* ptr);
// \return 0: equal; -1: less; 1: greater.
typedef int(* xlist_node_value_cmp_func)(void* ptr, void* value);

typedef struct xlist {
    struct xlist_node* head;
    struct xlist_node* tail;
    size_t len;
    xlist_node_value_dup_func dup;
    xlist_node_value_free_func free;
    xlist_node_value_cmp_func cmp;
} xlist;

typedef enum {
    // for xlist_insert_node
        BEFORE = 0,
    AFTER = 1,
} xlist_node_relative_pos;

static inline xlist_node* xlist_first(xlist* list) { return list->head; }

static inline xlist_node* xlist_last(xlist* list) { return list->tail; }

static inline size_t xlist_len(xlist* list) { return list->len; }

static inline xlist_node* xlist_node_prev(xlist_node* node) { return node->prev; }

static inline xlist_node* xlist_node_next(xlist_node* node) { return node->next; }

static inline void* xlist_node_value(xlist_node* node) { return node->value; }

static inline void xlist_set_dup_func(xlist* list, xlist_node_value_dup_func dup) { list->dup = dup; }

static inline void xlist_set_free_func(xlist* list, xlist_node_value_free_func free) { list->free = free; }

static inline void xlist_set_cmp_func(xlist* list, xlist_node_value_cmp_func cmp) { list->cmp = cmp; }

xlist* xlist_create(void);
void xlist_destroy(xlist* list);
void xlist_clear(xlist* list);
xlist* xlist_dup(xlist* origin);
xlist* xlist_join(xlist* list, xlist* other);

xlist_node* xlist_add_node_head(xlist* list, void* value);
xlist_node* xlist_add_node_tail(xlist* list, void* value);
xlist_node* xlist_insert_node(xlist* list, xlist_node* pos, xlist_node_relative_pos after, void* value);
void xlist_delete_node(xlist* list, xlist_node* node);
xlist_node* xlist_search_node(xlist* list, void* value);

xlist_iter* xlist_iter_create(xlist* list, xlist_iter_direction direction);
void xlist_iter_destroy(xlist_iter* iter);
xlist_node* xlist_iter_next(xlist_iter* iter);
void xlist_iter_rewind_head(xlist* list, xlist_iter* iter);
void xlist_iter_rewind_tail(xlist* list, xlist_iter* iter);
