//
// Created by koushiro on 10/18/18.
//
// References from https://github.com/antirez/redis/blob/unstable/src/server.h
// and https://github.com/google/leveldb/blob/master/db/skiplist.h

#pragma once

#include <stddef.h>

#include "xstr.h"

#define XSKIPLIST_RANDOM_PROBABILITY    0.5

//            +----+      +----+      +----+      +----+      +----+      +----+      +----+      +----+
//  level3    |    | ---> |    | ---> |    | ---> |    | ---> |    | ---> |    | ---> |    | ---> |NULL| --+
//            +----+      +----+      +----+      +----+      +----+      +----+      +----+      +----+   |
//  level2    |    | ---> |    | ---> |    | ---> |    | ---> |    | ---> |    | ---> |    | ---> |NULL|   |
//            +----+      +----+      +----+      +----+      +----+      +----+      +----+      +----+   | - forward pointer
//  level1    |    | ---> |    | ---> |    | ---> |    | ---> |    | ---> |    | ---> |    | ---> |NULL|   |
//            +----+      +----+      +----+      +----+      +----+      +----+      +----+      +----+   |
//  level0    |    | ---> |    | ---> |    | ---> |    | ---> |    | ---> |    | ---> |    | ---> |NULL| --+
//            +----+      +----+      +----+      +----+      +----+      +----+      +----+      +----+
//   key      |head|      |key1|      |key2|      |key3|      |key4|      |key5|      |key6|      |tail|
//            +----+      +----+      +----+      +----+      +----+      +----+      +----+      +----+
//   value    |-INF|      |val1|      |val2|      |val3|      |val4|      |val5|      |val6|      | INF|
//            +----+      +----+      +----+      +----+      +----+      +----+      +----+      +----+
//               |           |                                                                      |
//             header     xskiplist_node                                                          tailer
//            |________________________________________________________________________________________|
//                                                          |
//                                                      xskiplist
//
typedef struct xskiplist_node {
    xstr key;                           // unique signature.
    double value;                       // value used for sorting.
    size_t level;                       // level count of the node. (could be 1 ~ xskiplist->max_level)
    struct xskiplist_node *forward[];   // forward[level].
} xskiplist_node;

typedef struct xskiplist {
    struct xskiplist_node *header;  // special node contains all level, header->forward[0] is the first node.
    struct xskiplist_node *tailer;  // special node contains none level.
    size_t length;                  // the size of nodes (not contains header and tailer).
    size_t level;                   // max level of current nodes. (could be 1 ~ max_level)
    size_t max_level;               // the level of header (level limitation).
} xskiplist;

xskiplist_node* xskiplist_node_create(xstr key, double value, size_t level);
void xskiplist_node_destroy(xskiplist_node *node);

xskiplist* xskiplist_create(size_t max_level);
void xskiplist_destroy(xskiplist *list);

xskiplist_node* xskiplist_insert_node(xskiplist *list, xstr key, double value);
int xskiplist_delete_node(xskiplist *list, xstr key, double value);
xskiplist_node* xskiplist_search_node(xskiplist *list, xstr key, double value);
size_t xskiplist_len(xskiplist *list);