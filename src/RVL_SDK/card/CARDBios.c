#include <types.h>

// HYPOTHESIS: untyped placeholders for the zero-initialized .bss of this unit, sizes and order from the original layout.
unsigned char lbl_804ECE00[0x240] __attribute__((aligned(8))) = { 0 };
// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
unsigned char lbl_805A0B98[0x8] __attribute__((aligned(4))) = { 0 };

// Function bodies below are taken from https://github.com/jj-link/brawl.

int fn_8020A53C(int);

int fn_8020A4EC(int arg0) {
    if (arg0 == 0) {
        if (fn_8020A53C(0) == -1 || fn_8020A53C(1) == -1) {
            return 0;
        }
    }
    return 1;
}
