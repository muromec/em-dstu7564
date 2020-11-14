/*
 * Copyright (c) 2016 PrivatBank IT <acsk@privatbank.ua>. All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */

#include <stddef.h>
#include "utest.h"

static size_t error_count = 0;

int main(void)
{
    utest_dstu7564();

    return error_count == 0 ? 0 : -1;
}
