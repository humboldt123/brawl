#include <types.h>
// The combined unit exports the entry helpers. Keep its declarations local:
// the sibling pf_entry.h gives those helpers internal linkage.
#define RVL_SDK_VF_PF_ENTRY_H
#include <FAT/fat_vf.h>
#include <revolution/OS/OSTime.h>

u32 VFiPFENT_compareAttr(u8, u8, u8);
s32 VFiPFENT_compareEntryName(PF_DIR_ENT*, PF_STR*, u8);
s32 VFiPFENT_getEntry(PF_DIR_ENT*, PF_ENT_ITER*, PF_STR*, u8, u8, u32*);
s32 VFiPFENT_searchEmptyTailSFN(PF_FFD*, u32, const s8*, u32*);
s32 VFiPFENT_findEmptyTailSFN(PF_DIR_ENT*, const s8*, u32*);
u8 VFiPFENT_CalcCheckSum(PF_DIR_ENT*);
void VFiPFENT_LoadShortNameFromBuf(PF_DIR_ENT*, const u8*);
void VFiPFENT_loadEntryNumericFieldsFromBuf(PF_DIR_ENT*, const u8*);
s32 VFiPFENT_LoadLFNEntryFieldsFromBuf(PF_DIR_ENT*, const u8*);
void VFiPFENT_storeLFNEntryFieldsToBuf(u8*, PF_DIR_ENT*, u8, u8, u32);
s32 VFiPFENT_findEntryPos(PF_FFD*, PF_DIR_ENT*, u32, PF_STR*, u8, u8, u32*, u32*);
s32 VFiPFENT_findEntry(PF_FFD*, PF_DIR_ENT*, u32, PF_STR*, u8, u8);
s32 VFiPFENT_allocateEntryPos(PF_DIR_ENT*, u8, PF_FFD*, u32*, PF_STR*, u32*);
s32 VFiPFENT_allocateEntry(PF_DIR_ENT*, u8, PF_FFD*, u32*, PF_STR*);
s32 VFiPFENT_GetRootDir(PF_VOLUME*, PF_DIR_ENT*);
s32 VFiPFENT_MakeRootDir(PF_VOLUME*);
s32 VFiPFENT_updateEntry(PF_DIR_ENT*, u32);
s32 VFiPFENT_AdjustSFN(PF_DIR_ENT*, s8*);
s32 VFiPFENT_RemoveEntry(PF_DIR_ENT*, PF_ENT_ITER*);
u8 VFiPFENT_getcurrentDateTimeForEnt(u16*, u16*);

// HYPOTHESIS: the disk manager set and the PrFile2 volume set (see fat_vf.h);
// the remaining buffers are still untyped, sizes from the original .bss layout.
PDM_DISK_SET lbl_8053A6E0 = { 0 };
PF_VOLUME_SET lbl_8053B248 = { 0 };
OSMutex lbl_80563200 = { 0 };
u8 lbl_80563218[0x68] = { 0 };
u8 lbl_80563280[0xD0] = { 0 };
u8 lbl_80563350[0x10] = { 0 };
dHashEntry lbl_80563360[31] = { 0 };
VF_DRIVE_INFO lbl_80563590[26] = { 0 };

// HYPOTHESIS: the zero-initialized .sbss of this unit, order from the original layout.
PF_SYSTEM_SET lbl_805A1200 = { 0 };
u32 lbl_805A1208 = 0;
u32 lbl_805A120C = 0;
u32 lbl_805A1210 = 0;
s32 lbl_805A1214 = 0;
unsigned char lbl_805A1218[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A121C[0x4] __attribute__((aligned(4))) = { 0 };
VF_HANDLE_TYPE* lbl_805A1220 = NULL;
VFTimeStampCallback lbl_805A1224 = NULL;

// Function bodies below are derived from https://github.com/jj-link/brawl, where each
// one was matched in a translation unit of its own. HYPOTHESIS: this unit spans
// several original source files, so its functions are not auto-inlined into one
// another; helpers the original inlined are marked inline.
// Checked: without this pragma 53 functions in this unit stop matching.
#pragma auto_inline off

s32 VFiPFAPI_ParseOpenModeString(const s8* mode_str) {
    s32 mode;
    u32 index;
    if (mode_str == NULL)
        return 10;
    index = 1;
    switch (mode_str[0]) {
    case 'r': mode = 2; break;
    case 'w': mode = 1; break;
    case 'a': mode = 4; break;
    default: return 0;
    }
    if (mode_str[1] == 'b')
        index = 2;
    switch (mode_str[index++]) {
    case 0: return mode;
    case '+': break;
    case 't':
    default: return 0;
    }
    switch (mode_str[index]) {
    case 0: return mode | 8;
    default: return 0;
    }
}

// ---------------------------------------------------------------- pf_clib

void* fn_803A9644(void* dst, const void* src, u32 length) { // VFipf_memcpy
    u8* d = dst;
    const u8* s = src;

    if (((u32)d % 4) == 0 && ((u32)s % 4) == 0) {
        for (; length > 3; length -= 4) {
            *(u32*)d = *(u32*)s;
            d += 4;
            s += 4;
        }
    }
    for (; length != 0; length--) {
        *d++ = *s++;
    }
    return dst;
}

int fn_803A962C(int a) {
    if (a < 0x61)
        return a;
    if (a <= 0x7A)
        return a - 0x20;
    return a;
}

void* fn_803A976C(void* dst, int c, u32 length) {
    u8* p = dst;
    u32* word;
    u32 fill;

    while (((u32)p & 3) && length) {
        *p++ = c;
        --length;
    }

    fill = c | (c << 8) | ((c << 24) | (c << 16));

    word = (u32*)p;
    while (length > 3) {
        *word++ = fill;
        length -= 4;
    }
    p = (u8*)word;

    while (length--) {
        *p++ = c;
    }

    return dst;
}

u32 fn_803A9864(const s8* s) {
    const s8* p;

    for (p = s; *p != 0; p++) {
    }
    return p - s;
}

s8* fn_803A9884(s8* dst, const s8* src) {
    s8* d = dst;

    while ((*d = *src) != 0) {
        src++;
        d++;
    }
    return dst;
}

s32 fn_803A98A8(u8 *arg0, u8 *arg1) {
    u8 c;
    u8 d;
    while ((c = *arg0) != 0 && (d = *arg1) != 0 && c == d) {
        arg0++;
        arg1++;
    }
    return c - *arg1;
}

s32 fn_803A98E0(u8 *s1, u8 *s2, u32 n) {
    u32 i;
    u8 c1;

    for (i = n; i != 0; i--) {
        c1 = *s1;
        if (c1 == 0 || *s2 == 0 || c1 != *s2) {
            return c1 - *s2;
        }
        s1++;
        s2++;
    }
    return 0;
}

// ---------------------------------------------------------------- pf_code / pf_str

int VFiPFCODE_Combine_Width(int arg0, int arg1) {
    return (arg0 << 0x10) + arg1;
}

void VFiPFCODE_Divide_Width(unsigned int arg0, short *arg1, short *arg2) {
    *arg1 = arg0 >> 16;
    *arg2 = arg0;
}

u16 fn_803A9948(const u8* buf) {
    return (buf[0] & ~0xFF00) | ((buf[1] << 8) & 0xFF00);
}

void fn_803A9A70(PF_STR* p_str, u32 code_mode) { // VFiPFSTR_SetCodeMode
    p_str->code_mode = code_mode;
}

u32 fn_803A9A78(PF_STR* p_str) { // VFiPFSTR_GetCodeMode
    return p_str->code_mode;
}

void fn_803A9A80(PF_STR* p_str, s8* p_local) {
    if (p_str->code_mode == 1 || p_local == NULL) {
        p_str->p_local = p_str->p_head;
        return;
    }
    p_str->p_local = p_local;
}

const s8* fn_803A9AA8(PF_STR* p_str, u32 target) {
    if (target == 1) {
        return p_str->p_head;
    }
    if (target == 2) {
        return p_str->p_tail;
    }
    return p_str->p_local;
}

s32 VFiPFSTR_InitStr(PF_STR* p_str, const s8* s, u32 code_mode) {
    if (p_str == NULL || s == NULL) {
        return 10;
    }
    if (code_mode == 1) {
        p_str->p_head = s;
        p_str->p_tail = s + fn_803A9864(s);
    } else if (code_mode == 2) {
        p_str->p_head = s;
        p_str->p_tail = s + fn_803A9FF8((const u16*)s) * 2;
    } else {
        return 10;
    }
    p_str->code_mode = code_mode;
    return 0;
}

void VFiPFSTR_MoveStrPos(PF_STR* p_str, s16 num_s8) {
    s16 i;
    s16 offset;
    const s8* p;
    const u16* w;
    s32 size;
    offset = 0;
    if (p_str->code_mode == 1) {
        p = p_str->p_head;
        while (num_s8 != 0) {
            if (lbl_8053B248.codeset.is_oem_mb_char(*p, 1)) {
                offset++;
            }
            offset++;
            num_s8--;
        }
    } else {
        w = (const u16*)p_str->p_head;
        for (i = 0; i < num_s8; i++) {
            size = lbl_8053B248.codeset.unicode_char_width(w);
            w += size;
            offset += (s16)size;
        }
    }
    p_str->p_head += offset;
}

u16 VFiPFSTR_StrLen(PF_STR* p_str) {
    return p_str->p_tail - p_str->p_head;
}

s32 VFiPFSTR_StrCmp(const PF_STR* p_str, const s8* s) {
    if (p_str->code_mode == 1) {
        return fn_803A98A8((u8*)p_str->p_head, (u8*)s);
    }
    {
    const u16* wide = (const u16*)p_str->p_head;
    u16 unicode;
    u16 ch;
    do {
        lbl_8053B248.codeset.oem2unicode(s, &unicode);
        ch = *wide;
        s++;
        wide++;
        if (ch != unicode) break;
        if (wide[-1] == 0) break;
    } while (unicode != 0);
    return wide[-1] - unicode;
    }
}

s32 VFiPFSTR_StrNCmp(PF_STR* p_str, const s8* s, u32 target, s16 offset, u16 num) {
    const s8* p;

    if (p_str->code_mode == 1 || target == 3) {
        if (target == 1) {
            p = p_str->p_head + offset;
        } else if (target == 2) {
            p = p_str->p_tail + offset;
        } else {
            p = p_str->p_local + offset;
        }
        return fn_803A98E0((u8*)p, (u8*)s, num);
    }
    {
        const u16* wide;
        u16 unicode;
        u16 ch;

        if (target == 1) {
            wide = (const u16*)p_str->p_head + offset;
        } else {
            wide = (const u16*)p_str->p_tail + offset;
        }
        do {
            lbl_8053B248.codeset.oem2unicode(s, &unicode);
            ch = *wide;
            s++;
            num--;
            wide++;
            if (ch != unicode) break;
            if (num == 0) break;
            if (wide[-1] == 0) break;
        } while (unicode != 0);
        return wide[-1] - unicode;
    }
}

void VFiPFSTR_ToUpperNStr(PF_STR* p_str, u16 num, s8* dest) {
    u32 c;
    int is_lower;
    if (p_str->code_mode == 1) {
        const s8* p = p_str->p_head;
        while (num != 0 && *p != 0) {
            *dest = fn_803A962C(*p);
            dest++;
            p++;
            num--;
        }
    } else {
        const u16* w = (const u16*)p_str->p_head;
        while (num != 0 && *w != 0) {
            c = *w;
            is_lower = c >= 0x61 && c <= 0x7a;
            if (is_lower) {
                c -= 0x20;
            }
            dest[0] = c;
            dest[1] = (u8)(c >> 8);
            w++;
            num--;
            dest += 2;
        }
        *dest = 0;
    }
    *dest = 0;
}

s32 fn_803A9FF8(const u16* s) {
    const u16* p = s;

    while (*p != 0) {
        p++;
    }
    return (s32)((u32)p - (u32)s) >> 1;
}

u16* fn_803AA01C(u16* dst, const u16* src) {
    u16* d = dst;

    while ((*d = *src) != 0) {
        src++;
        d++;
    }
    return dst;
}

s32 fn_803AA040(u16 *arg0, u16 *arg1, s32 arg2) {
    u16 *s1 = arg0;
    u16 *s2 = arg1;
    s32 n = arg2;

    while (n != 0) {
        u16 c1 = *s1;
        if (c1 == 0 || *s2 == 0 || c1 != *s2) {
            return c1 - *s2;
        }
        s1++;
        s2++;
        n--;
    }
    return 0;
}

// ---------------------------------------------------------------- pf_driver

s32 VFiPFDRV_GetBPBInformation(u8* p_buf, PF_BPB* p_bpb) {
    PDM_BPB bpb;

    if (VFipdm_bpb_get_bpb_information(p_buf, &bpb) != 0) {
        return 7;
    }
    p_bpb->bytes_per_sector = bpb.bytes_per_sector;
    p_bpb->num_reserved_sectors = bpb.num_reserved_sectors;
    p_bpb->num_root_dir_entries = bpb.num_root_dir_entries;
    p_bpb->sectors_per_cluster = bpb.sectors_per_cluster;
    p_bpb->num_FATs = bpb.num_FATs;
    p_bpb->total_sectors = bpb.total_sectors;
    p_bpb->sectors_per_FAT = bpb.sectors_per_FAT;
    p_bpb->root_dir_cluster = bpb.root_dir_cluster;
    p_bpb->fs_info_sector = bpb.fs_info_sector;
    p_bpb->backup_boot_sector = bpb.backup_boot_sector;
    p_bpb->ext_flags = bpb.ext_flags;
    p_bpb->media = bpb.media;
    p_bpb->fat_type = bpb.fat_type;
    p_bpb->log2_bytes_per_sector = bpb.log2_bytes_per_sector;
    p_bpb->log2_sectors_per_cluster = bpb.log2_sectors_per_cluster;
    p_bpb->num_active_FATs = bpb.num_active_FATs;
    p_bpb->num_root_dir_sectors = bpb.num_root_dir_sectors;
    p_bpb->active_FAT_sector = bpb.active_FAT_sector;
    p_bpb->first_root_dir_sector = bpb.first_root_dir_sector;
    p_bpb->first_data_sector = bpb.first_data_sector;
    p_bpb->num_clusters = bpb.num_clusters;
    return 0;
}

u32 VFiPFDRV_IsInserted(PF_VOLUME* p_vol) {
    u32 is_insert;

    VFipdm_part_check_media_insert(p_vol->p_part, &is_insert);
    return is_insert;
}

u32 VFiPFDRV_IsDetected(PF_VOLUME* p_vol) {
    u32 is_detect;

    VFipdm_part_check_media_detect(p_vol->p_part, &is_detect);
    return is_detect;
}

u32 VFiPFDRV_IsWProtected(PF_VOLUME* p_vol) {
    u32 is_wprotect;

    VFipdm_part_check_media_write_protect(p_vol->p_part, &is_wprotect);
    return is_wprotect;
}

s32 VFiPFDRV_init(PF_VOLUME* p_vol) {
    if (p_vol == NULL) {
        return 10;
    }
    return 0;
}

s32 VFiPFDRV_finalize(PF_VOLUME* p_vol) {
    return p_vol ? 0 : 10;
}

s32 VFiPFDRV_unmount(PF_VOLUME* p_vol, u32 mode) {
    s32 err;
    s32 driver_error;

    if (p_vol == NULL) {
        return 10;
    }

    err = VFipdm_part_release_permission(p_vol->p_part, mode == 1);
    if (err != 0) {
        if (err == 0x15) {
            driver_error = VFipdm_part_get_driver_error_code(p_vol->p_part);
            lbl_8053B248.last_driver_error = driver_error;
            p_vol->last_driver_error = driver_error;
            return 0x1000;
        }
        return -1;
    }
    return 0;
}

s32 VFiPFDRV_format(PF_VOLUME* p_vol, const u8* param) {
    s32 err;
    s32 driver_error;

    if (p_vol == NULL) {
        return 10;
    }
    if (!(p_vol->flags & 2)) {
        err = VFipdm_part_get_permission(p_vol->p_part);
        if (err != 0) {
            if (err == 0x15) {
                driver_error = VFipdm_part_get_driver_error_code(p_vol->p_part);
                lbl_8053B248.last_driver_error = driver_error;
                p_vol->last_driver_error = driver_error;
                return 0x1000;
            }
            return -1;
        }
    }
    err = VFipdm_part_format(p_vol->p_part, param);
    if (err != 0) {
        if (err == 0x15) {
            driver_error = VFipdm_part_get_driver_error_code(p_vol->p_part);
            lbl_8053B248.last_driver_error = driver_error;
            p_vol->last_driver_error = driver_error;
            return 0x1000;
        }
        return -1;
    }
    if (!(p_vol->flags & 2)) {
        err = VFipdm_part_release_permission(p_vol->p_part, 1);
        if (err != 0) {
            if (err == 0x15) {
                driver_error = VFipdm_part_get_driver_error_code(p_vol->p_part);
                lbl_8053B248.last_driver_error = driver_error;
                p_vol->last_driver_error = driver_error;
                return 0x1000;
            }
            return -1;
        }
    }
    return 0;
}

s32 VFiPFDRV_lread(PF_VOLUME* p_vol, u8* buf, u32 sector, u32 num_sectors, u32* p_num_success) {
    s32 err;
    s32 driver_error;

    if (p_vol == NULL) {
        return 10;
    }

    err = VFipdm_part_logical_read(p_vol->p_part, buf, sector, num_sectors, p_vol->bpb.bytes_per_sector, p_num_success);
    if (err != 0) {
        if (err == 0x15) {
            driver_error = VFipdm_part_get_driver_error_code(p_vol->p_part);
            lbl_8053B248.last_driver_error = driver_error;
            p_vol->last_driver_error = driver_error;
            return 0x1000;
        }
        return -1;
    }
    return 0;
}

s32 VFiPFDRV_lwrite(PF_VOLUME* p_vol, const u8* buf, u32 sector, u32 num_sectors, u32* p_num_success) {
    s32 err;
    s32 driver_error;

    if (p_vol == NULL) {
        return 10;
    }

    err = VFipdm_part_logical_write(p_vol->p_part, buf, sector, num_sectors, p_vol->bpb.bytes_per_sector, p_num_success);
    if (err != 0) {
        if (err == 0x15) {
            driver_error = VFipdm_part_get_driver_error_code(p_vol->p_part);
            lbl_8053B248.last_driver_error = driver_error;
            p_vol->last_driver_error = driver_error;
            return 0x1000;
        }
        return -1;
    }
    return 0;
}

// ---------------------------------------------------------------- pdm_bpb / pdm_disk

s32 VFipdm_bpb_check_boot_sector(u8* buf, u32* is_boot) {
    u16 bytes_per_sector;
    u8 sectors_per_cluster;
    u8 media;

    if (buf == NULL || is_boot == NULL) {
        return 1;
    }
    *is_boot = 1;
    if (!((buf[0] == 0xEB && buf[2] == 0x90) || buf[0] == 0xE9)) {
        *is_boot = 0;
    }
    if (!(buf[0x1FE] == 0x55 && buf[0x1FF] == 0xAA)) {
        *is_boot = 0;
    }
    bytes_per_sector = buf[0xB] | (buf[0xC] << 8);
    if (!(bytes_per_sector == 0x200 || bytes_per_sector == 0x400 || bytes_per_sector == 0x800 || bytes_per_sector == 0x1000)) {
        *is_boot = 0;
    }
    sectors_per_cluster = buf[0xD];
    if (!(sectors_per_cluster == 1 || sectors_per_cluster == 2 || sectors_per_cluster == 4 || sectors_per_cluster == 8 || sectors_per_cluster == 0x10 || sectors_per_cluster == 0x20 || sectors_per_cluster == 0x40 || sectors_per_cluster == 0x80)) {
        *is_boot = 0;
    }
    media = buf[0x15];
    if (!(media == 0xF0 || media == 0xF8 || media == 0xF9 || media == 0xFA || media == 0xFB || media == 0xFC || media == 0xFD || media == 0xFE || media == 0xFF)) {
        *is_boot = 0;
    }
    return 0;
}

s32 VFipdm_bpb_check_fsinfo_sector(u8* buf, u32* is_fsinfo) {
    u32 lead_sig;
    u32 struct_sig;
    u32 trail_sig;

    if (buf == NULL || is_fsinfo == NULL) {
        return 1;
    }
    lead_sig = buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24);
    struct_sig = buf[0x1E4] | (buf[0x1E5] << 8) | (buf[0x1E6] << 16) | (buf[0x1E7] << 24);
    trail_sig = buf[0x1FC] | (buf[0x1FD] << 8) | (buf[0x1FE] << 16) | (buf[0x1FF] << 24);
    if (lead_sig == 0x41615252 && struct_sig == 0x61417272 && trail_sig == 0xAA550000) {
        *is_fsinfo = 1;
    } else {
        *is_fsinfo = 0;
    }
    return 0;
}

s32 VFipdm_bpb_get_fsinfo_information(u8* buf, PDM_FSINFO* p_fsinfo) {
    if (buf == NULL || p_fsinfo == NULL) {
        return 1;
    }
    p_fsinfo->free_count = buf[0x1E8] | (buf[0x1E9] << 8) | (buf[0x1EA] << 16) | (buf[0x1EB] << 24);
    p_fsinfo->next_free = buf[0x1EC] | (buf[0x1ED] << 8) | (buf[0x1EE] << 16) | (buf[0x1EF] << 24);
    return 0;
}

s32 VFipdm_bpb_set_fsinfo_information(PDM_FSINFO* p_fsinfo, u8* buf) {
    if (buf == NULL || p_fsinfo == NULL) {
        return 1;
    }
    buf[0] = 0x52;
    buf[1] = 0x52;
    buf[2] = 0x61;
    buf[3] = 0x41;
    buf[0x1E4] = 0x72;
    buf[0x1E5] = 0x72;
    buf[0x1E6] = 0x41;
    buf[0x1E7] = 0x61;
    buf[0x1E8] = p_fsinfo->free_count;
    buf[0x1E9] = p_fsinfo->free_count >> 8;
    buf[0x1EA] = p_fsinfo->free_count >> 16;
    buf[0x1EB] = p_fsinfo->free_count >> 24;
    buf[0x1EC] = p_fsinfo->next_free;
    buf[0x1ED] = p_fsinfo->next_free >> 8;
    buf[0x1EE] = p_fsinfo->next_free >> 16;
    buf[0x1EF] = p_fsinfo->next_free >> 24;
    buf[0x1FC] = 0;
    buf[0x1FD] = 0;
    buf[0x1FE] = 0x55;
    buf[0x1FF] = 0xAA;
    return 0;
}

