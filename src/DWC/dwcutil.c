#include <types.h>

// HYPOTHESIS: untyped placeholders for the zero-initialized .bss of this unit, sizes and order from the original layout.
unsigned char lbl_80534008[0x28] __attribute__((aligned(8))) = { 0 };
// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
unsigned char lbl_805A0FB8[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A0FBC[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A0FC0[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A0FC4[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A0FC8[0x8] __attribute__((aligned(4))) = { 0 };

// Function bodies below are taken from https://github.com/jj-link/brawl.

void fn_80350868(u8 *arg0, u32 arg1) {
    u8 *p = arg0;
    u32 i;
    u32 v;
    u32 n;

    for (i = 0; i < 0x100; i++, p++) {
        v = i;
        for (n = 0; n < 8; n++) {
            if (v & 0x80) {
                v = (v << 1) ^ arg1;
            } else {
                v = v << 1;
            }
        }
        *p = v;
    }
}

void fn_80350908(u32 *arg0, u32 arg1) {
    u32 *p = arg0;
    u32 i;
    u32 v;
    u32 n;

    for (i = 0; i < 0x100; i++) {
        v = i;
        for (n = 0; n < 8; n++) {
            if (v & 1) {
                v = (v >> 1) ^ arg1;
            } else {
                v = v >> 1;
            }
        }
        *p++ = v;
    }
}

int fn_80350C4C(unsigned *p) {
    unsigned x = *p >> 11;
    int r;
    if ((x & 3) == 3)
        r = (x >> 2) & 1;
    else
        return 0;
    return r;
}

int fn_80350C70(const unsigned int *p)
{
    return (p[0] >> 0xB) & 3;
}
