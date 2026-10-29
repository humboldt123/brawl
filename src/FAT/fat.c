#include <types.h>
#include <FAT/fat_vf.h>

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

// ---------------------------------------------------------------- pf_clib

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

u16 VFiPFSTR_StrLen(PF_STR* p_str) {
    return p_str->p_tail - p_str->p_head;
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

s32 fn_803B19A0(PF_VOLUME* p_vol, PF_STR* p_path, s8 attr, PF_DTA* p_dta) {
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

s32 fn_803B1C78(PF_DTA* p_dta) {
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

s32 fn_803B1F80(PF_STR* p_path) {
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

s32 VFiPFENT_findEntry(PF_FFD* p_ffd, PF_DIR_ENT* p_ent, u32 index_search_from, PF_STR* p_pattern, u8 attr_required, u8 attr_unwanted) {
    u32 lpos;
    u32 ppos;

    return VFiPFENT_findEntryPos(p_ffd, p_ent, index_search_from, p_pattern, attr_required, attr_unwanted, &lpos, &ppos);
}

s32 VFiPFENT_allocateEntry(PF_DIR_ENT* p_ent, u8 num_entries, PF_FFD* p_ffd, u32* p_next_chain, PF_STR* p_filename) {
    u32 pos;

    return VFiPFENT_allocateEntryPos(p_ent, num_entries, p_ffd, p_next_chain, p_filename, &pos);
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

s32 VFiPFENT_ITER_GetEntryOfPath(PF_ENT_ITER* p_iter, PF_DIR_ENT* p_ent, PF_VOLUME* p_vol, PF_STR* p_path, u32 no_look_last_token) {
    return fn_803B4910(p_iter, p_ent, p_vol, p_path, 0, no_look_last_token);
}

s32 fn_803B5644(PF_ENT_ITER* p_iter, PF_DIR_ENT* p_ent, PF_VOLUME* p_vol, PF_STR* p_path) {
    return fn_803B4910(p_iter, p_ent, p_vol, p_path, 1, 1);
}

// ---------------------------------------------------------------- pf_fat

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

// ---------------------------------------------------------------- pf_file

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

s32 fn_803C29DC(const s8* path) {
    PF_STR str;
    s32 err;

    err = VFiPFSTR_InitStr(&str, path, 1);
    if (err == 0) {
        err = fn_803B1F80(&str);
    } else {
        lbl_8053B248.last_error = err;
    }
    return VFiPFAPI_convertReturnValue(err);
}

PF_FILE* fn_803C2A2C(const s8* path) {
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

s32 fn_803C2BB8(s8 drv_char) {
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

s32 fn_803C2CA0(PF_DTA* p_dta) {
    return VFiPFAPI_convertReturnValue(fn_803B1C78(p_dta));
}

u32 VFipf2_fwrite(void* p_buf, u32 size, u32 count, PF_FILE* p_file) {
    u32 count_written;

    VFiPFFILE_fwrite(p_buf, size, count, p_file, &count_written);
    return count_written;
}

s32 VFipf2_devinf(s8 drv_char, PF_DEV_INF* dev_inf) {
    return VFiPFAPI_convertReturnValue(VFiPFVOL_getdev(drv_char, dev_inf));
}

s32 VFipf2_init_prfile2(u32 config, void* param) {
    s32 err;

    err = VFiPFFATFS_initializeFATFS(config, param);
    if (err == 0) {
        VFiPFSYS_initializeSYS();
    }
    return VFiPFAPI_convertReturnValue(err);
}

s32 fn_803C2DB4(const s8* path) {
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

s32 fn_803C31B4(const s8* i_drive, const s8* i_sys_file_name_p) {
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
        fn_803C68D8();
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

s32 VFGetFileSizeByFd(void* i_file_p) {
    s32 size = -1;
    s32 err = VFSysGetFileSizeByFd(&size, i_file_p);
    if (err != 0) {
        VFSysSetLastError(err);
    }
    return size;
}

s32 fn_803C3DE0(void* unk0, const s8* i_path_p, u32 unk2) {
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

s32 VFSysCheckExistPrfFile_nandflash(VF_HANDLE_DEVICE* i_device_p, const s8* i_prf_file_name_p, void* i_memory_p, u32 i_handle_idx) {
    return VFSysCheckExistPrfFile_nandflash_sub(i_prf_file_name_p, i_handle_idx);
}

s32 VFSysCheckExistPrfFile_ram(VF_HANDLE_DEVICE* i_device_p, const s8* i_prf_file_name_p, void* i_memory_p, u32 i_handle_idx) {
    return dCommon_IsPrfFile(i_memory_p) ? 0 : 0xB006;
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

s32 VFSysGetLastError(void) {
    return lbl_805A1214;
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

// ---------------------------------------------------------------- d_hash

void dHash_InitHashTable(void) {
    int i;
    dHashEntry* p = lbl_80563360;

    for (i = 0; i < 31; ++i, ++p) {
        p->key = 0;
        p->used = 0;
    }
}

// ---------------------------------------------------------------- d_common

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

s32 fn_803C8014(s32 i_handle_idx, int i_setLastDeviceError) {
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
