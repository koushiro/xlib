//
// Created by koushiro on 10/18/18.
//

#include <assert.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "xskiplist.h"
#include "xalloc.h"

static size_t gen_random_level(size_t max_level, double probability) {
    assert(probability > 0.0 && probability < 1.0);
    srand((unsigned int)time(NULL));
    size_t level = 1;
    double random = (double)rand() / RAND_MAX;
    while (random < probability && level < max_level) {
        ++level;
        random = (double)rand() / RAND_MAX;
    }
    return level;
}

// ============================================================================

xskiplist_node *xskiplist_node_create(xstr key, double value, size_t level) {
    assert(!isnan(value));
    xskiplist_node *node = xmalloc(sizeof(xskiplist_node) + level * sizeof(xskiplist_node));
    node->key = key;
    node->value = value;
    node->level = level;
    return node;
}

void xskiplist_node_destroy(xskiplist_node *node) {
    assert(node);
    xstr_destroy(node->key);
    xfree(node);
}

xskiplist* xskiplist_create(size_t max_level) {
    assert(max_level > 0);

    xskiplist *list = xmalloc(sizeof(xskiplist));

    list->tailer = xskiplist_node_create(xstr_create("tailer"), INFINITY, 0);
    list->header = xskiplist_node_create(xstr_create("header"), -INFINITY, max_level);
    for (size_t i = 0; i < max_level; ++i) {
        list->header->forward[i] = list->tailer;
    }
    list->length = 0;
    list->level = 0;
    list->max_level = max_level;
    return list;
}

void xskiplist_destroy(xskiplist *list) {
    assert(list);
    xskiplist_node *next = NULL;
    for (xskiplist_node *node = list->header->forward[0]; node != list->tailer; node = next) {
        next = node->forward[0];
        xskiplist_node_destroy(node);
    }
    xskiplist_node_destroy(list->header);
    xskiplist_node_destroy(list->tailer);
    xfree(list);
}

#define XSKIPLIST_NODE_LESS(value1, value2, key1, key2) \
    ((value1 < value2) || (fabs(value1 - value2) < 1e6 && xstr_cmp(key1, key2) < 0))

// The skiplist takes ownership of the passed xstr 'key', and 'key' is unique.
// If the key is existed, update the value of node, the function will return existed node.
xskiplist_node *xskiplist_insert_node(xskiplist *list, xstr key, double value) {
    assert(list && !isnan(value));

    xskiplist_node *update[list->max_level];    // all nodes needed to update forward pointer (before insert position).
    // search the insert position.
    xskiplist_node *current = list->header;
    for (size_t i = list->max_level - 1; i >= 0; --i) {
        while (current->forward[i] != list->tailer &&
                XSKIPLIST_NODE_LESS(current->forward[i]->value, value, current->forward[i]->key, key)) {
            current = current->forward[i];
        }
        update[i] = current;     // when level i inserting the first node, update[i] = list->header.
    }

    // when inserting the first node, current->forward[0] == list->header->forward[0] == list->tailer.
    current = current->forward[0];
    if (xstr_cmp(current->key, key) == 0) { // key is exist.
        current->value = value;
        return current;
    }

    size_t random_level = gen_random_level(list->max_level, XSKIPLIST_RANDOM_PROBABILITY);
    // If random level is greater than list's current level (node with highest level inserted in list so far),
    // initialize update value with pointer to header for further use.
    if (random_level > list->level) {
//        for (size_t i = random_level - 1; i > list->level; --i) {
//            update[i] = list->header; // TODO ??
//        }
        list->level = random_level;    // current max level (<= list->max_level).
    }
    // adjust forward pointer about inserted node and update nodes.
    xskiplist_node *node = xskiplist_node_create(key, value, random_level);
    for (size_t i = random_level - 1; i >= 0; --i) {
        node->forward[i] = update[i]->forward[i];
        update[i]->forward[i] = node;
    }

    ++list->length;
    return node;
}

int xskiplist_delete_node(xskiplist *list, xstr key, double value) {
    assert(list);

    xskiplist_node *update[list->max_level];    // all nodes needed to update forward pointer (before insert position).
    // search the delete position.
    xskiplist_node *current = list->header;
    for (size_t i = list->max_level - 1; i >= 0; --i) {
        while (current->forward[i] != list->tailer &&
               XSKIPLIST_NODE_LESS(current->forward[i]->value, value, current->forward[i]->key, key)) {
            current = current->forward[i];
        }
        update[i] = current;
    }
    current = current->forward[0];
    if (current != list->tailer &&
        (fabs(current->value - value) < 1e6) &&
        (xstr_cmp(current->key, key) == 0)) {
        for (size_t i = list->level - 1; i >= 0; --i) {
            if (update[i]->forward[i] == current) {
                update[i]->forward[i] = current->forward[i];
            }
        }
        // update max level of current nodes in the list.
        while (list->level > 1 && list->header->forward[list->level-1] == list->tailer) {
            --list->level;
        }
        --list->length;
        xskiplist_node_destroy(current);
        return 0;
    }
    return -1;
}

xskiplist_node* xskiplist_search_node(xskiplist *list, xstr key, double value) {
    assert(list);
    xskiplist_node *current = list->header;
    // from the high level to low level
    for (size_t i = list->level - 1; i >= 0; --i) {
        while (current->forward[i] != list->tailer &&
               XSKIPLIST_NODE_LESS(current->forward[i]->value, value, current->forward[i]->key, key)) {
            current = current->forward[i];
        }
    }

    current = current->forward[0];
    return (current != list->tailer) &&
            (fabs(current->value - value) < 1e6) &&
            (xstr_cmp(current->key, key) == 0)
            ? current : NULL;
}

size_t xskiplist_len(xskiplist *list) {
    assert(list);
    return list->length;
}