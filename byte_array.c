/*
 * Copyright (c) 2016 PrivatBank IT <acsk@privatbank.ua>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */

#include <stddef.h>
#include <string.h>

#include "byte_array.h"
#include "byte_array_internal.h"
#include "byte_utils_internal.h"
#include "macros_internal.h"

#undef FILE_MARKER
#define FILE_MARKER "cryptonite/byte_array.c"

ByteArray *ba_alloc(void)
{
    ByteArray *ba = NULL;
    int ret = RET_OK;

    MALLOC_CHECKED(ba, sizeof (ByteArray));

    ba->buf = NULL;
    ba->len = 0;

cleanup:

    return ba;
}

ByteArray *ba_alloc_by_len(size_t len)
{
    ByteArray *ba = NULL;
    int ret = RET_OK;

    MALLOC_CHECKED(ba, sizeof (ByteArray));
    MALLOC_CHECKED(ba->buf, len);

    ba->len = len;

    return ba;
cleanup:
    return NULL;
}

ByteArray *ba_alloc_from_uint8(const uint8_t *buf, size_t buf_len)
{
    ByteArray *ba = NULL;
    int ret = RET_OK;

    if (buf != NULL) {
        MALLOC_CHECKED(ba, sizeof (ByteArray));
        if (buf_len != 0) {
            MALLOC_CHECKED(ba->buf, buf_len);
            memcpy(ba->buf, buf, buf_len);
        } else {
            ba->buf = NULL;
        }

        ba->len = buf_len;
    }

    return ba;

cleanup:

    free(ba);

    return NULL;
}


static bool is_char_0_f(char ch)
{
    return (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F') || (ch >= '0' && ch <= '9');
}

ByteArray *ba_alloc_from_le_hex_string(const char *data)
{
    ByteArray *out_ba = NULL;
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

    CHECK_NOT_NULL(out_ba = ba_alloc_from_uint8(out, len / 2));

cleanup:

    free(out);

    return out_ba;
}

ByteArray *ba_alloc_from_str(const char *buf)
{
    ByteArray *ans = NULL;
    int ret = RET_OK;

    if (buf != NULL) {
        CHECK_NOT_NULL(ans = ba_alloc_from_uint8((const uint8_t *)buf, strlen(buf)));
    }

cleanup:

    return ans;
}



int ba_cmp(const ByteArray *a, const ByteArray *b)
{
    if (a && b) {
        if (a->len != b->len) {
            return (int)(a->len - b->len);
        }

        return memcmp(a->buf, b->buf, a->len);
    }

    return -1;
}

size_t ba_get_len(const ByteArray *ba)
{
    return (ba != NULL) ? ba->len : 0;
}

const uint8_t *ba_get_buf(const ByteArray *ba)
{
    if (ba) {
        return ba->buf;
    }

    return NULL;
}

int ba_to_uint8_with_alloc(const ByteArray *ba, uint8_t **buf, size_t *buf_len)
{
    int ret = RET_OK;

    CHECK_PARAM(ba != NULL);
    CHECK_PARAM(buf != NULL);
    CHECK_PARAM(buf_len != NULL);

    MALLOC_CHECKED(*buf, ba->len);
    memcpy(*buf, ba->buf, ba->len);
    *buf_len = ba->len;

cleanup:

    return ret;
}

int ba_to_uint8(const ByteArray *ba, uint8_t *buf, size_t buf_len)
{
    int ret = RET_OK;

    CHECK_PARAM(ba != NULL);
    CHECK_PARAM(buf != NULL);
    CHECK_PARAM(ba->len <= buf_len);

    memcpy(buf, ba->buf, ba->len);
cleanup:
    return ret;
}


int ba_print(FILE *stream, const ByteArray *ba)
{
    int ret = RET_OK;

    CHECK_PARAM(stream != NULL);

    size_t j = 0;
    uint8_t *u8 = (uint8_t *) ba_get_buf(ba);
    size_t len = ba_get_len(ba);
    for (j = 0; j < len; j++) {
        DO(fprintf(stream, "%02X", u8[j]) > 0 ? RET_OK : RET_FILE_WRITE_ERROR);
        fflush(stream);
    }
    fprintf(stream, "\n");
    fflush(stream);

cleanup:
    return ret;
}

void ba_free(ByteArray *ba)
{
    if (ba) {
        free(ba->buf);
    }
    free(ba);
}
