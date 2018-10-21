//
// Created by koushiro on 10/18/18.
//
// References from dicttps://github.com/antirez/redis/blob/unstable/deps/hiredis/dict.h

#pragma once

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define XDICT_OK  0
#define XDICT_ERR -1

typedef void* (*key_dup_func)(void *data, const void *key);
typedef void* (*val_dup_func)(void *data, const void *val);
typedef void (*key_dtor_func)(void *data, void *key);
typedef void (*val_dtor_func)(void *data, void *val);
typedef int (*key_cmp_func)(void *data, const void *key1, const void *key2);
typedef size_t (*hash_func)(const void *key);

typedef struct xdict_type {
    key_dup_func key_dup;
    val_dup_func val_dup;
    key_dtor_func key_dtor;
    val_dtor_func val_dtor;
    key_cmp_func key_cmp;
    hash_func hash;
} xdict_type;

typedef struct xdict_entry {
    void *key;
    void *val;
//    union {
//        void *ptr;
//        uint64_t u64;
//        int64_t i64;
//        double f64;
//    } val;
    struct xdict_entry *next;
} xdict_entry;

#define XDICT_HT_INIT_SIZE  4

typedef struct xdict {
    struct xdict_entry **table; // A xdict_entry pointer array.
    size_t used;                // The used size of array; load functor = used / size.
    size_t size;                // The total size of array; always equals to 2^n.
    size_t size_mask;           // Equals to size-1, for computing hash table index.
    struct xdict_type *type;
    void *data;
} xdict;

typedef struct xdict_iter {
    struct xdict *dict;
    size_t index;
    struct xdict_entry *next;
} xdict_iter;

static inline void xdict_set_key(xdict *dict, xdict_entry *entry, void *key) {
    if (dict->type->key_dup) {
        entry->key = dict->type->key_dup(dict->data, key);
    } else {
        entry->key = key;
    }
}

static inline void xdict_free_key(xdict *dict, xdict_entry *entry) {
    if (dict->type->key_dtor) {
        dict->type->key_dtor(dict->data, entry->key);
    }
}

static inline void xdict_set_val(xdict *dict, xdict_entry *entry, void *val) {
    if (dict->type->val_dup) {
        entry->val = dict->type->val_dup(dict->data, val);
    } else {
        entry->val = val;
    }
}

//static inline void xdict_set_val(xdict *dict, xdict_entry *entry, void *val) {
//    if (dict->type->val_dup) {
//        entry->val.ptr = dict->type->val_dup(dict->data, val);
//    } else {
//        entry->val.ptr = val;
//    }
//}
//static inline void xdict_set_u64_val(xdict_entry *entry, uint64_t val) { entry->val.u64 = val; }
//static inline void xdict_set_i64_val(xdict_entry *entry, int64_t val) { entry->val.i64 = val; }
//static inline void xdict_set_f64_val(xdict_entry *entry, double val) { entry->val.f64 = val; }

static inline void xdict_free_val(xdict *dict, xdict_entry *entry) {
    if (dict->type->val_dtor) {
        dict->type->val_dtor(dict->data, entry->val);
    }
}

static inline int xdict_cmp_key(xdict *dict, const void *key1, const void *key2) {
    if (dict->type->key_cmp) {
        return dict->type->key_cmp(dict->data, key1, key2);
    } else {
        return key1 == key2 ? XDICT_OK : XDICT_ERR;
    }
}

static inline size_t xdict_hash_key(xdict *dict, const void *key) {
    return dict->type->hash(key);
}

static inline void* xdict_get_key(xdict_entry *entry) { return entry->key; }
static inline void* xdict_get_val(xdict_entry *entry) { return entry->val; }
//static inline void* xdict_get_val(xdict_entry *entry) { return entry->val.ptr; }
//static inline void* xdict_get_u64_val(xdict_entry *entry) { return entry->val.u64; }
//static inline void* xdict_get_i64_val(xdict_entry *entry) { return entry->val.i64; }
//static inline void* xdict_get_f64_val(xdict_entry *entry) { return entry->val.f64; }
static inline size_t xdict_used_size(xdict *dict) { return dict->used; }
static inline size_t xdict_total_size(xdict *dict) { return dict->size; }

xdict* xdict_create(xdict_type *type, void *data);
void xdict_destroy(xdict *dict);
void xdict_clear(xdict *dict);

int xdict_expand(xdict *dict, unsigned long size);
int xdict_add(xdict *dict, void *key, void *val);
int xdict_replace(xdict *dict, void *key, void *val);
int xdict_delete(xdict *dict, const void *key);
xdict_entry* xdict_search(xdict *dict, const void *key);

xdict_iter* xdict_iter_create(xdict *dict);
void xdict_iter_destroy(xdict_iter *iter);
xdict_entry* xdict_iter_next(xdict_iter *iter);