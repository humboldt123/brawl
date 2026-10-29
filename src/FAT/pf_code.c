#include <types.h>
#include <FAT/pf2.h>

// Function bodies below are derived from https://github.com/jj-link/brawl.

s32 fn_803EB304(s32 arg0, s32 arg1)
{
    u8 c = arg0;

    switch (arg1) {
    case 1:
        return (c >= 0x81U && c <= 0x9FU) ||
               (c >= 0xE0U && c <= 0xFCU);
    case 2:
        return (c >= 0x40U && c <= 0x7EU) ||
               (c >= 0x80U && c <= 0xFCU);
    default:
        return 0;
    }
}

s16 fn_803EB2D0(const s8* buf) {
    u16 uc;
    short oem_width;
    short uni_width;

    fn_803D65A4(fn_803EAEAC(buf, &uc), &oem_width, &uni_width);
    return oem_width;
}

s16 fn_803EB380(const u16* buf) {
    s8 oem[4];
    short oem_width;
    short uni_width;

    fn_803D65A4(fn_803EB068(buf, oem), &oem_width, &uni_width);
    return uni_width;
}
