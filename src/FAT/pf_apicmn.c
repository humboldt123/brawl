#include <types.h>
#include <FAT/pf2.h>

// Function bodies below are derived from https://github.com/jj-link/brawl, where each
// one was matched in a translation unit of its own. HYPOTHESIS: this unit spans
// several original source files, so its functions are not auto-inlined into one
// another; helpers the original inlined are marked inline.
// Checked: without this pragma 10 functions in this unit stop matching.
#pragma auto_inline off

void* fn_803EB4EC(void* arg0, void* arg1) {
    if (arg0 != 0) {
        return 0;
    }
    return arg1;
}

s32 fn_803EB504(s32 arg0) {
    if (arg0 == 0) {
        return 0;
    }
    if (arg0 == 1) {
        return 1;
    }
    return -1;
}

s32 fn_803EAAE4(s32 arg0);

s32 fn_803EBA18(s8 arg0) {
    s32 result = fn_803EAAE4(arg0);
    return fn_803EB504(result);
}

// PrFile2 -> FA error table
extern s32 lbl_80492FE8[0xA0 / 4];

s32 fn_803EB48C(s32 err) {
    if (err == 0) return 0;
    if (err == -1) return -1;
    if (err > 0 && err < 0xA0) return lbl_80492FE8[err];
    if (err == 0x1000) return 5;
    return err;
}

int PFAPI_convertReturnValue(int err) {
    if (err != 0) {
        return -1;
    }
    return 0;
}

int fn_803EB5D8(PF2_DIR* dir) {
    return PFAPI_convertReturnValue(fn_803DC358(dir));
}

int fn_803EB5FC(void) {
    return fn_803EB48C(fn_803EA188());
}

int fn_803EB620(PF2_FILE* file) {
    return PFAPI_convertReturnValue(fn_803E5774(file));
}

u32 fn_803EB6E0(void* buf, u32 size, u32 count, PF2_FILE* file) {
    u32 count_read;

    fn_803E5A50(buf, size, count, file, &count_read);
    return count_read;
}

int fn_803EB708(PF2_FILE* file, s32 offset, s32 origin) {
    return PFAPI_convertReturnValue(fn_803E5D00(file, offset, origin));
}

int fn_803EB7A0(FAEntryInfo* dta) {
    return PFAPI_convertReturnValue(fn_803DC120(dta));
}

u32 fn_803EB824(void* buf, u32 size, u32 count, PF2_FILE* file) {
    u32 count_written;

    fn_803E5B8C(buf, size, count, file, &count_written);
    return count_written;
}

int fn_803EB84C(s8 drv_char) {
    return PFAPI_convertReturnValue(fn_803EA198(drv_char));
}

int fn_803EB874(void) {
    int err;

    err = fn_803E3610();
    if (err == 0) {
        fn_803EBBD8();
    }
    return PFAPI_convertReturnValue(err);
}

int fn_803EB8B4(s8 drv_char) {
    int result = 0;
    int err = fn_803EAE34(drv_char, &result);

    if (err == 0) {
        return result;
    }
    return PFAPI_convertReturnValue(err);
}

int fn_803EB948(s8 drv_char) {
    return PFAPI_convertReturnValue(fn_803EA788(drv_char));
}
