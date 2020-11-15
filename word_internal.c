/*
 * Copyright (c) 2016 PrivatBank IT <acsk@privatbank.ua>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */

#include <memory.h>

#include "word_internal.h"
#include "byte_utils_internal.h"
#include "macros_internal.h"

#undef FILE_MARKER
#define FILE_MARKER "cryptonite/word_internal.c"

WordArray *wa_alloc(size_t len)
{
    WordArray *wa = NULL;
    int ret = RET_OK;

    CHECK_PARAM(len != 0);

    CALLOC_CHECKED(wa, sizeof(WordArray));
    MALLOC_CHECKED(wa->buf, len * WORD_BYTE_LENGTH);
    wa->len = len;

cleanup:
    if (ret != RET_OK) {
        wa_free(wa);
        wa = NULL;
    }

    return wa;
}



WordArray *wa_alloc_from_ba(const ByteArray *ba)
{
    WordArray *ans = NULL;
    int ret;

    CHECK_PARAM(ba != NULL);

    CHECK_NOT_NULL(ans = wa_alloc_from_le(ba->buf, ba->len));

cleanup:

    return ans;
}

WordArray *wa_alloc_from_le(const uint8_t *in, size_t in_len)
{
    WordArray *wa = NULL;
    int ret = RET_OK;

    CHECK_PARAM(in != NULL);
    CHECK_PARAM(in_len != 0);

    MALLOC_CHECKED(wa, sizeof(WordArray));
    wa->len = (in_len + WORD_BYTE_LENGTH - 1) / WORD_BYTE_LENGTH;
    MALLOC_CHECKED(wa->buf, wa->len * WORD_BYTE_LENGTH);

#ifdef ARCH64
    DO(uint8_to_uint64(in, in_len, wa->buf, wa->len));
#else
    DO(uint8_to_uint32(in, in_len, wa->buf, wa->len));
#endif

cleanup:
    if (ret != RET_OK) {
        wa_free(wa);
        wa = NULL;
    }
    return wa;
}

ByteArray *wa_to_ba(const WordArray *wa)
{
    ByteArray *ans = NULL;
    int ret;

    if (wa != NULL) {
#ifdef ARCH64
        CHECK_NOT_NULL(ans = ba_alloc_from_uint64(wa->buf, wa->len));
#else
        CHECK_NOT_NULL(ans = ba_alloc_from_uint32(wa->buf, wa->len));
#endif
    }

cleanup:

    return ans;
}

void wa_free(WordArray *in)
{
    if (in) {
        free(in->buf);
        free(in);
    }
}
