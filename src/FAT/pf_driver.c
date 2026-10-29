#include <types.h>
#include <FAT/pf2.h>

// HYPOTHESIS: untyped placeholders for the zero-initialized .bss of this unit, sizes and order from the original layout.
unsigned char lbl_805639C0[0xa30] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_805643F0[0xd0] __attribute__((aligned(8))) = { 0 };

// Function bodies below are derived from https://github.com/jj-link/brawl.

extern void fn_803D8E00(void *self, int mode);

void fn_803D80B0(int *arg0) {
    *arg0 &= ~4;
    fn_803D8E00(arg0, 0);
}

extern void fn_803D8E00(void* self, int mode);

void fn_803D80C4(u32* self) {
    *self |= 0x4;
    fn_803D8E00(self, 1);
}

int fn_803D8988(const unsigned int *p)
{
    return (p[0] >> 2) & 1;
}

// The disk-geometry converters of this build, as in VFipdm_disk_convert_*; the
// disk argument is unused.
s32 fn_803D781C(void* p_disk, u32 sector, u32 num_sector, u16 bps, u32* p_block, u32* p_num_block) {
    u32 ratio;
    s32 err = 0;

    *p_block = sector;
    *p_num_block = num_sector;
    if (bps > 0x200) {
        ratio = (bps >> 9) & 0x7F;
        if (ratio == 2) *p_block *= 2;
        else if (ratio == 4) *p_block *= 4;
        else if (ratio == 8) *p_block *= 8;
        if (ratio == 2) *p_num_block *= 2;
        else if (ratio == 4) *p_num_block *= 4;
        else if (ratio == 8) *p_num_block *= 8;
    } else if (bps < 0x200) {
        err = 4;
    }
    return err;
}

s32 fn_803D78CC(void* p_disk, u32 block, u32 num_block, u16 bps, u32* p_sector, u32* p_num_sector) {
    u32 ratio;
    s32 err = 0;

    *p_sector = block;
    *p_num_sector = num_block;
    if (bps > 0x200) {
        ratio = (bps >> 9) & 0x7F;
        if (ratio == 2) *p_sector >>= 1;
        else if (ratio == 4) *p_sector >>= 2;
        else if (ratio == 8) *p_sector >>= 3;

        if (ratio == 2) *p_num_sector >>= 1;
        else if (ratio == 4) *p_num_sector >>= 2;
        else if (ratio == 8) *p_num_sector >>= 3;
    } else if (bps < 0x200) {
        err = 4;
    }
    return err;
}
