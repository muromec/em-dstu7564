/*
 * Copyright (c) 2016 PrivatBank IT <acsk@privatbank.ua>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */

#ifndef CRYPTONITE_TEST_UTILS_H
#define CRYPTONITE_TEST_UTILS_H

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <time.h>

#include "cryptonite_errors.h"


extern size_t error_count;

#define MB 1024 * 1024

#ifdef USE_COLOR
#ifdef _WIN32
#include <windows.h>
#define COLOR_RED  SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
#define COLOR_GREEN SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
#define COLOR_RESET SetConsoleTextAttribute(hConsole, saved_attributes);
#elif __APPLE__
#define COLOR_RESET   PR("\033[0m");
#define COLOR_RED     PR("\033[1;31m");
#define COLOR_GREEN   PR("\033[1;32m");
#elif __linux__
#define COLOR_RESET   PR("\033[0m");
#define COLOR_RED     PR("\033[1;31m");
#define COLOR_GREEN   PR("\033[1;32m");
#elif __unix__ // all unices not caught above
#define COLOR_RESET   PR("\033[0m");
#define COLOR_RED     PR("\033[1;31m");
#define COLOR_GREEN   PR("\033[1;32m");
#elif _POSIX_VERSION
#define COLOR_RESET
#define COLOR_RED
#define COLOR_GREEN
#else
#define COLOR_RESET
#define COLOR_RED
#define COLOR_GREEN
#endif

#else
#define COLOR_RESET
#define COLOR_RED
#define COLOR_GREEN
#endif

#define DOT_STRING_SIZE  80
#define SIZE_STR         80

#ifdef _WIN32
#include <windows.h>
#endif
/* Microsoft compilers */
#ifdef _MSC_VER

#define EXPAND(x) x
#define __NARGS(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, VAL, ...) VAL
#define NARGS_1(...) EXPAND(__NARGS(__VA_ARGS__, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0))
#define AUGMENTER(...) unused, __VA_ARGS__
#define NARGS(...) NARGS_1(AUGMENTER(__VA_ARGS__))

#elif __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu"
#define NARGS(...) __NARGS(0, ## __VA_ARGS__, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define __NARGS(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, N, ...) N
#pragma clang diagnostic pop
#else

#define NARGS(...) __NARGS(0, ## __VA_ARGS__, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define __NARGS(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, N, ...) N
#endif /* _MSC_VER */

#define MB_STRING_VALUE "MB\\sec"
#define OP_STRING_VALUE "op\\sec"

extern char DOT_STRING80[];
extern char WSPACE_STRING80[];

typedef enum {
    GREEN,
    RED,
    WHITE,
} COLOR_en;

size_t data_size_byte;

unsigned int LOOP_NUM;

#define PR(...)        printf(__VA_ARGS__); fflush(stdout)

#define ASN_EXECUTE(ret) ASSERT_RET_OK(ret.code); goto cleanup;
#define ASSERT_RET_OK(func) { int _ret = (func); if (_ret != RET_OK) { assert_ret_ok_core(_ret, __FILE__, __LINE__); goto cleanup;} }
#define ASSERT_RET(exp_ret, func) { int _ret = (func); if (_ret != exp_ret) { assert_ret_core(exp_ret, _ret, __FILE__, __LINE__); goto cleanup;} }
#define ASSERT_EQUALS(expected, actual, size) if (!assert_equals_core(expected, actual, size, __FILE__, __LINE__)) return
#define ASSERT_EQUALS_STR(expected, actual) if (!assert_equals_str_core(expected, actual, __FILE__, __LINE__)) return
#define ASSERT_EQUALS_SIZE_T(expected, actual) if (!assert_equals_size_t_core(expected, actual,  __FILE__, __LINE__)) return
#define ASSERT_EQUALS_WA(expected, actual) if (!assert_equals_wa_core(expected, actual, __FILE__, __LINE__)) return
#define ASSERT_TRUE(expression) if(!assert_true_core(expression, __FILE__, __LINE__)) return
#define ASSERT_NOT_NULL(p)                                              \
    if ((p) == NULL) {                                                  \
        PR("FILE: %s LINE: %d Assert NULL!!!\n", __FILE__, __LINE__);   \
        error_count++;                                                  \
        goto cleanup;                                                   \
    }

#ifdef _WIN32
#define DEFAULT_CLOCKS_PS_VALUE 10000000
#else
#define DEFAULT_CLOCKS_PS_VALUE 1000000
#endif

bool assert_true_core(bool expression, char *file, int line);
bool assert_ret_ok_core(int ret, char *file, int line);
bool assert_ret_core(int exp_ret, int act_ret, char *file, int line);
bool assert_equals_size_t_core(size_t expected, size_t actual, char *file, int line);
bool assert_equals_str_core(const char *expected, const char *actual, char *file, int line);
bool assert_equals_core(const void *expected, const void *actual, size_t size, char *file, int line);

uint8_t *uint8_alloc_from_le_hex_string(const char *data, size_t* out_len);

#endif