s32 VFipdm_disk_convert_sector_into_block(PDM_DISK* p_disk, u32 sector, u32 num_sector, u16 bps, u32* p_block, u32* p_num_block) {
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

s32 VFipdm_disk_convert_block_into_sector(PDM_DISK* p_disk, u32 block, u32 num_block, u16 bps, u32* p_sector, u32* p_num_sector) {
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

// Disk and partition handles encode the table index (bits 0-7), the handle kind
// (bits 8-15: 3 = disk, 4 = partition) and the slot signature (bits 16-31).
static inline s32 pdm_disk_check_handle(u32 handle) {
    u32 index = handle & 0xFF;
    u32 kind = handle & 0xFF00;
    u32 signature = handle >> 16;
    PDM_DISK* p_disk;
    u16 i;

    if (index >= 26 || kind != 0x300 || signature > lbl_8053A6E0.disk[index].signature) {
        return 1;
    }
    p_disk = &lbl_8053A6E0.disk[index];
    for (i = 0; i < 26; i++) {
        if (lbl_8053A6E0.disk_handle[i].handle == p_disk && (handle >> 16) == lbl_8053A6E0.disk_handle[i].signature) {
            break;
        }
    }
    return i >= 26;
}

s32 VFipdm_disk_check_disk_handle(PDM_DISK* p_disk) {
    return pdm_disk_check_handle((u32)p_disk);
}

s32 VFipdm_disk_release_part_permission(PDM_DISK* p_disk, u32 mode) {
    u32 index;
    s32 err;
    PDM_DISK* p_entry;

    if (p_disk == NULL) {
        return 1;
    }
    index = (u32)p_disk & 0xFF;
    err = pdm_disk_check_handle((u32)p_disk);
    if (err) {
        return err;
    }
    p_entry = &lbl_8053A6E0.disk[index];
    if (p_entry->disk_lock_cnt == 0) {
        return 0xE;
    }
    err = 0;
    if (p_entry->disk_lock_cnt == 1) {
        err = p_entry->disk_tbl.p_func->unmount(p_disk);
        if (err != 0) {
            if (p_entry->p_cur_part != NULL) {
                VFipdm_part_set_driver_error_code(p_entry->p_cur_part, err);
            }
            err = 0x15;
        }
    }
    if (err == 0 || mode == 1) {
        p_entry->disk_lock_cnt--;
        if (p_entry->disk_lock_cnt == 0) {
            p_entry->status &= ~2;
        }
    }
    return err;
}

s32 VFipdm_disk_physical_read(PDM_DISK* p_disk, u8* buf, u32 psector, u32 num_sector, u16 bps, u32* p_num_success) {
    u32 index;
    s32 err;
    s32 drv_err;
    PDM_DISK* p_entry;
    u32 block;
    u32 num_block;
    u32 sector;

    if (p_disk == NULL || buf == NULL || num_sector == 0 || bps == 0 || p_num_success == NULL) {
        return 1;
    }
    index = (u32)p_disk & 0xFF;
    err = pdm_disk_check_handle((u32)p_disk);
    if (err) {
        return err;
    }
    p_entry = &lbl_8053A6E0.disk[index];
    err = VFipdm_disk_convert_sector_into_block(p_entry, psector, num_sector, bps, &block, &num_block);
    if (err != 0) {
        return err;
    }
    drv_err = p_entry->disk_tbl.p_func->physical_read(p_disk, buf, block, num_block, p_num_success);
    VFipdm_disk_convert_block_into_sector(p_entry, block, *p_num_success, bps, &sector, p_num_success);
    if (drv_err != 0) {
        if (p_entry->p_cur_part != NULL) {
            VFipdm_part_set_driver_error_code(p_entry->p_cur_part, drv_err);
        }
        return 0x15;
    }
    return 0;
}

s32 VFipdm_disk_physical_write(PDM_DISK* p_disk, const u8* buf, u32 psector, u32 num_sector, u16 bps, u32* p_num_success) {
    u32 index;
    s32 err;
    s32 drv_err;
    PDM_DISK* p_entry;
    u32 block;
    u32 num_block;
    u32 sector;

    if (p_disk == NULL || buf == NULL || num_sector == 0 || bps == 0 || p_num_success == NULL) {
        return 1;
    }
    index = (u32)p_disk & 0xFF;
    err = pdm_disk_check_handle((u32)p_disk);
    if (err) {
        return err;
    }
    p_entry = &lbl_8053A6E0.disk[index];
    err = VFipdm_disk_convert_sector_into_block(p_entry, psector, num_sector, bps, &block, &num_block);
    if (err != 0) {
        return err;
    }
    drv_err = p_entry->disk_tbl.p_func->physical_write(p_disk, (u8*)buf, block, num_block, p_num_success);
    VFipdm_disk_convert_block_into_sector(p_entry, block, *p_num_success, bps, &sector, p_num_success);
    if (drv_err != 0) {
        if (p_entry->p_cur_part != NULL) {
            VFipdm_part_set_driver_error_code(p_entry->p_cur_part, drv_err);
        }
        return 0x15;
    }
    return 0;
}

s32 VFipdm_disk_format(PDM_DISK* p_disk, const u8* param) {
    u32 index;
    s32 err;
    s32 drv_err;
    PDM_DISK* p_entry;

    if (p_disk == NULL) {
        return 1;
    }
    index = (u32)p_disk & 0xFF;
    err = pdm_disk_check_handle((u32)p_disk);
    if (err) {
        return err;
    }
    p_entry = &lbl_8053A6E0.disk[index];
    drv_err = p_entry->disk_tbl.p_func->format(p_disk, (u8*)param);
    if (drv_err != 0) {
        if (p_entry->p_cur_part != NULL) {
            VFipdm_part_set_driver_error_code(p_entry->p_cur_part, drv_err);
        }
        return 0x15;
    }
    return 0;
}

s32 VFipdm_disk_get_lba_size(PDM_DISK* p_disk, u16* p_lba_size) {
    u32 index;
    s32 err;

    if (p_disk == NULL || p_lba_size == NULL) {
        return 1;
    }
    index = (u32)p_disk & 0xFF;
    err = pdm_disk_check_handle((u32)p_disk);
    if (err) {
        return err;
    }
    *p_lba_size = lbl_8053A6E0.disk[index].disk_info.bytes_per_sector;
    return 0;
}

s32 VFipdm_disk_get_media_information(PDM_DISK* p_disk, PDM_DISK_INFO* p_disk_info) {
    u32 index;
    s32 err;

    if (p_disk == NULL || p_disk_info == NULL) {
        return 1;
    }
    index = (u32)p_disk & 0xFF;
    err = pdm_disk_check_handle((u32)p_disk);
    if (err) {
        return err;
    }
    *p_disk_info = lbl_8053A6E0.disk[index].disk_info;
    return 0;
}

s32 VFipdm_disk_check_media_insert(PDM_DISK* p_disk, u32* is_insert) {
    u32 index;
    s32 err;

    if (p_disk == NULL || is_insert == NULL) {
        return 1;
    }
    index = (u32)p_disk & 0xFF;
    err = pdm_disk_check_handle((u32)p_disk);
    if (err) {
        return err;
    }
    if (lbl_8053A6E0.disk[index].status & 4) {
        *is_insert = 1;
    } else {
        *is_insert = 0;
    }
    return 0;
}

s32 VFipdm_disk_set_disk(PDM_DISK* p_disk, PDM_PARTITION* p_part) {
    u32 index;
    s32 err;

    if (p_disk == NULL || p_part == NULL) {
        return 1;
    }
    index = (u32)p_disk & 0xFF;
    err = pdm_disk_check_handle((u32)p_disk);
    if (err) {
        return err;
    }
    lbl_8053A6E0.disk[index].p_cur_part = p_part;
    return 0;
}

void VFipdm_disk_notify_media_insert(PDM_DISK* p_disk) {
    u32 index = (u32)p_disk & 0xFF;
    s32 err;

    err = pdm_disk_check_handle((u32)p_disk);
    if (err) {
        return;
    }
    lbl_8053A6E0.disk[index].status |= 0x2C;
    VFipdm_part_set_change_media_state(p_disk, 1);
}

s32 fn_803AB6EC(PDM_DISK* p_disk, PDM_DISK* lp_disk) { // VFipdm_disk_do_get_permission
    s32 err;

    if (lp_disk->disk_lock_cnt == 0 || (lp_disk->status & 0x20)) {
        err = lp_disk->disk_tbl.p_func->mount(p_disk);
        if (err != 0) {
            if (lp_disk->p_cur_part != NULL) {
                VFipdm_part_set_driver_error_code(lp_disk->p_cur_part, err);
            }
            return 0x15;
        }
        err = lp_disk->disk_tbl.p_func->get_disk_info(p_disk, &lp_disk->disk_info);
        if (err != 0) {
            if (lp_disk->p_cur_part != NULL) {
                VFipdm_part_set_driver_error_code(lp_disk->p_cur_part, err);
            }
            return 0x15;
        }
        if (lp_disk->disk_info.bytes_per_sector & 0x1FF) {
            return 0x16;
        }
        if (lp_disk->disk_info.media_attr & 1) {
            lp_disk->status |= 0x10;
        } else {
            lp_disk->status &= ~0x10;
        }
        lp_disk->status |= 2;
        lp_disk->status &= ~0x20;
    }
    lp_disk->disk_lock_cnt++;
    return 0;
}

s32 VFipdm_disk_get_part_permission(PDM_DISK* p_disk) {
    u32 index;
    s32 err;

    if (p_disk == NULL) {
        return 1;
    }
    index = (u32)p_disk & 0xFF;
    err = pdm_disk_check_handle((u32)p_disk);
    if (err) {
        return err;
    }
    return fn_803AB6EC(p_disk, &lbl_8053A6E0.disk[index]);
}

// ---------------------------------------------------------------- pdm_partition

static inline s32 pdm_part_check_handle(u32 handle) {
    u32 index = handle & 0xFF;
    u32 kind = handle & 0xFF00;
    u32 signature = handle >> 16;
    PDM_PARTITION* p_part;
    u16 i;

    if (index >= 26 || kind != 0x400 || signature > lbl_8053A6E0.partition[index].signature) {
        return 1;
    }
    p_part = &lbl_8053A6E0.partition[index];
    for (i = 0; i < 26; i++) {
        if (lbl_8053A6E0.partition_handle[i].handle == p_part && (handle >> 16) == lbl_8053A6E0.partition_handle[i].signature) {
            break;
        }
    }
    return i >= 26;
}

static inline s32 pdm_part_find_handle(u32 handle, PDM_PARTITION* p_part, u16* p_handle_no) {
    u16 i;

    for (i = 0; i < 26; i++) {
        if (lbl_8053A6E0.partition_handle[i].handle == p_part && (handle >> 16) == lbl_8053A6E0.partition_handle[i].signature) {
            break;
        }
    }
    if (i >= 26) {
        return 1;
    }
    *p_handle_no = i;
    return 0;
}

s32 VFipdm_part_close_partition(PDM_PARTITION* p_part) {
    u32 index;
    s32 err;
    PDM_PARTITION* lp_part;
    u32 status;
    u16 i;

    if (p_part == NULL) {
        return 1;
    }
    index = (u32)p_part & 0xFF;
    err = pdm_part_check_handle((u32)p_part);
    if (err) {
        return err;
    }

    lp_part = &lbl_8053A6E0.partition[index];
    status = lp_part->status;
    if (!(status & 1)) {
        return 12;
    }
    if ((status & 2) == 2) {
        return 13;
    }

    err = pdm_part_find_handle((u32)p_part, lp_part, &i);
    if (err) {
        return err;
    }

    if (lp_part->open_part_cnt == 1) {
        lp_part->status &= ~1;
        lbl_8053A6E0.num_partition--;
    }
    lp_part->open_part_cnt--;
    lbl_8053A6E0.partition_handle[i].handle = NULL;
    return 0;
}

u32 VFipdm_part_chg_ltop(PDM_PARTITION* p_part, u32 sector, u16 bps) {
    u16 lba_size;
    u32 phys;
    u32 lba_ratio;
    u32 bps_ratio;

    VFipdm_disk_get_lba_size(p_part->p_disk, &lba_size);
    if (bps == lba_size) {
        phys = p_part->mbr_sector + sector;
        return phys + p_part->start_sector;
    }
    lba_ratio = (lba_size >> 9) & 0x7F;
    phys = p_part->start_sector + p_part->mbr_sector;
    if (lba_ratio == 2) phys <<= 1;
    else if (lba_ratio == 4) phys <<= 2;
    else if (lba_ratio == 8) phys <<= 3;
    bps_ratio = (bps >> 9) & 0x7F;
    if (bps_ratio == 2) phys >>= 1;
    else if (bps_ratio == 4) phys >>= 2;
    else if (bps_ratio == 8) phys >>= 3;
    return sector + phys;
}

s32 VFipdm_part_format(PDM_PARTITION* p_part, const u8* param) {
    u32 index;
    s32 err;
    PDM_PARTITION* p_entry;

    if (p_part == NULL) {
        return 1;
    }
    index = (u32)p_part & 0xFF;
    err = pdm_part_check_handle((u32)p_part);
    if (err) {
        return err;
    }
    p_entry = &lbl_8053A6E0.partition[index];
    err = VFipdm_disk_set_disk(p_entry->p_disk, p_part);
    if (err != 0) {
        return err;
    }
    err = VFipdm_disk_format(p_entry->p_disk, param);
    if (err != 0) {
        return err;
    }
    return 0;
}

s32 VFipdm_part_logical_read(PDM_PARTITION* p_part, u8* buf, u32 lsector, u32 num_sector, u16 bps, u32* p_num_success) {
    u32 index;
    s32 err;
    PDM_PARTITION* p_entry;

    if (p_part == NULL || buf == NULL || num_sector == 0 || bps == 0 || p_num_success == NULL) {
        return 1;
    }
    index = (u32)p_part & 0xFF;
    err = pdm_part_check_handle((u32)p_part);
    if (err) {
        return err;
    }
    p_entry = &lbl_8053A6E0.partition[index];
    err = VFipdm_disk_set_disk(p_entry->p_disk, p_part);
    if (err != 0) {
        return err;
    }
    if (p_entry->total_sector <= lsector) {
        return 0x12;
    }
    if (p_entry->total_sector < lsector + num_sector) {
        num_sector -= (lsector + num_sector) - p_entry->total_sector;
    }
    err = VFipdm_disk_physical_read(p_entry->p_disk, buf, VFipdm_part_chg_ltop(p_entry, lsector, bps), num_sector, bps, p_num_success);
    if (err != 0) {
        return err;
    }
    return 0;
}

s32 VFipdm_part_logical_write(PDM_PARTITION* p_part, const u8* buf, u32 lsector, u32 num_sector, u16 bps, u32* p_num_success) {
    u32 index;
    s32 err;
    PDM_PARTITION* p_entry;

    if (p_part == NULL || buf == NULL || num_sector == 0 || bps == 0 || p_num_success == NULL) {
        return 1;
    }
    index = (u32)p_part & 0xFF;
    err = pdm_part_check_handle((u32)p_part);
    if (err) {
        return err;
    }
    p_entry = &lbl_8053A6E0.partition[index];
    err = VFipdm_disk_set_disk(p_entry->p_disk, p_part);
    if (err != 0) {
        return err;
    }
    if (p_entry->total_sector <= lsector) {
        return 0x12;
    }
    if (p_entry->total_sector < lsector + num_sector) {
        num_sector -= (lsector + num_sector) - p_entry->total_sector;
    }
    err = VFipdm_disk_physical_write(p_entry->p_disk, buf, VFipdm_part_chg_ltop(p_entry, lsector, bps), num_sector, bps, p_num_success);
    if (err != 0) {
        return err;
    }
    return 0;
}

s32 VFipdm_part_check_media_write_protect(PDM_PARTITION* p_part, u32* p_flag) {
    u32 index;
    s32 err;

    if (p_part == NULL || p_flag == NULL) {
        return 1;
    }
    index = (u32)p_part & 0xFF;
    err = pdm_part_check_handle((u32)p_part);
    if (err) {
        return err;
    }
    if (lbl_8053A6E0.partition[index].status & 0x10) {
        *p_flag = 1;
    } else {
        *p_flag = 0;
    }
    return 0;
}

s32 VFipdm_part_check_media_insert(PDM_PARTITION* p_part, u32* p_flag) {
    u32 index;
    s32 err;

    if (p_part == NULL || p_flag == NULL) {
        return 1;
    }
    index = (u32)p_part & 0xFF;
    err = pdm_part_check_handle((u32)p_part);
    if (err) {
        return err;
    }
    if (lbl_8053A6E0.partition[index].status & 4) {
        *p_flag = 1;
    } else {
        *p_flag = 0;
    }
    return 0;
}

s32 VFipdm_part_check_media_detect(PDM_PARTITION* p_part, u32* p_flag) {
    u32 index;
    s32 err;

    if (p_part == NULL || p_flag == NULL) {
        return 1;
    }
    index = (u32)p_part & 0xFF;
    err = pdm_part_check_handle((u32)p_part);
    if (err) {
        return err;
    }
    if (lbl_8053A6E0.partition[index].status & 8) {
        *p_flag = 1;
    } else {
        *p_flag = 0;
    }
    return 0;
}

s32 VFipdm_part_get_media_information(PDM_PARTITION* p_part, PDM_DISK_INFO* p_disk_info) {
    u32 index;
    s32 err;

    if (p_part == NULL || p_disk_info == NULL) {
        return 1;
    }
    index = (u32)p_part & 0xFF;
    err = pdm_part_check_handle((u32)p_part);
    if (err) {
        return err;
    }
    err = VFipdm_disk_get_media_information(lbl_8053A6E0.partition[index].p_disk, p_disk_info);
    if (err != 0) {
        return err;
    }
    return 0;
}

void VFipdm_part_set_driver_error_code(PDM_PARTITION* p_part, s32 error_code) {
    lbl_8053A6E0.partition[(u32)p_part & 0xFF].driver_last_error = error_code;
}

s32 VFipdm_part_get_driver_error_code(PDM_PARTITION* p_part) {
    u8 index = (u32)p_part;

    return lbl_8053A6E0.partition[index].driver_last_error;
}

// ---------------------------------------------------------------- pdm_mbr

s32 VFipdm_mbr_check_master_boot_record(PDM_DISK* p_disk, u8* buf, u32* is_master_boot) {
    u32 is_boot;
    PDM_MBR mbr;
    PDM_DISK* lp_disk;
    PDM_PART_TBL* p_tbl;
    s32 err;
    s32 i;

    if (p_disk == NULL || buf == NULL || is_master_boot == NULL) {
        return 1;
    }
    err = VFipdm_disk_check_disk_handle(p_disk);
    if (err != 0) {
        return err;
    }
    *is_master_boot = 0;
    if (buf[0x1FE] != 0x55 || buf[0x1FF] != 0xAA) {
        return 0;
    }
    VFipdm_mbr_get_table(buf, 0, &mbr);

    lp_disk = &lbl_8053A6E0.disk[(u8)(u32)p_disk];
    p_tbl = mbr.partition_table;
    for (i = 0; i < 4; i++) {
        u32 num;
        u32 start;

        start = p_tbl[i].lba_start_sector;
        num = p_tbl[i].lba_num_sectors;
        if (num > (u32)(-1 - start) || start + num > lp_disk->disk_info.total_sectors) {
            *is_master_boot = 0;
            break;
        }
        if (start != 0) {
            *is_master_boot = 1;
        }
    }

    if (*is_master_boot == 0 && mbr.partition_table[0].lba_start_sector == 0 && mbr.partition_table[1].lba_start_sector == 0 &&
        mbr.partition_table[2].lba_start_sector == 0 && mbr.partition_table[3].lba_start_sector == 0) {
        VFipdm_bpb_check_boot_sector(buf, &is_boot);
        if (is_boot == 0) {
            *is_master_boot = 1;
        } else {
            *is_master_boot = 0;
        }
    }
    return 0;
}

s32 VFipdm_mbr_get_mbr_part_table(PDM_DISK* p_disk, PDM_MBR* p_mbr) {
    u8 buf[0x200];
    u32 num_success;
    u32 is_master;
    s32 err;

    if (p_disk == NULL || p_mbr == NULL) {
        return 1;
    }
    err = VFipdm_disk_check_disk_handle(p_disk);
    if (err != 0) {
        return err;
    }
    err = VFipdm_disk_physical_read(p_disk, buf, 0, 1, 0x200, &num_success);
    if (err != 0) {
        return err;
    }
    VFipdm_mbr_check_master_boot_record(p_disk, buf, &is_master);
    if (is_master != 0) {
        VFipdm_mbr_get_table(buf, 0, p_mbr);
    } else {
        return 5;
    }
    return 0;
}

s32 VFipdm_mbr_get_epbr_part_table(PDM_DISK* p_disk, PDM_MBR* p_mbr) {
    u8 buf[0x200];
    PDM_DISK_INFO disk_info;
    u32 num_success;
    u32 is_master;
    u32 sector;
    u16 i;
    s32 err;

    if (p_disk == NULL || p_mbr == NULL) {
        return 1;
    }
    err = VFipdm_disk_check_disk_handle(p_disk);
    if (err != 0) {
        return err;
    }
    sector = 0;
    for (i = 0; i < 4; i++) {
        if (p_mbr->partition_table[i].partition_type == 5 || p_mbr->partition_table[i].partition_type == 0xF) {
            sector = p_mbr->epbr_base_sector + p_mbr->partition_table[i].lba_start_sector;
            break;
        }
    }
    if (i == 4) {
        return 7;
    }
    err = VFipdm_disk_get_media_information(p_disk, &disk_info);
    if (err != 0) {
        return err;
    }
    err = VFipdm_disk_physical_read(p_disk, buf, sector * (disk_info.bytes_per_sector >> 9), 1, 0x200, &num_success);
    if (err != 0) {
        return err;
    }
    VFipdm_mbr_check_master_boot_record(p_disk, buf, &is_master);
    if (is_master != 0) {
        VFipdm_mbr_get_table(buf, sector, p_mbr);
    } else {
        return 6;
    }
    return 0;
}

// ---------------------------------------------------------------- pdm_dskmng

s32 VFipdm_open_disk(PDM_INIT_DISK* p_init_disk_tbl, PDM_DISK** pp_disk) {
    s32 err;

    if (pp_disk == NULL) {
        return 1;
    }
    *pp_disk = NULL;
    err = fn_803AB8C4(p_init_disk_tbl, pp_disk);
    if (err != 0) {
        return err;
    }
    return 0;
}

s32 fn_803ACCC8(PDM_DISK* p_disk, u16 part_id, PDM_PARTITION** pp_part); // VFipdm_part_open_partition

s32 fn_803ACCC8(PDM_DISK* p_disk, u16 part_id, PDM_PARTITION** pp_part) { // VFipdm_part_open_partition
    PDM_PARTITION* p_part;
    u16 part_no;
    u16 handle_no;
    u32 is_insert;
    s32 err;

    if (p_disk == NULL || pp_part == NULL) {
        return 1;
    }
    err = VFipdm_part_get_handle(p_disk, part_id, &p_part, &part_no, &handle_no);
    if (err != 0) {
        return err;
    }
    p_part->signature++;
    p_part->signature &= 0xFFFF;
    p_part->open_part_cnt++;
    lbl_8053A6E0.partition_handle[handle_no].signature = p_part->signature;
    lbl_8053A6E0.partition_handle[handle_no].handle = p_part;
    err = VFipdm_disk_check_media_insert(p_disk, &is_insert);
    if (err != 0) {
        return err;
    }
    if (is_insert != 0) {
        p_part->status |= 4;
    }
    *pp_part = (PDM_PARTITION*)((part_no & 0xFF) | 0x400 | (lbl_8053A6E0.partition[part_no].signature << 16));
    return 0;
}

s32 VFipdm_open_partition(PDM_DISK* p_disk, s32 part_id, PDM_PARTITION** pp_part) {
    s32 err;

    if (p_disk == NULL) {
        return 1;
    }
    if (pp_part == NULL) {
        return 1;
    }
    *pp_part = NULL;
    err = fn_803ACCC8(p_disk, part_id, pp_part);
    if (err != 0) {
        return err;
    }
    return 0;
}

s32 VFipdm_close_disk(PDM_DISK* p_disk) {
    if (p_disk == NULL) {
        return 1;
    }
    return fn_803ABA30(p_disk);
}

s32 VFipdm_close_partition(PDM_PARTITION* p_part) {
    s32 err;

    if (p_part == NULL) {
        return 1;
    }
    err = VFipdm_part_close_partition(p_part);
    if (err != 0) {
        return err;
    }
    return 0;
}

// ---------------------------------------------------------------- pf_cache

u32 VFiPFCACHE_SearchForFreePage(PF_CACHE_PAGE* p_head, PF_CACHE_PAGE** pp_page) {
    PF_CACHE_PAGE* p_page = p_head->p_prev;

    while (p_page != p_head) {
        if (!(p_page->stat & 1)) {
            *pp_page = p_page;
            return 1;
        }
        if (p_page->sector != 0xFFFFFFFF) {
            *pp_page = p_page;
            return 0;
        }
        p_page = p_page->p_prev;
    }
    if (!(p_page->stat & 1)) {
        *pp_page = p_page;
        return 1;
    }
    if (p_page->sector != 0xFFFFFFFF) {
        *pp_page = p_page;
        return 0;
    }
    *pp_page = NULL;
    return 0;
}

s32 VFiPFCACHE_FlushPageIfNeeded(PF_VOLUME* p_vol, PF_CACHE_PAGE* p_page) {
    u32 num_success;
    u8* p_mod_sbuf;
    u32 num_sector;
    u32 sector;
    s32 err;

    if (p_page != NULL && (p_page->stat & 2)) {
        if (p_page->sector != 0xFFFFFFFF) {
            p_mod_sbuf = p_page->p_mod_sbuf;
            sector = p_page->sector + ((u32)(p_mod_sbuf - p_page->buffer) >> p_vol->bpb.log2_bytes_per_sector);
            num_sector = ((u32)p_page->p_mod_ebuf >> p_vol->bpb.log2_bytes_per_sector) - ((u32)p_mod_sbuf >> p_vol->bpb.log2_bytes_per_sector) + 1;
            err = VFiPFDRV_lwrite(p_vol, p_mod_sbuf, sector, num_sector, &num_success);
            if (err != 0) return err;
            if (num_success != num_sector) return 0x11;
            if (p_page->stat & 4) {
                err = VFiPFFAT_UpdateAlternateFATEntry(p_vol, p_mod_sbuf, sector, num_sector);
                if (err != 0) return err;
            }
            p_page->p_mod_sbuf = NULL;
            p_page->p_mod_ebuf = NULL;
            p_page->stat &= ~2;
        }
    }
    return 0;
}

static inline void pfcache_free_page(PF_CACHE_PAGE** pp_head, PF_CACHE_PAGE* p_page) {
    PF_CACHE_PAGE* p_head;

    p_page->stat &= 0xFFFC;
    p_page->p_mod_sbuf = NULL;
    p_page->p_mod_ebuf = NULL;
    p_page->sector = 0xFFFFFFFF;
    p_page->signature = NULL;
    p_head = *pp_head;
    if (p_page == p_head) {
        *pp_head = p_page->p_next;
    } else if (p_page != p_head->p_prev) {
        p_page->p_prev->p_next = p_page->p_next;
        p_page->p_next->p_prev = p_page->p_prev;
        p_head = *pp_head;
        p_page->p_next = p_head;
        p_page->p_prev = (*pp_head)->p_prev;
        p_head->p_prev = p_page;
        p_page->p_prev->p_next = p_page;
    }
}

s32 VFiPFCACHE_DoReadPage(PF_VOLUME* p_vol, PF_CACHE_PAGE** pp_head, u32 sector, PF_CACHE_PAGE** pp_page, u32 set_sig) {
    u32 num_success;
    u32 is_hit;
    s32 err;
    PF_CACHE_PAGE* p_page;

    err = VFiPFCACHE_DoAllocatePage(p_vol, pp_head, sector, pp_page, &is_hit);
    if (err != 0) {
        return err;
    }
    if (is_hit == 0) {
        p_page = *pp_page;
        err = VFiPFDRV_lread(p_vol, p_page->buffer, p_page->sector, p_page->size, &num_success);
        if (err != 0) {
            pfcache_free_page(pp_head, *pp_page);
            return err;
        }
        p_page = *pp_page;
        if (num_success != p_page->size && p_vol->bpb.total_sectors != p_page->sector + num_success) {
            pfcache_free_page(pp_head, p_page);
            return 0x11;
        }
        p_page->stat = p_page->stat & ~2;
        (*pp_page)->p_mod_sbuf = NULL;
        (*pp_page)->p_mod_ebuf = NULL;
        if (set_sig != 0) {
            (*pp_page)->signature = p_vol->cache.signature;
        } else {
            (*pp_page)->signature = NULL;
        }
    }
    return 0;
}

s32 VFiPFCACHE_DoFlushCache(PF_VOLUME* p_vol, PF_CACHE_PAGE* p_head) {
    PF_CACHE_PAGE* p_page;
    s32 err;
    u32 is_modified;
    u32 is_flushed;
    int is_ok;

    is_flushed = 0;
    p_page = p_head;
    err = 0;

    if (!(p_head->stat & 1)) {
        return 0;
    }

    do {
        s32 ret;

        if (!(p_page->stat & 1)) {
            break;
        }

        is_modified = 0;
        if (p_page != NULL && (p_page->stat & 2) && p_page->sector != 0xFFFFFFFF) {
            is_modified = 1;
        }

        ret = VFiPFCACHE_FlushPageIfNeeded(p_vol, p_page);
        if (is_modified != 0 && ret == 0) {
            is_flushed = 1;
        }

        if (ret != 0 && err == 0) {
            err = ret;
        }

        p_page = p_page->p_next;
    } while (p_page != p_head);

    if (is_flushed != 0) {
        is_ok = err == 0;
        if (dCommon_FlushFromVol(p_vol, is_ok) != 0 && is_ok == 1) {
            return 0x11;
        }
    }

    return err;
}

void VFiPFCACHE_SetCache(PF_VOLUME* p_vol, PF_CACHE_PAGE* p_cache_page, u8 (*p_cache_buf)[512], u16 num_fat_pages, u16 num_data_pages) {
    p_vol->cache.pages = p_cache_page;
    p_vol->cache.buffers = p_cache_buf;
    p_vol->cache.num_fat_pages = num_fat_pages;
    p_vol->cache.num_data_pages = num_data_pages;
}

void VFiPFCACHE_SetFATBufferSize(PF_VOLUME* p_vol, u32 size) {
    if (size != 0) {
        p_vol->cache.fat_buff_size = size;
    }
}

void VFiPFCACHE_SetDataBufferSize(PF_VOLUME* p_vol, u32 size) {
    if (size != 0) {
        p_vol->cache.data_buff_size = size;
    }
}

s32 VFiPFCACHE_UpdateModifiedSector(PF_CACHE_PAGE* p_page) {
    if (p_page->p_mod_sbuf == NULL) {
        p_page->stat |= 2;
        p_page->p_mod_ebuf = p_page->p_mod_sbuf = p_page->p_buf;
    } else if (p_page->p_buf < p_page->p_mod_sbuf) {
        p_page->p_mod_sbuf = p_page->p_buf;
    } else if (p_page->p_buf > p_page->p_mod_ebuf) {
        p_page->p_mod_ebuf = p_page->p_buf;
    }

    return 0;
}

void VFiPFCACHE_FreeFATPage(PF_VOLUME* p_vol, PF_CACHE_PAGE* p_page) {
    PF_CACHE_PAGE* p_head = p_vol->cache.p_current_fat;

    if (p_head == NULL) {
        return;
    }
    p_page->p_mod_sbuf = NULL;
    p_page->stat = p_page->stat & 0xFFFC;
    p_page->p_mod_ebuf = NULL;
    p_page->sector = 0xFFFFFFFF;
    p_page->signature = NULL;
    p_head = p_vol->cache.p_current_fat;
    if (p_page == p_head) {
        p_vol->cache.p_current_fat = p_page->p_next;
        return;
    }
    if (p_page == p_head->p_prev) {
        return;
    }
    p_page->p_prev->p_next = p_page->p_next;
    p_page->p_next->p_prev = p_page->p_prev;
    p_head = p_vol->cache.p_current_fat;
    p_page->p_next = p_head;
    p_page->p_prev = p_vol->cache.p_current_fat->p_prev;
    p_head->p_prev = p_page;
    p_page->p_prev->p_next = p_page;
}

void VFiPFCACHE_FreeDataPage(PF_VOLUME* p_vol, PF_CACHE_PAGE* p_page) {
    PF_CACHE_PAGE* p_head;
    u16 stat;

    if (p_vol->cache.p_current_data == NULL) {
        return;
    }
    stat = p_page->stat;
    p_page->p_mod_sbuf = NULL;
    p_page->stat = stat & 0xFFFC;
    p_page->p_mod_ebuf = NULL;
    p_page->sector = 0xFFFFFFFF;
    p_page->signature = NULL;
    p_head = p_vol->cache.p_current_data;
    if (p_page == p_head) {
        p_vol->cache.p_current_data = p_page->p_next;
        return;
    }
    if (p_page == p_head->p_prev) {
        return;
    }
    p_page->p_prev->p_next = p_page->p_next;
    p_page->p_next->p_prev = p_page->p_prev;
    p_head = p_vol->cache.p_current_data;
    p_page->p_next = p_head;
    p_page->p_prev = p_vol->cache.p_current_data->p_prev;
    p_head->p_prev = p_page;
    p_page->p_prev->p_next = p_page;
}

s32 VFiPFCACHE_ReadFATPage(PF_VOLUME* p_vol, u32 sector, PF_CACHE_PAGE** pp_page) {
    s32 err = VFiPFCACHE_DoReadPage(p_vol, &p_vol->cache.p_current_fat, sector, pp_page, 0);
    if (err != 0) {
        return err;
    }
    return 0;
}

s32 VFiPFCACHE_ReadDataPage(PF_VOLUME* p_vol, u32 sector, PF_CACHE_PAGE** pp_page, u32 set_sig) {
    s32 err = VFiPFCACHE_DoReadPage(p_vol, &p_vol->cache.p_current_data, sector, pp_page, set_sig);
    if (err != 0) {
        return err;
    }
    return 0;
}

s32 VFiPFCACHE_AllocateFATPage(PF_VOLUME* p_vol, u32 sector, PF_CACHE_PAGE** pp_page) {
    s32 err;
    u32 is_hit;

    if (p_vol->cache.p_current_fat == NULL) {
        p_vol->cache.pages->buffer = (u8*)p_vol->cache.buffers;
        p_vol->cache.pages->p_buf = (u8*)p_vol->cache.buffers;
        *pp_page = p_vol->cache.pages;
        return 0;
    }
    err = VFiPFCACHE_DoAllocatePage(p_vol, &p_vol->cache.p_current_fat, sector, pp_page, &is_hit);
    if (err != 0) {
        return err;
    }
    return 0;
}

s32 VFiPFCACHE_AllocateDataPage(PF_VOLUME* p_vol, u32 sector, PF_CACHE_PAGE** pp_page) {
    s32 err;
    u32 is_hit;

    if (p_vol->cache.p_current_data == NULL) {
        p_vol->cache.pages->buffer = (u8*)p_vol->cache.buffers;
        p_vol->cache.pages->p_buf = (u8*)p_vol->cache.buffers;
        *pp_page = p_vol->cache.pages;
        return 0;
    }
    err = VFiPFCACHE_DoAllocatePage(p_vol, &p_vol->cache.p_current_data, sector, pp_page, &is_hit);
    if (err != 0) {
        return err;
    }
    return 0;
}

s32 VFiPFCACHE_WriteFATPage(PF_VOLUME* p_vol, PF_CACHE_PAGE* p_page) {
    s32 err = VFiPFCACHE_DoWritePage(p_vol, &p_vol->cache.p_current_fat, p_page, 0);
    if (err != 0) {
        return err;
    }
    return 0;
}

s32 VFiPFCACHE_WriteDataPage(PF_VOLUME* p_vol, PF_CACHE_PAGE* p_page, u32 set_sig) {
    s32 err = VFiPFCACHE_DoWritePage(p_vol, &p_vol->cache.p_current_data, p_page, set_sig);
    if (err != 0) {
        return err;
    }
    return 0;
}

s32 VFiPFCACHE_WriteFATSectorAndFreeIfNeeded(PF_VOLUME* p_vol, const u8* p_buf, u32 sector) {
    s32 err;

    if (sector >= p_vol->bpb.total_sectors) {
        return 0x10;
    }

    err = VFiPFCACHE_DoWriteSector(p_vol, &p_vol->cache.p_current_fat, p_buf, sector);
    if (err != 0) {
        return err;
    }
    return 0;
}

PF_CACHE_PAGE* VFiPFCACHE_SearchDataCache(PF_VOLUME* p_vol, u32 sector) {
    return VFiPFCACHE_SearchForPage(p_vol, p_vol->cache.p_current_data, sector);
}

s32 VFiPFCACHE_FlushDataCacheSpecific(PF_VOLUME* p_vol, void* signature) {
    s32 err;
    PF_CACHE_PAGE* p_head;
    PF_CACHE_PAGE* p_page;
    u32 is_modified;
    u32 is_flushed;

    err = 0;
    is_flushed = 0;
    if ((p_vol->cache.mode & 7) != 0) {
        p_head = p_vol->cache.p_current_data;
        if (p_head->signature == NULL || p_head->signature == signature) {
            if (p_head != NULL && (p_head->stat & 2) && p_head->sector != 0xFFFFFFFF) {
                is_flushed = 1;
            }
            err = VFiPFCACHE_FlushPageIfNeeded(p_vol, p_head);
            if (err != 0) {
                return err;
            }
        }
        for (p_page = p_head->p_next; p_page != p_head; p_page = p_page->p_next) {
            if (p_page->signature == NULL || p_page->signature == signature) {
                is_modified = 0;
                if (p_page != NULL && (p_page->stat & 2) && p_page->sector != 0xFFFFFFFF) {
                    is_modified = 1;
                }
                err = VFiPFCACHE_FlushPageIfNeeded(p_vol, p_page);
                if (err != 0) {
                    if (is_flushed != 0) {
                        dCommon_FlushFromVol(p_vol, 0);
                    }
                    return err;
                }
                if (is_modified != 0) {
                    is_flushed = 1;
                }
            }
        }
        if (is_flushed != 0) {
            if (dCommon_FlushFromVol(p_vol, 1) != 0) {
                return 0x11;
            }
        }
    }
    return err;
}

s32 VFiPFCACHE_FlushFATCache(PF_VOLUME* p_vol) {
    s32 err = VFiPFCACHE_DoFlushCache(p_vol, p_vol->cache.p_current_fat);
    if (err != 0) {
        return err;
    }
    return 0;
}

s32 VFiPFCACHE_FlushAllCaches(PF_VOLUME* p_vol) {
    s32 err;

    err = VFiPFCACHE_DoFlushCache(p_vol, p_vol->cache.p_current_fat);
    if (err != 0) {
        return err;
    }
    err = VFiPFCACHE_DoFlushCache(p_vol, p_vol->cache.p_current_data);
    if (err != 0) {
        return err;
    }
    return 0;
}

// ---------------------------------------------------------------- pf_dir

s32 VFiPFDIR_p_fsfirst(PF_VOLUME* p_vol, PF_STR* p_path, s8 attr, PF_DTA* p_dta) {
    PF_DIR_ENT ent;
    PF_ENT_ITER iter;
    PF_STR dir_path;
    PF_STR filename;
    s32 err;

    err = fn_803BE3D0(p_path, &dir_path, &filename);
    if (err != 0) return err;

    err = fn_803B5644(&iter, &ent, p_vol, &dir_path);
    if (err != 0) return err;

    if (!(ent.attr & 0x10)) return 0x14;

    p_dta->start_cluster = ent.start_cluster;
    p_dta->index = 0;
    p_dta->p_vol = ent.p_vol;
    p_dta->attr = attr;
    p_dta->status = 0;

    if (fn_803A9A78(&filename) == 1) {
        fn_803BF90C(p_dta->pattern, NULL, &filename);
        p_dta->status |= 1;
    } else {
        fn_803BF90C(p_dta->pattern, p_dta->unk444, &filename);
        p_dta->status |= 2;
    }

    return fn_803B161C(p_dta);
}

void VFiPFDIR_FinalizeAllDirs(PF_VOLUME* p_vol) {
    u32 i;
    u32 zero;
    PF_SDD* p_sdd;

    zero = 0;
    i = 0;
    do {
        p_sdd = &p_vol->sdds[(u16)i];
        p_sdd->stat = zero;
        VFiPFFAT_FinalizeFFD(&p_sdd->ffd);
        i++;
    } while (i < 3);

    p_vol->udds[0].stat &= ~1;
    p_vol->udds[1].stat &= ~1;
    p_vol->udds[2].stat &= ~1;
    p_vol->num_opened_directories = 0;
}

s32 VFiPFDIR_fsnext(PF_DTA* p_dta) {
    PF_VOLUME* p_vol;
    s32 err;

    if (p_dta == NULL) {
        lbl_8053B248.last_error = 10;
        return 10;
    }
    p_vol = p_dta->p_vol;
    if (p_vol == NULL) {
        lbl_8053B248.last_error = 10;
        return 10;
    }
    err = VFiPFVOL_CheckForRead(p_vol);
    if (err != 0) {
        lbl_8053B248.last_error = err;
        p_vol->last_error = err;
        return err;
    }
    err = fn_803B161C(p_dta);
    if (err != 0) {
        lbl_8053B248.last_error = err;
        p_vol->last_error = err;
    }
    return err;
}

s32 VFiPFDIR_chdir(PF_STR* p_path) {
    PF_VOLUME* p_vol;
    s32 err;

    if (p_path == NULL) {
        lbl_8053B248.last_error = 10;
        return 10;
    }

    if (VFiPFSTR_StrNCmp(p_path, lbl_8059FE30, 1, 0, 1) == 0) {
        lbl_8053B248.last_error = 10;
        return 10;
    }

    p_vol = VFiPFPATH_GetVolumeFromPath(p_path);
    if (p_vol == NULL) {
        lbl_8053B248.last_error = 9;
        return 9;
    }

    err = VFiPFVOL_CheckForRead(p_vol);
    if (err != 0) {
        lbl_8053B248.last_error = err;
        p_vol->last_error = err;
        return err;
    }

    err = fn_803B11A4(p_vol, p_path);
    if (err != 0) {
        lbl_8053B248.last_error = err;
        p_vol->last_error = err;
    } else {
        VFiPFVOL_SetCurrentVolume(p_vol);
    }

    return err;
}

void fn_803B2968(unsigned char *p)
{
    int i;

    p[0] = 0x2E;
    for (i = 1; i < 11; i++) {
        p[i] = 0x20;
    }
}

void fn_803B29A0(unsigned char *p)
{
    int i;

    for (i = 0; i < 2; i++) {
        p[i] = 0x2E;
    }
    for (i = 2; i < 11; i++) {
        p[i] = 0x20;
    }
}

// ---------------------------------------------------------------- pf_entry

s32 VFiPFENT_findEmptyTailSFN(PF_DIR_ENT* p_ent_containig_dir, const s8* name, u32* p_tails) {
    PF_FAT_HINT hint;
    PF_FFD ffd;
    u32 tail_index;
    s32 err;
    u32 i;
    PF_VOLUME* p_vol = p_ent_containig_dir->p_vol;

    *p_tails = 1;
    VFiPFFAT_InitFFD(&ffd, &hint, p_ent_containig_dir->p_vol, &p_ent_containig_dir->start_cluster);
    for (tail_index = 1; tail_index <= 999999; tail_index += p_vol->tail_entry.tracker_size * 32) {
        err = VFiPFENT_searchEmptyTailSFN(&ffd, tail_index, name, p_vol->tail_entry.tracker_bits);
        if (err != 0) return err;
        for (i = 0; i < p_vol->tail_entry.tracker_size; i++) {
            if (p_vol->tail_entry.tracker_bits[i] != 0xFFFFFFFF) {
                while (p_vol->tail_entry.tracker_bits[i] & 1) {
                    p_vol->tail_entry.tracker_bits[i] >>= 1;
                    ++*p_tails;
                }
                tail_index = 1000000;
                break;
            }
            *p_tails += 32;
        }
    }
    return 0;
}

s32 VFiPFENT_compareEntryName(PF_DIR_ENT* p_ent, PF_STR* p_pattern, u8 attr) {
    u8 buf[11];
    u8 sum;
    u16 i;
    s32 result = 1;

    if (p_ent->num_entry_LFNs != 0 && p_ent->ordinal == 1) {
        VFiPFPATH_putShortName(buf, p_ent->short_name, 0);
        sum = 0;
        for (i = 0; i < 11; i++) {
            sum = (u8)(((sum & 1) ? 0x80 : 0) + (sum >> 1) + buf[i]);
        }
        if (p_ent->check_sum == sum) {
            result = VFiPFPATH_cmpNameUni(p_ent->long_name, p_pattern) != 0;
        }
    }
    if (result == 1) {
        if ((lbl_8053B248.setting & 2) == 2 && !(attr & 8)) {
            if (VFiPFPATH_cmpName(p_ent->short_name, p_pattern, 0) == 0) result = 0;
        } else {
            if (VFiPFPATH_cmpName(p_ent->short_name, p_pattern, 1) == 0) result = 0;
        }
    }
    return result;
}

s32 VFiPFDIR_fsfirst(PF_STR* p_path, u32 attr, PF_DTA* p_dta) {
    PF_VOLUME* p_vol;
    s32 err;

    if (p_path == NULL) {
        lbl_8053B248.last_error = 10;
        return 10;
    }
    p_vol = VFiPFPATH_GetVolumeFromPath(p_path);
    if (p_vol == NULL) {
        lbl_8053B248.last_error = 9;
        return 9;
    }
    if (p_dta == NULL) {
        lbl_8053B248.last_error = 10;
        p_vol->last_error = 10;
        return 10;
    }
    if (attr != 0x7F && ((attr & 0xFFFFFF00) || (s32)(attr & 0x18) == 0x18)) {
        lbl_8053B248.last_error = 10;
        p_vol->last_error = 10;
        return 10;
    }
    err = VFiPFVOL_CheckForRead(p_vol);
    if (err != 0) {
        lbl_8053B248.last_error = err;
        p_vol->last_error = err;
        return err;
    }
    // BYTES: the original entry point receives the full attribute word.
    err = ((s32 (*)(PF_VOLUME*, PF_STR*, u32, PF_DTA*))VFiPFDIR_p_fsfirst)(p_vol, p_path, attr, p_dta);
    if (err != 0) {
        p_dta->p_vol = NULL;
        lbl_8053B248.last_error = err;
        p_vol->last_error = err;
    }
    return err;
}

u8 VFiPFENT_CalcCheckSum(PF_DIR_ENT* p_ent) {
    u8 buf[11];
    u8 sum;
    u16 i;

    VFiPFPATH_putShortName(buf, p_ent->short_name, 0);
    sum = 0;
    for (i = 0; i < 11; i++) {
        sum = (u8)(((sum & 1) ? 0x80 : 0) + (sum >> 1) + buf[i]);
    }
    return sum;
}

void VFiPFENT_LoadShortNameFromBuf(PF_DIR_ENT* p_ent, const u8* buf) {
    VFiPFPATH_getShortName(p_ent->short_name, buf, buf[0xb]);
    if (p_ent->short_name[0] == 5) {
        p_ent->short_name[0] = -0x1B;
    }
}

static inline u16 vf_bswap16(u16 x) {
    return (u16)((x << 8) | (x >> 8));
}

static inline u32 vf_bswap32(u32 x) {
    return (x << 24) | ((x << 8) & 0x00FF0000) | ((x >> 8) & 0x0000FF00) | (x >> 24);
}

static inline void vf_store_le16(void* dst, u16 x) {
    __sthbrx(x, dst, 0);
}

u32 VFiPFENT_compareAttr(u8 attr, u8 attr_required, u8 attr_unwanted) {
    u32 match;

    match = 1;
    if (attr == 0) {
        attr = 0x40;
    }
    if (attr_required & 0x80) {
        s32 unwanted = attr_unwanted & 0x7F;
        s32 required = attr_required & 0x7F;

        if ((required != 0 && (attr & required) != required) ||
            (unwanted != 0 && (attr & unwanted) == unwanted)) {
            match = 0;
        }
    } else if (attr_required != 0x7F && attr != attr_required &&
               ((attr & attr_required) == 0 || (attr & attr_unwanted) != 0)) {
        match = 0;
    }
    return match;
}

void VFiPFENT_loadEntryNumericFieldsFromBuf(PF_DIR_ENT* p_ent, const u8* buf) {
    p_ent->attr = buf[0x0B];
    p_ent->small_letter_flag = buf[0x0C];
    p_ent->create_time_ms = buf[0x0D];
    p_ent->create_time = vf_bswap16(*(const u16*)(buf + 0x0E));
    p_ent->create_date = vf_bswap16(*(const u16*)(buf + 0x10));
    p_ent->access_date = vf_bswap16(*(const u16*)(buf + 0x12));
    p_ent->modify_time = vf_bswap16(*(const u16*)(buf + 0x16));
    p_ent->modify_date = vf_bswap16(*(const u16*)(buf + 0x18));
    p_ent->file_size = vf_bswap32(*(const u32*)(buf + 0x1C));
    p_ent->start_cluster = ((u32)vf_bswap16(*(const u16*)(buf + 0x14)) << 16) |
                           vf_bswap16(*(const u16*)(buf + 0x1A));
}

void fn_803B2B3C(u8* buf, const PF_DIR_ENT* p_ent) { // VFiPFENT_StoreEntryNumericFieldsToBuf
    u32 cluster;

    buf[0x0B] = p_ent->attr;
    buf[0x0C] = p_ent->small_letter_flag;
    buf[0x0D] = p_ent->create_time_ms;
    *(u16*)(buf + 0x0E) = vf_bswap16(p_ent->create_time);
    *(u16*)(buf + 0x10) = vf_bswap16(p_ent->create_date);
    *(u16*)(buf + 0x12) = vf_bswap16(p_ent->access_date);
    *(u16*)(buf + 0x16) = vf_bswap16(p_ent->modify_time);
    *(u16*)(buf + 0x18) = vf_bswap16(p_ent->modify_date);
    *(u32*)(buf + 0x1C) = vf_bswap32(p_ent->file_size);
    cluster = p_ent->start_cluster;
    *(u16*)(buf + 0x14) = vf_bswap16((u16)(cluster >> 16));
    *(u16*)(buf + 0x1A) = vf_bswap16((u16)cluster);
}

s32 VFiPFENT_findEntry(PF_FFD* p_ffd, PF_DIR_ENT* p_ent, u32 index_search_from, PF_STR* p_pattern, u8 attr_required, u8 attr_unwanted) {
    u32 lpos;
    u32 ppos;

    return VFiPFENT_findEntryPos(p_ffd, p_ent, index_search_from, p_pattern, attr_required, attr_unwanted, &lpos, &ppos);
}

s32 VFiPFENT_allocateEntry(PF_DIR_ENT* p_ent, u8 num_entries, PF_FFD* p_ffd, u32* p_next_chain, PF_STR* p_filename) {
    u32 pos;

    return VFiPFENT_allocateEntryPos(p_ent, num_entries, p_ffd, p_next_chain, p_filename, &pos);
}

s32 VFiPFFILE_p_finfo(PF_FILE* p_file, PF_INFO* p_info) {
    PF_VOLUME* p_vol;
    u32 cluster_size;
    u32 rem;
    s32 err;

    p_file->p_sfd->ffd.p_hint = &p_file->hint;
    if (p_file == NULL) {
        p_vol = NULL;
    } else {
        p_vol = p_file->p_sfd->dir_entry.p_vol;
    }
    p_info->file_size = p_file->p_sfd->dir_entry.file_size;
    p_info->io_pointer = p_file->cursor.position;
    cluster_size = p_vol->bpb.bytes_per_sector << p_vol->bpb.log2_sectors_per_cluster;
    if (p_file->p_sfd->dir_entry.start_cluster != 0) {
        err = VFiPFCLUSTER_GetAppendSize(p_file, &p_info->allocated_size);
        if (err != 0) {
            return err;
        }
        rem = p_file->p_sfd->dir_entry.file_size % cluster_size;
        p_info->empty_size = p_info->allocated_size + (rem != 0 ? cluster_size - rem : 0);
    } else {
        p_info->allocated_size = 0;
        p_info->empty_size = 0;
    }
    p_info->lock_mode = p_file->p_sfd->lock.mode & 3;
    p_info->lock_owner = p_file->p_sfd->lock.owner;
    p_info->lock_count = p_file->lock_count;
    p_info->lock_tcount = p_file->p_sfd->lock.count;
    return 0;
}

s32 VFiPFVOL_detach(s8 drv_char) {
    s16 index = fn_803A962C(drv_char) - 'A';
    PF_VOLUME* p_vol;
    s32 err;

    if (index < 0 || index >= 26) {
        p_vol = NULL;
    } else {
        p_vol = &lbl_8053B248.volumes[index];
    }
    if (p_vol == NULL) {
        lbl_8053B248.last_error = 10;
        return 10;
    }
    if (p_vol->flags & 2) {
        lbl_8053B248.last_error = 10;
        p_vol->last_error = 10;
        return 10;
    }
    if (!(p_vol->flags & 1)) {
        lbl_8053B248.last_error = 10;
        p_vol->last_error = 10;
        return 10;
    }
    err = VFiPFDRV_finalize(p_vol);
    err = (err != 0) ? err : 0;
    if (err != 0) {
        lbl_8053B248.last_error = err;
        p_vol->last_error = err;
        return err;
    }
    p_vol->flags &= ~1;
    lbl_8053B248.num_attached_drives--;
    return 0;
}

s32 VFiPFENT_GetRootDir(PF_VOLUME* p_vol, PF_DIR_ENT* p_ent) {
    if (p_vol == NULL) {
        return 10;
    }
    if (p_ent == NULL) {
        return 10;
    }
    if (!(p_vol->flags & 2)) {
        return 9;
    }
    p_ent->p_vol = p_vol;
    p_ent->long_name[0] = '\\';
    p_ent->long_name[1] = 0;
    p_ent->num_entry_LFNs = 0;
    p_ent->ordinal = 0;
    p_ent->check_sum = 0;
    p_ent->short_name[0] = '\\';
    p_ent->short_name[1] = 0;
    p_ent->small_letter_flag = 0;
    p_ent->attr = 0x10;
    p_ent->create_time_ms = 0;
    p_ent->create_time = 0;
    p_ent->create_date = 0;
    p_ent->access_date = 0;
    p_ent->modify_time = 0;
    p_ent->modify_date = 0;
    p_ent->file_size = 0;
    p_ent->path_len = 3;
    p_ent->start_cluster = 1;
    p_ent->entry_sector = 0xFFFFFFFF;
    p_ent->entry_offset = 0;
    return 0;
}

u8 VFiPFENT_getcurrentDateTimeForEnt(u16* p_date, u16* p_time) {
    PF_SYS_DATE sdate;
    PF_SYS_TIME stime;

    VFiPFSYS_TimeStamp(&sdate, &stime);
    *p_date = (((sdate.sys_year - 1980) << 9) & 0xFE00) | ((sdate.sys_month << 5) & 0x1E0) | (sdate.sys_day & 0x1F);
    *p_time = (((stime.sys_hour << 11) & 0xF800) | ((stime.sys_min << 5) & 0x7E0) | ((stime.sys_sec >> 1) & 0x1F));
    return stime.sys_ms;
}

s32 VFiPFENT_ITER_IteratorInitialize(PF_ENT_ITER* p_iter, u32 index_start_from) {
    u32 num_success;
    s32 err;
    u32 index;
    s32 status;
    s32 read_err;
    u8 log2;

    p_iter->p_vol = p_iter->ffd.p_vol;
    if (p_iter->p_vol == NULL)
        return 10;

    index = p_iter->index;
    log2 = p_iter->p_vol->bpb.log2_bytes_per_sector - 5;
    p_iter->file_sector_index = 0xFFFFFFFF;
    p_iter->index = index_start_from;
    p_iter->log2_entries_per_sector = log2;
    p_iter->offset_mask = (1 << p_iter->log2_entries_per_sector) - 1;

    err = VFiPFENT_RecalcEntryIterator(p_iter, 0);
    if (err != 0) {
        p_iter->index = index;
        VFiPFENT_RecalcEntryIterator(p_iter, 0);
    } else {
        if (p_iter->sector == 0xFFFFFFFF) {
            status = 16;
        } else {
            read_err = VFiPFSEC_ReadData(p_iter->p_vol, p_iter->buf, p_iter->sector, p_iter->offset, 32, &num_success, 0);
            status = 17;
            if (num_success == 32)
                status = read_err;
        }
        err = status != 0 ? status : 0;
    }

    return err;
}

// HYPOTHESIS: VFiPFENT_ITER_FindEntry (named by the map); DoFindEntry is defined elsewhere in the original unit.
s32 VFiPFENT_ITER_DoFindEntry(PF_ENT_ITER* p_iter, PF_DIR_ENT* p_ent, PF_STR* p_pattern, u8 attr_required, u8 attr_unwanted, u32* p_is_found, u32 is_skip);

s32 fn_803B555C(PF_ENT_ITER* p_iter, PF_DIR_ENT* p_ent, PF_STR* p_pattern, u8 attr_required, u8 attr_unwanted, u32* p_is_found, u32 is_skip) {
    s32 err;

    if (p_ent == NULL) {
        return 10;
    }
    if (p_pattern->p_head == NULL) {
        return 10;
    }
    if (VFiPFSTR_StrLen(p_pattern) == 0) {
        return 10;
    }
    if (p_is_found == NULL) {
        return 10;
    }
    *p_is_found = 0;
    if ((attr_required & attr_unwanted) != 0) {
        return 10;
    }
    err = VFiPFENT_ITER_DoFindEntry(p_iter, p_ent, p_pattern, attr_required, attr_unwanted, p_is_found, is_skip);
    return err != 0 ? err : 0;
}

// HYPOTHESIS: VFiPF_LE16_TO_U16_STR (named by the map): swaps each byte pair of a UTF-16 string.
void fn_803A9958(u8* p_str, u32 length) {
    u32 i;

    for (i = 0; i < length; i += 2) {
        const char tmp = p_str[i];
        p_str[i] = p_str[i + 1];
        p_str[i + 1] = tmp;
    }
}

s32 VFiPFPATH_transformFromUnicodeToNormal(s8* dst, const u16* src) {
    const u16* cur;
    s32 index;
    s16 oem_width;
    s16 uni_width;
    u16 dot[2];
    u16 space[2];

    if ((lbl_8053B248.setting & 2) == 2) {
        dot[0] = '.';
        dot[1] = 0;
        space[0] = ' ';
        space[1] = 0;
        cur = src;
        while (*cur != 0) {
            if (fn_803AA040((u16*)src, dot, 1) == 0) {
                *dst = '.';
            } else if (fn_803AA040((u16*)src, space, 1) == 0) {
                *dst = ' ';
            } else {
                *dst = '_';
            }
            dst++;
            src++;
            cur++;
        }
    } else {
        index = 0;
        while (src[index] != 0) {
            s32 width = lbl_8053B248.codeset.unicode2oem(src + index, dst);
            VFiPFCODE_Divide_Width(width, &oem_width, &uni_width);
            dst += oem_width;
            index += uni_width >> 1;
        }
    }
    *dst = 0;
    return 0;
}

// HYPOTHESIS: VFiPFPATH_SetSearchPattern (named by the map).
void fn_803BF90C(u8* short_pattern, u8* long_pattern_bytes, PF_STR* p_pattern) {
    u16* long_pattern = (u16*)long_pattern_bytes;
    u16 star[2];

    if (fn_803A9A78(p_pattern) == 1) {
        if (fn_803A98A8((u8*)p_pattern->p_head, (u8*)"*.*") == 0) {
            fn_803A9884((s8*)short_pattern, (const s8*)"*");
        } else {
            fn_803A9884((s8*)short_pattern, (const s8*)p_pattern->p_head);
        }
    } else {
        if (VFiPFSTR_StrCmp(p_pattern, (const s8*)"*.*") == 0) {
            star[0] = '*';
            star[1] = 0;
            fn_803AA01C(long_pattern, star);
        } else {
            fn_803AA01C(long_pattern, (const u16*)p_pattern->p_head);
        }
        if ((lbl_8053B248.setting & 2) == 2) {
            lbl_8053B248.setting = (lbl_8053B248.setting & ~3) | 1;
            VFiPFPATH_transformFromUnicodeToNormal((s8*)short_pattern, long_pattern);
            lbl_8053B248.setting = (lbl_8053B248.setting & ~3) | 2;
        } else {
            VFiPFPATH_transformFromUnicodeToNormal((s8*)short_pattern, long_pattern);
        }
    }
}

s32 VFiPFPATH_cmpName(const s8* sShort, PF_STR* p_pattern, u32 is_short_search) {
    u32 is_end = 0;
    char buf[13];
    char* cur = buf;
    const s8* pattern = fn_803A9AA8(p_pattern, 3);

    fn_803A9884((s8*)buf, sShort);
    if ((lbl_8053B248.setting & 2) == 2 && fn_803A9A78(p_pattern) == 2) {
        if (fn_803A98A8((u8*)buf, (u8*)".") != 0 && fn_803A98A8((u8*)buf, (u8*)"..") != 0
            && VFiPFPATH_CheckExtShortName(p_pattern, 3, 1) == 0 && is_short_search == 0) {
            return 1;
        }
    }
    if (fn_803A98A8((u8*)pattern, (u8*)"*.") == 0) {
        while (*cur != 0 && *cur != '.') {
            cur++;
        }
        if (*cur == 0) {
            cur[0] = '.';
            cur[1] = 0;
        }
    } else if (*pattern == 0) {
        return 1;
    } else if (buf[0] == 0) {
        return 1;
    }
    return VFiPFPATH_cmpNameImpl((const s8*)buf, pattern, &is_end);
}

s32 VFiPFENT_MakeRootDir(PF_VOLUME* p_vol) {
    PF_CACHE_PAGE* p_page;
    u32 written;
    u32 sector;
    s32 err;

    if (p_vol == NULL) {
        return 10;
    }
    switch (p_vol->bpb.fat_type) {
    case 2:
        VFiPFFAT_MakeRootDir(p_vol);
        break;
    case 0:
    case 1:
        err = VFiPFCACHE_AllocateDataPage(p_vol, 0xFFFFFFFF, &p_page);
        if (err != 0) {
            return err;
        }
        fn_803A976C(p_page->p_buf, 0, p_vol->bpb.bytes_per_sector);
        for (sector = p_vol->bpb.first_root_dir_sector; sector < p_vol->bpb.first_data_sector; sector++) {
            err = VFiPFSEC_WriteData(p_vol, p_page->p_buf, sector, 0, p_vol->bpb.bytes_per_sector, &written, 0);
            if (err != 0) {
                VFiPFCACHE_FreeDataPage(p_vol, p_page);
                return err;
            }
            if (written != p_vol->bpb.bytes_per_sector) {
                VFiPFCACHE_FreeDataPage(p_vol, p_page);
                return 17;
            }
        }
        VFiPFCACHE_FreeDataPage(p_vol, p_page);
        break;
    default:
        return 7;
    }
    VFiPFVOL_LoadVolumeLabelFromBuf((const u8*)"NO NAME    ", p_vol);
    return 0;
}

s32 VFiPFPATH_putShortName(u8* pDirEntry, const s8* short_name, u8 attr) {
    s32 i = 0;

    while (i < 8 && *short_name != 0 && *short_name != '.') {
        *pDirEntry++ = *short_name++;
        i++;
    }
    if (i == 0) {
        i = 0;
        while (i < 2 && *short_name != 0) {
            *pDirEntry++ = *short_name++;
            i++;
        }
    }
    if (i > 8) {
        return 10;
    }
    if ((attr & 8) == 0) {
        while (i < 8) {
            *pDirEntry++ = ' ';
            i++;
        }
    }
    if (*short_name != 0) {
        if ((attr & 8) == 0) {
            short_name++;
        }
        while (*short_name != 0) {
            *pDirEntry++ = *short_name++;
            i++;
        }
    }
    while (i < 11) {
        *pDirEntry++ = ' ';
        i++;
    }
    return 0;
}

static inline s32 pfent_iter_advance(PF_ENT_ITER* p_iter, u32 may_allocate) {
    u32 num_success;
    u32 index;
    u32 next;
    s32 err;
    s32 status;

    index = p_iter->index;
    next = index + 1;
    if (index != next || p_iter->sector != 0xFFFFFFFF || may_allocate == 0) {
        p_iter->index = next;
        err = VFiPFENT_RecalcEntryIterator(p_iter, may_allocate);
        if (err != 0) {
            p_iter->index = index;
            VFiPFENT_RecalcEntryIterator(p_iter, 0);
        } else {
            if (p_iter->sector == 0xFFFFFFFF) {
                status = 0x10;
            } else {
                s32 read_err = VFiPFSEC_ReadData(p_iter->p_vol, p_iter->buf, p_iter->sector, p_iter->offset, 0x20, &num_success, 0);
                status = 0x11;
                if (num_success == 0x20)
                    status = read_err;
            }
            err = status != 0 ? status : 0;
        }
        if (err != 0)
            return err;
    }
    return 0;
}

s32 VFiPFENT_ITER_Advance(PF_ENT_ITER* p_iter, u32 may_allocate) {
    return pfent_iter_advance(p_iter, may_allocate);
}

// The iterator's sector-read helper also handles callers already at the end.
static inline s32 pfent_iter_read(PF_ENT_ITER* p_iter) {
    u32 num_success;
    s32 status;
    s32 err;

    if (p_iter->sector == 0xFFFFFFFF) {
        status = 0x10;
    } else {
        err = VFiPFSEC_ReadData(p_iter->p_vol, p_iter->buf, p_iter->sector, p_iter->offset, 0x20, &num_success, 0);
        status = 0x11;
        if (num_success == 0x20) status = err;
    }
    return status;
}

u32 VFiPFENT_ITER_IsAtLogicalEnd(PF_ENT_ITER* p_iter) {
    u32 sector_index;
    u32 index;
    s32 err;

    if (p_iter->sector == 0xFFFFFFFF || p_iter->buf[0] == 0 || p_iter->index >= 999999) {
        sector_index = p_iter->file_sector_index;
        index = p_iter->index;
        p_iter->index = (sector_index + 1) << p_iter->log2_entries_per_sector;
        err = VFiPFENT_RecalcEntryIterator(p_iter, 0);
        if (err != 0) {
            p_iter->index = index;
            p_iter->file_sector_index = sector_index;
            return 1;
        }
        if (p_iter->sector != 0xFFFFFFFF) {
            return pfent_iter_read(p_iter) != 0;
        }
        return 1;
    }
    return 0;
}

s32 VFiPFENT_ITER_GetEntryOfPath(PF_ENT_ITER* p_iter, PF_DIR_ENT* p_ent, PF_VOLUME* p_vol, PF_STR* p_path, u32 no_look_last_token) {
    return fn_803B4910(p_iter, p_ent, p_vol, p_path, 0, no_look_last_token);
}

s32 fn_803B5644(PF_ENT_ITER* p_iter, PF_DIR_ENT* p_ent, PF_VOLUME* p_vol, PF_STR* p_path) {
    return fn_803B4910(p_iter, p_ent, p_vol, p_path, 1, 1);
}

// ---------------------------------------------------------------- pf_fat

s32 VFiPFFAT32_WriteFATEntry(PF_VOLUME* p_vol, u32 cluster, u32 value) {
    u32 fat_value = value;
    u32 sector;
    u16 pos;
    u32 offset;
    u32 entry;
    s32 err;

    if (p_vol == NULL) return 10;
    if ((cluster < 2 || cluster >= p_vol->bpb.num_clusters + 2) && cluster != 0 && cluster != 1) return 14;
    offset = cluster * 4;
    sector = (u16)(p_vol->bpb.active_FAT_sector + (offset >> p_vol->bpb.log2_bytes_per_sector));
    pos = offset & (p_vol->bpb.bytes_per_sector - 1);
    err = VFiPFSEC_ReadFAT(p_vol, (u8*)&entry, sector, pos, 4);
    if (err != 0) return err;
    entry = ((entry << 24) | ((entry << 8) & 0xFF0000)) | ((entry >> 24) | ((entry >> 8) & 0xFF00));
    entry &= 0xF0000000;
    entry |= fat_value & 0x0FFFFFFF;
    entry = ((entry << 24) | ((entry << 8) & 0xFF0000)) | ((entry >> 24) | ((entry >> 8) & 0xFF00));
    return VFiPFSEC_WriteFAT(p_vol, (u8*)&entry, sector, pos, 4);
}

s32 VFiPFFAT_SearchForNumFreeClusters(PF_VOLUME* p_vol, u32 start_cluster, u32 end_cluster, u32 num_cluster, u32* p_start_free_cluster, u32* p_last_free_cluster) {
    u32 limit;
    u32 num_free;
    u32 best_start;
    u32 best_num;
    u32 wrapped;
    u32 first_cluster;
    u32 value;
    PF_CACHE_PAGE* p_page;
    s32 err;

    *p_start_free_cluster = 0xFFFFFFFF;
    *p_last_free_cluster = 0xFFFFFFFF;
    wrapped = 0;
    num_free = 0;
    best_start = 0xFFFFFFFF;
    limit = p_vol->bpb.num_clusters + 2;
    best_num = 0;
    if (start_cluster < 2 || start_cluster >= limit) {
        start_cluster = 2;
    }
    if (end_cluster < 2 || end_cluster >= limit) {
        end_cluster = limit - 1;
    }
    first_cluster = start_cluster;
    err = VFiPFFAT_ReadFATSector(p_vol, &p_page, start_cluster);
    if (err != 0) {
        return err;
    }
    p_page->option = 0;

    while (start_cluster <= end_cluster) {
        if (limit <= start_cluster) {
            start_cluster = 2;
        }
        switch (p_vol->bpb.fat_type) {
        case FAT_12:
            err = VFiPFFAT12_ReadFATEntryPage(p_vol, (u16)start_cluster, &value, &p_page);
            break;
        case FAT_16:
            err = VFiPFFAT16_ReadFATEntryPage(p_vol, start_cluster, &value, &p_page);
            break;
        case FAT_32:
            err = VFiPFFAT32_ReadFATEntryPage(p_vol, start_cluster, &value, &p_page);
            break;
        default:
            err = 0xF;
            break;
        }
        if (err != 0) {
            return err;
        }
        if (value == 0) {
            if (*p_start_free_cluster == 0xFFFFFFFF) {
                *p_start_free_cluster = start_cluster;
            }
            num_free++;
            if (num_free >= num_cluster) {
                *p_last_free_cluster = start_cluster;
                return 0;
            }
        } else {
            if (best_num < num_free) {
                best_start = *p_start_free_cluster;
                best_num = num_free;
            }
            *p_start_free_cluster = 0xFFFFFFFF;
            num_free = 0;
        }
        start_cluster++;
        if (wrapped == 0 && num_free == 0 && start_cluster > 2 && start_cluster == end_cluster) {
            end_cluster = first_cluster;
            start_cluster = 2;
            wrapped = 1;
        }
    }

    if (best_num < num_free) {
        best_start = *p_start_free_cluster;
        best_num = num_free;
    }
    if (best_num != 0) {
        *p_start_free_cluster = best_start;
        *p_last_free_cluster = (best_num + best_start) - 1;
    }
    return 0;
}

s32 VFiPFFAT_UpdateFATEntry(PF_VOLUME* p_vol, PF_CACHE_PAGE* p_page) {
    s32 err = 0;

    if ((p_page->stat & 2) == 2) {
        err = VFiPFCACHE_WriteFATPage(p_vol, p_page);
    }

    return err;
}

// HYPOTHESIS: fsi_flag bit 1 means an FSINFO sector exists, bit 2 means its free count is stale.
s32 fn_803B80F0(PF_VOLUME* p_vol) { // VFiPFFAT_RefreshFSINFO
    u16 flag;
    s32 err;

    err = 0;
    if (p_vol->bpb.fat_type != FAT_32) {
        return 0xC;
    }
    flag = p_vol->fsi_flag;
    if ((flag & 2) == 0) {
        return 0x24;
    }
    if ((flag & 4) != 0) {
        err = VFiPFDRV_StoreFreeCountToFSINFO(p_vol);
    }
    return err;
}

s32 VFiPFFAT_GetContinuousSector(PF_FFD* p_ffd, u32 file_sector_index, u32 size, u32* p_sector, u32* p_num_sector) {
    PF_VOLUME* p_vol;
    u32 cluster;
    u32 sector;
    s32 err;

    cluster = 0xFFFFFFFF;
    if (p_ffd == NULL) {
        return 10;
    }
    if (p_ffd->p_hint == NULL) {
        return 10;
    }
    if (p_ffd->file_version == 0) {
        return 10;
    }
    if (p_ffd->p_hint->file_version > p_ffd->file_version) {
        return 10;
    }
    p_vol = p_ffd->p_vol;
    if (p_vol->drv_char < 'A' || p_vol->drv_char > 'Z') {
        return 0x23;
    }
    if (file_sector_index >= ((0xFFFFFFFF >> p_vol->bpb.log2_bytes_per_sector) + 1)) {
        return 0x25;
    }

    sector = *p_sector;
    if (sector != 0xFFFFFFFF) {
        cluster = ((sector - p_vol->bpb.first_data_sector) >> p_vol->bpb.log2_sectors_per_cluster) + 2;
    } else {
        err = VFiPFFAT_GetSectorSpecified(p_ffd, file_sector_index, 0, p_sector);
        if (err != 0) {
            return err;
        }
        sector = *p_sector;
        if (sector != 0xFFFFFFFF) {
            cluster = ((sector - p_vol->bpb.first_data_sector) >> p_vol->bpb.log2_sectors_per_cluster) + 2;
        }
    }

    if (sector != 0xFFFFFFFF) {
        *p_num_sector = (p_vol->bpb.first_data_sector + ((cluster - 1) << p_vol->bpb.log2_sectors_per_cluster)) - sector;
    } else {
        *p_num_sector = 0;
    }

    if (*p_sector != 0xFFFFFFFF && (*p_num_sector << p_vol->bpb.log2_bytes_per_sector) < size) {
        err = VFiPFFAT_GetClusterContinuousSectorInChain(p_ffd, cluster, (file_sector_index >> p_vol->bpb.log2_sectors_per_cluster) + 1, size, p_num_sector);
        if (err != 0) {
            return err;
        }
    }
    return 0;
}

s32 VFiPFFAT_MakeRootDir(PF_VOLUME* p_vol) {
    u32 num_success;
    PF_CACHE_PAGE* p_page;
    s32 err;
    u32 sector;
    u32 i;
    enum FatType fat_type = p_vol->bpb.fat_type;
    u32 cluster = p_vol->bpb.root_dir_cluster;
    u32 eoc = lbl_804169B0[fat_type].eoc2;

    switch (fat_type) {
    case FAT_12:
        if (eoc > 0xFFF) err = 0x10;
        else err = VFiPFFAT12_WriteFATEntry(p_vol, cluster, eoc);
        break;
    case FAT_16:
        if (eoc > 0xFFFF) err = 0x10;
        else err = VFiPFFAT16_WriteFATEntry(p_vol, cluster, eoc);
        break;
    case FAT_32:
        if (eoc > 0xFFFFFFF) err = 0x10;
        else err = VFiPFFAT32_WriteFATEntry(p_vol, cluster, eoc);
        break;
    default:
        err = 0xF;
    }
    if (err != 0) return err;
    err = VFiPFCACHE_FlushFATCache(p_vol);
    if (err != 0) return err;
    err = VFiPFCACHE_AllocateDataPage(p_vol, 0xFFFFFFFF, &p_page);
    if (err != 0) return err;
    fn_803A976C(p_page->p_buf, 0, p_vol->bpb.bytes_per_sector);
    sector = p_vol->bpb.first_data_sector + ((p_vol->bpb.root_dir_cluster - 2) << p_vol->bpb.log2_sectors_per_cluster);
    for (i = 0; i < p_vol->bpb.sectors_per_cluster; i++) {
        err = VFiPFSEC_WriteData(p_vol, p_page->p_buf, sector + i, 0, p_vol->bpb.bytes_per_sector, &num_success, 0);
        if (err != 0) {
            VFiPFCACHE_FreeDataPage(p_vol, p_page);
            return err;
        }
        if (num_success != p_vol->bpb.bytes_per_sector) {
            VFiPFCACHE_FreeDataPage(p_vol, p_page);
            return 0x11;
        }
    }
    VFiPFCACHE_FreeDataPage(p_vol, p_page);
    return 0;
}

void VFiPFFAT_InitHint(PF_FAT_HINT* p_hint) {
    p_hint->chain_index = 0;
    p_hint->cluster = 0;
    p_hint->file_version = 0;
}

s32 VFiPFFAT_ReadValueToSpecifiedCluster(PF_VOLUME* p_vol, u32 cluster, u32* value) {
    s32 err;

    switch (p_vol->bpb.fat_type) {
    case FAT_12:
        err = VFiPFFAT12_ReadFATEntry(p_vol, (u16)cluster, value);
        break;
    case FAT_16:
        err = VFiPFFAT16_ReadFATEntry(p_vol, cluster, value);
        break;
    case FAT_32:
        err = VFiPFFAT32_ReadFATEntry(p_vol, cluster, value);
        break;
    default:
        err = 0xF;
        *value = 0xFFFFFFFF;
        break;
    }
    if (err != 0) {
        return err;
    }
    return 0;
}

s32 VFiPFFAT_FinalizeFFD(PF_FFD* p_ffd) {
    if (p_ffd == NULL) {
        return 10;
    }
    p_ffd->cluster_link.buffer = NULL;
    return 0;
}

u32 VFiPFFAT_GetValueOfEOC2(PF_VOLUME* p_vol) {
    return lbl_804169B0[p_vol->bpb.fat_type].eoc2;
}

s32 VFiPFFAT_ResetFFD(PF_FFD* p_ffd, u32* p_start_cluster) {
    if (p_ffd == NULL) {
        return 10;
    }
    if (p_start_cluster == NULL) {
        return 10;
    }
    p_ffd->p_start_cluster = p_start_cluster;
    p_ffd->file_version = 1;
    if (p_ffd->p_hint != NULL) {
        p_ffd->p_hint->file_version = 0;
    }
    p_ffd->last_access_cluster.cluster = 0;
    p_ffd->last_access_cluster.chain_index = 0;
    p_ffd->last_cluster.num_last_cluster = 0;
    p_ffd->last_cluster.max_chain_index = 0;
    p_ffd->cluster_link.buffer = NULL;
    return 0;
}

s32 VFiPFFAT_InitFFD(PF_FFD* p_ffd, PF_FAT_HINT* p_hint, PF_VOLUME* p_vol, u32* p_start_cluster) {
    if (p_ffd == NULL) {
        return 10;
    }
    if (p_start_cluster == NULL) {
        return 10;
    }
    p_ffd->p_hint = p_hint;
    p_ffd->p_vol = p_vol;
    if (p_ffd != NULL && p_start_cluster != NULL) {
        p_ffd->p_start_cluster = p_start_cluster;
        p_ffd->file_version = 1;
        if (p_hint != NULL) {
            p_hint->file_version = 0;
        }
        p_ffd->last_access_cluster.cluster = 0;
        p_ffd->last_access_cluster.chain_index = 0;
        p_ffd->last_cluster.num_last_cluster = 0;
        p_ffd->last_cluster.max_chain_index = 0;
        p_ffd->cluster_link.buffer = NULL;
    }
    return 0;
}

s32 VFiPFFAT16_WriteFATEntry(PF_VOLUME* p_vol, u32 cluster, u32 value) {
    u32 offset;
    u16 entry;
    u16 sector;
    u16 pos;

    if (p_vol == NULL) {
        return 10;
    }
    if (cluster < 2 || cluster >= p_vol->bpb.num_clusters + 2) {
        if (cluster != 0 && cluster != 1) {
            return 14;
        }
    }
    offset = cluster << 1;
    sector = (u16)(p_vol->bpb.active_FAT_sector + (offset >> p_vol->bpb.log2_bytes_per_sector));
    pos = (u16)(offset & (p_vol->bpb.bytes_per_sector - 1));
    entry = ((value << 8) & 0xFF00) | ((value >> 8) & 0xFF);
    return VFiPFSEC_WriteFAT(p_vol, (const u8*)&entry, sector, pos, 2);
}

s32 VFiPFFAT_UpdateAlternateFATEntry(PF_VOLUME* p_vol, u8* p_buf, u32 sector, u32 size) {
    u32 sectors_per_fat;
    u32 diff;
    u32 quot;
    u32 num_success;
    s32 err;
    s32 result;
    u32 fat_index;

    sectors_per_fat = p_vol->bpb.sectors_per_FAT;
    diff = sector - p_vol->bpb.num_reserved_sectors;
    quot = diff / sectors_per_fat;
    result = 0;
    if (quot != 0) {
        sector = diff - quot * sectors_per_fat;
        fat_index = 0;
    } else {
        sector = sector + sectors_per_fat;
        fat_index = 1;
    }
    while (fat_index < p_vol->bpb.num_active_FATs) {
        err = VFiPFDRV_lwrite(p_vol, p_buf, sector, size, &num_success);
        if (err != 0 && result == 0) {
            result = err;
        }
        if (num_success != size) {
            result = 0x11;
        }
        fat_index++;
        sector += p_vol->bpb.sectors_per_FAT;
    }
    return result;
}

// ---------------------------------------------------------------- pf_file

void VFiPFCLUSTER_UpdateLastAccessCluster(PF_FILE* p_file, u32 sector);

u32 VFiPFFILE_Cursor_AdvanceToRead(PF_FILE* p_file, u32 n, u32 sector) {
    u32 new_sector;
    u32 sector_offset;
    u32 ok;
    PF_VOLUME* p_vol;
    PF_VOLUME* p_vol_end;

    ok = 1;
    p_vol = (p_file == NULL) ? NULL : p_file->p_sfd->dir_entry.p_vol;
    sector_offset = p_file->cursor.offset_in_sector + n;
    new_sector = sector + (sector_offset >> p_vol->bpb.log2_bytes_per_sector);
    if (p_file->cursor.position + n < p_file->p_sfd->dir_entry.file_size) {
        if (((p_vol->bpb.bytes_per_sector - 1) & sector_offset) != 0) {
            p_file->cursor.sector = new_sector;
        } else {
            p_file->cursor.sector = 0xFFFFFFFF;
        }
        p_file->cursor.position = p_file->cursor.position + n;
    } else if (p_file->p_sfd->dir_entry.file_size == 0) {
        p_file->cursor.sector = 0xFFFFFFFF;
        p_file->cursor.position = 0;
        if (n != 0) {
            ok = 0;
        }
    } else {
        ok = 0;
        p_file->cursor.sector = 0xFFFFFFFF;
        p_file->cursor.position = p_file->p_sfd->dir_entry.file_size;
    }
    if (p_file == NULL) {
        p_vol_end = NULL;
    } else {
        p_vol_end = p_file->p_sfd->dir_entry.p_vol;
    }
    p_file->cursor.file_sector_index = p_file->cursor.position >> p_vol_end->bpb.log2_bytes_per_sector;
    p_file->cursor.offset_in_sector = p_file->cursor.position & (p_vol_end->bpb.bytes_per_sector - 1);
    VFiPFCLUSTER_UpdateLastAccessCluster(p_file, new_sector);
    return ok;
}

s32 VFiPFFILE_fopen(PF_STR* p_path_str, s32 mode, PF_FILE** pp_file) {
    PF_VOLUME* p_vol;
    s32 err;

    if (pp_file == NULL) {
        lbl_8053B248.last_error = 10;
        return 10;
    }
    *pp_file = NULL;
    if (p_path_str == NULL) {
        lbl_8053B248.last_error = 10;
        return 10;
    }
    p_vol = VFiPFPATH_GetVolumeFromPath(p_path_str);
    if (p_vol == NULL) {
        lbl_8053B248.last_error = 10;
        return 10;
    }
    err = VFiPFVOL_CheckForRead(p_vol);
    if (err != 0) {
        lbl_8053B248.last_error = err;
        p_vol->last_error = err;
        return err;
    }
    if (VFiPFDRV_IsWProtected(p_vol) != 0 && mode != 2) {
        return 11;
    }
    err = VFiPFFILE_p_fopen(p_vol, p_path_str, mode, pp_file);
    if (err != 0) {
        lbl_8053B248.last_error = err;
        p_vol->last_error = err;
        return err;
    }
    p_vol->num_opened_files++;
    return err;
}

s32 VFiPFFILE_Cursor_Read(PF_FILE* p_file, u8* p_buf, u32 size, u32* p_size_read) {
    u32 num_read;
    s32 err;
    PF_VOLUME* p_vol;

    p_file->p_sfd->ffd.p_hint = &p_file->hint;
    *p_size_read = 0;
    if (p_file == NULL) p_vol = NULL; else p_vol = p_file->p_sfd->dir_entry.p_vol;
    err = VFiPFFILE_Cursor_ReadHeadSector(p_vol, p_file, p_buf, size, &num_read);
    *p_size_read += num_read;
    if (err != 0) return err;
    if (num_read >= size) return 0;
    size -= num_read;
    while (size >= p_vol->bpb.bytes_per_sector) {
        err = VFiPFFILE_Cursor_ReadBodySectors(p_vol, p_file, p_buf + *p_size_read, size, &num_read);
        *p_size_read += num_read;
        if (err != 0) return err;
        if (num_read == 0) break;
        size -= num_read;
    }
    err = VFiPFFILE_Cursor_ReadTailSector(p_vol, p_file, p_buf + *p_size_read, size, &num_read);
    *p_size_read += num_read;
    if (err != 0) return err;
    return 0;
}

s32 VFiPFFILE_GetOpenedFile(PF_DIR_ENT* p_ent, PF_DIR_ENT** pp_open_ent) {
    int i;

    if (p_ent == NULL || pp_open_ent == NULL || p_ent->p_vol == NULL) {
        return 10;
    }
    *pp_open_ent = NULL;
    for (i = 0; i < 5; i++) {
        if ((p_ent->p_vol->sfds[i].stat & 1) && (p_ent->p_vol->sfds[i].stat & 2) &&
            p_ent->p_vol == p_ent->p_vol->sfds[i].dir_entry.p_vol &&
            p_ent->entry_sector == p_ent->p_vol->sfds[i].dir_entry.entry_sector &&
            p_ent->entry_offset == p_ent->p_vol->sfds[i].dir_entry.entry_offset) {
            *pp_open_ent = &p_ent->p_vol->sfds[i].dir_entry;
        }
    }
    return 0;
}

void VFiPFFILE_FinalizeAllFiles(PF_VOLUME* p_vol) {
    u16 i;
    PF_SFD* p_sfd;

    for (i = 0; i < 5; i++) {
        p_sfd = &p_vol->sfds[i];
        p_sfd->stat = 0;
        VFiPFFAT_FinalizeFFD(&p_sfd->ffd);
    }
    p_vol->ufds[0].stat &= ~1;
    p_vol->ufds[1].stat &= ~1;
    p_vol->ufds[2].stat &= ~1;
    p_vol->ufds[3].stat &= ~1;
    p_vol->ufds[4].stat &= ~1;
    p_vol->num_opened_files = 0;
}

// ---------------------------------------------------------------- pf_path

s32 VFiPFPATH_SplitPath(PF_STR* p_path, PF_STR* p_dir_path, PF_STR* p_filename) {
    return fn_803BD16C(p_path, p_dir_path, p_filename, 0);
}

s32 fn_803BE3D0(PF_STR* p_path, PF_STR* p_dir_path, PF_STR* p_filename) {
    return fn_803BD16C(p_path, p_dir_path, p_filename, 1);
}

s32 VFiPFPATH_cmpNameUni(const u16* p_name, PF_STR* sPattern) {
    return VFiPFPATH_MatchFileNameWithPattern((const s8*)p_name, sPattern, 1) == 0;
}

s32 VFiPFPATH_cmpTailSFN(const s8* sfn_name, const s8* pattern) {
    return fn_803A98A8((u8*)sfn_name, (u8*)pattern) != 0;
}

void VFiPFPATH_InitTokenOfPath(PF_STR* p_str, s8* path, u32 code_mode) {
    p_str->p_head = path;
    p_str->p_tail = path;
    p_str->code_mode = code_mode;
}

PF_VOLUME* VFiPFPATH_GetVolumeFromPath(PF_STR* p_path) {
    s8 drv_char;

    if (VFiPFSTR_StrLen(p_path) == 0) {
        return NULL;
    }
    if (VFiPFSTR_StrNumChar(p_path, 1) >= 2 && VFiPFSTR_StrNCmp(p_path, lbl_8059FE60, 1, 1, 1) == 0) {
        VFiPFSTR_ToUpperNStr(p_path, 1, &drv_char);
        return VFiPFVOL_GetVolumeFromDrvChar(drv_char);
    }
    return VFiPFVOL_GetCurrentVolume();
}

s32 VFiPFPATH_transformInUnicode(u16* sDestStr, const s8* sSrcStr) {
    s16 oem_width;
    s16 uni_width;
    s32 offset;
    s32 num_char;

    num_char = 0;
    offset = 0;
    while (sSrcStr[offset] != 0) {
        s32 width = lbl_8053B248.codeset.oem2unicode(sSrcStr + offset, sDestStr);
        VFiPFCODE_Divide_Width(width, &oem_width, &uni_width);
        num_char++;
        sDestStr += uni_width >> 1;
        offset += oem_width;
    }
    *sDestStr = 0;
    return num_char;
}

s32 VFiPFPATH_parseShortNameNumeric(s8* buf, u32 num);

static inline int update_suffix(s8 *buf, u32 num, int i, int j,
                                u32 ext, u32 end, u32 *count,
                                u32 *position)
{
    s8 digits[8];
    s8 *p = digits;
    s8 *dst, *src;

    *count = 0;
    while (num != 0) {
        *p++ = num % 10 + '0';
        num /= 10;
        (*count)++;
    }

    if (i + (*count) >= ext) {
        (*position) = i + (*count) + 1;
        if ((*position) > 8)
            (*position) = 8;

        *(buf + (*position) + 4) = buf[end];
        *(buf + (*position) + 3) = *(buf + ext + 2);
        *(buf + (*position) + 2) = *(buf + ext + 1);
        *(buf + (*position) + 1) = buf[ext];
        buf[(*position)] = buf[j];
    }

    if (i + (*count) >= 8)
        i = 7 - (*count);

    buf[i++] = 0x7E;
    src = digits + (*count);
    dst = buf + i;

    while ((*count) > 0) {
        *dst++ = *--src;
        (*count)--;
    }

    return 0;
}

s32 VFiPFPATH_parseShortNameNumeric(s8 *buf, u32 num)
{
    int i, j;
    u32 n, ext, end, pos;

    if (num == 0)
        return 0;

    i = 1;
    while (buf[i] != 0x7E)
        i++;

    j = i + 1;
    while (buf[j] != '.' && buf[j] != 0)
        j++;

    ext = j + 1;
    if (buf[j] == '.' && buf[ext] != 0) {
        end = ext + 1;
        while (buf[end] != 0)
            end++;
    } else {
        end = ext;
    }

    return update_suffix(buf, num, i, j, ext, end, &n, &pos);
}

// ---------------------------------------------------------------- pf_sector

s32 VFiPFSEC_ReadFAT(PF_VOLUME* p_vol, u8* p_buf, u32 sector, u16 offset, u16 size) {
    PF_CACHE_PAGE* p_page;
    s32 err;

    if (p_vol == NULL) return 10;
    if (p_buf == NULL) return 10;
    if ((p_vol->flags & 1) == 0 || VFiPFDRV_IsInserted(p_vol) == 0) {
        return 9;
    }
    err = VFiPFCACHE_ReadFATPage(p_vol, sector, &p_page);
    if (err != 0) return err;
    fn_803A9644(p_buf, p_page->p_buf + offset, size);
    return 0;
}

s32 VFiPFSEC_WriteFAT(PF_VOLUME* p_vol, const u8* p_buf, u32 sector, u16 offset, u16 size) {
    PF_CACHE_PAGE* p_page;
    s32 err;

    if (p_vol == NULL) return 10;
    if (p_buf == NULL) return 10;
    if ((p_vol->flags & 1) == 0 || VFiPFDRV_IsInserted(p_vol) == 0 || VFiPFDRV_IsWProtected(p_vol) != 0) {
        return 9;
    }
    err = VFiPFCACHE_ReadFATPage(p_vol, sector, &p_page);
    if (err != 0) return err;
    fn_803A9644(p_page->p_buf + offset, p_buf, size);
    err = VFiPFCACHE_WriteFATPage(p_vol, p_page);
    if (err != 0) return err;
    return 0;
}

// ---------------------------------------------------------------- pf_volume

static inline s32 pfvol_release_driver(PF_VOLUME* p_vol, u32 mode) {
    s32 err = VFiPFDRV_unmount(p_vol, mode);

    if (err != 0)
        return err;

    if (p_vol->flags & 2)
        p_vol->current_dir[0].stat = 0;

    p_vol->flags &= ~2;
    return 0;
}

s32 VFiPFVOL_p_unmount(PF_VOLUME* p_vol, u32 mode) {
    s32 result = 0;
    s32 err;

    if (!(p_vol->flags & 2))
        return 9;

    VFiPFFILE_FinalizeAllFiles(p_vol);
    VFiPFDIR_FinalizeAllDirs(p_vol);
    err = VFiPFCACHE_FlushAllCaches(p_vol);

    if (err != 0)
        result = err;

    if (err == 0 || (mode & 1)) {
        VFiPFCACHE_FreeAllCaches(p_vol);
        err = pfvol_release_driver(p_vol, mode);
        if (err != 0 && result == 0)
            result = err;
    }

    if (result == 0 || (mode & 1))
        --lbl_8053B248.num_mounted_volumes;

    return result;
}

PF_VOLUME* VFiPFVOL_GetVolumeFromDrvChar(s8 drv_char) {
    s16 index = fn_803A962C(drv_char) - 'A';

    if (index < 0 || index >= 26) {
        return NULL;
    }
    return &lbl_8053B248.volumes[index];
}

void VFiPFVOL_LoadVolumeLabelFromBuf(const u8* buf, PF_VOLUME* p_vol) {
    fn_803A9644(p_vol->label, buf, 11);
    p_vol->label[11] = 0;
}

s32 VFiPFVOL_errnum(void) {
    return lbl_8053B248.last_error;
}

s32 VFiPFVOL_unmount(s8 drv_char, u32 mode) {
    s16 index;
    PF_VOLUME* p_vol;
    s32 result;
    s32 err;

    result = 0;
    index = fn_803A962C(drv_char) - 'A';
    if (index < 0 || index >= 26) {
        p_vol = NULL;
    } else {
        p_vol = &lbl_8053B248.volumes[index];
    }
    if (p_vol == NULL) {
        lbl_8053B248.last_error = 10;
        return 10;
    }
    if (!(p_vol->flags & 1)) {
        lbl_8053B248.last_error = 10;
        return 10;
    }
    if (mode != 0 && !(mode & 1)) {
        lbl_8053B248.last_error = 10;
        p_vol->last_error = 10;
        return 10;
    }
    if (((p_vol->flags >> 1) & 1) & VFiPFDRV_IsDetected(p_vol)) {
        VFiPFFILE_FinalizeAllFiles(p_vol);
        VFiPFDIR_FinalizeAllDirs(p_vol);
        VFiPFCACHE_FreeAllCaches(p_vol);
        if (VFiPFDRV_unmount(p_vol, 1) == 0) {
            if (p_vol->flags & 2) {
                p_vol->current_dir[0].stat = 0;
            }
            p_vol->flags &= ~2;
        }
        lbl_8053B248.num_mounted_volumes--;
    }
    if (!(p_vol->flags & 2)) {
        return 0;
    }
    if (p_vol->num_opened_files != 0 || p_vol->num_opened_directories != 0) {
        result = 1;
        lbl_8053B248.last_error = 0x13;
        p_vol->last_error = 0x13;
        if (!(mode & 1)) {
            return 0x13;
        }
    }
    err = VFiPFVOL_p_unmount(p_vol, mode);
    if (err != 0 && result == 0) {
        result = 1;
        lbl_8053B248.last_error = err;
        p_vol->last_error = err;
        if (!(mode & 1)) {
            result = err;
        }
    }
    return result;
}

// ---------------------------------------------------------------- pf_code (CP932)

s32 VFiPFCODE_CP932_OEMCharWidth(const s8* buf) {
    u16 uc;
    s16 oem_width;
    s16 uni_width;

    VFiPFCODE_Divide_Width(VFiPFCODE_CP932_OEM2Unicode(buf, &uc), &oem_width, &uni_width);
    return oem_width;
}

s32 VFiPFCODE_CP932_isOEMMBchar(u32 value, s32 type)
{
    u8 c = value;

    switch (type) {
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

s32 VFiPFCODE_CP932_UnicodeCharWidth(const u16* buf) {
    s8 oem[4];
    s16 oem_width;
    s16 uni_width;

    VFiPFCODE_Divide_Width(VFiPFCODE_CP932_Unicode2OEM(buf, oem), &oem_width, &uni_width);
    return uni_width;
}

// ---------------------------------------------------------------- pf_apicmn

s32 VFiPFAPI_convertError(s32 err) {
    if (err == 0)
        return 0;
    if (err == -1)
        return -1;
    if (err > 0 && err < 0xA0)
        return lbl_80492730[err];
    if (err == 0x1000)
        return 5;
    return err;
}

s32 VFiPFAPI_convertReturnValue(s32 err) {
    if (err != 0) {
        return -1;
    }
    return 0;
}

void* VFiPFAPI_convertReturnValue2NULL(s32 err, void* p_stream) {
    if (err != 0) {
        return NULL;
    }
    return p_stream;
}

s32 VFiPFAPI_convertReturnValue4unmount(s32 err) {
    if (err == 0) {
        return 0;
    }
    return (err == 1) ? 1 : -1;
}

// ---------------------------------------------------------------- pf_w_* (API wrappers)

int VFipf2_attach(PF_DRV_TBL** drv_tbl) {
    PF_DRV_TBL** p = drv_tbl;
    s32 err;

    if (drv_tbl == NULL || *drv_tbl == NULL) {
        err = 10;
        lbl_8053B248.last_error = err;
    } else {
        for (; *p != NULL; p++) {
            err = VFiPFVOL_attach(*p);
            if (err != 0) {
                return VFiPFAPI_convertReturnValue(err);
            }
        }
    }

    return VFiPFAPI_convertReturnValue(err);
}

s32 VFipf2_chdir(const s8* path) {
    PF_STR str;
    s32 err;

    err = VFiPFSTR_InitStr(&str, path, 1);
    if (err == 0) {
        err = VFiPFDIR_chdir(&str);
    } else {
        lbl_8053B248.last_error = err;
    }
    return VFiPFAPI_convertReturnValue(err);
}

PF_FILE* VFipf2_create(const s8* path) {
    PF_STR str;
    PF_FILE* p_file;
    s32 err;

    err = VFiPFSTR_InitStr(&str, path, 1);
    if (err == 0) {
        err = VFiPFFILE_fopen(&str, 0x19, &p_file);
    } else {
        lbl_8053B248.last_error = err;
    }
    return VFiPFAPI_convertReturnValue2NULL(err, p_file);
}

s32 VFipf2_detach(s8 drv_char) {
    return VFiPFAPI_convertReturnValue(VFiPFVOL_detach(drv_char));
}

int VFipf2_errnum(void) {
    return VFiPFAPI_convertError(VFiPFVOL_errnum());
}

s32 VFipf2_fclose(PF_FILE* p_file) {
    return VFiPFAPI_convertReturnValue(VFiPFFILE_fclose(p_file));
}

s32 VFipf2_finfo(PF_FILE* p_file, PF_INFO* p_info) {
    return VFiPFAPI_convertReturnValue(VFiPFFILE_finfo(p_file, p_info));
}

PF_FILE* VFipf2_fopen(const s8* path, const s8* mode) {
    PF_STR str;
    PF_FILE* p_file;
    s32 open_mode;
    s32 err;

    open_mode = VFiPFAPI_ParseOpenModeString(mode);
    if (open_mode == 0) {
        lbl_8053B248.last_error = 10;
        return NULL;
    }
    err = VFiPFSTR_InitStr(&str, path, 1);
    if (err == 0) {
        err = VFiPFFILE_fopen(&str, open_mode, &p_file);
    } else {
        lbl_8053B248.last_error = err;
    }
    return VFiPFAPI_convertReturnValue2NULL(err, p_file);
}

s32 VFipf2_format(s8 drv_char) {
    return VFiPFAPI_convertReturnValue(fn_803C1F84(drv_char));
}

u32 VFipf2_fread(void* p_buf, u32 size, u32 count, PF_FILE* p_file) {
    u32 count_read;

    VFiPFFILE_fread(p_buf, size, count, p_file, &count_read);
    return count_read;
}

s32 VFipf2_fseek(PF_FILE* p_file, s32 offset, s32 origin) {
    return VFiPFAPI_convertReturnValue(VFiPFFILE_fseek(p_file, offset, origin));
}

s32 VFipf2_fsnext(PF_DTA* p_dta) {
    return VFiPFAPI_convertReturnValue(VFiPFDIR_fsnext(p_dta));
}

s32 fn_803B1D30(PF_STR* p_path, void* p_stat);

s32 fn_803C2CC4(const s8* path, void* p_stat) { // VFipf2_fstat
    PF_STR str;
    s32 err;

    err = VFiPFSTR_InitStr(&str, path, 1);
    if (err == 0) {
        err = fn_803B1D30(&str, p_stat);
    } else {
        lbl_8053B248.last_error = err;
    }
    return VFiPFAPI_convertReturnValue(err);
}

u32 VFipf2_fwrite(void* p_buf, u32 size, u32 count, PF_FILE* p_file) {
    u32 count_written;

    VFiPFFILE_fwrite(p_buf, size, count, p_file, &count_written);
    return count_written;
}

s32 VFipf2_devinf(s8 drv_char, PF_DEV_INF* dev_inf) {
    return VFiPFAPI_convertReturnValue(VFiPFVOL_getdev(drv_char, dev_inf));
}

s32 VFiPFVOL_InitModule(u32 config, void* param);

s32 VFiPFFATFS_initializeFATFS(u32 config, void* param) {
    return VFiPFVOL_InitModule(config, param);
}

s32 VFipf2_init_prfile2(u32 config, void* param) {
    s32 err;

    err = VFiPFFATFS_initializeFATFS(config, param);
    if (err == 0) {
        VFiPFSYS_initializeSYS();
    }
    return VFiPFAPI_convertReturnValue(err);
}

s32 VFipf2_mkdir(const s8* path) {
    PF_STR str;
    s32 err;

    err = VFiPFSTR_InitStr(&str, path, 1);
    if (err == 0) {
        err = fn_803B1E20(&str);
    } else {
        lbl_8053B248.last_error = err;
    }
    return VFiPFAPI_convertReturnValue(err);
}

s32 VFipf2_remove(const s8* path) {
    PF_STR str;
    s32 err;

    err = VFiPFSTR_InitStr(&str, path, 1);
    if (err == 0) {
        err = VFiPFFILE_remove(&str);
    } else {
        lbl_8053B248.last_error = err;
    }
    return VFiPFAPI_convertReturnValue(err);
}

s32 VFipf2_unmount(s8 drv_char, u32 mode) {
    return VFiPFAPI_convertReturnValue4unmount(VFiPFVOL_unmount(drv_char, mode));
}

void VFiPF_InitLockFile(void) {
    lbl_805A1200.flock_count = 0;
}

// ---------------------------------------------------------------- pf_system

s32 VFiPFSYS_GetCurrentContextID(s32* context_id) {
    *context_id = 0;
    return 0;
}

void dTM_GetNowTime(PF_SYS_DATE* sdate, PF_SYS_TIME* stime);

void VFiPFSYS_TimeStamp(PF_SYS_DATE* sdate, PF_SYS_TIME* stime) {
    sdate->sys_year = 2000;
    sdate->sys_month = 4;
    sdate->sys_day = 1;
    stime->sys_hour = 0;
    stime->sys_min = 0;
    stime->sys_sec = 0;
    stime->sys_ms = 1;
    dTM_GetNowTime(sdate, stime);
    stime->sys_ms = 1;
}

// ---------------------------------------------------------------- d_vf

s32 VFIsAvailable(void) {
    return lbl_805A1208 != 0;
}

s32 VF_activate_drive_common(s32 i_handle_idx, const s8* i_sys_file_name_p, void* i_memory_p) {
    s32 err;
    VF_HANDLE_DRIVE* p_drive;
    PF_DRV_TBL* drv_tbl[2];

    err = VFSysCheckExistPrfFile(i_handle_idx, i_sys_file_name_p, i_memory_p);
    if (err != 0) {
        VFSysSetLastError(err);
        return err;
    }

    err = VFSysMountDrv(i_handle_idx, i_sys_file_name_p, i_memory_p);
    if (err != 0) {
        VFSysSetLastError(err);
        return err;
    }

    p_drive = VFSysGetDriveP(i_handle_idx);
    if (p_drive == NULL) {
        VFSysSetLastError(0xB002);
        return 0xB002;
    }

    drv_tbl[0] = &p_drive->pf_drv;
    drv_tbl[1] = NULL;
    if (VFipf2_attach(drv_tbl) != 0) {
        err = VFipf2_errnum();
        VFSysSetLastError(err);
        VFipdm_close_partition(p_drive->pf_part_p);
        return err;
    }

    return 0;
}

s32 VFMountDriveNANDFlash(const s8* i_drive, const s8* i_sys_file_name_p) {
    s32 handle_idx;
    s32 err;
    s32 result;
    s32 arg;

    err = 0xB004;
    handle_idx = -1;

    if (lbl_805A120C != 0) {
        OSLockMutex(&lbl_80563200);
    }

    if (dHash_GetArg(i_drive) == -1) {
        result = VFSysSetDeviceNANDFlash(&handle_idx, NULL, 0);
        err = result;
        if (result == 0) {
            dHash_SetArg(i_drive, (u8)handle_idx);
        }
    }

    VFSysSetLastError(err);

    if (lbl_805A120C != 0) {
        OSUnlockMutex(&lbl_80563200);
    }

    if (err == 0 || err == 0xB004) {
        if (lbl_805A120C != 0) {
            OSLockMutex(&lbl_80563200);
        }

        arg = dHash_GetArg(i_drive);
        fn_803C68D8(arg);
        err = VF_activate_drive_common(arg, i_sys_file_name_p, NULL);

        if (lbl_805A120C != 0) {
            OSUnlockMutex(&lbl_80563200);
        }

        if (err != 0 && err != 0xB005) {
            if (lbl_805A120C != 0) {
                OSLockMutex(&lbl_80563200);
            }

            if (VFSysUnsetDevice(dHash_GetArg(i_drive)) == 0) {
                dHash_DeleteData(i_drive);
            }

            if (lbl_805A120C != 0) {
                OSUnlockMutex(&lbl_80563200);
            }
        }
    }

    return err;
}

s32 VFUnmountDrive(const s8* i_drive) {
    s32 err;

    if (lbl_805A120C != 0) {
        OSLockMutex(&lbl_80563200);
    }
    err = VFSysUnmountDrv(dHash_GetArg(i_drive), 0);
    VFSysSetLastError(err);
    if (lbl_805A120C != 0) {
        OSUnlockMutex(&lbl_80563200);
    }
    if (err == 0) {
        if (lbl_805A120C != 0) {
            OSLockMutex(&lbl_80563200);
        }
        err = VFSysUnsetDevice(dHash_GetArg(i_drive));
        if (err == 0) {
            dHash_DeleteData(i_drive);
        }
        VFSysSetLastError(err);
        if (lbl_805A120C != 0) {
            OSUnlockMutex(&lbl_80563200);
        }
    }
    return err;
}

// HYPOTHESIS: callee prototypes local to this unit (not declared by a shared header).
VFTimeStampCallback VFSysSetTimeStampCallback(VFTimeStampCallback callback);
void VFSysInit(void* i_heap_start_address_p, u32 i_size);
s32 VFipdm_init_diskmanager(u32 a, void* b);

void VFInitEx(void* i_heap_start_address_p, u32 i_size) {
    if (lbl_805A120C == 0) {
        OSInitMutex(&lbl_80563200);
        lbl_805A120C = 1;
    }
    if (lbl_805A120C != 0) {
        OSLockMutex(&lbl_80563200);
    }
    if (lbl_805A1208 == 0) {
        lbl_805A1208 = 1;
        VFSysInit(i_heap_start_address_p, i_size);
        VFipdm_init_diskmanager(0, 0);
        VFipf2_init_prfile2(0, 0);
        dHash_InitHashTable();
        VFSysSetTimeStampCallback(NULL);
    }
    if (lbl_805A120C != 0) {
        OSUnlockMutex(&lbl_80563200);
    }
}

s8* VF_path2handleidx(s32* o_handle_idx_p, const s8* i_path_p) {
    s8 name[8];
    const s8* src = i_path_p;
    s32 len = 0;
    s8* result = (s8*)i_path_p;

    *o_handle_idx_p = -1;
    fn_803A976C(name, 0, 8);
    for (; *src != 0; src++, len++) {
        if (*src == '\\' || *src == '/') {
            break;
        }
        if (*src == ':') {
            *o_handle_idx_p = dHash_GetArg(name);
            if (*o_handle_idx_p == -1 || len > 7) {
                result = NULL;
            } else {
                result = (s8*)(src + 1);
            }
            break;
        }
        if (len < 7) {
            name[len] = *src;
        }
    }
    return result;
}

// HYPOTHESIS: callee prototypes local to this unit (no shared header declares them).
struct PF_FILE* VFSysCreateFile_current(const s8* i_path_p);
struct PF_FILE* VFSysCreateFile(s32 i_handle_idx, const s8* i_path_p);
s32 VFSysDeleteFile_current(const s8* i_path_p);
s32 VFSysDeleteFile(s32 i_handle_idx, const s8* i_path_p);

struct PF_FILE* VFOpenFile(const s8* i_path_p, const s8* i_mode, u32 i_attr) {
    s32 handle_idx = -1;
    s8* path;
    struct PF_FILE* file;

    if (lbl_805A120C != 0) {
        OSLockMutex(&lbl_80563200);
    }

    path = VF_path2handleidx(&handle_idx, i_path_p);
    if (path == NULL) {
        VFSysSetLastError(0xB003);
        if (lbl_805A120C != 0) {
            OSUnlockMutex(&lbl_80563200);
        }
        return NULL;
    }

    if (handle_idx != -1) {
        file = VFSysOpenFile(handle_idx, path, i_mode);
    } else {
        file = VFSysOpenFile_current(path, i_mode);
    }

    if (lbl_805A120C != 0) {
        OSUnlockMutex(&lbl_80563200);
    }
    return file;
}

s32 VFCreateFile(const s8* i_path_p) {
    s32 handle_idx = -1;
    s8* path;
    s32 result;

    if (lbl_805A120C != 0) {
        OSLockMutex(&lbl_80563200);
    }

    path = VF_path2handleidx(&handle_idx, i_path_p);
    if (path == NULL) {
        VFSysSetLastError(0xB003);
        if (lbl_805A120C != 0) {
            OSUnlockMutex(&lbl_80563200);
        }
        return 0;
    }

    if (handle_idx != -1) {
        result = (s32)VFSysCreateFile(handle_idx, path);
    } else {
        result = (s32)VFSysCreateFile_current(path);
    }

    if (lbl_805A120C != 0) {
        OSUnlockMutex(&lbl_80563200);
    }
    return result;
}

s32 VFDeleteFile(const s8* i_path_p) {
    s32 handle_idx = -1;
    s8* path;
    s32 err;

    if (lbl_805A120C != 0) {
        OSLockMutex(&lbl_80563200);
    }

    path = VF_path2handleidx(&handle_idx, i_path_p);
    if (path == NULL) {
        VFSysSetLastError(0xB003);
        if (lbl_805A120C != 0) {
            OSUnlockMutex(&lbl_80563200);
        }
        return 0xB003;
    }

    if (handle_idx != -1) {
        err = VFSysDeleteFile(handle_idx, path);
    } else {
        err = VFSysDeleteFile_current(path);
    }

    VFSysSetLastError(err);
    if (lbl_805A120C != 0) {
        OSUnlockMutex(&lbl_80563200);
    }
    return err;
}

s32 VFSeekFile(void* i_file_p, s32 i_offset, s32 i_origin) {
    s32 err;

    if (lbl_805A120C != 0) {
        OSLockMutex(&lbl_80563200);
    }
    err = VFSysSeekFile(i_file_p, i_offset, i_origin);
    VFSysSetLastError(err);
    if (lbl_805A120C != 0) {
        OSUnlockMutex(&lbl_80563200);
    }
    return err;
}

s32 VFReadFile(void* i_file_p, void* o_buf_p, u32 i_size, u32* o_read_size_p) {
    s32 err;

    if (lbl_805A120C != 0) {
        OSLockMutex(&lbl_80563200);
    }
    err = VFSysReadFile(o_read_size_p, o_buf_p, i_size, i_file_p);
    VFSysSetLastError(err);
    if (lbl_805A120C != 0) {
        OSUnlockMutex(&lbl_80563200);
    }
    return err;
}

s32 VFWriteFile(void* i_file_p, void* i_buf_p, u32 i_size) {
    s32 err;

    if (lbl_805A120C != 0) {
        OSLockMutex(&lbl_80563200);
    }

    err = VFSysWriteFile(i_buf_p, i_size, i_file_p);
    VFSysSetLastError(err);

    if (lbl_805A120C != 0) {
        OSUnlockMutex(&lbl_80563200);
    }

    return err;
}

s32 VFCloseFile(void* i_file_p) {
    s32 err;

    if (lbl_805A120C != 0) {
        OSLockMutex(&lbl_80563200);
    }
    err = VFSysCloseFile(i_file_p);
    VFSysSetLastError(err);
    if (lbl_805A120C != 0) {
        OSUnlockMutex(&lbl_80563200);
    }
    return err;
}

s32 VFGetFileSizeByFd(void* i_file_p) {
    s32 size = -1;
    s32 err = VFSysGetFileSizeByFd(&size, i_file_p);
    if (err != 0) {
        VFSysSetLastError(err);
    }
    return size;
}

s32 VFFileSearchFirst(void* unk0, const s8* i_path_p, u32 unk2) {
    s32 handle_idx = -1;
    s8* path;
    s32 err;

    if (lbl_805A120C != 0) {
        OSLockMutex(&lbl_80563200);
    }

    path = VF_path2handleidx(&handle_idx, i_path_p);
    if (path == NULL) {
        VFSysSetLastError(0xB003);
        if (lbl_805A120C != 0) {
            OSUnlockMutex(&lbl_80563200);
        }
        return 0xB003;
    }

    if (handle_idx != -1) {
        err = fn_803C64EC(unk0, handle_idx, path, unk2);
    } else {
        err = fn_803C63FC(unk0, path, unk2);
    }

    VFSysSetLastError(err);
    if (lbl_805A120C != 0) {
        OSUnlockMutex(&lbl_80563200);
    }

    return err;
}

// HYPOTHESIS: VFGetFileSize; the two file state calls are VFSysGetFileState and its _current form.
s32 fn_803C61FC(void* o_stat_p, const s8* i_path_p);
s32 fn_803C62DC(void* o_stat_p, s32 i_handle_idx, const s8* i_path_p);
s32 fn_803C3CF8(const s8* i_path_p) {
    s32 handle_idx = -1;
    // HYPOTHESIS: the file state block is 0x20 bytes with the size first.
    struct { s32 size; u8 rest[0x18]; } st;
    s8* path;
    s32 err;

    if (lbl_805A120C != 0) {
        OSLockMutex(&lbl_80563200);
    }

    path = VF_path2handleidx(&handle_idx, i_path_p);
    if (path == NULL) {
        VFSysSetLastError(0xB003);
        if (lbl_805A120C != 0) {
            OSUnlockMutex(&lbl_80563200);
        }
        err = 0xB003;
    } else {
        if (handle_idx != -1) {
            err = fn_803C62DC(&st, handle_idx, path);
        } else {
            err = fn_803C61FC(&st, path);
        }
        VFSysSetLastError(err);
        if (lbl_805A120C != 0) {
            OSUnlockMutex(&lbl_80563200);
        }
    }
    if (err == 0) {
        return st.size;
    }
    return -1;
}

s32 VFGetLastDeviceError(const s8* i_drive) {
    s32 err;
    s32 handle_idx;

    if (lbl_805A120C != 0) {
        OSLockMutex(&lbl_80563200);
    }
    if (i_drive != NULL) {
        handle_idx = dHash_GetArg(i_drive);
        err = VFSysGetLastDeviceError(handle_idx);
    } else {
        err = VFSysGetLastDeviceError_current();
    }
    VFSysSetLastError(err);
    if (lbl_805A120C != 0) {
        OSUnlockMutex(&lbl_80563200);
    }
    return err;
}

s32 VFGetDriveFreeSize(const s8* i_drive)
{
    s32 size;

    if (lbl_805A120C != 0) {
        OSLockMutex(&lbl_80563200);
    }
    if (i_drive != NULL) {
        size = VFSysGetDriveFreeSize(dHash_GetArg(i_drive));
    } else {
        VFSysSetLastError(-1);
        size = -1;
    }
    if (lbl_805A120C != 0) {
        OSUnlockMutex(&lbl_80563200);
    }
    return size;
}

s32 VFSetSyncMode(const s8* i_drive, u32 i_mode) {
    s32 err;
    s32 handle_idx;

    if (lbl_805A120C != 0) {
        OSLockMutex(&lbl_80563200);
    }
    if (i_drive != NULL) {
        handle_idx = dHash_GetArg(i_drive);
        err = VFSysSetSyncMode(handle_idx, i_mode);
    } else {
        err = -1;
    }
    VFSysSetLastError(err);
    if (lbl_805A120C != 0) {
        OSUnlockMutex(&lbl_80563200);
    }
    return err;
}

// ---------------------------------------------------------------- d_vf_sys

void VFSysSetLastError(s32 i_err) {
    if (i_err != 0) {
        lbl_805A1214 = i_err;
    }
}

static inline VF_HANDLE_TYPE* vfsys_get_handle(s32 i_idx) {
    if (i_idx < lbl_805A1210 && lbl_805A1220 != NULL) {
        return &lbl_805A1220[i_idx];
    }
    return NULL;
}

VF_HANDLE_TYPE* VFSysGetHandleP(s32 i_idx) {
    if (i_idx < lbl_805A1210 && lbl_805A1220 != NULL) {
        return &lbl_805A1220[i_idx];
    }
    return NULL;
}

VF_HANDLE_DRIVE* VFSysGetDriveP(s32 i_idx) {
    VF_HANDLE_TYPE* handle_p = vfsys_get_handle(i_idx);

    if (handle_p != NULL) {
        return &handle_p->drive;
    }
    return NULL;
}

s32 VFSysHandleP2Idx(VF_HANDLE_TYPE* i_handle_p) {
    VF_HANDLE_TYPE* handle;
    VF_HANDLE_TYPE* end;
    s32 index;
    u32 count;

    if (i_handle_p != NULL) {
        count = lbl_805A1210;
        if (count != 0 && lbl_805A1220 != NULL) {
            handle = lbl_805A1220;
        } else {
            handle = NULL;
        }
        end = handle + count;
        index = 0;
        for (; handle != end; handle++, index++) {
            if (i_handle_p == handle) {
                return index;
            }
        }
    }
    return -1;
}

VF_HANDLE_TYPE* VFSysVol2HandleP(PF_VOLUME* i_vol_p) {
    VF_HANDLE_TYPE* handle;
    VF_HANDLE_TYPE* end;
    u32 count;

    if (i_vol_p != NULL) {
        count = lbl_805A1210;
        if (count != 0 && lbl_805A1220 != NULL) {
            handle = lbl_805A1220;
        } else {
            handle = NULL;
        }
        end = handle + count;
        for (; handle != end; handle++) {
            if (handle->device_p != NULL && handle->drive.pf_drv.drive == i_vol_p->drv_char) {
                return handle;
            }
        }
    }
    return NULL;
}

int VFSysPDMDisk2HandleIdx(const PDM_DISK* i_disk_p) {
    s32 index;
    VF_HANDLE_TYPE* handle;
    u32 count;
    u32 i;

    index = -1;
    if (i_disk_p != NULL) {
        count = lbl_805A1210;
        if (count != 0 && lbl_805A1220 != NULL) {
            handle = lbl_805A1220;
        } else {
            handle = NULL;
        }
        for (i = 0; i < count; handle++, i++) {
            if (i_disk_p == handle->drive.pf_disk_p) {
                index = i;
                break;
            }
        }
    }
    return index;
}

VF_HANDLE_DRIVE* VFSysPDMDisk2DriveP(PDM_DISK* disk_p) {
    VF_HANDLE_TYPE* handle_p;

    if (disk_p != NULL) {
        handle_p = vfsys_get_handle(dCommon_getHandleIdxFromDisk(disk_p));
        if (handle_p != NULL) {
            return &handle_p->drive;
        }
        return NULL;
    }
    return NULL;
}

s32 VFSysSetSyncMode(s32 i_handle_idx, u32 i_mode) {
    VF_HANDLE_TYPE* handle_p;
    VF_HANDLE_DEVICE* device_p;

    handle_p = vfsys_get_handle(i_handle_idx);
    if (handle_p == NULL || handle_p->drive.pf_disk_p == NULL || (device_p = handle_p->device_p) == NULL) {
        return 0xB003;
    }
    if (i_mode <= 1) {
        device_p->sync_mode = i_mode;
        return 0;
    }
    return -1;
}

#include <revolution/DVD/dvdfs.h>

s32 VFSysCheckExistPrfFile_dvd(struct VF_HANDLE_DEVICE* i_device_p, const s8* i_prf_file_name_p, void* i_memory_p, u32 i_handle_idx) {
    s32 i;
    const s8* src = i_prf_file_name_p;
    s8* dst;
    s8 path[255];
    DVDFileInfo info;
    u8 buf[0x20] __attribute__((aligned(32)));
    u32 length;
    s32 err = 0xB001;

    dst = path;
    fn_803A976C(dst, 0, 255);
    i = 0;
    while (i < 255 && *src != 0) {
        if (*src == '/') {
            *dst = 0x5C;
        } else {
            *dst = *src;
        }
        src++;
        dst++;
        i++;
    }
    if (DVDOpen((const char*)path, &info)) {
        s32 read = DVDReadPrio(&info, buf, 0x20, 0, 2);
        length = info.size;
        if (read && dCommon_IsPrfFile(buf) && length == ((buf[8] << 24) | (buf[9] << 16) | (buf[10] << 8) | buf[11])) {
            err = 0;
        } else {
            err = 0xB006;
        }
        DVDClose(&info);
    }
    return err;
}

s32 VFSysCheckExistPrfFile_nandflash(VF_HANDLE_DEVICE* i_device_p, const s8* i_prf_file_name_p, void* i_memory_p, u32 i_handle_idx) {
    return VFSysCheckExistPrfFile_nandflash_sub(i_prf_file_name_p, i_handle_idx);
}

s32 VFSysCheckExistPrfFile_ram(VF_HANDLE_DEVICE* i_device_p, const s8* i_prf_file_name_p, void* i_memory_p, u32 i_handle_idx) {
    return dCommon_IsPrfFile(i_memory_p) ? 0 : 0xB006;
}

void dCommon_initDriveInfo(void) {
    s32 i;
    VF_DRIVE_INFO* p = lbl_80563590;

    for (i = 0; i < 26; i++, p++) {
        p->prf_file_size = 0;
        p->last_device_error = 0;
        p->fat_type = 0;
        p->reserved_sec_num = 0;
        p->root_ent_num = 0;
    }
}

// HYPOTHESIS: dCommon_CopyPrfFileHeader (named by the map): builds a 0x20-byte PrFile header and copies it to dst.
void fn_803C74C8(void* dst, u32 size, u32 version, u8 flag) {
    u8 h[0x20];

    h[7] = version;
    h[11] = size;
    h[0] = 'V';
    h[1] = 'F';
    h[2] = 'F';
    h[3] = ' ';
    *(u16*)(h + 4) = 0xFEFF;
    h[6] = version >> 8;
    h[8] = size >> 24;
    h[9] = size >> 16;
    h[10] = size >> 8;
    *(u16*)(h + 12) = 0x20;
    *(u16*)(h + 14) = 0;
    h[16] = flag;
    h[17] = 0;
    h[18] = 0;
    h[19] = 0;
    h[20] = 0;
    h[21] = 0;
    h[22] = 0;
    h[23] = 0;
    h[24] = 0;
    h[25] = 0;
    h[26] = 0;
    h[27] = 0;
    h[28] = 0;
    h[29] = 0;
    h[30] = 0;
    h[31] = 0;
    fn_803A9644(dst, h, 0x20);
}

s32 VFSysCheckExistPrfFile(s32 i_handle_idx, const s8* i_prf_file_name_p, void* i_memory_p) {
    VF_HANDLE_TYPE* handle_p = vfsys_get_handle(i_handle_idx);
    VF_HANDLE_DEVICE* device_p;
    u32 type;

    if (handle_p == NULL || (device_p = handle_p->device_p) == NULL) {
        return 0xB003;
    }

    type = device_p->type;
    if (type < 4) {
        VFSysCheckExistPrfFileFunc func = lbl_804927D0[type];
        if (func != NULL) {
            return func(device_p, i_prf_file_name_p, i_memory_p, i_handle_idx);
        }
        return 0;
    }

    return -1;
}

s32 VFSysUnmountDrv(s32 i_handle_idx, u32 i_mode) {
    VF_HANDLE_TYPE* handle_p = vfsys_get_handle(i_handle_idx);
    s32 err;

    if (handle_p == NULL || handle_p->device_p == NULL) {
        return 0xB003;
    }
    if (handle_p->drive.pf_disk_p == NULL) {
        return 0xB003;
    }
    if (handle_p != NULL && handle_p->drive.pf_disk_p != NULL) {
        dCommon_setLastDeviceErrorToDisk(handle_p->drive.pf_disk_p, 0);
    }
    err = VFipf2_unmount(handle_p->drive.pf_drv.drive, i_mode);
    if ((i_mode == 0 && err != 0) || (i_mode == 1 && err != 0 && err != 1)) {
        return VFipf2_errnum();
    }
    if (VFipf2_detach(handle_p->drive.pf_drv.drive) != 0) {
        return VFipf2_errnum();
    }
    if (VFipdm_close_partition(handle_p->drive.pf_part_p) == 0) {
        dCommon_setFileSizeToDisk(handle_p->drive.pf_disk_p, 0);
        dCommon_setLastDeviceErrorToDisk(handle_p->drive.pf_disk_p, 0);
        handle_p->drive.pf_drv.p_part = NULL;
        handle_p->drive.pf_drv.cache = NULL;
        handle_p->drive.file_p = NULL;
        fn_803A976C(handle_p->drive.pf_filename, 0, 0xFF);
        return 0;
    }
    return VFipf2_errnum();
}

static inline VF_HANDLE_TYPE* vfsys_first_handle(void) {
    if (lbl_805A1210 != 0 && lbl_805A1220 != NULL) {
        return lbl_805A1220;
    }
    return NULL;
}

static inline VF_HANDLE_TYPE* vfsys_vol_to_handle(PF_VOLUME* p_vol) {
    VF_HANDLE_TYPE* handle_p;
    VF_HANDLE_TYPE* end;

    if (p_vol != NULL) {
        handle_p = vfsys_first_handle();
        end = handle_p + lbl_805A1210;
        for (; handle_p != end; ++handle_p) {
            if (handle_p->device_p != NULL && handle_p->drive.pf_drv.drive == p_vol->drv_char) {
                return handle_p;
            }
        }
    }
    return NULL;
}

static inline VF_HANDLE_TYPE* vfsys_file_to_handle(PF_FILE* i_file_p) {
    VF_HANDLE_TYPE* handle_p = NULL;

    if (i_file_p != NULL) {
        PF_SFD* p_sfd = i_file_p->p_sfd;
        if (p_sfd != NULL) {
            VF_HANDLE_TYPE* end;
            PF_VOLUME* p_vol = p_sfd->ffd.p_vol;
            if (p_vol != NULL) {
                handle_p = vfsys_first_handle();
                end = handle_p + lbl_805A1210;
                for (; handle_p != end; ++handle_p) {
                    if (handle_p->device_p != NULL && handle_p->drive.pf_drv.drive == p_vol->drv_char) {
                        return handle_p;
                    }
                }
            }
            handle_p = NULL;
        }
    }
    return handle_p;
}

s32 VFSysGetFileSizeByFd(s32* o_size_p, PF_FILE* i_file_p) {
    PF_INFO info;
    *o_size_p = -1;
    if (i_file_p != NULL) {
        if (VFipf2_finfo(i_file_p, &info) == 0) {
            *o_size_p = info.file_size;
            return 0;
        }
        return VFipf2_errnum();
    }
    return -1;
}

s32 VFSysSeekFile(PF_FILE* i_file_p, s32 i_offset, s32 i_origin) {
    VF_HANDLE_TYPE* handle_p = vfsys_file_to_handle(i_file_p);

    if (handle_p != NULL && handle_p->drive.pf_disk_p != NULL) {
        dCommon_setLastDeviceErrorToDisk(handle_p->drive.pf_disk_p, 0);
    }
    if (VFipf2_fseek(i_file_p, i_offset, i_origin) == 0) {
        return 0;
    }
    return VFipf2_errnum();
}

s32 VFSysDeleteFile_current(const s8* i_path_p) {
    VF_HANDLE_TYPE* handle_p = vfsys_vol_to_handle(VFiPFVOL_GetCurrentVolume());

    if (handle_p != NULL && handle_p->drive.pf_disk_p != NULL) {
        dCommon_setLastDeviceErrorToDisk(handle_p->drive.pf_disk_p, 0);
    }
    if (VFipf2_remove(i_path_p) == 0) {
        return 0;
    }
    return VFipf2_errnum();
}

s32 VFSysCreateDir_current(const s8* i_path_p) {
    VF_HANDLE_TYPE* handle_p = vfsys_vol_to_handle(VFiPFVOL_GetCurrentVolume());

    if (handle_p != NULL && handle_p->drive.pf_disk_p != NULL) {
        dCommon_setLastDeviceErrorToDisk(handle_p->drive.pf_disk_p, 0);
    }
    if (VFipf2_mkdir(i_path_p) == 0) {
        return 0;
    }
    return VFipf2_errnum();
}

static inline s32 vfsys_select_volume(s32 i_handle_idx) {
    VF_HANDLE_TYPE* handle_p = vfsys_get_handle(i_handle_idx);

    if (handle_p != NULL) {
        PF_VOLUME* p_vol = VFiPFVOL_GetVolumeFromDrvChar(handle_p->drive.pf_drv.drive);
        if (p_vol != NULL) {
            VFiPFVOL_SetCurrentVolume(p_vol);
            return 0;
        }
        {
            s32 err = VFipf2_errnum();
            if (err == 0) {
                return -1;
            }
            return err;
        }
    }
    return 0xB003;
}

static inline void vfsys_clear_dev_err(VF_HANDLE_TYPE* handle_p) {
    if (handle_p != NULL && handle_p->drive.pf_disk_p != NULL) {
        dCommon_setLastDeviceErrorToDisk(handle_p->drive.pf_disk_p, 0);
    }
}

s32 VFSysCreateDir(s32 i_handle_idx, const s8* i_path_p) {
    s32 err = vfsys_select_volume(i_handle_idx);
    VF_HANDLE_TYPE* handle_p;

    if (err == 0) {
        handle_p = vfsys_get_handle(i_handle_idx);
        if (handle_p != NULL) {
            vfsys_clear_dev_err(handle_p);
        }
        if (VFipf2_mkdir(i_path_p) == 0) {
            return 0;
        }
        return VFipf2_errnum();
    }
    return err;
}

s32 VFSysChangeDir(s32 i_handle_idx, const s8* i_path_p) {
    s32 err = vfsys_select_volume(i_handle_idx);
    VF_HANDLE_TYPE* handle_p;

    if (err == 0) {
        handle_p = vfsys_get_handle(i_handle_idx);
        if (handle_p != NULL) {
            vfsys_clear_dev_err(handle_p);
        }
        if (VFipf2_chdir(i_path_p) == 0) {
            return 0;
        }
        return VFipf2_errnum();
    }
    return err;
}

s32 VFSysDeleteFile(s32 i_handle_idx, const s8* i_path_p) {
    s32 err = vfsys_select_volume(i_handle_idx);
    VF_HANDLE_TYPE* handle_p;

    if (err == 0) {
        handle_p = vfsys_get_handle(i_handle_idx);
        if (handle_p != NULL) {
            vfsys_clear_dev_err(handle_p);
        }
        if (VFipf2_remove(i_path_p) == 0) {
            return 0;
        }
        return VFipf2_errnum();
    }
    return err;
}

PF_FILE* VFSysOpenFile(s32 i_handle_idx, const s8* i_path_p, const s8* i_mode) {
    s32 err = vfsys_select_volume(i_handle_idx);
    VF_HANDLE_TYPE* handle_p;
    PF_FILE* file_p;

    if (err == 0) {
        handle_p = vfsys_get_handle(i_handle_idx);
        if (handle_p != NULL) {
            vfsys_clear_dev_err(handle_p);
        }
        file_p = VFipf2_fopen(i_path_p, i_mode);
        if (file_p == NULL) {
            err = VFipf2_errnum();
            if (err != 0) {
                lbl_805A1214 = err;
            }
        }
        return file_p;
    }
    if (err != 0) {
        lbl_805A1214 = err;
    }
    return NULL;
}

s32 VFipf2_fsfirst(const s8* path, u32 attr, PF_DTA* p_dta);

// HYPOTHESIS: VFSysCreateFile; the original also passes a zero second argument to VFipf2_create.
PF_FILE* VFSysCreateFile(s32 i_handle_idx, const s8* i_path_p) {
    s32 err = vfsys_select_volume(i_handle_idx);
    VF_HANDLE_TYPE* handle_p;
    PF_FILE* file_p;

    if (err == 0) {
        handle_p = vfsys_get_handle(i_handle_idx);
        if (handle_p != NULL) {
            vfsys_clear_dev_err(handle_p);
        }
        file_p = ((PF_FILE * (*)(const s8*, int)) VFipf2_create)(i_path_p, 0);
        if (file_p == NULL) {
            err = VFipf2_errnum();
            if (err != 0) {
                lbl_805A1214 = err;
            }
        }
        return file_p;
    }
    if (err != 0) {
        lbl_805A1214 = err;
    }
    return NULL;
}

s32 fn_803C62DC(void* o_stat_p, s32 i_handle_idx, const s8* i_path_p) { // VFSysGetFileState
    s32 err = vfsys_select_volume(i_handle_idx);
    VF_HANDLE_TYPE* handle_p;

    if (err == 0) {
        handle_p = vfsys_get_handle(i_handle_idx);
        if (handle_p != NULL) {
            vfsys_clear_dev_err(handle_p);
        }
        if (fn_803C2CC4(i_path_p, o_stat_p) == 0) {
            return 0;
        }
        return VFipf2_errnum();
    }
    return err;
}

s32 fn_803C64EC(void* unk0, s32 i_handle_idx, s8* path, u32 unk3) { // VFSysFileSearchFirst
    s32 err = vfsys_select_volume(i_handle_idx);
    VF_HANDLE_TYPE* handle_p;

    if (err == 0) {
        handle_p = vfsys_get_handle(i_handle_idx);
        if (handle_p != NULL) {
            vfsys_clear_dev_err(handle_p);
        }
        if (VFipf2_fsfirst(path, unk3, unk0) == 0) {
            return 0;
        }
        return VFipf2_errnum();
    }
    return err;
}

s32 VFSysChangeDir_current(const s8* i_path_p) {
    VF_HANDLE_TYPE* handle_p = vfsys_vol_to_handle(VFiPFVOL_GetCurrentVolume());

    if (handle_p != NULL && handle_p->drive.pf_disk_p != NULL) {
        dCommon_setLastDeviceErrorToDisk(handle_p->drive.pf_disk_p, 0);
    }
    if (VFipf2_chdir(i_path_p) == 0) {
        return 0;
    }
    return VFipf2_errnum();
}

PF_FILE* VFSysOpenFile_current(const s8* i_path_p, const s8* i_mode) {
    VF_HANDLE_TYPE* handle_p = vfsys_vol_to_handle(VFiPFVOL_GetCurrentVolume());
    PF_FILE* file_p;
    s32 err;

    if (handle_p != NULL && handle_p->drive.pf_disk_p != NULL) {
        dCommon_setLastDeviceErrorToDisk(handle_p->drive.pf_disk_p, 0);
    }
    file_p = VFipf2_fopen(i_path_p, i_mode);
    if (file_p == NULL) {
        err = VFipf2_errnum();
        if (err != 0) {
            lbl_805A1214 = err;
        }
    }
    return file_p;
}

// HYPOTHESIS: VFSysCreateFile_current; the original also passes a zero second argument to VFipf2_create.
PF_FILE* VFSysCreateFile_current(const s8* i_path_p) {
    VF_HANDLE_TYPE* handle_p = vfsys_vol_to_handle(VFiPFVOL_GetCurrentVolume());
    PF_FILE* file_p;
    s32 err;

    if (handle_p != NULL && handle_p->drive.pf_disk_p != NULL) {
        dCommon_setLastDeviceErrorToDisk(handle_p->drive.pf_disk_p, 0);
    }
    file_p = ((PF_FILE * (*)(const s8*, int)) VFipf2_create)(i_path_p, 0);
    if (file_p == NULL) {
        err = VFipf2_errnum();
        if (err != 0) {
            lbl_805A1214 = err;
        }
    }
    return file_p;
}

s32 fn_803C61FC(void* o_stat_p, const s8* i_path_p) { // VFSysGetFileState_current
    VF_HANDLE_TYPE* handle_p = vfsys_vol_to_handle(VFiPFVOL_GetCurrentVolume());

    if (handle_p != NULL && handle_p->drive.pf_disk_p != NULL) {
        dCommon_setLastDeviceErrorToDisk(handle_p->drive.pf_disk_p, 0);
    }
    if (fn_803C2CC4(i_path_p, o_stat_p) == 0) {
        return 0;
    }
    return VFipf2_errnum();
}

s32 fn_803C63FC(void* unk0, s8* path, u32 unk2) { // VFSysFileSearchFirst_current
    VF_HANDLE_TYPE* handle_p = vfsys_vol_to_handle(VFiPFVOL_GetCurrentVolume());

    if (handle_p != NULL && handle_p->drive.pf_disk_p != NULL) {
        dCommon_setLastDeviceErrorToDisk(handle_p->drive.pf_disk_p, 0);
    }
    if (VFipf2_fsfirst(path, unk2, unk0) == 0) {
        return 0;
    }
    return VFipf2_errnum();
}

s32 VFSysCloseFile(PF_FILE* i_file_p) {
    VF_HANDLE_TYPE* handle_p = vfsys_file_to_handle(i_file_p);

    if (handle_p != NULL && handle_p->drive.pf_disk_p != NULL) {
        dCommon_setLastDeviceErrorToDisk(handle_p->drive.pf_disk_p, 0);
    }
    if (VFipf2_fclose(i_file_p) == 0) {
        return 0;
    }
    return VFipf2_errnum();
}

s32 VFSysWriteFile(void* i_buf_p, u32 i_size, PF_FILE* i_file_p) {
    VF_HANDLE_TYPE* handle_p = vfsys_file_to_handle(i_file_p);

    if (handle_p != NULL && handle_p->drive.pf_disk_p != NULL) {
        dCommon_setLastDeviceErrorToDisk(handle_p->drive.pf_disk_p, 0);
    }
    if (VFipf2_fwrite(i_buf_p, i_size, 1, i_file_p) == 1) {
        return 0;
    }
    return VFipf2_errnum();
}

s32 VFSysReadFile(u32* o_read_size_p, void* o_buf_p, u32 i_size, PF_FILE* i_file_p) {
    PF_INFO info;
    u32 remain;
    VF_HANDLE_TYPE* handle_p;
    s32 err;

    if (o_read_size_p != NULL) {
        *o_read_size_p = 0;
    }
    if (VFipf2_finfo(i_file_p, &info) == 0) {
        remain = info.file_size - i_file_p->cursor.position;
        if (i_size > remain) {
            fn_803A976C(o_buf_p, 0, i_size);
            i_size = remain;
        }
        handle_p = vfsys_file_to_handle(i_file_p);
        if (handle_p != NULL && handle_p->drive.pf_disk_p != NULL) {
            dCommon_setLastDeviceErrorToDisk(handle_p->drive.pf_disk_p, 0);
        }
        if (i_size != 0) {
            if (VFipf2_fread(o_buf_p, i_size, 1, i_file_p) == 1) {
                if (o_read_size_p != NULL) {
                    *o_read_size_p = i_size;
                }
                err = 0;
            } else {
                err = VFipf2_errnum();
            }
        } else {
            err = 0;
        }
    } else {
        err = VFipf2_errnum();
    }
    return err;
}

s32 VFSysGetLastError(void) {
    return lbl_805A1214;
}

s32 VFSysGetDriveFreeSize(s32 i_handle_idx) {
    PF_DEV_INF info;
    VF_HANDLE_TYPE* handle_p = vfsys_get_handle(i_handle_idx);
    s32 err;

    if (handle_p == NULL || handle_p->device_p == NULL) {
        lbl_805A1214 = 0xB003;
        return -1;
    }
    if (handle_p->drive.pf_disk_p == NULL) {
        lbl_805A1214 = 0xB003;
        return -1;
    }
    if (handle_p != NULL && handle_p->drive.pf_disk_p != NULL) {
        dCommon_setLastDeviceErrorToDisk(handle_p->drive.pf_disk_p, 0);
    }
    if (VFipf2_devinf(handle_p->drive.pf_drv.drive, &info) != 0) {
        err = VFipf2_errnum();
        if (err != 0) lbl_805A1214 = err;
        return -1;
    }
    return info.spc * (info.ecl * info.bps);
}

s32 VFSysGetLastDeviceError_current(void) {
    VF_HANDLE_TYPE* handle_p = vfsys_vol_to_handle(VFiPFVOL_GetCurrentVolume());

    if (handle_p != NULL && handle_p->drive.pf_disk_p != NULL) {
        return dCommon_getLastDeviceErrorFromDisk(handle_p->drive.pf_disk_p);
    }
    return -1;
}

s32 VFSysGetLastDeviceError(s32 i_handle_idx) {
    VF_HANDLE_TYPE* handle_p = vfsys_get_handle(i_handle_idx);

    if (handle_p != NULL) {
        PDM_DISK* p_disk = handle_p->drive.pf_disk_p;
        if (p_disk != NULL)
            return dCommon_getLastDeviceErrorFromDisk(p_disk);
    }
    return -1;
}

VFTimeStampCallback VFSysSetTimeStampCallback(VFTimeStampCallback callback) {
    VFTimeStampCallback old;

    old = lbl_805A1224;
    lbl_805A1224 = callback;
    return old;
}

VFTimeStampCallback VFSysGetTimeStampCallback(void) {
    return lbl_805A1224;
}

// HYPOTHESIS: the time stamp callback fills an OSCalendarTime-shaped block (sec, min, hour, mday, mon, year).
void dTM_GetNowTime(PF_SYS_DATE* sdate, PF_SYS_TIME* stime) {
    struct { s32 sec, min, hour, mday, mon, year; } stamp;
    OSCalendarTime cal;
    VFTimeStampCallback callback = VFSysGetTimeStampCallback();

    sdate->sys_year = 0;
    sdate->sys_month = 0;
    sdate->sys_day = 0;
    stime->sys_hour = 0;
    stime->sys_min = 0;
    stime->sys_sec = 0;
    if (callback != NULL) {
        ((void (*)(void*))callback)(&stamp);
        sdate->sys_year = stamp.year;
        sdate->sys_month = stamp.mon;
        sdate->sys_day = stamp.mday;
        stime->sys_hour = stamp.hour;
        stime->sys_min = stamp.min;
        stime->sys_sec = stamp.sec;
    } else {
        OSTicksToCalendarTime(OSGetTime(), &cal);
        sdate->sys_year = cal.year;
        sdate->sys_month = cal.month + 1;
        sdate->sys_day = cal.mday;
        stime->sys_hour = cal.hour;
        stime->sys_min = cal.min;
        stime->sys_sec = cal.sec;
    }
}

// ---------------------------------------------------------------- d_hash

void dHash_InitHashTable(void) {
    int i;
    dHashEntry* p = lbl_80563360;

    for (i = 0; i < 31; ++i, ++p) {
        p->key = 0;
        p->used = 0;
    }
}

// HYPOTHESIS: _MakeWStr writes the wide name into lbl_80563350, and the byte
// dHash_InitHashTable clears is the value GetArg returns.
int dHash_GetArg(const s8* i_Name) {
    s32 index;

    if (_MakeWStr(i_Name) != 0) {
        index = dHash_SearchHashW((const u16*)lbl_80563350);
        if (index == -1) {
            return -1;
        }
        return lbl_80563360[index].used;
    }
    return -1;
}

int dHash_DeleteData(const s8* i_Name) {
    s32 index;
    dHashEntry* entry;

    if (_MakeWStr(i_Name) != 0) {
        index = dHash_SearchHashW((const u16*)lbl_80563350);
        if (index != -1) {
            if (index >= 0 && index < 31) {
                entry = &lbl_80563360[index];
                entry->key = 0;
                entry->used = 0;
            }
            return 1;
        }
        return 0;
    }
    return 0;
}

// ---------------------------------------------------------------- d_common

// HYPOTHESIS: splits [buf, buf+size) into a head up to the next 32-byte
// boundary, a 32-byte-aligned middle, and a tail of the original misalignment.
// A middle of exactly zero bytes (size 32, unaligned) is reported as empty.
void dCommon_DevideBuff32(void* i_buf, u32 i_size, u32* i_1st_size_p, void* i_2nd_pp, u32* i_2nd_size_p,
                          void* i_3rd_pp, u32* i_3rd_size_p) {
    u32 off;
    u32 head;

    *(void**)i_3rd_pp = NULL;
    *(void**)i_2nd_pp = NULL;
    *i_3rd_size_p = 0;
    *i_2nd_size_p = 0;
    *i_1st_size_p = 0;
    off = (u32)i_buf & 0x1F;
    if (off == 0) {
        *i_1st_size_p = 0;
        *(void**)i_2nd_pp = i_buf;
        *i_2nd_size_p = i_size;
        *(void**)i_3rd_pp = NULL;
        *i_3rd_size_p = 0;
        return;
    }
    head = 0x20 - off;
    {
        u32 mid = i_size;
        mid -= 0x20;

        *i_1st_size_p = head;
        if (mid != 0) {
            *(void**)i_2nd_pp = (u8*)i_buf + head;
            *i_2nd_size_p = mid;
        }
    }
    *i_3rd_size_p = off;
    *(void**)i_3rd_pp = (u8*)i_buf + i_size - off;
}

int dCommon_IsPrfFile(void* i_buff_p) {
    s8 buf[0x20];

    fn_803A9644(buf, i_buff_p, 0x20);
    if (!(buf[0] == 'V' && buf[1] == 'F' && buf[2] == 'F' && buf[3] == ' ')) {
        return 0;
    }
    return 1;
}

s32 dCommon_MakeFsInfoSec(u8* buf, u32 fsInfoSize) {
    PDM_FSINFO fsinfo;

    fn_803A976C(buf, 0, fsInfoSize);
    fsinfo.free_count = 0xFFFFFFFF;
    fsinfo.next_free = 0xFFFFFFFF;
    return VFipdm_bpb_set_fsinfo_information(&fsinfo, buf);
}

u32 dCommon_GetPhysicalOffset(u32 arg0, u32 arg1, u32 arg2) {
    return arg1 * (arg0 - arg2) + 0x20;
}

enum FatType dCommon_GetNiceFatType(u32 *arg0, u32 arg1, u32 arg2, u32 arg3) {
    u32 q;

    q = arg1 / arg2;
    if (q < 0xFF5U) {
        if (arg0 != 0) {
            *arg0 = (arg3 + ((q * 4 - q) >> 1) - 1) / arg3;
        }
        return FAT_12;
    }
    if (q < 0xFFF5U) {
        if (arg0 != 0) {
            *arg0 = (arg3 + (q * 2) - 1) / arg3;
        }
        return FAT_16;
    }
    if (arg0 != 0) {
        *arg0 = (arg3 + (q * 4) - 1) / arg3;
    }
    return FAT_32;
}

u32 dCommon_GetReservedSecFromFatType(enum FatType arg0)
{
    if (arg0 == FAT_32) {
        return 0x20;
    }
    return 1;
}

u32 dCommon_GetRootEntNumFromFatType(enum FatType arg0) {
    return arg0 == FAT_32 ? 0 : 0x80;
}

static inline VF_DRIVE_INFO* dcommon_get_drive_info(PDM_DISK* p_disk) {
    u32 index = VFSysPDMDisk2HandleIdx(p_disk);

    if (index < 26) {
        return &lbl_80563590[index];
    }
    return NULL;
}

int dCommon_ReadDummyBPB(u32 num_blocks, u8* buf, u32 block, u32* p_num_success, PDM_DISK* p_disk, s32* p_err,
                         s32 (*makeBS)(PDM_DISK*, u8*, enum FatType*), s32 (*makeFS)(u8*)) {
    enum FatType type;
    VF_DRIVE_INFO* p_info;
    u32 reserved;

    *p_err = 0;
    p_info = dcommon_get_drive_info(p_disk);
    if (p_info != NULL) {
        reserved = p_info->reserved_sec_num;
    } else {
        reserved = 0;
    }
    if (block < reserved) {
        if (block == 0) {
            s32 err = makeBS(p_disk, buf, &type);
            *p_err = err;
            if (err != 0) {
                return 0;
            }
            *p_num_success = num_blocks;
            *p_err = 0;
            return 0;
        }
        if (block == 1) {
            VF_DRIVE_INFO* p_info2 = dcommon_get_drive_info(p_disk);
            u32 fat_type;

            if (p_info2 != NULL) {
                fat_type = p_info2->fat_type;
            } else {
                fat_type = 1;
            }
            if (fat_type == 2) {
                s32 err = makeFS(buf);
                *p_err = err;
                if (err != 0) {
                    return 0;
                }
                *p_num_success = num_blocks;
                *p_err = 0;
                return 0;
            }
        }
        *p_err = -0x16;
        return 0;
    }
    return 1;
}

// HYPOTHESIS: the original reads the FAT type through an inlined accessor
// (no dCommon_getFatTypeFromDisk is emitted); FAT16 is the default.
static inline u32 dcommon_get_fat_type(PDM_DISK* p_disk) {
    VF_DRIVE_INFO* p_info = dcommon_get_drive_info(p_disk);

    if (p_info != NULL) {
        return p_info->fat_type;
    }
    return 1;
}

int dCommon_WriteDummyBPB(u32 num_blocks, u32 block, u32* p_num_success, PDM_DISK* p_disk, s32* p_err) {
    VF_DRIVE_INFO* p_info;
    u32 reserved;

    *p_err = 0;
    p_info = dcommon_get_drive_info(p_disk);
    if (p_info != NULL) {
        reserved = p_info->reserved_sec_num;
    } else {
        reserved = 0;
    }
    if (block < reserved) {
        if (block == 0 || (block == 1 && dcommon_get_fat_type(p_disk) == 2)) {
            *p_num_success = num_blocks;
            *p_err = 0;
            return 0;
        }
        *p_err = -22;
        return 0;
    }
    return 1;
}

u32 dCommon_getFileSizeFromDisk(PDM_DISK* p_disk) {
    VF_DRIVE_INFO* p_info = dcommon_get_drive_info(p_disk);

    if (p_info != NULL) {
        return p_info->prf_file_size;
    }
    return 0;
}

void dCommon_setFileSizeToDisk(PDM_DISK* p_disk, u32 i_fileSize) {
    VF_DRIVE_INFO* p_info = dcommon_get_drive_info(p_disk);

    if (p_info != NULL) {
        p_info->prf_file_size = i_fileSize;
    }
}

s32 dCommon_getLastDeviceErrorFromDisk(PDM_DISK* p_disk) {
    VF_DRIVE_INFO* p_info = dcommon_get_drive_info(p_disk);

    if (p_info != NULL) {
        return p_info->last_device_error;
    }
    return 0;
}

void dCommon_setLastDeviceErrorToDisk(PDM_DISK* p_disk, s32 i_lastDevErr) {
    VF_DRIVE_INFO* p_info = dcommon_get_drive_info(p_disk);

    if (p_info != NULL) {
        p_info->last_device_error = i_lastDevErr;
    }
}

void dCommon_setLastDeviceErrorToDisk2(s32 i_handleIdx, s32 i_lastDevErr) {
    if (i_handleIdx < 0) {
        return;
    }
    if (i_handleIdx >= 26) {
        return;
    }
    lbl_80563590[i_handleIdx].last_device_error = i_lastDevErr;
}

void dCommon_setFatTypeToDisk(PDM_DISK* p_disk, u32 i_fatType) {
    VF_DRIVE_INFO* p_info = dcommon_get_drive_info(p_disk);

    if (p_info != NULL) {
        p_info->fat_type = i_fatType;
    }
}

u32 dCommon_getResvSecNumFromDisk(PDM_DISK* p_disk) {
    VF_DRIVE_INFO* p_info = dcommon_get_drive_info(p_disk);

    if (p_info != NULL) {
        return p_info->reserved_sec_num;
    }
    return 0;
}

void dCommon_setResvSecNumToDisk(PDM_DISK* p_disk, u32 i_reservedSecNum) {
    VF_DRIVE_INFO* p_info = dcommon_get_drive_info(p_disk);

    if (p_info != NULL) {
        p_info->reserved_sec_num = i_reservedSecNum;
    }
}

u32 dCommon_getRootEntNumFromDisk(PDM_DISK* p_disk) {
    VF_DRIVE_INFO* p_info = dcommon_get_drive_info(p_disk);

    if (p_info != NULL) {
        return p_info->root_ent_num;
    }
    return 0;
}

void dCommon_setRootEntNumToDisk(PDM_DISK* p_disk, u32 i_rootEntNum) {
    VF_DRIVE_INFO* p_info = dcommon_get_drive_info(p_disk);

    if (p_info != NULL) {
        p_info->root_ent_num = i_rootEntNum;
    }
}

s32 dCommon_FlushFromVol(PF_VOLUME* p_vol, int i_setLastDeviceError) {
    s32 err;
    VF_HANDLE_TYPE* handle_p;
    s32 handle_idx;

    handle_p = VFSysVol2HandleP(p_vol);
    err = -1;
    if (handle_p != NULL) {
        VF_HANDLE_DEVICE* device_p = handle_p->device_p;
        if (device_p != NULL) {
            if (device_p->type == 0) {
                handle_idx = VFSysHandleP2Idx(handle_p);
                if (handle_idx != -1) {
                    err = VFi_NandFlushNANDFromHandleIdx(handle_idx, i_setLastDeviceError);
                }
            } else {
                err = 0;
            }
        }
    }
    return err;
}

s32 dCommon_FlushFromHandleIdx(s32 i_handle_idx, int i_setLastDeviceError) {
    s32 err;
    VF_HANDLE_TYPE* handle_p = VFSysGetHandleP(i_handle_idx);

    err = -1;
    if (handle_p != NULL) {
        VF_HANDLE_DEVICE* device_p = handle_p->device_p;
        if (device_p != NULL) {
            if (device_p->type == 0) {
                s32 handle_idx = VFSysHandleP2Idx(handle_p);
                if (handle_idx != -1) {
                    err = VFi_NandFlushNANDFromHandleIdx(handle_idx, i_setLastDeviceError);
                }
            } else {
                err = 0;
            }
        }
    }
    return err;
}

void VFFinalize(void) {
    if (lbl_805A120C != 0) {
        OSLockMutex(&lbl_80563200);
    }
    if (lbl_805A1208 != 0) {
        VFSysFinalize();
        lbl_805A1208 = 0;
    }
    if (lbl_805A120C != 0) {
        OSUnlockMutex(&lbl_80563200);
    }
}

s32 VFFileSearchNext(void* info) {
    s32 err;
    if (lbl_805A120C != 0) {
        OSLockMutex(&lbl_80563200);
    }
    err = VFSysFileSearchNext(info);
    VFSysSetLastError(err);
    if (lbl_805A120C != 0) {
        OSUnlockMutex(&lbl_80563200);
    }
    return err;
}

s32 VFFormatDrive(const s8* i_drive) {
    s32 err;
    if (lbl_805A120C != 0) {
        OSLockMutex(&lbl_80563200);
    }
    err = VFSysFormatDrive(dHash_GetArg(i_drive));
    VFSysSetLastError(err);
    if (lbl_805A120C != 0) {
        OSUnlockMutex(&lbl_80563200);
    }
    return err;
}

s32 VFCreateSystemFileNANDFlash(const s8* i_path, u32 i_size) {
    s32 err;
    if (lbl_805A120C != 0) {
        OSLockMutex(&lbl_80563200);
    }
    err = VFSysCreatePrfFileNANDFlash(i_path, i_size);
    if (lbl_805A120C != 0) {
        OSUnlockMutex(&lbl_80563200);
    }
    return err;
}

s32 VFCreateDir(const s8* i_path) {
    s32 handle_idx = -1;
    s8* path;
    s32 err;

    if (lbl_805A120C != 0) {
        OSLockMutex(&lbl_80563200);
    }

    path = VF_path2handleidx(&handle_idx, i_path);
    if (path == NULL) {
        VFSysSetLastError(0xB003);
        if (lbl_805A120C != 0) {
            OSUnlockMutex(&lbl_80563200);
        }
        return 0xB003;
    }

    if (handle_idx != -1) {
        err = VFSysCreateDir(handle_idx, path);
    } else {
        err = VFSysCreateDir_current(path);
    }

    VFSysSetLastError(err);
    if (lbl_805A120C != 0) {
        OSUnlockMutex(&lbl_80563200);
    }
    return err;
}

s32 VFChangeDir(const s8* i_path) {
    s32 handle_idx = -1;
    s8* path;
    s32 err;

    if (lbl_805A120C != 0) {
        OSLockMutex(&lbl_80563200);
    }

    path = VF_path2handleidx(&handle_idx, i_path);
    if (path == NULL) {
        VFSysSetLastError(0xB003);
        if (lbl_805A120C != 0) {
            OSUnlockMutex(&lbl_80563200);
        }
        return 0xB003;
    }

    if (handle_idx != -1) {
        err = VFSysChangeDir(handle_idx, path);
    } else {
        err = VFSysChangeDir_current(path);
    }

    VFSysSetLastError(err);
    if (lbl_805A120C != 0) {
        OSUnlockMutex(&lbl_80563200);
    }
    return err;
}

void VFipdm_part_set_change_media_state(PDM_DISK* p_disk, u32 event) {
    u16 i;

    for (i = 0; i < 26; i++) {
        if ((lbl_8053A6E0.partition[i].status & 1) && lbl_8053A6E0.partition[i].p_disk == p_disk) {
            if (event == 0) {
                lbl_8053A6E0.partition[i].status &= ~4;
            } else {
                lbl_8053A6E0.partition[i].status |= 4;
            }
            lbl_8053A6E0.partition[i].status |= 8;
        }
    }
}

s32 VFipf2_fsfirst(const s8* path, u32 attr, PF_DTA* p_dta) {
    PF_STR str;
    s32 err;

    err = VFiPFSTR_InitStr(&str, path, 1);
    if (err == 0) {
        err = VFiPFDIR_fsfirst(&str, attr, p_dta);
    } else {
        lbl_8053B248.last_error = err;
    }
    return VFiPFAPI_convertReturnValue(err);
}

// ---------------------------------------------------------------- trivial stubs and thunks

#include <ogws/revolution/VF/pf_filelock.h>

void VFi_NandSetNANDFuncEx(u32 i_handleIdx);

s32 VFGetLastError(void) {
    return VFSysGetLastError();
}

void VFSysSetNandFuncEx(u32 i_handle_idx) {
    VFi_NandSetNANDFuncEx(i_handle_idx);
}

void fn_803C68D8(u32 i_handleIdx) {
    VFi_NandSetNANDFuncNormal(i_handleIdx);
}

u32 VFiPFCODE_CP932_isUnicodeMBchar(u16 uc_src, u32 num) {
    return 0;
}

s32 VFiPF_UnLockFile(PF_FILE* p_file) {
    return -1;
}

void VFiPFSYS_initializeSYS(void) {
}

void dCommon_PrintSignature(struct PR_BINHEADER* i_header_p) {
}

s32 dCommon_getHandleIdxFromDisk(PDM_DISK* p_disk) {
    return VFSysPDMDisk2HandleIdx(p_disk);
}

u16 VFiPFSTR_StrNumChar(PF_STR* p_str, u32 target) {
    const s8* p;
    u16 count;
    if (target == 1) {
        p = p_str->p_head;
    } else {
        p = p_str->p_tail;
    }
    if (p_str->code_mode == 1) {
        count = 0;
        while (*p != 0) {
            if (lbl_8053B248.codeset.is_oem_mb_char(*p, 1)) {
                p++;
            }
            p++;
            count++;
        }
    } else {
        count = 0;
        while (p[0] != 0 || p[1] != 0) {
            p += lbl_8053B248.codeset.unicode_char_width((const u16*)p);
            count++;
        }
    }
    return count;
}

// ---------------------------------------------------------------- pf_cache (adapted from PFCACHE_*)

PF_CACHE_PAGE* VFiPFCACHE_SearchForPage(PF_VOLUME* p_vol, PF_CACHE_PAGE* p_head, u32 sector) {
    PF_CACHE_PAGE* p_page;

    if (sector == 0xFFFFFFFF) {
        return 0;
    }
    if (!(p_head->stat & 1)) {
        return 0;
    }
    if (p_head->sector <= sector && p_head->sector + p_head->size - 1 >= sector) {
        p_head->p_buf = p_head->buffer + ((sector - p_head->sector) << p_vol->bpb.log2_bytes_per_sector);
        return p_head;
    }
    for (p_page = p_head->p_next; p_page != p_head; p_page = p_page->p_next) {
        if (!(p_page->stat & 1)) {
            return 0;
        }
        if (p_page->sector <= sector && p_page->sector + p_page->size - 1 >= sector) {
            p_page->p_buf = p_page->buffer + ((sector - p_page->sector) << p_vol->bpb.log2_bytes_per_sector);
            return p_page;
        }
    }
    return 0;
}

s32 VFiPFCACHE_ReadDataPageAndFlushIfNeeded(PF_VOLUME* p_vol, u32 sector, PF_CACHE_PAGE** pp_page, u32 set_sig) {
    s32 err = VFiPFCACHE_DoReadPageAndFlushIfNeeded(p_vol, &p_vol->cache.p_current_data, sector, pp_page, set_sig);
    if (err != 0) {
        return err;
    }
    return 0;
}

s32 VFiPFCACHE_ReadDataNumSector(PF_VOLUME* p_vol, u8* p_buf, u32 sector, u32 num_sector, u32* p_num_success) {
    s32 err;

    if (sector >= p_vol->bpb.total_sectors) {
        return 0x10;
    }

    err = VFiPFCACHE_DoReadNumSector(p_vol, &p_vol->cache.p_current_data, p_buf, sector, num_sector, p_num_success);
    if (err != 0) {
        return err;
    }
    return 0;
}

s32 VFiPFCACHE_WriteDataNumSectorAndFreeIfNeeded(PF_VOLUME* p_vol, const u8* p_buf, u32 sector, u32 num_sector, u32* p_num_success) {
    s32 err;

    if (sector >= p_vol->bpb.total_sectors) {
        return 0x10;
    }
    err = VFiPFCACHE_DoWriteNumSectorAndFreeIfNeeded(p_vol, &p_vol->cache.p_current_data, p_buf, sector, num_sector, p_num_success);
    if (err != 0) {
        return err;
    }
    return 0;
}

// ---- pf_cache cont.
s32 VFiPFCACHE_InitCaches(PF_VOLUME* p_vol) {
    s32 err;

    p_vol->cache.mode = ((p_vol->cache.mode & ~1) | 2) & ~4;
    fn_803A976C(p_vol->cache.buffers, 0, (p_vol->cache.num_fat_pages + p_vol->cache.num_data_pages) << 9);
    err = VFiPFCACHE_InitPageList(p_vol, &p_vol->cache.p_current_fat, p_vol->cache.pages, p_vol->cache.buffers, p_vol->cache.num_fat_pages, p_vol->cache.fat_buff_size, 1);
    if (err != 0) {
        return err;
    }
    err = VFiPFCACHE_InitPageList(p_vol, &p_vol->cache.p_current_data, p_vol->cache.pages + p_vol->cache.num_fat_pages, p_vol->cache.buffers + p_vol->cache.num_fat_pages, p_vol->cache.num_data_pages, p_vol->cache.data_buff_size, 0);
    if (err != 0) {
        return err;
    }
    return 0;
}

s32 VFiPFCACHE_DoWritePage(PF_VOLUME* p_vol, PF_CACHE_PAGE** pp_head, PF_CACHE_PAGE* p_page, u32 set_sig) {
    PF_CACHE_PAGE* p_head;
    s32 err;
    // HYPOTHESIS: set when a dirty page was written, requests a device flush.
    u32 need_flush = 0;

    p_head = *pp_head;
    if (p_page != p_head) {
        if (p_page == p_head->p_prev) {
            *pp_head = p_page;
        } else {
            p_page->p_prev->p_next = p_page->p_next;
            p_page->p_next->p_prev = p_page->p_prev;
            p_head = *pp_head;
            p_page->p_next = p_head;
            p_page->p_prev = (*pp_head)->p_prev;
            p_head->p_prev = p_page;
            p_page->p_prev->p_next = p_page;
            *pp_head = p_page;
        }
    }
    if (p_page->p_mod_sbuf == 0) {
        p_page->stat |= 2;
        p_page->p_mod_sbuf = p_page->p_buf;
        p_page->p_mod_ebuf = p_page->p_buf;
    } else if (p_page->p_buf < p_page->p_mod_sbuf) {
        p_page->p_mod_sbuf = p_page->p_buf;
    } else if (p_page->p_buf > p_page->p_mod_ebuf) {
        p_page->p_mod_ebuf = p_page->p_buf;
    }
    if (set_sig != 0) {
        p_page->signature = p_vol->cache.signature;
    } else {
        p_page->signature = 0;
    }
    if ((p_vol->cache.mode & 1) || ((p_vol->cache.mode & 4) && (p_page->stat & 4))) {
        if (p_page != 0 && (p_page->stat & 2) && p_page->sector != 0xFFFFFFFF) {
            need_flush = 1;
        }
        err = VFiPFCACHE_FlushPageIfNeeded(p_vol, p_page);
        if (err != 0) {
            return err;
        }
        if (need_flush != 0) {
            if (dCommon_FlushFromVol(p_vol, 1) != 0) {
                return 0x11;
            }
        }
    }
    return 0;
}

s32 VFiPFCACHE_DoWriteSector(PF_VOLUME* p_vol, PF_CACHE_PAGE** pp_head, const u8* p_buf, u32 sector) {
    PF_CACHE_PAGE* p_page;
    u32 num_success;
    s32 err;
    // HYPOTHESIS: set when a dirty page was written, requests a device flush.
    u32 need_flush = 0;

    p_page = VFiPFCACHE_SearchForPage(p_vol, *pp_head, sector);
    if (p_page != 0) {
        fn_803A9644(p_page->p_buf, p_buf, p_vol->bpb.bytes_per_sector);
        if (p_page->p_mod_sbuf == 0) {
            p_page->stat |= 2;
            p_page->p_mod_sbuf = p_page->p_buf;
            p_page->p_mod_ebuf = p_page->p_buf;
        } else if (p_page->p_buf < p_page->p_mod_sbuf) {
            p_page->p_mod_sbuf = p_page->p_buf;
        } else if (p_page->p_buf > p_page->p_mod_ebuf) {
            p_page->p_mod_ebuf = p_page->p_buf;
        }
        if (p_page != 0 && (p_page->stat & 2) && p_page->sector != 0xFFFFFFFF) {
            need_flush = 1;
        }
        err = VFiPFCACHE_FlushPageIfNeeded(p_vol, p_page);
        if (err != 0) {
            return err;
        }
        if (need_flush != 0) {
            if (dCommon_FlushFromVol(p_vol, 1) != 0) {
                return 0x11;
            }
        }
    } else {
        err = VFiPFDRV_lwrite(p_vol, p_buf, sector, 1, &num_success);
        if (err != 0) {
            return err;
        }
        if (num_success != 1) {
            return 0x11;
        }
        if (dCommon_FlushFromVol(p_vol, 1) != 0) {
            return 0x11;
        }
    }
    return 0;
}

// ---------------------------------------------------------------- pf_path (adapted from PFPATH_*)

extern s8 lbl_8059FE88;
extern s8 lbl_8059FE89;

u32 VFiPFPATH_CheckExtShortNameSignature(PF_STR* p_str) {
    s32 result = 0;
    s8 signature[2];
    signature[0] = lbl_8059FE88;
    signature[1] = lbl_8059FE89;
    if (p_str == NULL) {
        return 10;
    }
    if (VFiPFSTR_StrNCmp(p_str, signature, 1, 0, 2) == 0) {
        result = 1;
    }
    return result;
}

u32 VFiPFPATH_GetLengthFromShortname(const s8* sSrc) {
    s32 i;
    u32 length;

    i = 0;
    length = 0;
    while (sSrc[i] != 0) {
        if (i == 8 && (sSrc[i] != ' ' || sSrc[i + 1] != ' ' || sSrc[i + 2] != ' ')) {
            length++;
        }
        if (sSrc[i] != ' ') {
            length++;
        }
        i++;
    }
    return length;
}

s32 VFiPFPATH_AdjustExtShortName(s8* pName, u32 position) {
    u32 i;
    u32 base;
    u32 place;
    u32 rem;

    if (pName == NULL) {
        return 10;
    }
    pName += 7;
    i = 7;
    base = 10;
    place = 1;
    while (i > 1 && position != 0) {
        rem = position % base;
        if (rem != 0) {
            position -= rem;
            *(u8*)pName = (u8)(*(u8*)pName + (s8)(rem / place));
        }
        base *= 10;
        i--;
        pName--;
        place *= 10;
    }
    return 0;
}

u32 VFiPFPATH_GetLengthFromUnicode(const u16* str) {
    s32 index = 0;
    s32 length = 0;
    s16 oem_width;
    s16 uni_width;
    s8 buf[2];
    while (str[index] != 0) {
        s32 width = lbl_8053B248.codeset.unicode2oem(str + index, buf);
        VFiPFCODE_Divide_Width(width, &oem_width, &uni_width);
        length += oem_width;
        index += uni_width >> 1;
    }
    return length;
}

// ---------------------------------------------------------------- pf_file / pf_cluster (adapted)

s32 VFiPFFILE_p_fread(PF_VOLUME* p_vol, u8* p_buf, u32 size, u32 count, PF_FILE* p_file, u32* p_count_read) {
    u32 size_read;
    s32 err;

    size_read = 0;
    *p_count_read = 0;
    if ((p_file->open_mode & 8) == 0 && ((p_file->open_mode & 1) != 0 || (p_file->open_mode & 4) != 0)) {
        return 10;
    }
    if ((p_file->p_sfd->lock.mode & 3) != 0 && p_file->lock_count == 0) {
        return 25;
    }
    if (p_file->cursor.position >= p_file->p_sfd->dir_entry.file_size) {
        return 28;
    }
    err = VFiPFFILE_Cursor_Read(p_file, p_buf, size * count, &size_read);
    *p_count_read = size_read / size;
    if (err != 0) {
        return err;
    }
    return 0;
}

void VFiPFCLUSTER_UpdateLastAccessCluster(PF_FILE* p_file, u32 sector) {
    PF_BPB* bpb;
    u32 index;

    if (p_file->cursor.position == 0) {
        p_file->p_sfd->ffd.last_access_cluster.cluster = 0;
        p_file->p_sfd->ffd.last_access_cluster.chain_index = 0;
        return;
    }
    if (p_file == 0) {
        bpb = 0;
    } else {
        bpb = &p_file->p_sfd->dir_entry.p_vol->bpb;
    }
    if ((p_file->cursor.position & (bpb->bytes_per_sector - 1)) == 0) {
        index = p_file->cursor.file_sector_index;
        if ((index & (bpb->sectors_per_cluster - 1)) == 0) {
            if (index == 0) {
                return;
            }
            index--;
            p_file->p_sfd->ffd.last_access_cluster.chain_index = index >> bpb->log2_sectors_per_cluster;
            p_file->p_sfd->ffd.last_access_cluster.cluster = ((sector - 1 - bpb->first_data_sector) >> bpb->log2_sectors_per_cluster) + 2;
            return;
        }
    }
    p_file->p_sfd->ffd.last_access_cluster.chain_index = p_file->cursor.file_sector_index >> bpb->log2_sectors_per_cluster;
    p_file->p_sfd->ffd.last_access_cluster.cluster = ((sector - bpb->first_data_sector) >> bpb->log2_sectors_per_cluster) + 2;
}

s32 VFiPFDIR_p_fstat(PF_VOLUME* p_vol, PF_STR* p_path, void* p_stat);

s32 fn_803B1D30(PF_STR* p_path, void* p_stat) { // VFiPFDIR_fstat
    PF_VOLUME* p_vol;
    s32 err;

    if (p_path == NULL) {
        lbl_8053B248.last_error = 10;
        return 10;
    }
    p_vol = VFiPFPATH_GetVolumeFromPath(p_path);
    if (p_vol == NULL) {
        lbl_8053B248.last_error = 9;
        return 9;
    }
    if (p_stat == NULL) {
        lbl_8053B248.last_error = 10;
        p_vol->last_error = 10;
        return 10;
    }
    err = VFiPFVOL_CheckForRead(p_vol);
    if (err != 0) {
        lbl_8053B248.last_error = err;
        p_vol->last_error = err;
        return err;
    }
    err = VFiPFDIR_p_fstat(p_vol, p_path, p_stat);
    if (err != 0) {
        lbl_8053B248.last_error = err;
        p_vol->last_error = err;
    }
    return err;
}


s32 VFiPFFAT_GetSectorAllocated(PF_FFD* p_ffd, u32 file_sector_index, u32 size, u32* p_sector, u32* p_num_sector) {
    PF_VOLUME* p_vol;
    s32 err;

    if (p_ffd == NULL) {
        return 10;
    }
    if (p_ffd->p_hint == NULL) {
        return 10;
    }
    if (p_ffd->file_version == 0) {
        return 10;
    }
    if (p_ffd->p_hint->file_version > p_ffd->file_version) {
        return 10;
    }
    p_vol = p_ffd->p_vol;
    if (p_vol->drv_char < 'A' || p_vol->drv_char > 'Z') {
        return 0x23;
    }
    if (file_sector_index >= ((0xFFFFFFFF >> p_vol->bpb.log2_bytes_per_sector) + 1)) {
        return 0x25;
    }
    err = VFiPFFAT_GetSector(p_ffd, file_sector_index, 2, size, p_sector, p_num_sector);
    if (err != 0) {
        return err;
    }
    return 0;
}

void VFipdm_bpb_calculate_common_bpb_fields(PDM_BPB* p_bpb) {
    u16 n;
    u32 root_dir_sectors;
    u32 first_data_sector;
    u32 num_clusters;

    p_bpb->log2_bytes_per_sector = 0;
    for (n = p_bpb->bytes_per_sector; (n >>= 1) != 0;) {
        p_bpb->log2_bytes_per_sector++;
    }
    p_bpb->log2_sectors_per_cluster = 0;
    for (n = p_bpb->sectors_per_cluster; (n >>= 1) != 0;) {
        p_bpb->log2_sectors_per_cluster++;
    }
    root_dir_sectors = (p_bpb->bytes_per_sector + p_bpb->num_root_dir_entries * 32 - 1) >> p_bpb->log2_bytes_per_sector;
    p_bpb->num_root_dir_sectors = root_dir_sectors;
    first_data_sector = (u16)root_dir_sectors + p_bpb->num_reserved_sectors + p_bpb->num_FATs * p_bpb->sectors_per_FAT;
    p_bpb->first_data_sector = first_data_sector;
    num_clusters = (p_bpb->total_sectors - first_data_sector) >> p_bpb->log2_sectors_per_cluster;
    p_bpb->num_clusters = num_clusters;
    if (num_clusters < 0xFF5) {
        p_bpb->fat_type = 0;
    } else if (num_clusters < 0xFFF5) {
        p_bpb->fat_type = 1;
    } else {
        p_bpb->fat_type = 2;
    }
}

s32 VFiPFFAT_ReadFATSector(PF_VOLUME* p_vol, PF_CACHE_PAGE** p_page, u32 cluster) {
    u32 offset;
    s32 err;
    u32 fat_idx;
    u32 sector;
    s32 action;

    switch (p_vol->bpb.fat_type) {
    case 0:
        offset = (u16)(cluster + (cluster >> 1));
        break;
    case 1:
        offset = cluster << 1;
        break;
    case 2:
        offset = cluster << 2;
        break;
    default:
        return 15;
    }
    fat_idx = 1;
    sector = (u16)(p_vol->bpb.active_FAT_sector + (offset >> p_vol->bpb.log2_bytes_per_sector));
    if (p_vol->bpb.ext_flags & 0x80) {
        fat_idx = p_vol->bpb.ext_flags & 7;
    }
    do {
        err = VFiPFCACHE_ReadFATPage(p_vol, sector, p_page);
        if (err == 0x1000 && p_vol->p_callback != NULL) {
            action = ((s32 (*)(s32))p_vol->p_callback)(p_vol->last_driver_error);
            if (action == 0) {
                continue;
            }
            if (action == 1 && p_vol->bpb.num_active_FATs >= 2 && fat_idx < p_vol->bpb.num_active_FATs) {
                sector += p_vol->bpb.sectors_per_FAT;
                fat_idx++;
                continue;
            }
        }
        if (err != 0) {
            return err;
        }
    } while (err != 0);
    return err;
}

s32 VFiPFFAT_GetBeforeChain(PF_VOLUME* p_vol, u32 start_cluster, u32 lActive, u32* p_cluster) {
    u32 next_cluster;
    PF_CACHE_PAGE* p_page;
    u32 limit;
    s32 err;

    if (p_vol == NULL) {
        *p_cluster = 0xFFFFFFFF;
        return 10;
    }
    if (start_cluster < 2 || start_cluster >= p_vol->bpb.num_clusters + 2) {
        *p_cluster = 0xFFFFFFFF;
        return 10;
    }
    limit = lbl_804169B0[p_vol->bpb.fat_type].unk4;
    *p_cluster = 0xFFFFFFFF;
    err = VFiPFFAT_ReadFATSector(p_vol, &p_page, start_cluster);
    if (err != 0) {
        return err;
    }
    p_page->option = 0;
    while (start_cluster < limit) {
        switch (p_vol->bpb.fat_type) {
        case FAT_12:
            err = VFiPFFAT12_ReadFATEntryPage(p_vol, (u16)start_cluster, &next_cluster, &p_page);
            break;
        case FAT_16:
            err = VFiPFFAT16_ReadFATEntryPage(p_vol, start_cluster, &next_cluster, &p_page);
            break;
        case FAT_32:
            err = VFiPFFAT32_ReadFATEntryPage(p_vol, start_cluster, &next_cluster, &p_page);
            break;
        default:
            err = 0xF;
            break;
        }
        if (err != 0) {
            return err;
        }
        if (lActive == next_cluster) {
            *p_cluster = start_cluster;
            return 0;
        }
        start_cluster--;
        if (start_cluster < 2 || start_cluster >= p_vol->bpb.num_clusters + 2) {
            if (start_cluster < limit) {
                *p_cluster = lActive;
                return 0;
            }
        }
    }
    return 0xD;
}

s32 VFiPFFAT_TraceClustersChain(PF_FFD* p_ffd, u32 start_clst, u32 size, u32* p_target_clst, u32* p_next_clst) {
    PF_VOLUME* p_vol;
    u32 last_cluster;
    u32 chain_index;
    u32 num_clusters;
    u32 bytes_per_cluster;
    u32 next_cluster;
    u32 chk_clstlnk;
    PF_CACHE_PAGE* p_page;
    u32 i;
    s32 err;

    *p_target_clst = 0;
    *p_next_clst = 0;
    last_cluster = start_clst;
    if (p_ffd == NULL) {
        return 10;
    }
    if (p_ffd->p_hint == NULL) {
        return 10;
    }
    if (p_ffd->file_version == 0) {
        return 10;
    }
    if (p_ffd->p_hint->file_version > p_ffd->file_version) {
        return 10;
    }
    p_vol = p_ffd->p_vol;
    if (p_vol->drv_char < 'A' || p_vol->drv_char > 'Z') {
        return 0x23;
    }
    if (p_vol == NULL) {
        return 10;
    }
    if (*p_ffd->p_start_cluster < 2) {
        return 0;
    }
    if (start_clst < 2) {
        return 0;
    }
    if (size == 0) {
        return 0;
    }
    if (size == 0xFFFFFFFF) {
        if (p_ffd->last_cluster.num_last_cluster != 0) {
            *p_target_clst = p_ffd->last_cluster.num_last_cluster;
            *p_next_clst = lbl_804169B0[p_vol->bpb.fat_type].eoc2;
            return 0;
        }
    }
    bytes_per_cluster = p_vol->bpb.bytes_per_sector << p_vol->bpb.log2_sectors_per_cluster;
    num_clusters = size / bytes_per_cluster;
    if (size % bytes_per_cluster != 0) {
        num_clusters++;
    }
    if (*p_ffd->p_start_cluster == start_clst) {
        chain_index = 1;
    } else {
        chain_index = p_ffd->p_hint->chain_index + 1;
    }
    next_cluster = 0xFFFFFFFF;
    chk_clstlnk = 1;
    err = VFiPFFAT_ReadFATSector(p_vol, &p_page, start_clst);
    if (err != 0) {
        return err;
    }
    for (i = 0; i < num_clusters; i++) {
        err = VFiPFFAT_ReadClusterPage(p_ffd, start_clst, chain_index, &next_cluster, &chk_clstlnk, &p_page);
        if (err != 0) {
            return err;
        }
        last_cluster = start_clst;
        if (next_cluster == lbl_804169B0[p_vol->bpb.fat_type].eoc2) {
            p_ffd->last_cluster.num_last_cluster = start_clst;
            p_ffd->last_cluster.max_chain_index = chain_index - 1;
            break;
        }
        start_clst = next_cluster;
        chain_index++;
    }
    p_ffd->p_hint->chain_index = chain_index - 1;
    *p_target_clst = last_cluster;
    *p_next_clst = next_cluster;
    return 0;
}

static inline s32 vffat_refresh_fsinfo(PF_VOLUME* p_vol) {
    u16 flag;
    s32 err;

    err = 0;
    if (p_vol->bpb.fat_type != FAT_32) {
        return 0xC;
    }
    flag = p_vol->fsi_flag;
    if ((flag & 2) == 0) {
        return 0x24;
    }
    if ((flag & 4) != 0) {
        err = VFiPFDRV_StoreFreeCountToFSINFO(p_vol);
    }
    return err;
}

s32 VFiPFFAT_CountFreeClusters(PF_VOLUME* p_vol, u32* p_num_free_clusters) {
    s32 err;
    u32 cluster;
    u32 value;
    PF_CACHE_PAGE* p_page;

    if (p_vol == NULL) {
        return 10;
    }
    if (p_vol->bpb.fat_type != FAT_12 && p_vol->bpb.fat_type != FAT_16 && p_vol->bpb.fat_type != FAT_32) {
        return 0xF;
    }
    if ((p_vol->fsi_flag & 4) != 0 && p_vol->num_free_clusters != 0xFFFFFFFF) {
        *p_num_free_clusters = p_vol->num_free_clusters;
        return 0;
    }
    *p_num_free_clusters = 0xFFFFFFFF;
    cluster = 2;
    if ((p_vol->fsi_flag & 1) != 0 && p_vol->bpb.fat_type == FAT_32) {
        err = VFiPFDRV_GetFSINFOInformation(p_vol);
        if (err != 0) {
            return err;
        }
        *p_num_free_clusters = p_vol->num_free_clusters;
    }
    if (*p_num_free_clusters == 0xFFFFFFFF) {
        err = VFiPFFAT_ReadFATSector(p_vol, &p_page, 2);
        if (err != 0) {
            return err;
        }
        p_page->option = 0;
        for (; cluster >= 2 && cluster < p_vol->bpb.num_clusters + 2; cluster++) {
            switch (p_vol->bpb.fat_type) {
            case FAT_12:
                err = VFiPFFAT12_ReadFATEntryPage(p_vol, (u16)cluster, &value, &p_page);
                break;
            case FAT_16:
                err = VFiPFFAT16_ReadFATEntryPage(p_vol, cluster, &value, &p_page);
                break;
            case FAT_32:
                err = VFiPFFAT32_ReadFATEntryPage(p_vol, cluster, &value, &p_page);
                break;
            default:
                err = 0xF;
                break;
            }
            if (err != 0) {
                return err;
            }
            if (value == 0) {
                (*p_num_free_clusters)++;
            }
        }
        p_vol->num_free_clusters = *p_num_free_clusters;
        if (p_vol->bpb.fat_type == FAT_32 && (p_vol->fsi_flag & 2) != 0) {
            if (vffat_refresh_fsinfo(p_vol) != 0) {
                p_vol->num_free_clusters = 0xFFFFFFFF;
                p_vol->fsi_flag &= ~4;
            }
        }
    }
    p_vol->fsi_flag |= 4;
    return 0;
}

s32 VFiPFFAT_GetClusterAllocatedInChain(PF_FFD* p_ffd, u32 initial_cluster, u32 chain_index, u32 size, u32* p_num_clusters) {
    PF_VOLUME* p_vol;
    u32 next_cluster;
    u32 cluster;
    u32 chk_clstlnk;
    s32 err;
    PF_CACHE_PAGE* p_page;
    u32 bytes;

    cluster = initial_cluster;
    p_vol = p_ffd->p_vol;
    next_cluster = 0xFFFFFFFF;
    chk_clstlnk = 1;
    bytes = 0;
    err = VFiPFFAT_ReadFATSector(p_vol, &p_page, cluster);
    if (err != 0) {
        return err;
    }
    while (next_cluster != 0) {
        bytes += p_vol->bpb.bytes_per_sector << p_vol->bpb.log2_sectors_per_cluster;
        (*p_num_clusters)++;
        if (bytes >= size) {
            break;
        }
        err = VFiPFFAT_ReadClusterPage(p_ffd, cluster, chain_index, &next_cluster, &chk_clstlnk, &p_page);
        if (err != 0) {
            return err;
        }
        if (next_cluster == lbl_804169B0[p_vol->bpb.fat_type].eoc2) {
            p_ffd->last_cluster.num_last_cluster = cluster;
            p_ffd->last_cluster.max_chain_index = chain_index - 1;
            break;
        }
        cluster = next_cluster;
        chain_index++;
    }
    return 0;
}

s32 VFiPFFAT_CountAllocatedClusters(PF_FFD* p_ffd, u32 size, u32* p_num_alloc_clusters) {
    PF_VOLUME* p_vol;
    s32 err;
    u32 cluster;
    u32 chain_index;

    *p_num_alloc_clusters = 0;
    if (p_ffd == NULL) {
        return 10;
    }
    if (p_ffd->p_hint == NULL) {
        return 10;
    }
    if (p_ffd->file_version == 0) {
        return 10;
    }
    if (p_ffd->p_hint->file_version > p_ffd->file_version) {
        return 10;
    }
    p_vol = p_ffd->p_vol;
    if (p_vol->drv_char < 'A' || p_vol->drv_char > 'Z') {
        return 0x23;
    }
    if (*p_ffd->p_start_cluster < 2) {
        return 0;
    }
    if (p_ffd->last_cluster.num_last_cluster != 0) {
        *p_num_alloc_clusters = p_ffd->last_cluster.max_chain_index + 1;
        return 0;
    }
    if (p_ffd->p_hint->cluster != 0) {
        if (size <= ((p_ffd->p_hint->chain_index + 1) << (p_vol->bpb.log2_bytes_per_sector + p_vol->bpb.log2_sectors_per_cluster))) {
            *p_num_alloc_clusters = p_ffd->p_hint->chain_index + 1;
            return 0;
        }
        *p_num_alloc_clusters = p_ffd->p_hint->chain_index;
        p_vol = p_ffd->p_vol;
        size -= p_ffd->p_hint->chain_index << (p_vol->bpb.log2_bytes_per_sector + p_vol->bpb.log2_sectors_per_cluster);
        cluster = p_ffd->p_hint->cluster;
        chain_index = p_ffd->p_hint->chain_index + 1;
    } else {
        cluster = *p_ffd->p_start_cluster;
        chain_index = 1;
    }
    err = VFiPFFAT_GetClusterAllocatedInChain(p_ffd, cluster, chain_index, size, p_num_alloc_clusters);
    if (err != 0) {
        return err;
    }
    return 0;
}

s32 VFiPFFAT_GetClusterContinuousSectorInChain(PF_FFD* p_ffd, u32 initial_cluster, u32 chain_index, u32 size, u32* p_num_sector) {
    PF_VOLUME* p_vol;
    u32 next_cluster;
    u32 chk_clstlnk;
    PF_CACHE_PAGE* p_page;
    s32 err;
    u32 cluster;

    cluster = initial_cluster;
    p_vol = p_ffd->p_vol;
    next_cluster = 0xFFFFFFFF;
    chk_clstlnk = 1;
    err = VFiPFFAT_ReadFATSector(p_vol, &p_page, cluster);
    if (err != 0) {
        return err;
    }
    while (next_cluster != 0) {
        err = VFiPFFAT_ReadClusterPage(p_ffd, cluster, chain_index, &next_cluster, &chk_clstlnk, &p_page);
        if (err != 0) {
            return err;
        }
        if (cluster + 1 != next_cluster) {
            if (next_cluster == lbl_804169B0[p_vol->bpb.fat_type].eoc2) {
                p_ffd->last_cluster.num_last_cluster = cluster;
                p_ffd->last_cluster.max_chain_index = chain_index - 1;
            }
            break;
        }
        p_ffd->p_hint->chain_index++;
        p_ffd->p_hint->cluster = next_cluster;
        *p_num_sector += p_vol->bpb.sectors_per_cluster;
        if ((*p_num_sector << p_vol->bpb.log2_bytes_per_sector) >= size) {
            break;
        }
        cluster = next_cluster;
        chain_index++;
    }
    return 0;
}

s32 VFipdm_mbr_get_table(u8* buf, u32 sector, PDM_MBR* p_mbr_tbl) {
    u8* p;
    u32 i;
    u16 v;

    if (buf == NULL || p_mbr_tbl == NULL) {
        return 1;
    }
    if (sector == 0) {
        p_mbr_tbl->epbr_base_sector = 0;
        p_mbr_tbl->current_sector = 0;
    } else {
        if (p_mbr_tbl->epbr_base_sector == 0) {
            p_mbr_tbl->epbr_base_sector = sector;
        }
        p_mbr_tbl->current_sector = sector;
    }
    p = buf + 0x1BE;
    for (i = 0; i < 4; i++) {
        p_mbr_tbl->partition_table[i].boot_flag = p[0];
        p_mbr_tbl->partition_table[i].partition_type = p[4];
        p_mbr_tbl->partition_table[i].s_head = p[1];
        v = p[2] | (p[3] << 8);
        p_mbr_tbl->partition_table[i].s_cylinder = ((v << 2) & 0x300) + (v >> 8);
        p_mbr_tbl->partition_table[i].s_sector = v & 0x3F;
        p_mbr_tbl->partition_table[i].e_head = p[5] | (p[6] << 8);
        v = p[6] | (p[7] << 8);
        p_mbr_tbl->partition_table[i].e_cylinder = ((v << 2) & 0x300) + (v >> 8);
        p_mbr_tbl->partition_table[i].e_sector = v & 0x3F;
        p_mbr_tbl->partition_table[i].lba_start_sector = p[8] | (p[9] << 8) | (p[10] << 16) | (p[11] << 24);
        p_mbr_tbl->partition_table[i].lba_num_sectors = p[12] | (p[13] << 8) | (p[14] << 16) | (p[15] << 24);
        p += 16;
    }
    return 0;
}

#include <revolution/MEM/mem_expHeap.h>

void VFSys_create_cache(VF_HANDLE_TYPE* i_handle_p, u32 i_pages, struct MEMiHeapHead* i_heap_handle) {
    VF_HANDLE_CACHE* cache;
    u32 page_size;
    u32 buffer_size;
    u32 total_pages;

    if (i_handle_p == NULL) return;
    cache = &i_handle_p->drive.cache;
    total_pages = i_pages * 2;
    if (cache->pf_cache_page_p == NULL && cache->pf_cache_buf_p == NULL) {
        buffer_size = total_pages * 512;
        page_size = total_pages * sizeof(PF_CACHE_PAGE);
        cache->pf_cache_page_p = i_heap_handle == NULL ? NULL : MEMAllocFromExpHeapEx(i_heap_handle, page_size, 4);
        cache->pf_cache_buf_p = i_heap_handle == NULL ? NULL : MEMAllocFromExpHeapEx(i_heap_handle, buffer_size, 4);
        if (cache->pf_cache_page_p != NULL && cache->pf_cache_buf_p != NULL) {
            fn_803A976C(cache->pf_cache_page_p, 0, page_size);
            fn_803A976C(cache->pf_cache_buf_p, 0, buffer_size);
            cache->heap_handle = i_heap_handle;
            cache->cache_pages = i_pages;
        } else if (cache != NULL) {
            if (cache->pf_cache_page_p != NULL && cache->heap_handle != NULL) {
                MEMFreeToExpHeap(cache->heap_handle, cache->pf_cache_page_p);
            }
            if (cache->pf_cache_buf_p != NULL && cache->heap_handle != NULL) {
                MEMFreeToExpHeap(cache->heap_handle, cache->pf_cache_buf_p);
            }
            if (cache != NULL) {
                cache->heap_handle = NULL;
                cache->cache_pages = 0;
                cache->pf_cache_page_p = NULL;
                cache->pf_cache_buf_p = NULL;
            }
        }
    }
}

s32 fn_803AB8C4(PDM_INIT_DISK* p_init_disk_tbl, PDM_DISK** pp_disk) { // VFipdm_disk_open_disk
    PDM_DISK* p_disk;
    u16 disk_no;
    u16 handle_no;
    s32 err;

    if (pp_disk == NULL || p_init_disk_tbl == NULL || p_init_disk_tbl->p_func == NULL) return 1;
    err = VFipdm_disk_get_handle(p_init_disk_tbl, &p_disk, &disk_no, &handle_no);
    if (err != 0) return err;
    p_disk->signature++;
    p_disk->signature &= 0xFFFF;
    p_disk->open_disk_cnt++;
    lbl_8053A6E0.disk_handle[handle_no].signature = p_disk->signature;
    lbl_8053A6E0.disk_handle[handle_no].handle = p_disk;
    *pp_disk = (PDM_DISK*)((disk_no & 0xFF) | 0x300 | (lbl_8053A6E0.disk[disk_no].signature << 16));
    if (p_disk->open_disk_cnt == 1) {
        err = p_disk->disk_tbl.p_func->init(*pp_disk);
        if (err != 0) {
            if (p_disk->open_disk_cnt == 1) {
                p_disk->status &= ~1;
                lbl_8053A6E0.num_allocated_disk--;
            }
            p_disk->open_disk_cnt--;
            lbl_8053A6E0.disk_handle[handle_no].handle = NULL;
            return 21;
        }
    }
    return 0;
}

s32 VFSysUnsetDevice(s32 i_handle_idx) {
    VF_HANDLE_TYPE* handle = vfsys_get_handle(i_handle_idx);
    VF_HANDLE_CACHE* cache;
    struct MEMiHeapHead* heap;

    if (handle == NULL || handle->drive.pf_disk_p == NULL) return 0xB003;
    if (handle != NULL && handle->drive.pf_disk_p != NULL) {
        dCommon_setLastDeviceErrorToDisk(handle->drive.pf_disk_p, 0);
    }
    if (VFipdm_close_disk(handle->drive.pf_disk_p) != 0) return VFipf2_errnum();
    if (handle != NULL) {
        cache = &handle->drive.cache;
        heap = cache->heap_handle;
        if (cache != NULL) {
            if (cache->pf_cache_page_p != NULL && heap != NULL) MEMFreeToExpHeap(heap, cache->pf_cache_page_p);
            if (cache->pf_cache_buf_p != NULL && cache->heap_handle != NULL) MEMFreeToExpHeap(cache->heap_handle, cache->pf_cache_buf_p);
            if (cache != NULL) {
                cache->heap_handle = NULL;
                cache->cache_pages = 0;
                cache->pf_cache_page_p = NULL;
                cache->pf_cache_buf_p = NULL;
            }
        }
        // HYPOTHESIS: this global holds the default VF cache heap.
        if (heap != *(struct MEMiHeapHead**)lbl_805A1218 && heap != NULL) MEMDestroyExpHeap(heap);
    }
    fn_803A976C(handle, 0, sizeof(*handle));
    if (&handle->drive.cache != NULL) {
        handle->drive.cache.heap_handle = NULL;
        handle->drive.cache.cache_pages = 0;
        handle->drive.cache.pf_cache_page_p = NULL;
        handle->drive.cache.pf_cache_buf_p = NULL;
    }
    return 0;
}

s32 VFipdm_part_get_start_sector(PDM_PARTITION* p_part) {
    PDM_MBR mbr;
    PDM_DISK_INFO info;
    u16 part_id = p_part->part_id;
    u16 i;
    s32 err;

    err = VFipdm_mbr_get_mbr_part_table(p_part->p_disk, &mbr);
    if (err != 0 && err != 5) return err;
    if (err != 5) {
        if (part_id < 4) {
            if (mbr.partition_table[part_id].lba_start_sector == 0) return 7;
            p_part->start_sector = mbr.partition_table[part_id].lba_start_sector;
            p_part->total_sector = mbr.partition_table[part_id].lba_num_sectors;
            p_part->partition_type = mbr.partition_table[part_id].partition_type;
            p_part->mbr_sector = mbr.current_sector;
        } else {
            for (i = 4; ; i++) {
                err = VFipdm_mbr_get_epbr_part_table(p_part->p_disk, &mbr);
                if (err != 0 && err != 6) return err;
                if (err != 6) {
                    if (part_id == i) {
                        p_part->start_sector = mbr.partition_table[0].lba_start_sector;
                        p_part->total_sector = mbr.partition_table[0].lba_num_sectors;
                        p_part->partition_type = mbr.partition_table[0].partition_type;
                        p_part->mbr_sector = mbr.current_sector;
                        break;
                    }
                } else return 7;
            }
        }
    } else {
        if (part_id >= 1) return 7;
        err = VFipdm_disk_get_media_information(p_part->p_disk, &info);
        if (err != 0) return err;
        p_part->start_sector = 0;
        p_part->total_sector = info.total_sectors;
        p_part->partition_type = 0;
        p_part->mbr_sector = 0;
    }
    return 0;
}

void VFiPFENT_storeLFNEntryFieldsToBuf(u8* buf, PF_DIR_ENT* p_ent, u8 ord, u8 sum, u32 is_last) {
    u16* name;
    u16* end;
    u16* pos;

    if (is_last) buf[0] = ord | 0x40;
    else buf[0] = ord;
    buf[11] = 15;
    buf[13] = sum;
    buf[12] = 0;
    *(u16*)(buf + 26) = 0;
    name = p_ent->long_name + (u32)(ord - 1) * 13;
    if (is_last) {
        end = name + 13;
        for (pos = name; pos < end; pos++) {
            if (*pos == 0) {
                pos++;
                break;
            }
        }
        for (; pos < end; pos++) *pos = 0xFFFF;
    }
    fn_803A9644(buf + 1, name, 10);
    fn_803A9644(buf + 14, name + 5, 12);
    fn_803A9644(buf + 28, name + 11, 4);
    fn_803A9958(buf + 1, 10);
    fn_803A9958(buf + 14, 12);
    fn_803A9958(buf + 28, 4);
}

s32 VFSysCheckExistPrfFile_nandflash_sub(const s8* i_prf_file_name_p, u32 i_handle_idx) {
    s32 i;
    s8* dst;
    const s8* src = i_prf_file_name_p;
    NANDFileInfo info;
    s8 path[255];
    u8 buf[32] __attribute__((aligned(32)));
    u32 length;
    s32 length_err;
    s32 err = 0xB001;

    dst = path;
    fn_803A976C(dst, 0, 255);
    i = 0;
    while (i < 255 && *src != 0) {
        if (*src == 0x5C) *dst = '/';
        else *dst = *src;
        src++;
        dst++;
        i++;
    }
    if (VFi_NandOpenSp((const char*)path, &info, 1, i_handle_idx) == 0) {
        length = 0;
        length_err = VFi_NandGetLength(&info, &length);
        if (VFi_NandRead(&info, buf, 32) >= 0 && length_err >= 0 && dCommon_IsPrfFile(buf) &&
            length == ((buf[8] << 24) | (buf[9] << 16) | (buf[10] << 8) | buf[11])) {
            err = 0;
        } else err = 0xB006;
        VFi_NandClose(&info);
    }
    return err;
}

s32 VFipdm_part_get_handle(PDM_DISK* p_disk, u16 part_id, PDM_PARTITION** pp_part, u16* p_part_no, u16* p_handle_no) {
    u16 handle_no;
    PDM_PARTITION* free_part;
    u16 free_no;
    u16 part_no;

    *pp_part = NULL;
    free_part = NULL;
    free_no = 0;
    for (handle_no = 0; handle_no < 26; handle_no++) {
        if (lbl_8053A6E0.partition_handle[handle_no].handle == NULL) break;
    }
    if (handle_no >= 26) return 10;
    *p_handle_no = handle_no;
    for (part_no = 0; part_no < 26; part_no++) {
        if (!(lbl_8053A6E0.partition[part_no].status & 1)) {
            if (free_part == NULL) {
                free_part = &lbl_8053A6E0.partition[part_no];
                free_no = part_no;
            }
        } else if (part_id == lbl_8053A6E0.partition[part_no].part_id && lbl_8053A6E0.partition[part_no].p_disk == p_disk) {
            *pp_part = &lbl_8053A6E0.partition[part_no];
            *p_part_no = part_no;
            return 0;
        }
    }
    if (free_part == NULL) return 10;
    free_part->status |= 1;
    lbl_8053A6E0.num_partition++;
    free_part->p_disk = p_disk;
    free_part->part_id = part_id;
    *pp_part = free_part;
    *p_part_no = free_no;
    return 0;
}

s32 VFiPFENT_RecalcEntryIterator(PF_ENT_ITER* p_iter, u32 may_allocate) {
    PF_CACHE_PAGE* page;
    u32 num_success;
    u32 old_sector;
    u32 sector;
    u32 i;
    s32 err;

    sector = p_iter->index >> p_iter->log2_entries_per_sector;
    old_sector = p_iter->file_sector_index;
    if (sector != old_sector) {
        p_iter->file_sector_index = sector;
        err = VFiPFFAT_GetSectorSpecified(&p_iter->ffd, sector, 0, &p_iter->sector);
        if (err != 0) return err;
        if (p_iter->sector == 0xFFFFFFFF && may_allocate) {
            err = VFiPFFAT_GetSectorSpecified(&p_iter->ffd, sector, 1, &p_iter->sector);
            if (err != 0) return err;
            if (p_iter->sector != 0xFFFFFFFF && old_sector != 0xFFFFFFFF) {
                err = VFiPFCACHE_AllocateDataPage(p_iter->p_vol, 0xFFFFFFFF, &page);
                if (err != 0) return err;
                fn_803A976C(page->p_buf, 0, p_iter->p_vol->bpb.bytes_per_sector);
                for (i = 0; i < p_iter->p_vol->bpb.sectors_per_cluster; i++) {
                    err = VFiPFSEC_WriteData(p_iter->p_vol, page->p_buf, p_iter->sector + i, 0, p_iter->p_vol->bpb.bytes_per_sector, &num_success, 0);
                    if (err != 0) {
                        VFiPFCACHE_FreeDataPage(p_iter->p_vol, page);
                        return err;
                    }
                    if (num_success != p_iter->p_vol->bpb.bytes_per_sector) {
                        VFiPFCACHE_FreeDataPage(p_iter->p_vol, page);
                        return 17;
                    }
                }
                VFiPFCACHE_FreeDataPage(p_iter->p_vol, page);
            }
        }
    }
    p_iter->offset = (p_iter->index & p_iter->offset_mask) << 5;
    return 0;
}

s32 VFMountDriveNANDFlashEx(const s8* i_drive, const s8* i_sys_file_name_p) {
    s32 handle_idx;
    s32 err;
    s32 result;
    s32 arg;

    err = 0xB004;
    handle_idx = -1;

    if (lbl_805A120C != 0) {
        OSLockMutex(&lbl_80563200);
    }

    if (dHash_GetArg(i_drive) == -1) {
        result = VFSysSetDeviceNANDFlash(&handle_idx, NULL, 0);
        err = result;
        if (result == 0) {
            dHash_SetArg(i_drive, (u8)handle_idx);
        }
    }

    VFSysSetLastError(err);

    if (lbl_805A120C != 0) {
        OSUnlockMutex(&lbl_80563200);
    }

    if (err == 0 || err == 0xB004) {
        if (lbl_805A120C != 0) {
            OSLockMutex(&lbl_80563200);
        }

        arg = dHash_GetArg(i_drive);
        VFSysSetNandFuncEx(arg);
        err = VF_activate_drive_common(arg, i_sys_file_name_p, NULL);

        if (lbl_805A120C != 0) {
            OSUnlockMutex(&lbl_80563200);
        }

        if (err != 0 && err != 0xB005) {
            if (lbl_805A120C != 0) {
                OSLockMutex(&lbl_80563200);
            }

            if (VFSysUnsetDevice(dHash_GetArg(i_drive)) == 0) {
                dHash_DeleteData(i_drive);
            }

            if (lbl_805A120C != 0) {
                OSUnlockMutex(&lbl_80563200);
            }
        }
    }

    return err;
}

void VFi_InitSDWrok(void);

void VFSysInit(void* i_heap_start_address_p, u32 i_size) {
    VF_HANDLE_TYPE* end;
    VF_HANDLE_TYPE* handle;
    void** work;
    u32 count;
    u32 i;
    u32 capacity = i_size >> 14;

    // HYPOTHESIS: these VF globals contain the default heap and work-array state.
    if (i_heap_start_address_p != NULL && i_size != 0 && *(struct MEMiHeapHead**)lbl_805A1218 == NULL) {
        *(struct MEMiHeapHead**)lbl_805A1218 = MEMCreateExpHeapEx(i_heap_start_address_p, i_size, 0);
    }
    {
        u32 limit = 26;
        if (capacity <= 26) limit = capacity;
        lbl_805A1210 = limit;
    }
    if (lbl_805A1220 == NULL) {
        lbl_805A1220 = *(struct MEMiHeapHead**)lbl_805A1218 == NULL ? NULL :
            MEMAllocFromExpHeapEx(*(struct MEMiHeapHead**)lbl_805A1218, lbl_805A1210 * sizeof(VF_HANDLE_TYPE), 32);
    }
    {
        u32 num_handles = lbl_805A1210;
        if (num_handles != 0 && lbl_805A1220 != NULL) handle = lbl_805A1220;
        else handle = NULL;
        end = handle + num_handles;
        for (; handle != end; handle++) {
            fn_803A976C(handle, 0, sizeof(*handle));
            if (&handle->drive.cache != NULL) {
                handle->drive.cache.heap_handle = NULL;
                handle->drive.cache.cache_pages = 0;
                handle->drive.cache.pf_cache_page_p = NULL;
                handle->drive.cache.pf_cache_buf_p = NULL;
            }
        }
    }
    count = lbl_805A1210;
    if (*(u32*)lbl_805A121C == 0) {
        work = (void**)lbl_80563218;
        for (i = 0; i < count; i++, work++) {
            *work = *(struct MEMiHeapHead**)lbl_805A1218 == NULL ? NULL :
                MEMAllocFromExpHeapEx(*(struct MEMiHeapHead**)lbl_805A1218, 0xA0, 32);
        }
        *(u32*)lbl_805A121C = 1;
    }
    dCommon_initDriveInfo();
    lbl_805A1214 = 0;
    lbl_805A122C = 2;
    lbl_805A1228 = 8;
    VFi_InitSDWrok();
}

void VFSysFinalize(void) {
    u32 i;
    u32 count;
    void** work;
    struct MEMiHeapHead* heap;
    VF_HANDLE_CACHE* cache;
    VF_HANDLE_TYPE* end;
    VF_HANDLE_TYPE* handle;

    if (lbl_805A1220 != NULL) {
        u32 num_handles = lbl_805A1210;
        if (num_handles != 0 && lbl_805A1220 != NULL) handle = lbl_805A1220;
        else handle = NULL;
        if (handle != NULL) {
            end = handle + num_handles;
            for (; handle != end; handle++) {
                if (handle != NULL) {
                    cache = &handle->drive.cache;
                    heap = cache->heap_handle;
                    if (cache != NULL) {
                        if (cache->pf_cache_page_p != NULL && heap != NULL) MEMFreeToExpHeap(heap, cache->pf_cache_page_p);
                        if (cache->pf_cache_buf_p != NULL && cache->heap_handle != NULL) MEMFreeToExpHeap(cache->heap_handle, cache->pf_cache_buf_p);
                        if (cache != NULL) {
                            cache->heap_handle = NULL;
                            cache->cache_pages = 0;
                            cache->pf_cache_page_p = NULL;
                            cache->pf_cache_buf_p = NULL;
                        }
                    }
                    if (heap != *(struct MEMiHeapHead**)lbl_805A1218 && heap != NULL) MEMDestroyExpHeap(heap);
                }
            }
        }
        {
            void* handles = lbl_805A1220;
            if (*(struct MEMiHeapHead**)lbl_805A1218 != NULL) MEMFreeToExpHeap(*(struct MEMiHeapHead**)lbl_805A1218, handles);
        }
        lbl_805A1220 = NULL;
    }
    count = lbl_805A1210;
    if (*(u32*)lbl_805A121C != 0) {
        work = (void**)lbl_80563218;
        for (i = 0; i < count; i++, work++) {
            if (*work != NULL) {
                if (*(struct MEMiHeapHead**)lbl_805A1218 != NULL) MEMFreeToExpHeap(*(struct MEMiHeapHead**)lbl_805A1218, *work);
                *work = NULL;
            }
        }
        *(u32*)lbl_805A121C = 0;
    }
    if (*(struct MEMiHeapHead**)lbl_805A1218 != NULL) {
        MEMDestroyExpHeap(*(struct MEMiHeapHead**)lbl_805A1218);
        *(struct MEMiHeapHead**)lbl_805A1218 = NULL;
    }
    lbl_805A1210 = 0;
}

s32 VFiPFENT_LoadLFNEntryFieldsFromBuf(PF_DIR_ENT* p_ent, const u8* buf) {
    u32 last;
    u16* name;
    u8 ord = buf[0];
    u8 sum = buf[13];
    u16* end;

    if ((ord & ~0x40) > 20) {
        p_ent->ordinal = 0;
        p_ent->check_sum = 0;
        p_ent->num_entry_LFNs = 0;
        return 33;
    }
    if (ord & 0x40) {
        p_ent->num_entry_LFNs = 0;
        ord &= ~0x40;
        last = 1;
    } else {
        last = 0;
        if (p_ent->num_entry_LFNs == 0) return 33;
        if (ord != p_ent->ordinal - 1 || sum != p_ent->check_sum) {
            p_ent->ordinal = 0;
            p_ent->check_sum = 0;
            p_ent->num_entry_LFNs = 0;
            return 33;
        }
    }
    p_ent->ordinal = ord;
    p_ent->check_sum = sum;
    name = p_ent->long_name + (u32)(ord - 1) * 13;
    fn_803A9644(name, buf + 1, 10);
    fn_803A9644(name + 5, buf + 14, 12);
    fn_803A9644(name + 11, buf + 28, 4);
    fn_803A9958((u8*)name, 10);
    fn_803A9958((u8*)(name + 5), 12);
    fn_803A9958((u8*)(name + 11), 4);
    if (last) {
        end = name + 13;
        *end = 0;
        for (; name < end; name++) {
            if (*name == 0) {
                name++;
                break;
            }
        }
        for (; name < end; name++) {
            if (*name != 0xFFFF) {
                p_ent->num_entry_LFNs = 0;
                return 33;
            }
        }
    }
    p_ent->num_entry_LFNs++;
    return 0;
}

static inline s32 dhash_wlen(const u16* p) {
    s32 length = 0;
    while (*p != 0) {
        p++;
        length++;
    }
    return length;
}

static inline s32 dhash_key(const u16* name) {
    s32 length = dhash_wlen(name);
    const u16* pos;
    u32 hash;
    u32 shift;
    u32 i;

    if (length < 8) {
        pos = name;
        hash = 0;
        shift = 0;
        for (i = 0; i < (u32)length; i++) {
            if (shift > 7) shift = 0;
            hash += *pos << (shift * 4);
            pos++;
            shift++;
        }
        return hash % 31;
    }
    return -1;
}

s32 dHash_SearchHashW(const u16* i_Name) {
    s32 key = dhash_key(i_Name);
    s32 length;
    s32 i;
    s32 slot;
    s32 j;
    const u16* name;
    const u16* stored;
    s32 equal;

    if (key != -1) {
        length = dhash_wlen(i_Name);
        for (i = 0; i < 15; i++) {
            slot = (key + i * i) % 31;
            // HYPOTHESIS: the first 16 bytes of a hash entry hold its wide name.
            stored = (const u16*)&lbl_80563360[slot];
            if (length == dhash_wlen(stored)) {
                name = i_Name;
                equal = 1;
                for (j = 0; j < length; j++) {
                    if (*name != *stored) {
                        equal = 0;
                        break;
                    }
                    name++;
                    stored++;
                }
                if (equal) return slot;
            }
        }
    }
    return -1;
}


s32 VFiPFFAT_GetSectorSpecified(PF_FFD* p_ffd, u32 file_sector_index, u32 may_allocate, u32* p_sector) {
    PF_VOLUME* p_vol;
    s32 err;
    u32 num_sector;
    if (p_ffd == NULL) return 10;
    if (p_ffd->p_hint == NULL) return 10;
    if (p_ffd->file_version == 0) return 10;
    if (p_ffd->p_hint->file_version > p_ffd->file_version) return 10;
    p_vol = p_ffd->p_vol;
    if (p_vol->drv_char < 'A' || p_vol->drv_char > 'Z') return 0x23;
    if (file_sector_index >= ((0xFFFFFFFF >> p_vol->bpb.log2_bytes_per_sector) + 1)) return 0x25;
    err = VFiPFFAT_GetSector(p_ffd, file_sector_index, may_allocate ? 1 : 0, 0, p_sector, &num_sector);
    if (err != 0) return err;
    return 0;
}
