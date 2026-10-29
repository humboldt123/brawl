#include <types.h>
#include <FAT/pf2.h>

// Function bodies below are derived from https://github.com/jj-link/brawl, where each
// one was matched in a translation unit of its own.

int fn_803D72C0(PF2_VOLUME* p_vol, u32* p_result) {
    int err;

    *p_result = 0;
    err = fn_803D79B0(p_vol);
    if (err != 0) {
        return err;
    }
    return 0;
}

int fn_803D72F8(PF2_VOLUME* p_vol, const u8* param, u32* p_result) {
    int err;

    *p_result = 0;
    err = fn_803D8878(p_vol, param);
    if (err != 0) {
        return err;
    }
    return 0;
}
