/*
 * Copyright (c) 2016 PrivatBank IT <acsk@privatbank.ua>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */

#include <string.h>

#include "byte_array_internal.h"
#include "byte_utils_internal.h"
#include "macros_internal.h"

#undef FILE_MARKER
#define FILE_MARKER "cryptonite/byte_array_internal.c"

ByteArray *ba_alloc_from_uint64(const uint64_t *buf, size_t buf_len)
{
    ByteArray *ba = NULL;
    int ret = RET_OK;

    if (buf != NULL && buf_len != 0) {
        MALLOC_CHECKED(ba, sizeof (ByteArray));
        ba->len = buf_len * UINT64_LEN;
        MALLOC_CHECKED(ba->buf, ba->len);
        DO(uint64_to_uint8(buf, buf_len, ba->buf, ba->len));
    }

cleanup:

    if (ret != RET_OK) {
        ba_free(ba);
        ba = NULL;
    }

    return ba;
}

