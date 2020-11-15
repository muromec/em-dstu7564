/*
 * Copyright (c) 2016 PrivatBank IT <acsk@privatbank.ua>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#include <unistd.h>
#endif

#include "byte_array.h"
#include "test_utils.h"

size_t error_count = 0;
int success_count = 0;
size_t level_num = 0;

char DOT_STRING80[]    = "................................................................................";
char WSPACE_STRING80[] = "                                                                                ";

bool assert_true_core(bool expression, char *file, int line)
{
    if (!expression) {
        PR("-------------------------------------------------------------------------------\n");
        PR("Expected: true\n");
        PR("Actual  : false\n");
        error_count++;
        PR("-------------------------------------------------------------------------------\n");
        return false;
    }

    return true;
}

bool assert_ret_ok_core(int ret, char *file, int line)
{
    if (ret != RET_OK) {
        PR("-------------------------------------------------------------------------------\n");
        PR("Expected: RET_OK\n");
        switch (ret) {
        case RET_MEMORY_ALLOC_ERROR:
            PR("Actual  : RET_MEMORY_ALLOC_ERROR\n");
            break;
        case RET_INVALID_PARAM:
            PR("Actual  : RET_INVALID_PARAM\n");
            break;
        case RET_VERIFY_FAILED:
            PR("Actual  : RET_VERIFY_FAILED\n");
            break;
        case RET_CONTEXT_NOT_READY:
            PR("Actual  : RET_CONTEXT_NOT_READY\n");
            break;
        case RET_INVALID_CTX:
            PR("Actual  : RET_INVALID_CTX\n");
            break;
        case RET_DSTU_PRNG_LOOPED:
            PR("Actual  : RET_DSTU_PRNG_LOOPED\n");
            break;
        case RET_INVALID_PUBLIC_KEY:
            PR("Actual  : RET_INVALID_PUBLIC_KEY\n");
            break;
        case RET_INVALID_MODE:
            PR("Actual  : RET_INVALID_MODE\n");
            break;
        case RET_UNSUPPORTED:
            PR("Actual  : RET_UNSUPPORTED\n");
            break;
        default:
            PR("Actual  : UNKNOWN ERROR\n");
            break;
        }
        error_count++;
        PR("-------------------------------------------------------------------------------\n");
        return false;
    }

    return true;
}

bool assert_ret_core(int exp_ret, int act_ret, char *file, int line)
{
    if (act_ret != exp_ret) {
        PR("-------------------------------------------------------------------------------\n");
        PR("Expected: 0x%04x\n", exp_ret);
        PR("Actual  : 0x%04x\n", act_ret);
        error_count++;
        PR("-------------------------------------------------------------------------------\n");
        return false;
    }

    return true;
}

bool assert_equals_core(const void *expected, const void *actual, size_t size, char *file, int line)
{
    size_t i = 0;
    uint8_t *exp = (uint8_t *) expected;
    uint8_t *act = (uint8_t *) actual;
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    WORD saved_attributes;
    GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
    saved_attributes = consoleInfo.wAttributes;
#endif
    if (memcmp(expected, actual, size)) {
        PR("-------------------------------------------------------------------------------\n");
        PR("Expected bytes:\n");
        for (i = 0; i < size; i++) {
            if (exp[i] != act[i]) {
                COLOR_RED;
            } else {
                COLOR_GREEN;
            }
            PR(" 0x%02x,", exp[i]);
            if ((i + 1) % 16 == 0) {
                PR("\n");
            }
        }
        PR("\n");
        COLOR_RESET;
        PR("Actual bytes: \n");
        for (i = 0; i < size; i++) {
            if (exp[i] != act[i]) {
                COLOR_RED;
            } else {
                COLOR_GREEN;
            }
            PR(" 0x%02x,", act[i]);
            if ((i + 1) % 16 == 0) {
                PR("\n");
            }
        }
        error_count++;

        COLOR_RESET;
        PR("\n-------------------------------------------------------------------------------\n");

        return false;
    }

    return true;
}

bool assert_equals_str_core(const char *expected, const char *actual, char *file, int line)
{
    if (!expected && !actual) {
        error_count++;
        return false;
    }

    if (strcmp(expected, actual) != 0) {
        PR("-------------------------------------------------------------------------------\n");
        PR("Expected : %s\n", expected);
        PR("Actual   : %s\n", actual);
        PR("-------------------------------------------------------------------------------\n");
        error_count++;
        return false;
    }

    return true;
}

bool assert_equals_size_t_core(size_t expected, size_t actual, char *file, int line)
{
    if (expected != actual) {
        PR("-------------------------------------------------------------------------------\n");
        PR("Expected size_t: %i\n", (int) expected);
        PR("Actual size_t:   %i\n", (int) actual);
        error_count++;
        PR("-------------------------------------------------------------------------------\n");
        return false;
    }

    return true;
}

bool assert_equals_ptr_core(void *expected, void *actual, char *file, int line)
{
    if (expected != actual) {
        PR("-------------------------------------------------------------------------------\n");
        PR("Expected ptr: %p\n", expected);
        PR("Actual ptr:   %p\n", actual);
        error_count++;
        PR("-------------------------------------------------------------------------------\n");
        return false;
    }

    return true;
}

bool assert_equals_ba_core(ByteArray *expected, ByteArray *actual, char *file, int line)
{
    return (expected != NULL && actual != NULL)
            ? (assert_equals_size_t_core(ba_get_len(expected), ba_get_len(actual), file, line))
            && (assert_equals_core(ba_get_buf(expected), ba_get_buf(actual), ba_get_len(actual), file, line))
            : (assert_equals_ptr_core(expected, actual, file, line));
}

bool equals_ba(ByteArray *expected, ByteArray *actual)
{
    if (expected == actual) {
        return true;
    }

    if (expected == NULL || actual == NULL) {
        error_count++;
        return false;
    }

    if (expected->len != actual->len) {
        error_count++;
        return false;
    }

    if (memcmp(expected->buf, actual->buf, actual->len)) {
        error_count++;
        return false;
    }

    return true;
}

void ba_free_many(int num, ...)
{
    int i;
    va_list args;

    va_start(args, num);
    for (i = 0; i < num; i++) {
        ba_free(va_arg(args, ByteArray *));
    }
    va_end(args);
}



ByteArray *ba_alloc_from_be_hex_string(const char *data)
{
    ByteArray *out_ba = NULL;
    uint8_t *out = NULL;
    char *data_ext = NULL;
    size_t i;
    char tmp[3] = {0};
    size_t len;

    if (!data) {
        return NULL;
    }

    len = strlen(data);
    if (len % 2 != 0) {
        data_ext = malloc(len + 1);
        data_ext[0] = '0';
        memcpy(data_ext + 1, data, len);
        len++;
    } else {
        data_ext = malloc(len + 1);
        data_ext = strcpy(data_ext, data);
        data_ext[len] = '\0';
    }

    out = malloc(len / 2);

    for (i = 0; i < len / 2; i++) {
        memcpy(tmp, data_ext + 2 * i, 2);
        out[len / 2 - 1 - i] = (uint8_t) strtol(tmp, NULL, 16);
    }

    out_ba = ba_alloc_from_uint8(out, len / 2);

    free(data_ext);
    free(out);

    return out_ba;
}
