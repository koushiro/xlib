//
// Created by koushiro on 10/18/18.
//

#include "xalloc.h"

#include <stdio.h>
#include <stdlib.h>

static void xalloc_default_oom(size_t size) {
    fprintf(stderr, "Out of memory trying to allocate %zu bytes\n", size);
    fflush(stderr);
    abort();
}

static void (*xalloc_oom_handler)(size_t) = xalloc_default_oom;

void xalloc_set_oom_handler(void (*oom_handler)(size_t)) {
    xalloc_oom_handler = oom_handler;
}

void *xmalloc(size_t size) {
    void *ptr = malloc(size);
    if (!ptr) xalloc_oom_handler(size);
    return ptr;
}

void *xcalloc(size_t num, size_t size) {
    void *ptr = calloc(num, size);
    if (!ptr) xalloc_oom_handler(size);
    return ptr;
}

void *xrealloc(void *ptr, size_t size) {
    if (ptr == NULL) return NULL;
    void *newptr = realloc(ptr, size);
    if (!newptr) xalloc_oom_handler(size);
    return newptr;
}

void xfree(void* ptr) {
    if (ptr == NULL) return;
    free(ptr);
}