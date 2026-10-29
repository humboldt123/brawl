#include <types.h>

// HYPOTHESIS: untyped placeholders for the zero-initialized .bss of this unit, sizes and order from the original layout.
unsigned char lbl_805344B0[0x18] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_805344C8[0x10] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_805344D8[0xa8] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_80534580[0x18] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_80534598[0x80] __attribute__((aligned(8))) = { 0 };
// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
unsigned char lbl_805A0FE8[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A0FEC[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A0FF0[0x8] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A0FF8[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A0FFC[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A1000[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A1004[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A1008[0x8] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A1010[0x8] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A1018[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A101C[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A1020[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A1024[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A1028[0x8] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A1030[0x8] __attribute__((aligned(4))) = { 0 };

// Function bodies below are taken from https://github.com/jj-link/brawl.

extern int DWC_AllocEx(int, int, int);

int DWCi_NdAlloc(int arg0, int arg1) {
    return DWC_AllocEx(10, arg0, arg1);
}

void DWC_Free(s32 a, void* b, s32 c);

void DWCi_NdFree(void* arg0) {
    DWC_Free(0xA, arg0, 0);
}
