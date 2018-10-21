//
// Created by koushiro on 10/18/18.
//

#include <assert.h>
#include <ctype.h>
#include <string.h>

#include "xstr.h"
#include "xalloc.h"

xstr xstr_create_raw(const void *init, size_t init_len) {
    size_t hdr_size = sizeof(xstr_hdr);
    void *ptr = xmalloc(hdr_size + init_len + 1);
    if (ptr == NULL) return NULL;
    if (!init) {
        memset(ptr, 0, hdr_size + init_len + 1);
    }

    xstr_hdr *hdr = (xstr_hdr*)ptr;
    hdr->len = init_len;
    hdr->cap = init_len;
    xstr s = (xstr)(ptr + hdr_size);

    if (init_len && init) {
        memcpy(s, init, init_len);
    }
    s[init_len] = '\0';
    return s;
}

xstr xstr_create(const char *init) {
    size_t init_len = (init == NULL) ? 0 : strlen(init);
    return xstr_create_raw((void *)init, init_len);
}

void xstr_destroy(xstr s) {
    if (s == NULL) return;
    xfree(xstr_alloc_ptr(s));
}

void xstr_clear(xstr s) {
    if (s == NULL) return;
    s[0] = '\0';
    xstr_set_len(s, 0);
}

xstr xstr_expand(xstr s, size_t add_len) {
    assert(s);
    size_t avail = xstr_avail(s);
    if (avail > add_len) return s;

    size_t len = xstr_len(s);
    size_t new_len = len + add_len;
    if (new_len < XSTR_MAX_PREALLOC) {
        new_len *= 2;
    } else {
        new_len += XSTR_MAX_PREALLOC;
    }
    size_t hdr_size = sizeof(xstr_hdr);
    void *ptr = xrealloc(xstr_alloc_ptr(s), hdr_size + new_len + 1);
    if (ptr == NULL) return NULL;
    s = (char*)ptr + hdr_size;
    xstr_set_cap(s, new_len);
    return s;
}

xstr xstr_shrink(xstr s) {
    assert(s);
    size_t len = xstr_len(s);
    size_t hdr_size = sizeof(xstr_hdr);
    void *ptr = xrealloc(xstr_alloc_ptr(s), hdr_size + len + 1);
    if (ptr == NULL) return NULL;
    s = (xstr)(ptr + hdr_size);
    xstr_set_cap(s, len);
    return s;
}

xstr xstr_grow(xstr s, int ch, size_t len) {
    assert(s);
    size_t cur_len = xstr_len(s);
    if (len < cur_len) return s;

    s = xstr_expand(s, len - cur_len);
    if (s == NULL) return NULL;

    memset(s + cur_len, ch, len - cur_len + 1);
    xstr_set_len(s, len);
    return s;
}

int xstr_cmp(const xstr s1, const xstr s2) {
    assert(s1 && s2);
    size_t len1 = xstr_len(s1);
    size_t len2 = xstr_len(s2);
    size_t min_len = (len1 < len2) ? len1 : len2;
    int cmp = memcmp(s1, s2, min_len);
    if (cmp == 0) return len1 < len2 ? -1 : (len1 > len2 ? 1 : 0);
    return cmp;
}

xstr xstr_copy(xstr dest, const void *src, size_t len) {
    assert(dest && src);
    if (xstr_cap(dest) < len) {
        dest = xstr_expand(dest, len - xstr_len(dest));
        if (dest == NULL) return NULL;
    }
    memcpy(dest, src, len);
    dest[len] = '\0';
    xstr_set_len(dest, len);
    return dest;
}

xstr xstr_copy_cstr(xstr dest, const char *src) {
    return xstr_copy(dest, src, strlen(src));
}

xstr xstr_copy_xstr(xstr dest, const xstr src) {
    return xstr_copy(dest, src, xstr_len(src));
}

xstr xstr_cat(xstr dest, const void *src, size_t len) {
    assert(dest && src);
    size_t cur_len = xstr_len(dest);
    dest = xstr_expand(dest, len);
    if (dest == NULL) return NULL;
    memcpy(dest + cur_len, src, len);
    dest[cur_len + len] = '\0';
    xstr_set_len(dest, cur_len + len);
    return dest;
}

