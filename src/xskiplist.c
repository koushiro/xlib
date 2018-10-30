//
// Created by koushiro on 10/18/18.
//

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "xskiplist.h"
#include "xalloc.h"

static xskiplist_node* xskiplist_node_create(void *value, size_t level) {
    xskiplist_node *node = xmalloc(sizeof(xskiplist_node) +
                                   (level+1) * sizeof(xskiplist_node*));
    node->value = value;
    node->level = level;
    memset(node->forward, 0, (level+1) * sizeof(xskiplist_node*));
    return node;
}

static void xskiplist_node_destroy(xskiplist *list, xskiplist_node *node) {
    assert(list && list->free && node);
    list->free(node->value);
    xfree(node);
}

static size_t gen_random_level(size_t max_level, double probability) {
    assert(probability > 0.0 && probability < 1.0);
    srand((unsigned int)time(NULL));
    size_t level = 0;
    double random = (double)rand() / RAND_MAX;
    while (random < probability && level < max_level) {
        ++level;
        random = (double)rand() / RAND_MAX;
    }
    return level;
}

// ============================================================================

xskiplist* xskiplist_create() {
    xskiplist *list = xmalloc(sizeof(xskiplist));
    list->head = xskiplist_node_create(NULL, XSKIPLIST_MAX_LEVEL);
    list->length = 0;
    list->level = 0;
    return list;
}

void xskiplist_destroy(xskiplist *list) {
    assert(list && list->free);
    xskiplist_node *node = list->head->forward[0];
    xskiplist_node *next = NULL;
    while (node) {
        next = node->forward[0];
        xskiplist_node_destroy(list, node);
        node = next;
    }
    xskiplist_node_destroy(list, list->head);
    xfree(list);
}

xskiplist_node *xskiplist_insert_node(xskiplist *list, void *value) {
    assert(list && list->cmp && value);

    xskiplist_node *current = xskiplist_head(list);
    // all nodes needed to update forward pointer (before insert position).
    xskiplist_node *update[XSKIPLIST_MAX_LEVEL+1];// contains level 0.
    memset(update, 0, (XSKIPLIST_MAX_LEVEL+1) * sizeof(xskiplist_node*));
    // search the insert position.
    for (int i = list->level; i >= 0; --i) {
        while (current->forward[i] && list->cmp(current->forward[i]->value, value) == -1) {
            current = current->forward[i];
        }
        update[i] = current;     // when insert node firstly, update[i] = list->header.
    }

    // reach level 0 and forward pointer to right, which is desired position to insert node.
    current = current->forward[0];
    if (current && list->cmp(current->value, value) == 0) {    // same value is existed.
        return current;
    }

    size_t random_level = gen_random_level(XSKIPLIST_MAX_LEVEL, XSKIPLIST_RANDOM_PROBABILITY);
    // If random level is greater than list's current level (node with highest level inserted in list so far),
    // initialize update value with pointer to head of list for further use.
    if (random_level > list->level) {
        for (int i = random_level+1; i >= list->level+1; --i) {
            update[i] = xskiplist_head(list);
        }
        list->level = random_level;    // current max level (<= list->max_level).
    }
    // adjust forward pointer about inserted node and update nodes.
    xskiplist_node *node = xskiplist_node_create(value, random_level);
    for (int i = random_level; i >= 0; --i) {
        node->forward[i] = update[i]->forward[i];
        update[i]->forward[i] = node;
    }

    ++list->length;
    return node;
}

int xskiplist_delete_node(xskiplist *list, void *value) {
    assert(list && list->cmp && list->free && value);
    if (xskiplist_len(list) == 0) return -1;

    xskiplist_node *current = xskiplist_head(list);
    // all nodes needed to update forward pointer (before delete position).
    xskiplist_node *update[XSKIPLIST_MAX_LEVEL+1];// contains level 0.
    memset(update, 0, (XSKIPLIST_MAX_LEVEL+1) * sizeof(xskiplist_node*));
    // search the delete position.
    for (int i = list->level; i >= 0; --i) {
        while (current->forward[i] && list->cmp(current->forward[i]->value, value) == -1) {
            current = current->forward[i];
        }
        update[i] = current;
    }

    // reach level 0 and forward pointer to right, which is possibly desired node.
    current = current->forward[0];
    if (current && list->cmp(current->value, value) == 0) {
        for (int i = list->level; i >= 0; --i) {
            if (update[i]->forward[i] == current) {
                update[i]->forward[i] = current->forward[i];
            }
        }
        // remove levels having no elements.
        while (list->level > 0 && list->head->forward[list->level] == NULL) {
            --list->level;
        }
        --list->length;
        xskiplist_node_destroy(list, current);
        return 0;
    }
    return -1;
}

xskiplist_node* xskiplist_search_node(xskiplist *list, void *value) {
    assert(list && list->cmp && value);

    xskiplist_node *current = xskiplist_head(list);
    // from the high level to low level
    for (int i = list->level; i >= 0; --i) {
        while (current->forward[i] && list->cmp(current->forward[i]->value, value) == -1) {
            current = current->forward[i];
        }
    }

    // reach level 0 and forward pointer to right, which is possibly desired node.
    current = current->forward[0];
    return current && list->cmp(current->value, value) == 0
            ? current : NULL;
}
