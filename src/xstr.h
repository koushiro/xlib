//
// Created by koushiro on 10/18/18.
//

// References from https://github.com/antirez/redis/blob/unstable/deps/hiredis/sds.h

#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define XSTR_MAX_PREALLOC (1024*1024)

typedef char* xstr;

typedef struct __attribute__ ((__packed__)) xstr_hdr {
    size_t len;
    size_t cap;
    char buf[];
} xstr_hdr;

static inline xstr_hdr* xstr_hdr_ptr(const xstr s) { return (xstr_hdr*)(s - sizeof(xstr_hdr)); }
static inline void* xstr_alloc_ptr(const xstr s) { return (void*)xstr_hdr_ptr(s); }

static inline size_t xstr_len(const xstr s) { return xstr_hdr_ptr(s)->len; }
static inline size_t xstr_cap(const xstr s) { return xstr_hdr_ptr(s)->cap; }
static inline size_t xstr_avail(const xstr s) { xstr_hdr *hdr = xstr_hdr_ptr(s); return hdr->cap - hdr->len; }
static inline size_t xstr_alloc_size(const xstr s) { return sizeof(xstr_hdr) + xstr_cap(s) + 1; }
static inline void xstr_set_len(xstr s, size_t new_len) { xstr_hdr_ptr(s)->len = new_len; }
static inline void xstr_set_cap(xstr s, size_t new_cap) { xstr_hdr_ptr(s)->cap = new_cap; }

xstr xstr_create_raw(const void *init, size_t init_len);
xstr xstr_create(const char *init); // create xstr from a null terminated C string
void xstr_destroy(xstr s);
void xstr_clear(xstr s);

xstr xstr_expand(xstr s, size_t size);
xstr xstr_shrink(xstr s);
xstr xstr_grow(xstr s, int ch, size_t len);

int xstr_cmp(const xstr s1, const xstr s2);

xstr xstr_cat(xstr dest, const void *src, size_t len);
xstr xstr_cat_cstr(xstr dest, const char *src);
xstr xstr_cat_xstr(xstr s, const xstr t);

xstr xstr_copy(xstr dest, const void *src, size_t len);
xstr xstr_copy_cstr(xstr dest, const char *src);
xstr xstr_copy_xstr(xstr dest, const xstr src);

//xstr* xstr_split(const char *s, ssize_t len, const char *sep, int sep_len, int *count);
//void xstr_split_free(xstr *tokens, int count);

xstr xstr_join_cstr(char **argv, size_t argc, const char *sep);
xstr xstr_join_xstr(xstr *argv, size_t argc, const char *sep, size_t sep_len);

void xstr_toupper(xstr s);
void xstr_tolower(xstr s);

xstr xstr_cat_vprintf(xstr s, const char *fmt, va_list ap);
xstr xstr_cat_printf(xstr s, const char *fmt, ...);
xstr xstr_cat_fmt(xstr s, const char *fmt, ...);

xstr xstr_trim(xstr s, const char *cset);

void xstr_range(xstr s, ssize_t start, ssize_t end);
