//
// Created by koushiro on 10/18/18.
//
// References from https://github.com/antirez/redis/blob/unstable/src/server.h
// and https://github.com/google/leveldb/blob/master/db/skiplist.h

#pragma once

#include <stddef.h>

#define XSKIPLIST_MAX_LEVEL             32
#define XSKIPLIST_RANDOM_PROBABILITY    0.5

//  level i   ...
//            +----+      +----+      +----+      +----+               +----+      +----+      +----+
//  level 3   |    | ---> |    | ---> |    | ---> |    | ---> ... ---> |    | ---> |    | ---> |NULL| --+
//            +----+      +----+      +----+      +----+               +----+      +----+      +----+   |
//  level 2   |    | ---> |    | ---> |    | ---> |    | ---> ... ---> |    | ---> |    | ---> |NULL|   |
//            +----+      +----+      +----+      +----+               +----+      +----+      +----+   | - forward pointer
//  level 1   |    | ---> |    | ---> |    | ---> |    | ---> ... ---> |    | ---> |    | ---> |NULL|   |
//            +----+      +----+      +----+      +----+               +----+      +----+      +----+   |
//  level 0   |    | ---> |    | ---> |    | ---> |    | ---> ... ---> |    | ---> |    | ---> |NULL| --+
//            +----+      +----+      +----+      +----+               +----+      +----+      +----+
//              |           |
//             head       xskiplist_node
//            |________________________________________________________________________________________|
//                                                          |
//                                                      xskiplist
// level 0 is the original list.
//

typedef struct xskiplist_node {
    void *value;                       // value used for sorting.
    size_t level;                       // level of the node. (could be 0 ~ XSKIPLIST_MAX_LEVEL)
    struct xskiplist_node *forward[];   // the size of forward array is level.
} xskiplist_node;

typedef void(*xskiplist_node_free_func)(void *ptr);
// \return 0: equal; -1: less; 1: greater.
typedef int(*xskiplist_node_cmp_func)(void *ptr, void *value);

typedef struct xskiplist {
    struct xskiplist_node *head;    // special node contains all level, head->forward[0] is the first node.
    size_t length;                  // the size of nodes (not contains header and tailer).
    size_t level;                   // max level of current nodes. (could be 0 ~ XSKIPLIST_MAX_LEVEL)
    xskiplist_node_free_func free;
    xskiplist_node_cmp_func cmp;
} xskiplist;

static inline xskiplist_node* xskiplist_head(xskiplist *list) { return list->head; }
static inline size_t xskiplist_len(xskiplist *list) { return list->length; }

static inline void xskiplist_set_free_func(xskiplist *list, xskiplist_node_free_func free) { list->free = free; }
static inline void xskiplist_set_cmp_func(xskiplist *list, xskiplist_node_cmp_func cmp) { list->cmp = cmp; }

xskiplist* xskiplist_create();
void xskiplist_destroy(xskiplist *list);

xskiplist_node* xskiplist_insert_node(xskiplist *list, void *value);
// delete the first node whose value == value.
int xskiplist_delete_node(xskiplist *list, void *value);
// search the first node whose value == value.
xskiplist_node* xskiplist_search_node(xskiplist *list, void *value);