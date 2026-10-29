#include <types.h>

// HYPOTHESIS: untyped placeholders for the zero-initialized .bss of this unit, sizes and order from the original layout.
unsigned char lbl_80563940[0x20] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_80563960[0x60] __attribute__((aligned(8))) = { 0 };
// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
unsigned char lbl_805A1230[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A1234[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A1238[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A123C[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A1240[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A1244[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A1248[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A124C[0x4] __attribute__((aligned(4))) = { 0 };

// Function bodies below are taken from https://github.com/jj-link/brawl.

void fn_803CB99C(int a, int b, int c, int d, int e, int f);

void fn_803CB96C(int a, int b, int c, int d) {
    fn_803CB99C(a, b, c, d, 0, 0);
}

void fn_803CBCD8(int a, int b, int c, int d, int e, int f);

void fn_803CB978(int a, int b, int c, int d) {
    fn_803CBCD8(a, b, c, d, 0, 0);
}

extern void fn_803CB99C(int a, int b, int c, int d, int e, int f);

void fn_803CB984(int a, int b, int c, int d) {
    fn_803CB99C(a, b, c, d, 0, 0);
}

void fn_803CBCD8(int, int, int, int, int, int);

void fn_803CB990(int a, int b, int c, int d) {
    fn_803CBCD8(a, b, c, d, 0, 0);
}

int RFLGetWorkSize(int arg0) {
    if (arg0 != 0) {
        return 0x65F20;
    }
    return 0x4CF20;
}
