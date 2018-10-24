//
// Created by koushiro on 10/24/18.
//

#include <assert.h>

#include "xstr.h"
#include "xskiplist.h"

int main() {
    xskiplist *list = xskiplist_create(32);
//    xskiplist_insert_node(list, xstr_create("node1"), 1.0);
//    xskiplist_insert_node(list, xstr_create("node2"), 2.0);
//    xskiplist_insert_node(list, xstr_create("node3"), 3.0);
//    xskiplist_insert_node(list, xstr_create("node4"), 4.0);
//    xskiplist_insert_node(list, xstr_create("node5"), 5.0);
    xskiplist_destroy(list);
    return 0;
}