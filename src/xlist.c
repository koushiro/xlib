//
// Created by koushiro on 10/18/18.
//

#include <assert.h>

#include "xlist.h"
#include "xalloc.h"

xlist *xlist_create(void) {
    xlist *list;
    if ((list = xmalloc(sizeof(xlist))) == NULL) return NULL;

    list->head = NULL;
    list->tail = NULL;
    list->len = 0;
    list->dup = NULL;
    list->free = NULL;
    list->match = NULL;
    return list;
}

void xlist_destroy(xlist *list) {
    xlist_clear(list);
    xfree(list);
}

void xlist_clear(xlist *list) {
    if (list == NULL) return;

    xlist_node *next;
    xlist_node *cur = list->head;
    while (cur) {
        if (list->free) list->free(cur->value);
        next = cur->next;
        xfree(cur);
        cur = next;
    }
    list->head = list->tail = NULL;
    list->len = 0;
}

xlist *xlist_add_node_head(xlist *list, void *value) {
    assert(list && value);
    xlist_node *node;
    if ((node = xmalloc(sizeof(xlist_node))) == NULL) return NULL;

    node->value = value;
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
    return list;
}

xlist *xlist_add_node_tail(xlist *list, void *value) {
    assert(list && value);
    xlist_node *node;
    if ((node = xmalloc(sizeof(xlist_node))) == NULL) return NULL;

    node->value = value;
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
    return list;
}

xlist *xlist_insert_node(xlist *list, xlist_node *pos, int after, void *value) {
    assert(list && pos && value);
    xlist_node *node;
    if ((node = xmalloc(sizeof(xlist_node))) == NULL) return NULL;

    node->value = value;
    if (after) {
        // insert node after pos
        node->prev = pos;
        node->next = pos->next;
        if (pos == list->tail) {
            list->tail = node;
        }
    } else {
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
    return list;
}

void xlist_delete_node(xlist *list, xlist_node *node) {
    assert(list && node);
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
    if (list->free) list->free(node->value);
    xfree(node);
    list->len--;
}

xlist_node *xlist_search_node(xlist *list, void *value) {
    assert(list && value);
    xlist_iter iter;
    xlist_iter_rewind_head(list, &iter);

    xlist_node *node;
    while ((node = xlist_iter_next(&iter)) != NULL) {
        if (list->match) {
            if (list->match(node->value, value)) {
                return node;
            }
        } else {
            // If no 'match' method is set,
            // the 'value' pointer of every node is directly compared with the 'value' pointer.
            if (value == node->value) {
                return node;
            }
        }
    }
    return NULL;
}

xlist_iter *xlist_iter_create(xlist *list, xlist_iter_direction direction) {
    assert(list);
    xlist_iter *iter;
    if ((iter = xmalloc(sizeof(xlist_iter))) == NULL) return NULL;

    if (direction == BACKWARD) {
        iter->next = list->head;
    } else {
        // direction == FORWARD
        iter->next = list->tail;
    }
    iter->direction = direction;
    return iter;
}

void xlist_iter_destroy(xlist_iter *iter) {
    if (iter == NULL) return;
    xfree(iter);
}

xlist_node *xlist_iter_next(xlist_iter *iter) {
    assert(iter);
    xlist_node *cur_node = iter->next;
    if (cur_node) {
        if (iter->direction == BACKWARD) {
            iter->next = cur_node->next;
        } else {
            // directoin == FORWARD
            iter->next = cur_node->prev;
        }
    }
    return cur_node;
}

void xlist_iter_rewind_head(xlist *list, xlist_iter *iter) {
    assert(list && iter);
    iter->next = list->head;
    iter->direction = BACKWARD;
}

void xlist_iter_rewind_tail(xlist *list, xlist_iter *iter) {
    assert(list && iter);
    iter->next = list->tail;
    iter->direction = FORWARD;
}

xlist *xlist_dup(xlist *origin) {
    assert(origin);
    xlist *copy;
    if ((copy = xmalloc(sizeof(xlist))) == NULL) return NULL;

    copy->dup = origin->dup;
    copy->free = origin->free;
    copy->match = origin->match;

    xlist_iter origin_iter;
    xlist_iter_rewind_head(origin, &origin_iter);

    xlist_node *node;
    while ((node = xlist_iter_next(&origin_iter)) != NULL) {
        void *copy_value;
        if (copy->dup) {
            copy_value = copy->dup(node->value);
            if (copy_value == NULL) goto CLEANUP;
        } else {
            // If no 'dup' method is set,
            // the same 'value' pointer of the original node is used as 'value' of the copied node.
            copy_value = node->value;
        }

        if (xlist_add_node_tail(copy, copy_value) == NULL) goto CLEANUP;
    }
    return copy;

    CLEANUP:
    {
        xlist_destroy(copy);
        return NULL;
    }
}

void xlist_join(xlist *list, xlist *other) {
    assert(list && other);
    if (other->len == 0) return;

    if (list->len == 0) {
        list->head = other->head;
        list->tail = other->tail;
    } else {
        list->tail->next = other->head;
        other->head->prev = list->tail;
        list->tail = other->tail;
    }

    list->len += other->len;
    other->head = other->tail = NULL;
    other->len = 0;
}