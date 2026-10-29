#include <types.h>
#include <string.h>
#include <GameSpy/common/gsLargeInt.h>

// HYPOTHESIS: untyped placeholders for the zero-initialized .bss of this unit, sizes and order from the original layout.
unsigned char lbl_805349A8[0x98] __attribute__((aligned(8))) = { 0 };

// Function bodies below are derived from https://github.com/jj-link/brawl.

u32 fn_8035EC44(s32 *arg0, s32 *arg1, u32 *arg2)
{
    s32 *p0;
    s32 *p1;
    s32 n0;
    s32 n1;
    u32 size;

    n0 = *arg0;
    n1 = *arg1;
    p0 = arg0 + n0;
    while (n0 > 0 && *p0 == 0) {
        p0--;
        n0--;
    }

    p1 = arg1 + n1;
    while (n1 > 0 && *p1 == 0) {
        p1--;
        n1--;
    }

    if (n0 > n1) {
        n1 = n0;
    }

    *arg2 = (u32)n1;
    size = 1;
    while (size < (u32)n1) {
        size <<= 1;
    }

    *arg2 = size;
    return size <= 64u;
}

int fn_8035ECE0(u32* a, u32 an, u32* b, u32 bn)
{
    u32* p = a + an;
    u32* q;
    while (p[-1] == 0 && an != 0) {
        p--;
        an--;
    }
    q = b + bn;
    while (q[-1] == 0 && bn != 0) {
        q--;
        bn--;
    }
    if (an < bn) return -1;
    if (an > bn) return 1;
    q = b + an;
    p = a + an;
    for (; an != 0; an--) {
        u32 y = q[-1];
        u32 x = p[-1];
        if (x < y) return -1;
        if (x > y) return 1;
        q--;
        p--;
    }
    return 0;
}

int fn_8035EB8C(gsLargeInt_t* lint, l_word length) {
    if (length > GS_LARGEINT_MAX_DIGITS) {
        return 0;
    }
    if (lint->mLength >= length) {
        while (lint->mLength > length && lint->mData[lint->mLength - 1] == 0) {
            lint->mLength--;
        }
        if (lint->mLength == length) {
            return 1;
        }
        return 0;
    }
    memset(&lint->mData[lint->mLength], 0, (length - lint->mLength) * sizeof(l_word));
    lint->mLength = length;
    return 1;
}

int fn_803612EC(gsLargeInt_t* lint) {
    char* left;
    char* right;
    char temp;

    if (lint->mLength == 0) {
        return 1;
    }
    right = (char*)lint->mData + (lint->mLength * sizeof(l_word)) - 1;
    left = (char*)lint->mData;
    while (left < right) {
        temp = *left;
        *left++ = *right;
        *right-- = temp;
    }
    return 1;
}

int fn_80361878(SHA1Context* context, u8 Message_Digest[20]) {
    int i;

    if (!context || !Message_Digest) {
        return 1;
    }
    if (context->Corrupted) {
        return context->Corrupted;
    }
    if (!context->Computed) {
        fn_80362090(context);
        for (i = 0; i < 64; ++i) {
            context->Message_Block[i] = 0;
        }
        context->Length_Low = 0;
        context->Length_High = 0;
        context->Computed = 1;
    }
    for (i = 0; i < 20; ++i) {
        Message_Digest[i] = context->Intermediate_Hash[i >> 2] >> 8 * (3 - (i & 0x03));
    }
    return 0;
}
