#include <types.h>
#include <FAT/pf2.h>
#include <revolution/FA/FA.h>

// HYPOTHESIS: untyped placeholders for the zero-initialized .bss of this unit, sizes and order from the original layout.
unsigned char lbl_80597FB0[0x68] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_80598018[0x68] __attribute__((aligned(8))) = { 0 };
// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
unsigned char lbl_805A1268[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A126C[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A1270[0x8] __attribute__((aligned(4))) = { 0 };

// Function bodies below are derived from https://github.com/jj-link/brawl.

extern FAHandle* fn_803EB644(const char*, const char*);

FAHandle* FAFopen(const char* filepath, const char* mode) {
    FAHandle* handle = fn_803EB644(filepath, mode);
    if (handle == NULL)
        return NULL;
    return handle;
}

int fn_803EC094(s8 arg0) {
    int result = fn_803EB8B4(arg0);
    if (result != 0 && result != 1) {
        result = -1;
    }
    return result;
}

FACallbacks lbl_80598080 = { 0 };

int FAClosedir(FAHandle* handle) {
    int err = fn_803EB5D8(handle);

    if (err != 0) {
        return -1;
    }
    return 0;
}

int fn_803EBE4C(void) {
    int err = fn_803EB5FC();

    if (err == 5) {
        return 5;
    }
    if (err == 0) {
        return 0;
    }
    return err;
}

int FAFclose(FAHandle* handle) {
    int err = fn_803EB620(handle);

    if (err != 0) {
        return -1;
    }
    return 0;
}

int FAFseek(FAHandle* handle, int offset, int mode) {
    int err = fn_803EB708(handle, offset, mode);

    if (err != 0) {
        return -1;
    }
    return 0;
}

int FAFsfirst(const char* pattern, int filter, FAEntryInfo* ret) {
    int err = fn_803EB72C(pattern, filter, ret);

    if (err != 0) {
        return -1;
    }
    return 0;
}

int FAFsnext(FAEntryInfo* ret) {
    int err = fn_803EB7A0(ret);

    if (err != 0) {
        return -1;
    }
    return 0;
}

int FAFstat(const char* filepath, FAStat* st) {
    int err = fn_803EB7C4(filepath, st);

    if (err != 0) {
        return -1;
    }
    return 0;
}

int fn_803EBF9C(s8 drv_char) {
    int err = fn_803EB84C(drv_char);

    if (err != 0) {
        return -1;
    }
    return 0;
}

int fn_803EC0CC(const char* path) {
    int err = fn_803EB8F8(path);

    if (err != 0) {
        return -1;
    }
    return 0;
}

int fn_803EC0F8(s8 drv_char) {
    int err = fn_803EB948(drv_char);

    if (err != 0) {
        return -1;
    }
    return 0;
}

FAHandle* FAOpendir(const char* path) {
    FAHandle* dir = fn_803EB970(path);

    if (dir == NULL) {
        dir = NULL;
    }
    return dir;
}

void fn_803EC1BC(s8 drv_char) {
    if (lbl_80598080.unk0) {
        lbl_80598080.unk0(drv_char);
    }
}

void fn_803EC1DC(s8 drv_char) {
    if (lbl_80598080.unk4 == NULL) {
        return;
    }
    lbl_80598080.unk4(drv_char);
}

int FARemove(const char* filepath) {
    int err = fn_803EB9C8(filepath);

    if (err != 0) {
        return -1;
    }
    return 0;
}

int fn_803EC22C(s8 drv_char) {
    int result = fn_803EBA18(drv_char);

    if (result == -1) {
        return -1;
    } else if (result == 1) {
        return 1;
    } else {
        if (result != 0) {
            return -1;
        }
        return 0;
    }
}
