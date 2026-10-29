#include <types.h>
#include <stdarg.h>

// HYPOTHESIS: untyped placeholders for the zero-initialized .bss of this unit, sizes and order from the original layout.
unsigned char lbl_8049E560[0x10] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_8049E570[0xc] __attribute__((aligned(8))) = { 0 };
// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
unsigned char lbl_805A0310[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A0314[0x1] __attribute__((aligned(4))) = { 0 };

// Function bodies below are taken from https://github.com/jj-link/brawl.

extern "C" void fn_80101810(...) {}
