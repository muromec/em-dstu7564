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

#include "test_utils.h"
#include "macros_internal.h"

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


bool is_char_0_f(char ch)
{
    return (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F') || (ch >= '0' && ch <= '9');
}

uint8_t *uint8_alloc_from_le_hex_string(const char *data, size_t* out_len)
{
    uint8_t *out = NULL;
    char tmp[3] = {0};
    size_t i;
    size_t len;
    int ret = RET_OK;

    CHECK_PARAM(data != NULL);

    len = strlen(data);
    if (len % 2 != 0) {
        SET_ERROR(RET_INVALID_HEX_STRING);
    }

    MALLOC_CHECKED(out, len / 2);

    for (i = 0; i < len / 2; i++) {
        if (!is_char_0_f(data[2 * i]) || !is_char_0_f(data[2 * i + 1])) {
            SET_ERROR(RET_INVALID_HEX_STRING);
        }
        memcpy(tmp, data + 2 * i, 2);
        out[i] = (uint8_t) strtol(tmp, NULL, 16);
    }

    *out_len = len / 2;

    return out;

cleanup:
    if(out) {
        free(out);
    }
    return NULL;
}

