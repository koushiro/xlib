//
// Created by koushiro on 10/19/18.
//

#include <string.h>

#include "xstr.h"
#include "xtest.h"

int main() {
    xstr s1 = xstr_create("test");
    test_assert("xstr_create",
                xstr_len(s1) == 4 && xstr_cap(s1) == 4 && memcmp(s1, "test\0", 5) == 0);
    xstr_destroy(s1);

    s1 = xstr_create_raw("test", 3);
    test_assert("xstr_create_raw with less length",
                xstr_len(s1) == 3 && xstr_cap(s1) == 3 && memcmp(s1, "tes\0", 4) == 0);
    xstr_destroy(s1);

    s1 = xstr_create_raw("test", 5);
    test_assert("xstr_create_raw with more length",
                xstr_len(s1) == 5 && xstr_cap(s1) == 5 && memcmp(s1, "test\0\0", 6) == 0);
    xstr_destroy(s1);

    s1 = xstr_create("test");
    xstr_clear(s1);
    test_assert("xstr_clear",
                xstr_len(s1) == 0 && xstr_cap(s1) == 4 && memcmp(s1, "\0est\0", 5) == 0);
    xstr_destroy(s1);

    s1 = xstr_create("test");
    xstr s2 = xstr_dup(s1);
    test_assert("xstr_dup",
                xstr_len(s1) == xstr_len(s2) && xstr_cap(s1) == xstr_cap(s2) &&
                memcmp(s1, s2, 5) == 0);
    xstr_destroy(s1);
    xstr_destroy(s2);

    s1 = xstr_create("test");
    test_assert("xstr_expand before",
                xstr_len(s1) == 4 && xstr_avail(s1) == 0 && xstr_cap(s1) == 4);
    s1 = xstr_expand(s1, 2);
    test_assert("xstr_expand 2",
                xstr_len(s1) == 4 && xstr_avail(s1) == 8 && xstr_cap(s1) == 12);
    s1 = xstr_expand(s1, 8);
    test_assert("xstr_expand 8 more",
                xstr_len(s1) == 4 && xstr_avail(s1) == 8 && xstr_cap(s1) == 12);
    s1 = xstr_shrink(s1);
    test_assert("xstr_shrink",
                xstr_len(s1) == 4 && xstr_avail(s1) == 0 && xstr_cap(s1) == 4);
    xstr_destroy(s1);

    s1 = xstr_create("test");
    s2 = xstr_create("test");
    test_assert("xstr_cmp when xstr_len(s1) == xstr_len(s2)",
                xstr_len(s1) == 4 && xstr_len(s2) == 4 && xstr_cmp(s1, s2) == 0);
    xstr_destroy(s2);
    s2 = xstr_create("tes");
    test_assert("xstr_cmp when xstr_len(s1) > xstr_len(s2)",
                xstr_len(s1) == 4 && xstr_len(s2) == 3 && xstr_cmp(s1, s2) == 1);
    xstr_destroy(s2);
    s2 = xstr_create("tests");
    test_assert("xstr_cmp when xstr_len(s1) < xstr_len(s2)",
                xstr_len(s1) == 4 && xstr_len(s2) == 5 && xstr_cmp(s1, s2) == -1);
    xstr_destroy(s1);
    xstr_destroy(s2);

    s1 = xstr_create("test");
    s1 = xstr_copy(s1, "debug\0test", 10);
    test_assert("xstr_copy",
                xstr_len(s1) == 10 && xstr_avail(s1) == 10 && xstr_cap(s1) == 20 &&
                memcmp(s1, "debug\0test\0", 11) == 0);
    xstr_destroy(s1);

    s1 = xstr_create("test");
    s1 = xstr_copy_cstr(s1, "debug\0test");
    test_assert("xstr_copy_cstr",
                xstr_len(s1) == 5 && xstr_avail(s1) == 5 && xstr_cap(s1) == 10 &&
                memcmp(s1, "debug\0", 6) == 0);
    xstr_destroy(s1);

    s1 = xstr_create("test");
    s2 = xstr_create_raw("debug\0test", 10);
    s1 = xstr_copy_xstr(s1, s2);
    test_assert("xstr_copy_xstr",
                xstr_len(s1) == 10 && xstr_avail(s1) == 10 && xstr_cap(s1) == 20 &&
                memcmp(s1, "debug\0test\0", 11) == 0);
    xstr_destroy(s1);
    xstr_destroy(s2);

    s1 = xstr_create("test");
    s1 = xstr_cat(s1, "debug\0test", 10);
    test_assert("xstr_cat",
                xstr_len(s1) == 14 && xstr_avail(s1) == 14 && xstr_cap(s1) == 28 &&
                memcmp(s1, "testdebug\0test\0", 15) == 0);
    xstr_destroy(s1);

    s1 = xstr_create("test");
    s1 = xstr_cat_cstr(s1, "debug\0test");
    test_assert("xstr_cat_cstr",
                xstr_len(s1) == 9 && xstr_avail(s1) == 9 && xstr_cap(s1) == 18 &&
                memcmp(s1, "testdebug\0", 10) == 0);
    xstr_destroy(s1);

    s1 = xstr_create("test");
    s2 = xstr_create_raw("debug\0test", 10);
    s1 = xstr_cat_xstr(s1, s2);
    test_assert("xstr_cat_xstr",
                xstr_len(s1) == 14 && xstr_avail(s1) == 14 && xstr_cap(s1) == 28 &&
                memcmp(s1, "testdebug\0test\0", 15) == 0);
    xstr_destroy(s1);
    xstr_destroy(s2);

    s1 = xstr_create("test");
    s1 = xstr_cat_printf(s1, "%d", 123);
    test_assert("xstr_cat_printf",
                xstr_len(s1) == 7 && xstr_avail(s1) == 7 && xstr_cap(s1) == 14 &&
                memcmp(s1, "test123\0", 8) == 0);
    xstr_destroy(s1);

    char* char_argv[] = {"我", "打", "我", "自", "己"};
    s1 = xstr_join_cstr(char_argv, 5, " ");
    test_assert("xstr_join_cstr", memcmp(s1, "我 打 我 自 己\0", 20) == 0);
    xstr_destroy(s1);

    xstr xstr_argv[] = {xstr_create("我"), xstr_create("打"), xstr_create("我"), xstr_create("自"), xstr_create("己")};
    s1 = xstr_join_xstr(xstr_argv, 5, "\0", 1);
    test_assert("xstr_join_xstr", memcmp(s1, "我\0打\0我\0自\0己\0", 20) == 0);
    xstr_destroy(s1);
    for (size_t i = 0; i < 5; ++i) {
        xstr_destroy(xstr_argv[i]);
    }

    s1 = xstr_create_raw("debug\0test", 10);
    s1 = xstr_toupper(s1);
    test_assert("xstr_toupper", memcmp(s1, "DEBUG\0TEST", 10) == 0);
    s1 = xstr_tolower(s1);
    test_assert("xstr_tolower", memcmp(s1, "debug\0test", 10) == 0);
    xstr_destroy(s1);

    s1 = xstr_create(" test ");
    s1 = xstr_trim(s1, " t");
    test_assert("xstr_trim", xstr_len(s1) == 2 && memcmp(s1, "es\0", 3) == 0);
    xstr_destroy(s1);

    s1 = xstr_create(" test ");
    s1 = xstr_trim(s1, " ");
    test_assert("xstr_trim", xstr_len(s1) == 4 && memcmp(s1, "test\0", 5) == 0);
    xstr_destroy(s1);

    s1 = xstr_create("Rust is the best programming language");
    s1 = xstr_range(s1, 0, -1);
    test_assert("xstr_range(..., 1, -1)",
                xstr_len(s1) == 37 && memcmp(s1, "Rust is the best programming language\0", 38) == 0);
    xstr_destroy(s1);

    s1 = xstr_create("Rust is the best programming language");
    s1 = xstr_range(s1, 0, 3);
    test_assert("xstr_range(..., 1, 1)",
                xstr_len(s1) == 4 && memcmp(s1, "Rust\0", 5) == 0);
    xstr_destroy(s1);

    s1 = xstr_create("Rust is the best programming language");
    s1 = xstr_range(s1, -8, -1);
    test_assert("xstr_range(..., -8, -1)",
                xstr_len(s1) == 8 && memcmp(s1, "language\0", 9) == 0);
    xstr_destroy(s1);

    test_report();

    return 0;
}