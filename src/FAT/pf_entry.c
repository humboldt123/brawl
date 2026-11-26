#include <types.h>
#include <FAT/pf2.h>

// Function bodies below are derived from https://github.com/jj-link/brawl.

extern void fn_803EBBE0(s32 *, s32 *);

// HYPOTHESIS: same layout as PF_DIR_ENT in the VF copy of PrFile2.
struct PF2_DIR_ENT {
    u16 long_name[261];
    u8 num_entry_LFNs;
    u8 ordinal;
    u8 check_sum;
    u8 align_pad[1];
    s8 short_name[13];
    u8 small_letter_flag;
    u8 attr;
    u8 create_time_ms;
    u16 create_time;
    u16 create_date;
    u16 access_date;
    u16 modify_time;
    u16 modify_date;
    u32 file_size;
    PF2_VOLUME* p_vol;
    u32 path_len;
    u32 start_cluster;
    u32 entry_sector;
    u16 entry_offset;
};

// HYPOTHESIS: same layout as PF_ENT_ITER in the VF copy of PrFile2.
struct PF2_ENT_ITER {
    u32 index;
    PF_VOLUME* p_vol;
    PF_FFD ffd;
    u32 file_sector_index;
    u32 sector;
    u16 offset;
    u16 offset_mask;
    u8 buf[32];
    u8 log2_entries_per_sector;
};

// TODO: clean up, give variables real names
u8 PFENT_getcurrentDateTimeForEnt(s16 *arg0, s16 *arg1) {
    s32 date[3];
    s32 time[4];

    fn_803EBBE0(date, time);
    *arg0 = (s16)((date[2] & 0x1F) | ((date[1] << 5) & 0x1E0) | (((date[0] - 0x7BC) << 9) & 0xFE00));
    *arg1 = (s16)((((u32)time[2] >> 1) & 0x1F) | ((time[1] << 5) & 0x7E0) | ((time[0] << 11) & 0xF800));
    return (u8)time[3];
}


// TODO: clean up, give variables real names
int PFENT_ITER_GetEntryOfPath(PF2_ENT_ITER* iter, PF2_DIR_ENT* entry, PF2_VOLUME* vol, PF2_STR* str, u32 no_look_last_token) {
    return fn_803DE800(iter, entry, vol, str, 0, no_look_last_token);
}

s32 PFENT_findEntryPos(PF_FFD* p_ffd, PF2_DIR_ENT* p_ent, u32 index_search_from, PF2_STR* p_pattern, u8 attr_required, u8 attr_unwanted, u32* p_lpos, u32* p_ppos);

int PFENT_findEntry(PF_FFD* p_ffd, PF2_DIR_ENT* p_ent, u32 index, PF2_STR* pattern, u8 attr_required, u8 attr_unwanted) {
    u32 lpos;
    u32 ppos;

    return PFENT_findEntryPos(p_ffd, p_ent, index, pattern, attr_required, attr_unwanted, &lpos, &ppos);
}

s32 PFENT_allocateEntryPos(PF2_DIR_ENT* p_ent, u8 num_entries, PF_FFD* p_ffd, u32* p_next_chain, PF2_STR* p_filename, u32* p_pos);

int PFENT_allocateEntry(PF2_DIR_ENT* p_ent, u8 num_entries, PF_FFD* p_ffd, u32* p_next_chain, PF2_STR* p_filename) {
    u32 pos;

    return PFENT_allocateEntryPos(p_ent, num_entries, p_ffd, p_next_chain, p_filename, &pos);
}

int PFENT_ITER_GetEntryOfPattern(PF2_ENT_ITER* iter, PF2_DIR_ENT* ent, PF2_VOLUME* vol, PF2_STR* path) {
    return fn_803DE800(iter, ent, vol, path, 1, 1);
}

u8 PFENT_CalcCheckSum(PF2_DIR_ENT* p_ent) {
    u8 buf[11];
    u8 sum;
    u16 i;

    PFPATH_putShortName(buf, p_ent->short_name, 0);
    sum = 0;
    for (i = 0; i < 11; i++) {
        sum = (u8)(((sum & 1) ? 0x80 : 0) + (sum >> 1) + buf[i]);
    }
    return sum;
}

void PFENT_LoadShortNameFromBuf(PF2_DIR_ENT* p_ent, const u8* buf) {
    PFPATH_getShortName(p_ent->short_name, buf, buf[0xb]);
    if (p_ent->short_name[0] == 5) {
        p_ent->short_name[0] = -0x1B;
    }
}

