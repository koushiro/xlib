//
// Created by koushiro on 10/18/18.
//

#include <assert.h>
#include <stdio.h>

#include "xlist.h"
#include "xalloc.h"

static xlist_node* xlist_node_create(void* value) {
    xlist_node* node = xmalloc(sizeof(xlist_node));
    node->value = value;
    node->prev = NULL;
    node->next = NULL;
    return node;
}

static void xlist_node_destroy(xlist* list, xlist_node* node) {
    assert(list && list->free && node);
    list->free(node->value);
    xfree(node);
}

// ============================================================================

xlist* xlist_create(void) {
    xlist* list = xmalloc(sizeof(xlist));
    list->head = NULL;
    list->tail = NULL;
    list->len = 0;
    list->dup = NULL;
    list->free = NULL;
    list->cmp = NULL;
    return list;
}

void xlist_destroy(xlist* list) {
    assert(list);
    xlist_clear(list);
    xfree(list);
}

void xlist_clear(xlist* list) {
    assert(list);
    if (xlist_len(list) == 0) return;

    assert(list->free);

    xlist_node* next;
    xlist_node* cur = xlist_first(list);
    while (cur) {
        list->free(xlist_node_value(cur));
        next = xlist_node_next(cur);
        xfree(cur);
        cur = next;
    }
    list->head = list->tail = NULL;
    list->len = 0;
}

xlist* xlist_dup(xlist* origin) {
    assert(origin && origin->dup && origin->free && origin->cmp);
    xlist* copy = xmalloc(sizeof(xlist));
    copy->head = copy->tail = NULL;
    copy->len = 0;
    copy->dup = origin->dup;
    copy->free = origin->free;
    copy->cmp = origin->cmp;

    xlist_iter origin_iter;
    xlist_iter_rewind_head(origin, &origin_iter);

    xlist_node* node;
    while ((node = xlist_iter_next(&origin_iter)) != NULL) {
        void* copy_value;
        copy_value = copy->dup(xlist_node_value(node));
        if (copy_value == NULL) goto CLEANUP;
        if (xlist_add_node_tail(copy, copy_value) == NULL) goto CLEANUP;
    }
    return copy;

    CLEANUP:
    {
        xlist_destroy(copy);
        return NULL;
    }
}

xlist* xlist_join(xlist* list, xlist* other) {
    assert(list && other);
    if (other->len == 0) return list;

    if (list->len == 0) {
        list->head = xlist_first(other);
        list->tail = xlist_last(other);
    } else {
        list->tail->next = xlist_first(other);
        other->head->prev = xlist_last(list);
        list->tail = xlist_last(other);
    }

    list->len += other->len;
    other->head = other->tail = NULL;
    other->len = 0;
    return list;
}

xlist_node* xlist_add_node_head(xlist* list, void* value) {
    assert(list && value);
    xlist_node* node = xlist_node_create(value);
    if (list->len == 0) {
        list->head = list->tail = node;
        node->prev = node->next = NULL;
    } else {
        node->prev = NULL;
        node->next = list->head;
        list->head->prev = node;
        list->head = node;
    }
    list->len++;
    return node;
}

xlist_node* xlist_add_node_tail(xlist* list, void* value) {
    assert(list && value);
    xlist_node* node = xlist_node_create(value);
    if (list->len == 0) {
        list->head = list->tail = node;
        node->prev = node->next = NULL;
    } else {
        node->prev = list->tail;
        node->next = NULL;
        list->tail->next = node;
        list->tail = node;
    }
    list->len++;
    return node;
}

xlist_node* xlist_insert_node(xlist* list, xlist_node* pos, xlist_node_relative_pos after, void* value) {
    assert(list && pos && value);
    xlist_node* node = xlist_node_create(value);
    if (after == AFTER) {
        // insert node after pos
        node->prev = pos;
        node->next = pos->next;
        if (pos == list->tail) {
            list->tail = node;
        }
    } else {
        // after == BEFORE
        // insert node before pos
        node->prev = pos->prev;
        node->next = pos;
        if (pos == list->head) {
            list->head = node;
        }
    }
    if (node->prev) node->prev->next = node;
    if (node->next) node->next->prev = node;
    list->len++;
    return node;
}

void xlist_delete_node(xlist* list, xlist_node* node) {
    assert(list && list->free && node);
    if (node->prev) {
        node->prev->next = node->next;
    } else {
        list->head = node->next;
    }
    if (node->next) {
        node->next->prev = node->prev;
    } else {
        list->tail = node->prev;
    }
    xlist_node_destroy(list, node);
    list->len--;
}

xlist_node* xlist_search_node(xlist* list, void* value) {
    assert(list && list->cmp && value);
    xlist_iter iter;
    xlist_iter_rewind_head(list, &iter);

    xlist_node* node;
    while ((node = xlist_iter_next(&iter)) != NULL) {
        if (list->cmp(xlist_node_value(node), value) == 0) {
            return node;
        }
    }
    return NULL;
}

xlist_iter* xlist_iter_create(xlist* list, xlist_iter_direction direction) {
    assert(list);
    xlist_iter* iter = xmalloc(sizeof(xlist_iter));
    if (direction == FORWARD) {
        iter->node = xlist_first(list);
    } else {
        // direction == BACKWARD
        iter->node = xlist_last(list);
    }
    iter->direction = direction;
    return iter;
}

void xlist_iter_destroy(xlist_iter* iter) {
    xfree(iter);
}

xlist_node* xlist_iter_next(xlist_iter* iter) {
    assert(iter);
    xlist_node* cur_node = iter->node;
    if (cur_node) {
        if (iter->direction == FORWARD) {
            iter->node = xlist_node_next(cur_node);
        } else {
            // directoin == BACKWARD
            iter->node = xlist_node_prev(cur_node);
        }
    }
    return cur_node;
}

void xlist_iter_rewind_head(xlist* list, xlist_iter* iter) {
    assert(list && iter);
    iter->node = xlist_first(list);
    iter->direction = FORWARD;
}

void xlist_iter_rewind_tail(xlist* list, xlist_iter* iter) {
    assert(list && iter);
    iter->node = xlist_last(list);
    iter->direction = BACKWARD;
}
