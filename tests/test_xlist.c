//
// Created by koushiro on 10/24/18.
//

#include <string.h>

#include "xalloc.h"
#include "xlist.h"
#include "xstr.h"
#include "xtest.h"

int main() {
    xlist *list = xlist_create();
    test_assert("xlist_create",
        xlist_len(list) == 0 && xlist_first(list) == NULL && xlist_last(list) == NULL);
    xlist_destroy(list);

    list = xlist_create();
    xlist_set_free_func(list, (xlist_node_value_free_func)xstr_destroy);
    xlist_set_cmp_func(list, (xlist_node_value_cmp_func)xstr_cmp);
    xlist_set_dup_func(list, (xlist_node_value_dup_func)xstr_dup);

    xlist_node *node1 = xlist_add_node_head(list, xstr_create("node1"));
    test_assert("xlist_add_node_head",
        xlist_len(list) == 1 &&
            xlist_first(list) == node1 && xlist_last(list) == node1 &&
        xlist_node_next(node1) == NULL && xlist_node_prev(node1) == NULL &&
        memcmp(xlist_node_value(node1), "node1\0", 6) == 0);
    xlist_node *node4 = xlist_add_node_tail(list, xstr_create("node4"));
    test_assert("xlist_add_node_tail",
        xlist_len(list) == 2 &&
            xlist_first(list) == node1 && xlist_last(list) == node4 &&
            xlist_node_prev(node1) == NULL && xlist_node_next(node1) == node4 &&
            xlist_node_prev(node4) == node1 && xlist_node_next(node4) == NULL &&
            memcmp(xlist_node_value(node4), "node4\0", 6) == 0);

    xlist_node *node2 = xlist_insert_node(list, node1, AFTER, xstr_create("node2"));
    test_assert("xlist_insert_node AFTER",
        xlist_len(list) == 3 &&
        xlist_node_prev(node2) == node1 && xlist_node_next(node1) == node2 &&
        xlist_node_prev(node4) == node2 && xlist_node_next(node2) == node4 &&
        memcmp(xlist_node_value(node2), "node2\0", 6) == 0);

    xlist_node *node3 = xlist_insert_node(list, node4, BEFORE, xstr_create("node3"));
    test_assert("xlist_insert_node BEFORE",
        xlist_len(list) == 4 &&
        xlist_node_prev(node3) == node2 && xlist_node_next(node2) == node3 &&
        xlist_node_prev(node4) == node3 && xlist_node_next(node3) == node4 &&
        memcmp(xlist_node_value(node3), "node3\0", 6) == 0);

    xstr value = xstr_create("node2");
    xlist_node *search_node = xlist_search_node(list, value);
    test_assert("xlist_search_node",
        xlist_len(list) == 4 &&
        xlist_node_prev(node2) == node1 && xlist_node_next(node2) == node3 &&
        xlist_node_prev(node3) == node2 && xlist_node_next(node1) == node2 &&
        memcmp(xlist_node_value(search_node), "node2\0", 6) == 0);
    xstr_destroy(value);

    xlist_delete_node(list, search_node);
    test_assert("xlist_delete_node",
        xlist_len(list) == 3 &&
        xlist_node_prev(node3) == node1 && xlist_node_next(node1) == node3);

    xlist_iter *forward_iter = xlist_iter_create(list, FORWARD);
    test_assert("xlist_iter_create(FORWARD)", forward_iter->node == list->head);
    test_assert("xlist_iter_next",
        xlist_iter_next(forward_iter) == node1 && xlist_iter_next(forward_iter) == node3 &&
        xlist_iter_next(forward_iter) == node4 && xlist_iter_next(forward_iter) == NULL);

    xlist_iter_rewind_head(list, forward_iter);
    test_assert("xlist_iter_rewind_head",
        forward_iter->node == list->head && forward_iter->direction == FORWARD &&
        xlist_iter_next(forward_iter) == node1 && xlist_iter_next(forward_iter) == node3 &&
        xlist_iter_next(forward_iter) == node4 && xlist_iter_next(forward_iter) == NULL);
    xlist_iter_destroy(forward_iter);

    xlist_iter backward_iter;
    xlist_iter_rewind_tail(list, &backward_iter);
    test_assert("create xlist_iter(BACKWARD) in stack and init xlist_iter by calling xlist_iter_rewind_tail",
        backward_iter.node == list->tail && backward_iter.direction == BACKWARD &&
        xlist_iter_next(&backward_iter) == node4 && xlist_iter_next(&backward_iter) == node3 &&
        xlist_iter_next(&backward_iter) == node1 && xlist_iter_next(&backward_iter) == NULL);

    xlist *dup_list = xlist_dup(list);
    xlist_iter iter;
    xlist_iter_rewind_head(dup_list, &iter);
    test_assert("xlist_dup", xlist_len(dup_list) == 3);
    list = xlist_join(list, dup_list);
    test_assert("xlist_join", xlist_len(list) == 6);
    xlist_destroy(dup_list);
    xlist_destroy(list);

    test_report();
    return 0;
}