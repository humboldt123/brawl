#include <types.h>

// HYPOTHESIS: untyped placeholders for the zero-initialized .bss of this unit, sizes and order from the original layout.
unsigned char lbl_80534618[0x10] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_80534628[0x10] __attribute__((aligned(8))) = { 0 };

// Function bodies below are taken from https://github.com/jj-link/brawl.

int fn_80359B2C(int *arg0, int *arg1) {
    return *arg0 - *arg1;
}
