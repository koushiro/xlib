//
// Created by koushiro on 10/18/18.
//

#include "xalloc.h"

#include <stdlib.h>

void *xmalloc(size_t size) {
    return malloc(size);
}

void *xcalloc(size_t num, size_t size) {
    return calloc(num, size);
}

void *xrealloc(void *ptr, size_t size) {
    if (ptr == NULL) return NULL;
    return realloc(ptr, size);
}

void xfree(void* ptr) {
    if (ptr == NULL) return;
    free(ptr);
}