xstr xstr_cat_cstr(xstr dest, const char *src) {
    return xstr_cat(dest, src, strlen(src));
}

xstr xstr_cat_xstr(xstr dest, const xstr src) {
    return xstr_cat(dest, src, xstr_len(src));
}

xstr xstr_join_cstr(char **argv, size_t argc, const char *sep) {
    xstr join = xstr_create_raw((void *) "", 0);
    for (size_t i = 0; i < argc; ++i) {
        join = xstr_cat_cstr(join, argv[i]);
        if (i != argc - 1) {
            join = xstr_cat_cstr(join, sep);
        }
    }
    return join;
}

xstr xstr_join_xstr(xstr *argv, size_t argc, const char *sep, size_t sep_len) {
    xstr join = xstr_create_raw((void *) "", 0);
    for (size_t i = 0; i < argc; ++i) {
        join = xstr_cat_xstr(join, argv[i]);
        if (i != argc - 1) {
            join = xstr_cat(join, sep, sep_len);
        }
    }
    return join;
}

void xstr_toupper(xstr s) {
    assert(s);
    size_t len = xstr_len(s);
    for (size_t i = 0; i < len; ++i) {
        s[i] = (char) toupper(s[i]);
    }
}

void xstr_tolower(xstr s) {
    assert(s);
    size_t len = xstr_len(s);
    for (size_t i = 0; i < len; ++i) {
        s[i] = (char)tolower(s[i]);
    }
}

xstr xstr_cat_vprintf(xstr s, const char *fmt, va_list ap) {
    char static_buf[1024];
    char *buf = static_buf;
    size_t buf_len = strlen(fmt) * 2;

    if (buf_len > sizeof(static_buf)) {
        buf = xmalloc(buf_len);
        if (buf == NULL) return NULL;
    } else {
        buf_len = sizeof(static_buf);
    }

    va_list cpy;
    while (1) {
        buf[buf_len - 2] = '\0';
        va_copy(cpy, ap);
        vsnprintf(buf, buf_len, fmt, cpy);
        va_end(cpy);
        if (buf[buf_len - 2] != '\0') {
            if (buf != static_buf) xfree(buf);
            buf_len *= 2;
            buf = xmalloc(buf_len);
            if (buf == NULL) return NULL;
            continue;
        }
        break;
    }

    xstr t = xstr_cat_cstr(s, buf);
    if (buf != static_buf) xfree(buf);
    return t;
}

xstr xstr_cat_printf(xstr s, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    xstr t = xstr_cat_vprintf(s, fmt, ap);
    va_end(ap);
    return t;
}

xstr xstr_trim(xstr s, const char *cset) {
    assert(s && cset);
    char *start = s;
    char *sp = start;
    char *end = s + xstr_len(s) - 1;
    char *ep = end;

    while (sp <= end && strchr(cset, *sp)) sp++;
    while (ep > sp && strchr(cset, *ep)) ep--;
    size_t len = (sp > ep) ? 0 : ((ep - sp) + 1);
    if (s != sp) memmove(s, sp, len);
    s[len] = '\0';
    xstr_set_len(s, len);
    return s;
}

void xstr_range(xstr s, ssize_t start, ssize_t end) {
    assert(s);
    size_t len = xstr_len(s);
    if (len == 0) return;
    if (start < 0) {
        start += len;
        if (start < 0) start = 0;
    }
    if (end < 0) {
        end += len;
        if (end < 0) end = 0;
    }
    size_t new_len = (size_t) ((start > end) ? 0 : (end - start) + 1);
    if (new_len != 0) {
        if (start >= (ssize_t)len) {
            new_len = 0;
        } else if (end >= (ssize_t)len) {
            end = len - 1;
            new_len = (size_t) ((start > end) ? 0 : (end - start) + 1);
        }
    } else {
        start = 0;
    }

    if (start && new_len) {
        memmove(s, s + start, new_len);
    }
    s[new_len] = '\0';
    xstr_set_len(s, new_len);
}
