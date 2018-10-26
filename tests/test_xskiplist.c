//
// Created by koushiro on 10/24/18.
//

#include <assert.h>
#include <math.h>
#include <string.h>

#include "xalloc.h"
#include "xskiplist.h"
#include "xtest.h"

int main() {
    xskiplist *list = xskiplist_create();
    test_assert("xskiplist_create", xskiplist_len(list) == 0);
    for (size_t i = 0; i < 50; ++i) {
        xskiplist_insert_node(list, (double)i);
    }
    test_assert("xskiplist_insert_node", xskiplist_len(list) == 50);
    size_t index = 0;
    xskiplist_node *current = xskiplist_head(list)->forward[0];
    while (current) {
        assert(fabs(current->value - (double)index) < EPSILON);
        ++index;
        current = current->forward[0];
    }

    xskiplist_node *node49 = xskiplist_search_node(list, 49);
    test_assert("xskiplist_search_node successfully",
        node49 && fabs(node49->value - 49) < EPSILON);

    xskiplist_node *node50 = xskiplist_search_node(list, 50);
    test_assert("xskiplist_search_node failed", node50 == NULL);

    int ret = xskiplist_delete_node(list, 49);
    test_assert("xskiplist_delete_node successfully",
        ret == 0 && xskiplist_len(list) == 49);

    ret = xskiplist_delete_node(list, 50);
    test_assert("xskiplist_delete_node failed",
                ret == -1 && xskiplist_len(list) == 49);

    xskiplist_destroy(list);

    test_report();
    return 0;
}