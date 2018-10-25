//
// Created by koushiro on 10/24/18.
//

#pragma once

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

size_t test_num = 0;
size_t failed_test_num = 0;

#define test_assert(description, cond) \
    do { \
        ++test_num; \
        printf("%zu - %s: ", test_num, description); \
        if (cond) { \
            printf("PASSED\n"); \
        } else { \
            printf("FAILED\n"); \
            ++failed_test_num; \
        } \
    } while (0);

#define test_report() \
    do { \
        printf("%zu test(s), %zu passed, %zu failed, (%zu/%zu).\n", \
            test_num, test_num - failed_test_num, failed_test_num, \
            test_num - failed_test_num, test_num); \
        if (failed_test_num > 0) { \
            printf("=== WARNING === We have failed tests here...\n"); \
            exit(1); \
        } \
    } while (0);