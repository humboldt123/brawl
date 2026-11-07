#include "revolution.h"
#include "revolution/nwc24.h"
#include "revolution/nwc24/NWC24Internal.h"
#include "revolution/sc.h"
#include "revolution/vf.h"
#include <cstring>
#include <cstdarg>

// BYTES: preserve the SDK's explicit inline helpers.
#pragma auto_inline off

/* ---------------------------------------------------------------------------------------------
 * utils
 * ------------------------------------------------------------------------------------------- */

void NWC24Data_Init(NWC24Data *data) {
    data->pData = NULL;
    data->size = 0;
}

void NWC24Data_SetDataP(NWC24Data *data, const void *ptr, u32 size) {
    data->pData = ptr;
    data->size = size;
}

void NWC24Date_Init(NWC24Date *date) {
    date->year = 2000;
    date->month = 1;
    date->day = 1;
    date->hour = 12;
    date->min = 0;
    date->sec = 0;
    date->BYTE_0x7 = 0;
}

// writes addr as 16 decimal digits (despite the name), zero padded, plus a terminator
void NWC24iConvIdToStr(u64 addr, char *out) {
    u64 a = addr;
    int i;
    // BYTES: MWCC folds this real loop bound but emits an unused private constant.
    const int n = sizeof(u64) * 2;

    for (i = n - 1; i >= 0; i--) {
        out[i] = '0' + a % 10;
        a /= 10;
    }

    out[n] = '\0';
}

NWC24Err fn_80399EE0(const char *str, u32 minLen, u32 maxLen) {
    u32 len;

    if (str == NULL) {
        return NWC24_ERR_NULL;
    }

    len = STD_strnlen(str, maxLen + 1);
    if (len == 0) {
        return NWC24_ERR_NULL;
    }

    if (len > maxLen) {
        return NWC24_ERR_FULL;
    }

    if (len < minLen) {
        return NWC24_ERR_FORMAT;
    }

    return NWC24_OK;
}

int fn_80399F5C(char *dst, const char *src, int size) {
    char *d;
    int i;

    if (dst == NULL || src == NULL) {
        return 0;
    }

    d = dst;
    i = 0;
    while (i + 1 < size) {
        *d = *src;
        if (*src == 0) {
            break;
        }

        src++;
        i++;
        d++;
    }

    if (i + 1 >= size && size != 0) {
        dst[i] = 0;
    }

    return i;
}
