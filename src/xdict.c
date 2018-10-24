//
// Created by koushiro on 10/18/18.
//

#include <assert.h>

#include "xdict.h"
#include "xalloc.h"

static void xdict_reset(xdict *dict);
static int xdict_init(xdict *dict, xdict_type *type, void *data);

// ============================================================================

static void xdict_reset(xdict *dict) {
    dict->table = NULL;
    dict->used = 0;
    dict->size = 0;
    dict->size_mask = 0;
}

static int xdict_init(xdict *dict, xdict_type *type, void *data) {
    xdict_reset(dict);
    dict->type = type;
    dict->data = data;
    return XDICT_OK;
}

// ============================================================================

xdict* xdict_create(xdict_type *type, void *data) {
    assert(type && data);
    xdict *dict = xmalloc(sizeof(xdict));
    xdict_init(dict, type, data);
    return dict;
}

void xdict_destroy(xdict *dict) {
    xdict_clear(dict);
    xfree(dict);
}

void xdict_clear(xdict *dict) {
    assert(dict);
    for (size_t i = 0; i < dict->size && dict->used > 0; ++i) {
        xdict_entry *entry, *next_entry;
        if ((entry = dict->table[i]) == NULL) continue;
        while (entry) {
            next_entry = entry->next;
            xdict_free_key(dict, entry);
            xdict_free_val(dict, entry);
            xfree(entry);
            dict->used--;
            entry = next_entry;
        }
    }
    xfree(dict->table);
    xdict_reset(dict);
}

int xdict_expand(xdict *dict, unsigned long size) {
    assert(dict);

    return XDICT_OK;
}

int xdict_add(xdict *dict, void *key, void *val) {
    assert(dict && key && val);

    return XDICT_OK;
}

int xdict_replace(xdict *dict, void *key, void *val) {
    assert(dict && key && val);

    return XDICT_OK;
}

int xdict_delete(xdict *dict, const void *key) {
    assert(dict && key);
    if (dict->size == 0) return XDICT_ERR;

    size_t hash = xdict_hash_key(dict, key);
    size_t index = hash & dict->size_mask;

    xdict_entry *prev_entry = NULL;
    for (xdict_entry *entry = dict->table[index]; entry; entry = entry->next) {
        if (xdict_cmp_key(dict, key, entry->key) == XDICT_OK) {
            if (prev_entry) {
                prev_entry->next = entry->next;
            } else {
                dict->table[hash] = entry->next;
            }
            xdict_free_key(dict, entry);
            xdict_free_val(dict, entry);
            xfree(entry);
            dict->used--;
            return XDICT_OK;
        }
        prev_entry = entry;
    }
    return XDICT_ERR;
}

xdict_entry* xdict_search(xdict *dict, const void *key) {
    assert(dict && key);
    if (dict->size == 0) return NULL;
    size_t hash = xdict_hash_key(dict, key);
    size_t index = hash & dict->size_mask;
    for (xdict_entry *entry = dict->table[index]; entry; entry = entry->next) {
        if (xdict_cmp_key(dict, key, entry->key) == XDICT_OK) {
            return entry;
        }
    }
    return NULL;
}

xdict_iter* xdict_iter_create(xdict *dict) {
    assert(dict);
    xdict_iter *iter = xmalloc(sizeof(xdict_iter));
    iter->dict = dict;
    iter->index = 0;
    iter->node = dict->size > 0 ? dict->table[iter->index] : NULL;
    xdict_iter_next(iter);
    return iter;
}

void xdict_iter_destroy(xdict_iter *iter) {
    xfree(iter);
}

xdict_entry* xdict_iter_next(xdict_iter *iter) {
    assert(iter);
    while (1) {
        xdict_entry *cur_entry = iter->node;
        if (cur_entry == NULL) {
            iter->index++;
            if (iter->index >= iter->dict->size) break;
            iter->node = iter->dict->table[iter->index];
        } else {
            iter->node = cur_entry->next;
        }

        if (iter->node) {
            return cur_entry;
        }
    }
    return NULL;
}
