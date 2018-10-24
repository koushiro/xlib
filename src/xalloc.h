//
// Created by koushiro on 10/18/18.
//

#pragma once

#include <stddef.h>

void xalloc_set_oom_handler(void (*oom_handler)(size_t));
void *xmalloc(size_t size);
void *xcalloc(size_t num, size_t size);
void *xrealloc(void *ptr, size_t size);
void xfree(void* ptr);
