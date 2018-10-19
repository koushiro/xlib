//
// Created by koushiro on 10/18/18.
//

#pragma once

#include <stddef.h>

void *xmalloc(size_t size);
void *xcalloc(size_t num, size_t size);
void *xrealloc(void *ptr, size_t size);
void xfree(void* ptr);
