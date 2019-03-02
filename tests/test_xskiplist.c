//
// Created by koushiro on 10/24/18.
//

#include <assert.h>
#include <math.h>
#include <string.h>

#include "xalloc.h"
#include "xskiplist.h"
#include "xtest.h"

#define EPSILON 1e-6

static int double_cmp_func(void* ptr, void* value) {
    assert(ptr && value);
    double lhs = *(double*) ptr;
    double rhs = *(double*) value;
    if (fabs(lhs - rhs) < EPSILON) return 0;
    return lhs < rhs ? -1 : 1;
}

int main() {
    double* arr[50];
    for (size_t i = 0; i < 50; ++i) {
        arr[i] = xmalloc(sizeof(double));
        *arr[i] = (double) i;
    }

    xskiplist* list = xskiplist_create();
    xskiplist_set_free_func(list, xfree);
    xskiplist_set_cmp_func(list, double_cmp_func);
    test_assert("xskiplist_create", xskiplist_len(list) == 0);

    for (size_t i = 0; i < 50; ++i) {
        xskiplist_insert_node(list, arr[i]);
    }
    test_assert("xskiplist_insert_node", xskiplist_len(list) == 50);
    size_t index = 0;
    xskiplist_node* current = xskiplist_head(list)->forward[0];
    while (current) {
//        printf("lhs = %f, rhs = %f\n", *(double*)current->value, *arr[index]);
        assert(double_cmp_func(current->value, arr[index]) == 0);
        ++index;
        current = current->forward[0];
    }

    xskiplist_node* node49 = xskiplist_search_node(list, arr[49]);
    test_assert("xskiplist_search_node successfully",
                node49 && double_cmp_func(node49->value, arr[49]) == 0);

    double search_value = 50.;
    xskiplist_node* node50 = xskiplist_search_node(list, &search_value);
    test_assert("xskiplist_search_node failed", node50 == NULL);

    int ret = xskiplist_delete_node(list, arr[49]);
    test_assert("xskiplist_delete_node successfully",
                ret == 0 && xskiplist_len(list) == 49);

    double delete_value = 50.;
    ret = xskiplist_delete_node(list, &delete_value);
    test_assert("xskiplist_delete_node failed",
                ret == -1 && xskiplist_len(list) == 49);

    xskiplist_destroy(list);

    test_report();
    return 0;
}