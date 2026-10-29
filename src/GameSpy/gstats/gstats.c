#include <types.h>
#include <GameSpy/gstats/gstats.h>

// HYPOTHESIS: untyped placeholders for the zero-initialized .bss of this unit, sizes and order from the original layout.
unsigned char lbl_80535DE0[0x200] __attribute__((aligned(8))) = { 0 };
// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
unsigned char lbl_805A1098[0x8] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A10A0[0x8] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A10A8[0x8] __attribute__((aligned(4))) = { 0 };
statsgame_t lbl_805A10B0 = NULL;
unsigned char lbl_805A10B4[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A10B8[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A10BC[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A10C0[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A10C4[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A10C8[0x8] __attribute__((aligned(4))) = { 0 };

// Function bodies below are derived from https://github.com/jj-link/brawl, where each
// one was matched in a translation unit of its own; they follow the GameSpy SDK's
// gstats.c.

int fn_80383698(void) {
    return lbl_8059FAF0 != -1;
}

int fn_80384E70(statsgame_t game, char* name, bucketfunc_t func, int value) {
    int* ret;

    if (game == NULL) {
        game = lbl_805A10B0;
    }
    if (game == NULL) {
        ret = &value;
    } else {
        ret = func(game->buckets, name, &value);
        if (ret == NULL) {
            ret = fn_803822D8(game->buckets, name, bt_int, &value);
        }
    }
    return *ret;
}

double fn_80384EFC(statsgame_t game, char* name, bucketfunc_t func, double value) {
    double* ret;

    if (game == NULL) {
        game = lbl_805A10B0;
    }
    if (game == NULL) {
        ret = &value;
    } else {
        ret = func(game->buckets, name, &value);
        if (ret == NULL) {
            ret = fn_803822D8(game->buckets, name, bt_float, &value);
        }
    }
    return *ret;
}

char* fn_80384F88(statsgame_t game, char* name, bucketfunc_t func, char* value) {
    char* ret;

    if (game == NULL) {
        game = lbl_805A10B0;
    }
    if (game == NULL) {
        return value;
    }
    ret = func(game->buckets, name, value);
    if (ret == NULL) {
        ret = fn_803822D8(game->buckets, name, bt_string, value);
    }
    return ret;
}
