//
// Created by koushiro on 10/18/18.
//
// References from https://github.com/antirez/redis/blob/unstable/src/dict.h

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
    void *k;
    union {
        void *ptr;
        uint64_t u64;
        int64_t i64;
        double f64;
    } v;
    struct xdict_entry *next;
} xdict_entry;

typedef struct xdict_ht {
    struct xdict_entry **table; // A xdict_entry pointer array.
    size_t used;                // The used size of array; load functor = used / size.
    size_t size;                // The total size of array; always equals to 2^n.
    size_t size_mask;           // Equals to size - 1, for computing index.
} xdict_ht;

#define XDICT_HT_INIT_SIZE  4

typedef struct xdict {
    struct xdict_type *type;
    void *data;
    struct xdict_ht ht[2];  // 只有在重哈希的过程中，ht[0]和ht[1]才都有效。
                            // 而在平常情况下，只有ht[0]有效，ht[1]里面没有任何数据。
    long rehash_index;      // 当前重哈希索引（rehashidx）。
                            // 如果rehashidx = -1，表示当前没有在重哈希过程中；
                            // 否则，表示当前正在进行重哈希，且它的值记录了当前重哈希进行到哪一步了。
    int iterators;          // 当前正在进行遍历的iterator的个数。
} xdict;

//typedef struct xdict_iter {
//    struct xdict *dict;
//    long index;
//    int table;
//    int safe;
//    struct xdict_entry *entry, *next_entry;
//    long long fingerprint;
//} xdict_iter;

static inline void xdict_set_key(xdict *dict, xdict_entry *entry, void *key) {
    if (dict->type->key_dup) {
        entry->k = dict->type->key_dup(dict->data, key);
    } else {
        entry->k = key;
    }
}

static inline void xdict_free_key(xdict *dict, xdict_entry *entry) {
    if (dict->type->key_dtor) {
        dict->type->key_dtor(dict->data, entry->k);
    }
}

static inline void xdict_set_val(xdict *dict, xdict_entry *entry, void *val) {
    if (dict->type->val_dup) {
        entry->v.ptr = dict->type->val_dup(dict->data, val);
    } else {
        entry->v.ptr = val;
    }
}
static inline void xdict_set_i64_val(xdict_entry *entry, int64_t val) { entry->v.i64 = val; }
static inline void xdict_set_u64_val(xdict_entry *entry, uint64_t val) { entry->v.u64 = val; }
static inline void xdict_set_f64_val(xdict_entry *entry, double val) { entry->v.f64 = val; }

static inline void xdict_free_val(xdict *dict, xdict_entry *entry) {
    if (dict->type->val_dtor) {
        dict->type->val_dtor(dict->data, entry->v.ptr);
    }
}

static inline int xdict_cmp_key(xdict *dict, const void *key1, const void *key2) {
    if (dict->type->key_cmp) {
        return dict->type->key_cmp(dict->data, key1, key2);
    } else {
        return key1 == key2 ? XDICT_OK : XDICT_ERR;
    }
}

static inline void xdict_hash_key(xdict *dict, const void *key) {
    dict->type->hash(key);
}

static inline void* xdict_get_key(xdict_entry *entry) { return entry->k; }
static inline void* xdict_get_val(xdict_entry *entry) { return entry->v.ptr; }
static inline uint64_t xdict_get_u64_val(xdict_entry *entry) { return entry->v.u64; }
static inline int64_t xdict_get_i64_val(xdict_entry *entry) { return entry->v.i64; }
static inline double xdict_get_f64_val(xdict_entry *entry) { return entry->v.f64; }
static inline size_t xdict_used_size(xdict *dict) { return dict->ht[0].used + dict->ht[1].used; }
static inline size_t xdict_total_size(xdict *dict) { return dict->ht[0].size + dict->ht[1].size; }
static inline int xdict_is_rehashing(xdict *dict) { return dict->rehash_index != -1; }

xdict* xdict_create(xdict_type *type, void *data);
void xdict_destroy(xdict *dict);
int xdict_clear(xdict *dict, xdict_ht *ht, void(callback)(void *));

xdict_entry* xdict_search(xdict *dict, const void *key);
