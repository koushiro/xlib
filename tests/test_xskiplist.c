//
// Created by koushiro on 10/24/18.
//
#include <string.h>

#include "xalloc.h"
#include "xstr.h"
#include "xskiplist.h"
#include "xtest.h"

int main() {
    xskiplist *list = xskiplist_create(32);
//    xskiplist_insert_node(list, xstr_create("node1"), 1.0);
//    xskiplist_insert_node(list, xstr_create("node2"), 2.0);
//    xskiplist_insert_node(list, xstr_create("node3"), 3.0);
//    xskiplist_insert_node(list, xstr_create("node4"), 4.0);
//    xskiplist_insert_node(list, xstr_create("node5"), 5.0);
    xskiplist_destroy(list);
    test_report();
    return 0;
}