/*
 * Copyright (c) 2016 PrivatBank IT <acsk@privatbank.ua>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */

#include <stddef.h>
#include <string.h>

#include "byte_utils_internal.h"
#include "macros_internal.h"

#undef FILE_MARKER
#define FILE_MARKER "cryptonite/byte_utils_internal.c"

const int big_endian_test = 1;
#define is_bigendian() ((*(char*)&big_endian_test) == 0)

#define swap_word64(in, out, size)              \
    {                                           \
        uint8_t *_in = (uint8_t *)(in);         \
        uint8_t *_out = (uint8_t *)(out);       \
        uint8_t _tmp[8];                        \
        size_t _i;                              \
                                                \
        for (_i = 0; _i < size; _i += UINT64_LEN) {  \
            _tmp[0] = _in[_i + 7];                   \
            _tmp[1] = _in[_i + 6];                   \
            _tmp[2] = _in[_i + 5];                   \
            _tmp[3] = _in[_i + 4];                   \
            _tmp[4] = _in[_i + 3];                   \
            _tmp[5] = _in[_i + 2];                   \
            _tmp[6] = _in[_i + 1];                   \
            _tmp[7] = _in[_i + 0];                   \
            _out[_i + 0] = _tmp[0];                  \
            _out[_i + 1] = _tmp[1];                  \
            _out[_i + 2] = _tmp[2];                  \
            _out[_i + 3] = _tmp[3];                  \
            _out[_i + 4] = _tmp[4];                  \
            _out[_i + 5] = _tmp[5];                  \
            _out[_i + 6] = _tmp[6];                  \
            _out[_i + 7] = _tmp[7];                  \
        }                                            \
    }


int uint8_to_uint64(const uint8_t *in, size_t in_len, uint64_t *out, size_t out_len)
{
    int ret = RET_OK;

    CHECK_PARAM(in != NULL);
    CHECK_PARAM(in_len != 0);
    CHECK_PARAM(out != NULL);
    CHECK_PARAM(out_len * UINT64_LEN >= in_len);

    memcpy(out, in, in_len);
    if (out_len * UINT64_LEN > in_len) {
        memset((uint8_t *)out + in_len, 0, out_len * UINT64_LEN - in_len);
    }

    if (is_bigendian()) {
        swap_word64(out, out, in_len);
    }

cleanup:

    return ret;
}

int uint64_to_uint8(const uint64_t *in, size_t in_len, uint8_t *out, size_t out_len)
{
    int ret = RET_OK;

    CHECK_PARAM(in != NULL);
    CHECK_PARAM(in_len != 0);
    CHECK_PARAM(out != NULL);
    CHECK_PARAM(out_len >= in_len * UINT64_LEN);

    memcpy(out, in, in_len * UINT64_LEN);
    if (out_len > in_len * UINT64_LEN) {
        memset(out + in_len * UINT64_LEN, 0, out_len - in_len * UINT64_LEN);
    }

    if (is_bigendian()) {
        swap_word64(out, out, in_len * UINT64_LEN);
    }

cleanup:

    return ret;
}