s32 PFENT_GetRootDir(PF2_VOLUME* p_vol, PF2_DIR_ENT* p_ent) {
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

s32 PFENT_ITER_IteratorInitialize(PF2_ENT_ITER* p_iter, u32 index_start_from) {
    u32 num_success;
    s32 err;
    u32 index;
    s32 status;
    s32 read_err;
    u8 log2;

    p_iter->p_vol = p_iter->ffd.p_vol;
    index = p_iter->index;
    log2 = p_iter->p_vol->bpb.log2_bytes_per_sector - 5;
    p_iter->index = index_start_from;
    p_iter->log2_entries_per_sector = log2;
    p_iter->offset_mask = (1 << p_iter->log2_entries_per_sector) - 1;
    p_iter->file_sector_index = 0xFFFFFFFF;

    err = PFENT_RecalcEntryIterator(p_iter, 0);
    if (err != 0) {
        p_iter->index = index;
        PFENT_RecalcEntryIterator(p_iter, 0);
    } else {
        if (p_iter->sector == 0xFFFFFFFF) {
            status = 16;
        } else {
            read_err = PFSEC_ReadData(p_iter->p_vol, p_iter->buf, p_iter->sector, p_iter->offset, 32, &num_success, 0);
            status = 17;
            if (num_success == 32)
                status = read_err;
        }
        err = status != 0 ? status : 0;
    }

    return err;
}

// HYPOTHESIS: layout of the PrFile2 volume fields this unit touches.
struct PF2_VOLUME {
    PF_BPB bpb;
    u8 unk38[0x1F74 - sizeof(PF_BPB)];
    struct PF_DIR_TAIL tail_entry;
};

// HYPOTHESIS: the PrFile2 volume set; only the critical section is known here.
extern struct PF2_VOLUME_SET {
    u8 _0[0x60];
    u32 flags;
    PF_CRITICAL critical;
} lbl_805644C0;

u8 lbl_80492FD8[12] = "NO NAME    ";

s32 PFENT_MakeRootDir(PF2_VOLUME* p_vol) {
    PF_CACHE_PAGE* p_page;
    u32 written;
    u32 unused;
    u32 sector;
    s32 err;

    switch (p_vol->bpb.fat_type) {
    case 2:
        PFFAT_MakeRootDir(p_vol);
        break;
    case 0:
    case 1:
        err = PFCACHE_AllocateDataPage((PF_VOLUME*)p_vol, 0xFFFFFFFF, &p_page, &unused);
        if (err != 0) {
            return err;
        }
        VFipf_memset(p_page->p_buf, 0, p_vol->bpb.bytes_per_sector);
        for (sector = p_vol->bpb.first_root_dir_sector; sector < p_vol->bpb.first_data_sector; sector++) {
            err = PFSEC_WriteData((PF_VOLUME*)p_vol, p_page->p_buf, sector, 0, p_vol->bpb.bytes_per_sector, &written, 0);
            if (err != 0) {
                PFCACHE_FreeDataPage((PF_VOLUME*)p_vol, p_page);
                return err;
            }
            if (written != p_vol->bpb.bytes_per_sector) {
                PFCACHE_FreeDataPage((PF_VOLUME*)p_vol, p_page);
                return 17;
            }
        }
        PFCACHE_FreeDataPage((PF_VOLUME*)p_vol, p_page);
        break;
    default:
        return 7;
    }
    PF_enterCritical(&lbl_805644C0.critical);
    PFDRV_LoadVolumeLabelFromBuf(p_vol, lbl_80492FD8);
    PF_exitCritical(&lbl_805644C0.critical);
    return 0;
}

s32 PFENT_ITER_FindEntry(PF2_ENT_ITER* p_iter, PF2_DIR_ENT* p_ent, PF_STR* p_path, u32 wildcard, u32 no_look_last_token, u32* p_pos) {
    s32 err;

    *p_pos = 0;
    if (wildcard & no_look_last_token) {
        return 10;
    }
    if (PFSTR_StrNCmp(p_path, (const s8*)":", 1, 1, 1) == 0) {
        PFSTR_MoveStrPos(p_path, 2);
    }
    if (PFSTR_StrNCmp(p_path, (const s8*)"\\", 1, 0, 1) == 0 || PFSTR_StrNCmp(p_path, (const s8*)"/", 1, 0, 1) == 0) {
        err = PFENT_GetRootDir((PF2_VOLUME*)p_iter->p_vol, p_ent);
        if (err != 0) {
            return err;
        }
        if ((u16)PFSTR_StrNumChar(p_path, 1) == 1 && PFSTR_StrNCmp(p_path, (const s8*)"", 2, 0, 1) == 0) {
            return 0;
        }
    } else {
        err = PFVOL_GetCurrentDir((PF2_VOLUME*)p_iter->p_vol, p_ent);
        if (err != 0) {
            return err;
        }
    }
    err = PFENT_ITER_DoFindEntry(p_iter, p_ent, p_path, wildcard, no_look_last_token, p_pos);
    return err != 0 ? err : 0;
}

s32 PFENT_RecalcEntryIterator(PF2_ENT_ITER* p_iter, u32 may_allocate) {
    PF_CACHE_PAGE* page;
    u32 num_success;
    u32 is_hit;
    u32 old_sector;
    u32 sector;
    u32 i;
    s32 err;

    sector = p_iter->index >> p_iter->log2_entries_per_sector;
    old_sector = p_iter->file_sector_index;
    if (sector != old_sector) {
        p_iter->file_sector_index = sector;
        err = PFFAT_GetSectorSpecified(&p_iter->ffd, sector, 0, &p_iter->sector);
        if (err != 0) {
            return err;
        }
        if (p_iter->sector == 0xFFFFFFFF && may_allocate) {
            err = PFFAT_GetSectorSpecified(&p_iter->ffd, sector, 1, &p_iter->sector);
            if (err != 0) {
                return err;
            }
            if (p_iter->sector != 0xFFFFFFFF && old_sector != 0xFFFFFFFF) {
                err = PFCACHE_AllocateDataPage(p_iter->p_vol, 0xFFFFFFFF, &page, &is_hit);
                if (err != 0) {
                    return err;
                }
                VFipf_memset(page->p_buf, 0, p_iter->p_vol->bpb.bytes_per_sector);
                for (i = 0; i < p_iter->p_vol->bpb.sectors_per_cluster; i++) {
                    err = PFSEC_WriteData(p_iter->p_vol, page->p_buf, p_iter->sector + i, 0, p_iter->p_vol->bpb.bytes_per_sector, &num_success, 0);
                    if (err != 0) {
                        PFCACHE_FreeDataPage(p_iter->p_vol, page);
                        return err;
                    }
                    if (num_success != p_iter->p_vol->bpb.bytes_per_sector) {
                        PFCACHE_FreeDataPage(p_iter->p_vol, page);
                        return 17;
                    }
                }
                PFCACHE_FreeDataPage(p_iter->p_vol, page);
            }
        }
    }
    p_iter->offset = (p_iter->index & p_iter->offset_mask) << 5;
    return 0;
}

/* FAT directory entry fields in their on-disk order; numeric fields are little-endian. */
typedef struct PF2_DISK_DIR_ENT {
    u8 short_name[11];
    u8 attr;
    u8 small_letter_flag;
    u8 create_time_ms;
    u16 create_time;
    u16 create_date;
    u16 access_date;
    u16 cluster_high;
    u16 modify_time;
    u16 modify_date;
    u16 cluster_low;
    u32 file_size;
} PF2_DISK_DIR_ENT;

#define PFENT_SWAP16(x) ((((x) >> 8) & 0xFF) | (((x) << 8) & 0xFF00))
#define PFENT_SWAP32(x) (((x) >> 24) | (((x) >> 8) & 0xFF00) | (((x) << 8) & 0xFF0000) | ((x) << 24))

void PFENT_StoreEntryNumericFieldsToBuf(u8* buf, const PF2_DIR_ENT* p_ent) {
    PF2_DISK_DIR_ENT* disk = (PF2_DISK_DIR_ENT*)buf;
    disk->attr = p_ent->attr;
    disk->small_letter_flag = p_ent->small_letter_flag;
    disk->create_time_ms = p_ent->create_time_ms;
    disk->create_time = PFENT_SWAP16(p_ent->create_time);
    disk->create_date = PFENT_SWAP16(p_ent->create_date);
    disk->access_date = PFENT_SWAP16(p_ent->access_date);
    disk->modify_time = PFENT_SWAP16(p_ent->modify_time);
    disk->modify_date = PFENT_SWAP16(p_ent->modify_date);
    disk->cluster_high = PFENT_SWAP16((u16)(p_ent->start_cluster >> 16));
    disk->cluster_low = PFENT_SWAP16(p_ent->start_cluster);
    disk->file_size = PFENT_SWAP32(p_ent->file_size);
}

void PFENT_loadEntryNumericFieldsFromBuf(PF2_DIR_ENT* p_ent, const u8* buf) {
    const PF2_DISK_DIR_ENT* disk = (const PF2_DISK_DIR_ENT*)buf;
    p_ent->attr = disk->attr;
    p_ent->small_letter_flag = disk->small_letter_flag;
    p_ent->create_time_ms = disk->create_time_ms;
    p_ent->create_time = PFENT_SWAP16(disk->create_time);
    p_ent->create_date = PFENT_SWAP16(disk->create_date);
    p_ent->access_date = PFENT_SWAP16(disk->access_date);
    p_ent->modify_time = PFENT_SWAP16(disk->modify_time);
    p_ent->modify_date = PFENT_SWAP16(disk->modify_date);
    p_ent->file_size = PFENT_SWAP32(disk->file_size);
    p_ent->start_cluster = (PFENT_SWAP16(disk->cluster_high) << 16) | (u16)PFENT_SWAP16(disk->cluster_low);
}

s32 PFENT_findEmptyTailSFN(PF2_DIR_ENT* p_ent, const s8* name, u32* p_tails);
int fn_803E855C(char* buf, u32 num);

s32 PFENT_AdjustSFN(PF2_DIR_ENT* p_ent, s8* p_short_name) {
    u32 tail;
    s32 err;
    u32 index = 1;
    while (p_short_name[index] != '~' && p_short_name[index] && index < 7) index++;
    if (index < 7 && p_short_name[index] == '~') {
        index++;
        while (p_short_name[index] >= '0' && p_short_name[index] <= '9') index++;
        if (p_short_name[index] == '.' || p_short_name[index] == 0) {
            err = PFENT_findEmptyTailSFN(p_ent, p_short_name, &tail);
            if (err) return err;
            if (tail != 1) fn_803E855C((char*)p_short_name, tail);
        }
    }
    return 0;
}


u32 PFENT_ITER_IsAtLogicalEnd(PF2_ENT_ITER* p_iter) {
    u32 num_success;
    u32 sector_index;
    u32 index;
    s32 err;
    s32 status;

    if (p_iter->sector == 0xFFFFFFFF || p_iter->buf[0] == 0 || p_iter->index >= 999999) {
        sector_index = p_iter->file_sector_index;
        index = p_iter->index;
        p_iter->index = (sector_index + 1) << p_iter->log2_entries_per_sector;
        err = PFENT_RecalcEntryIterator(p_iter, 0);
        if (err != 0) {
            p_iter->index = index;
            p_iter->file_sector_index = sector_index;
            return 1;
        }
        if (p_iter->sector != 0xFFFFFFFF) {
            if (p_iter->sector == 0xFFFFFFFF) {
                status = 0x10;
            } else {
                err = PFSEC_ReadData(p_iter->p_vol, p_iter->buf, p_iter->sector, p_iter->offset, 0x20, &num_success, 0);
                status = 0x11;
                if (num_success == 0x20) status = err;
            }
            return status != 0;
        }
        return 1;
    }
    return 0;
}

/* FAT long-name entry: the first five UTF-16 code units are unaligned. */
typedef struct PF2_DISK_LFN_ENT {
    u8 ordinal;
    u8 name_first[10];
    u8 attr;
    u8 type;
    u8 check_sum;
    u16 name_middle[6];
    u16 cluster;
    u16 name_last[2];
} PF2_DISK_LFN_ENT;

void VFiPF_LE16_TO_U16_STR(u8* str, u32 num);

void PFENT_storeLFNEntryFieldsToBuf(u8* buf, PF2_DIR_ENT* p_ent, u8 ord, u8 sum, u32 is_last) {
    u16* name;
    u16* end;
    u16* pos;
    PF2_DISK_LFN_ENT* disk = (PF2_DISK_LFN_ENT*)buf;

    if (is_last) disk->ordinal = ord | 0x40;
    else disk->ordinal = ord;
    disk->attr = 15;
    disk->check_sum = sum;
    disk->type = 0;
    disk->cluster = 0;
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
    VFipf_memcpy(disk->name_first, name, 10);
    VFipf_memcpy(disk->name_middle, name + 5, 12);
    VFipf_memcpy(disk->name_last, name + 11, 4);
    VFiPF_LE16_TO_U16_STR(disk->name_first, 10);
    VFiPF_LE16_TO_U16_STR((u8*)disk->name_middle, 12);
    VFiPF_LE16_TO_U16_STR((u8*)disk->name_last, 4);
}

s32 PFENT_searchEmptyTailSFN(PF_FFD* p_ffd, u32 tail_index, const s8* name, u32* p_tail_bit);

s32 PFENT_findEmptyTailSFN(PF2_DIR_ENT* p_ent_containing_dir, const s8* name, u32* p_tails) {
    PF_FAT_HINT hint;
    PF_FFD ffd;
    u32 tail_index;
    s32 err;
    u32 i;
    PF2_VOLUME* p_vol = p_ent_containing_dir->p_vol;

    *p_tails = 1;
    PFFAT_InitFFD(&ffd, &hint, (PF_VOLUME*)p_ent_containing_dir->p_vol, &p_ent_containing_dir->start_cluster);
    for (tail_index = 1; tail_index <= 999999; tail_index += p_vol->tail_entry.tracker_size * 32) {
        err = PFENT_searchEmptyTailSFN(&ffd, tail_index, name, p_vol->tail_entry.tracker_bits);
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

s32 PFENT_LoadLFNEntryFieldsFromBuf(PF2_DIR_ENT* p_ent, const u8* buf) {
    const PF2_DISK_LFN_ENT* disk = (const PF2_DISK_LFN_ENT*)buf;
    u32 last;
    u16* name;
    u8 ord = disk->ordinal;
    u8 sum = disk->check_sum;
    u16* end;

    if ((ord & ~0x40) > 20U) {
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
    VFipf_memcpy(name, (void*)disk->name_first, 10);
    VFipf_memcpy(name + 5, (void*)disk->name_middle, 12);
    VFipf_memcpy(name + 11, (void*)disk->name_last, 4);
    VFiPF_LE16_TO_U16_STR((u8*)name, 10);
    VFiPF_LE16_TO_U16_STR((u8*)(name + 5), 12);
    VFiPF_LE16_TO_U16_STR((u8*)(name + 11), 4);
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


u32 fn_803E8BD0(PF_STR* pattern, u32* index);
s32 fn_803DEF54(PF2_ENT_ITER* p_iter, u32 may_allocate);
s32 fn_803DC790(PF2_DIR_ENT* p_ent, PF2_ENT_ITER* p_iter, PF2_STR* p_pattern, u8 attr_required, u8 attr_unwanted, u32* logical_index);

s32 PFENT_findEntryPos(struct PF_FFD* p_ffd, PF2_DIR_ENT* p_ent, u32 index_search_from, PF2_STR* p_pattern, u8 attr_required, u8 attr_unwanted, u32* p_lpos, u32* p_ppos) {
    s32 err;
    PF2_ENT_ITER iter;
    u32 logical_index;
    u32 is_extsfn;

    is_extsfn = 0;

    *p_lpos = 0;
    *p_ppos = 0;

    p_ent->num_entry_LFNs = 0;
    p_ent->ordinal = 0;
    p_ent->check_sum = 0;
    p_ent->long_name[0] = 0;

    logical_index = 0;

    if ((lbl_805644C0.flags & 2) == 2) {
        is_extsfn = fn_803E8BD0((PF_STR*)p_pattern, &index_search_from);
    }

    iter.ffd = *p_ffd;

    if (is_extsfn == 1) {
        err = PFENT_ITER_IteratorInitialize(&iter, index_search_from - 1);
        if (err != 0) {
            *p_lpos = 999999;
            *p_ppos = 999999;
            return err;
        }

        err = fn_803DC790(p_ent, &iter, p_pattern, attr_required, attr_unwanted, &logical_index);

        if (err == 0) {
            *p_lpos = 999999;
            *p_ppos = 999999;
            return 3;
        }

        if ((iter.buf[0] & 0x40) != 0) {
            err = PFENT_ITER_IteratorInitialize(&iter, index_search_from);
            if (err != 0) {
                *p_lpos = 999999;
                *p_ppos = 999999;
                return err;
            }

            err = fn_803DC790(p_ent, &iter, p_pattern, attr_required, attr_unwanted, &logical_index);

            if (err == 0) {
                p_ent->p_vol = (PF2_VOLUME*)p_ffd->p_vol;
                *p_lpos = logical_index;
                *p_ppos = iter.index;
                return 0;
            }
        }

    } else {
        err = PFENT_ITER_IteratorInitialize(&iter, index_search_from);
        while (PFENT_ITER_IsAtLogicalEnd(&iter) == 0) {
            if (err != 0) {
                *p_lpos = 999999;
                *p_ppos = 999999;
                return err;
            }
            if (iter.buf[0] == 0) {
                break;
            }
            if (iter.buf[0] == 0xE5) {
                p_ent->num_entry_LFNs = 0;
                p_ent->long_name[0] = 0;

            } else {
                err = fn_803DC790(p_ent, &iter, p_pattern, attr_required, attr_unwanted, &logical_index);

                if (err == 0) {
                    p_ent->p_vol = (PF2_VOLUME*)p_ffd->p_vol;
                    *p_lpos = logical_index;
                    *p_ppos = iter.index;
                    return 0;
                }
            }

            err = fn_803DEF54(&iter, 0);
        }
    }

    *p_lpos = 999999;
    *p_ppos = 999999;
    return 3;
}


int PFPATH_CheckExtShortNameSignature(PF_STR* p_str);

s32 PFENT_allocateEntryPos(PF2_DIR_ENT* p_ent, u8 num_entries, struct PF_FFD* p_ffd, u32* p_next_chain, PF2_STR* p_filename, u32* p_pos) {
    s32 err;
    struct PF_VOLUME* p_vol;
    PF2_ENT_ITER iter;
    PF2_DIR_ENT wk_ent;
    u32 wk_sector;
    u16 wk_offset;
    u32 prev_sector;
    u32 next_sector1;
    u32 next_sector2;
    u32 is_found;
    u32 sector;
    u32 count_unused_entries;
    u32 physical_index;
    u32 dummy_index;

    if ((lbl_805644C0.flags & 2) == 2) {
        if ((u32)PFPATH_CheckExtShortNameSignature((PF_STR*)p_filename) == 1) {
            return 1;
        }
    }

    p_vol = p_ffd->p_vol;
    count_unused_entries = 0;
    physical_index = 0;

    wk_sector = -1;
    wk_offset = 0;
    p_next_chain[1] = -1;
    p_next_chain[0] = -1;
    prev_sector = -1;
    next_sector2 = -1;
    next_sector1 = -1;
    is_found = 0;

    iter.sector = 0;
    iter.index = 0;
    iter.ffd = *p_ffd;
    err = PFENT_ITER_IteratorInitialize(&iter, 0);

    while (1) {
        if (err != 0) {
            return err;
        }

        if (iter.sector == -1) {
            break;
        }
        if (is_found == 0 && count_unused_entries == 0) {
            wk_offset = iter.offset;
            wk_sector = iter.sector;
            prev_sector = iter.sector;
        }
        if (is_found == 0) {
            if (iter.buf[0] == 0 || iter.buf[0] == 0xE5) {
                if (prev_sector != iter.sector) {
                    if (next_sector1 == -1) {
                        next_sector1 = iter.sector;
                    } else {
                        next_sector2 = iter.sector;
                    }
                    prev_sector = iter.sector;
                }
                count_unused_entries++;
            } else {
                count_unused_entries = 0;
                next_sector1 = -1;
                next_sector2 = -1;
            }
        }
        if (count_unused_entries >= num_entries) {
            if (is_found == 0) {
                physical_index = iter.index;
                is_found = 1;
            }
            if (iter.buf[0] == 0) {
                break;
            }
        }
        if (iter.buf[0] != 0 && iter.buf[0] != 0xE5) {
            err = fn_803DC790(&wk_ent, &iter, p_filename, 0x7F, 0, &dummy_index);
            if (err == 0) {
                *p_ent = wk_ent;
                p_ent->p_vol = (PF2_VOLUME*)p_ffd->p_vol;
                return 8;
            }
        }
        if (iter.offset + 32 == p_vol->bpb.bytes_per_sector) {
            err = PFFAT_GetSectorSpecified(p_ffd, iter.file_sector_index + 1, 0, &sector);
            if (err != 0) {
                return err;
            }
            if (sector == -1 && is_found == 1) {
                break;
            }
        }
        err = fn_803DEF54(&iter, 1);
    }

    if (is_found == 0) {
        return 5;
    }

    p_next_chain[0] = next_sector1;
    p_next_chain[1] = next_sector2;
    p_ent->entry_sector = wk_sector;
    p_ent->entry_offset = wk_offset;
    *p_pos = physical_index;
    return 0;
}
