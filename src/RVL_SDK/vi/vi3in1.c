#include <types.h>

// HYPOTHESIS: untyped placeholders for the zero-initialized .bss of this unit, sizes and order from the original layout.
unsigned char lbl_804DE6C0[0x20] __attribute__((aligned(8))) = { 0 };
// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
unsigned char lbl_805A0858[0x4] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_805A085C[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A0860[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A0864[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A0868[0x8] __attribute__((aligned(4))) = { 0 };

// Function bodies below are taken from https://github.com/jj-link/brawl.

void WaitMicroTime(int);
void __VISendI2CData(int, unsigned char *, int);

void __VISetFilter4EURGB60(unsigned char arg0) {
    unsigned char buf[2];

    buf[1] = arg0;
    buf[0] = 0x6E;
    __VISendI2CData(0xE0, buf, 2);
    WaitMicroTime(2);
}
