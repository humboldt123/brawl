#include <types.h>
#include <FAT/pf2.h>

// HYPOTHESIS: layout of the second PrFile2 volume as far as the FAT code uses it; the BPB leads
// it as in the VF copy, and the driver callback with its argument sit near the end.
struct PF2_VOLUME {
    PF_BPB bpb;
    u32 num_free_clusters;
    u32 last_free_cluster;
    u8 unk40[0x1F84 - 0x40];
    void* p_callback_arg;
    u8 unk1F88[0x1FAC - 0x1F88];
    u16 fsi_flag;
    u8 unk1FAE[0x1FC0 - 0x1FAE];
    int (*p_callback)(void*);
};

// HYPOTHESIS: per-FAT-type constants in this unit's .rodata, 0x14 bytes each; only the
// end-of-chain marker at 0x8 is known.
typedef struct PF_FAT_TYPE_INFO {
    u32 unk0;
    u32 unk4;
    u32 end_of_chain;
    u32 unkC;
    u32 unk10;
} PF_FAT_TYPE_INFO;
extern const PF_FAT_TYPE_INFO lbl_8041B0E0[3];

s32 PFFAT_GetClusterAllocatedInChain(PF_FFD* p_ffd, u32 initial_cluster, u32 chain_index, u32 size, u32* p_num_clusters);
s32 PFFAT12_ReadFATEntryWithBuf(PF2_VOLUME* p_vol, u16 cluster, u32* p_value, PF_CACHE_PAGE* p_page);
s32 PFFAT16_ReadFATEntryWithBuf(PF2_VOLUME* p_vol, u32 cluster, u32* p_value, PF_CACHE_PAGE* p_page);
s32 PFFAT32_ReadFATEntryWithBuf(PF2_VOLUME* p_vol, u32 cluster, u32* p_value, PF_CACHE_PAGE* p_page);
s32 PFFAT12_WriteFATEntry(PF2_VOLUME* p_vol, u16 cluster, u16 value);
s32 PFFAT16_WriteFATEntry(PF2_VOLUME* p_vol, u32 cluster, u32 value);
s32 PFFAT_FindClusterLink(PF_FFD* p_ffd, u32 chain_index, u32* p_cluster, u32* is_found);
s32 PFFAT_GetClusterSpecified(PF_FFD* p_ffd, u32 chain_index, u32 may_allocate, u32* p_cluster);
s32 PFFAT_GetSector(PF_FFD* p_ffd, u32 file_sector_index, u32 mode, u32 size, u32* p_sector);

// Function bodies below are derived from https://github.com/jj-link/brawl.

u32 PFFAT_GetSectorSpecified(PF_FFD* p_ffd, u32 index, u32 may_allocate, u32* p_cluster) {
    u32 err = PFFAT_GetSector(p_ffd, index, may_allocate ? 1 : 0, 0, p_cluster);

    if (err != 0) {
        return err;
    }
    return 0;
}

int PFFAT_GetSectorAllocated(PF_FFD* p_ffd, u32 index, u32 size, u32* p_cluster) {
    int err = PFFAT_GetSector(p_ffd, index, 2, size, p_cluster);

    if (err != 0) {
        return err;
    }
    return 0;
}

void PFFAT_InitHint(PF_FAT_HINT* p_hint) {
    p_hint->chain_index = 0;
    p_hint->cluster = 0;
    p_hint->file_version = 0;
}

int PFFAT_ResetFFD(PF_FFD* p_ffd, u32* p_start_cluster) {
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

// PFFAT_InitFFD; the FFD layout is the VF one.
int PFFAT_InitFFD(PF_FFD* p_ffd, PF_FAT_HINT* p_hint, PF_VOLUME* p_vol, u32* p_start_cluster) {
    p_ffd->p_hint = p_hint;
    p_ffd->p_vol = p_vol;
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
    return 0;
}

int PFFAT_FinalizeFFD(PF_FFD* p_ffd) {
    p_ffd->cluster_link.buffer = NULL;
    return 0;
}

s32 PFFAT16_WriteFATEntryWithBuf(PF2_VOLUME* p_vol, u32 cluster, u32 value, PF_CACHE_PAGE* p_page) {
    u32 offset = cluster * 2;
    u16 sector = p_vol->bpb.active_FAT_sector + (offset >> p_vol->bpb.log2_bytes_per_sector);
    u16 pos = offset & (p_vol->bpb.bytes_per_sector - 1);
    u32 i;
    s32 err = 0;
    s32 first_err = 0;

    if (p_page->sector != sector) {
        for (i = 0; i < p_vol->bpb.num_active_FATs; i++) {
            s32 werr = PFSEC_WriteFAT(p_vol, p_page->p_buf, p_page->sector + i * p_vol->bpb.sectors_per_FAT, 0, p_vol->bpb.bytes_per_sector);
            if (werr != 0 && first_err == 0) {
                first_err = werr;
            }
        }
        p_page->sector = sector;
        err = PFSEC_ReadFAT(p_vol, p_page->p_buf, sector, 0, p_vol->bpb.bytes_per_sector);
    }
    p_page->p_buf[pos] = value;
    p_page->p_buf[pos + 1] = value >> 8;
    if (err != 0) {
        first_err = err;
    }
    return first_err;
}

s32 PFFAT32_WriteFATEntry(PF2_VOLUME* p_vol, u32 cluster, u32 value) {
    u32 offset = cluster * 4;
    u32 sector = (u16)(p_vol->bpb.active_FAT_sector + (offset >> p_vol->bpb.log2_bytes_per_sector));
    u16 pos = offset & (p_vol->bpb.bytes_per_sector - 1);
    u32 buf;
    u16 i;
    s32 first_err;
    s32 err;

    err = PFSEC_ReadFAT(p_vol, (u8*)&buf, sector, pos, 4);
    if (err != 0) {
        return err;
    }
    first_err = 0;
    i = 0;
    buf = (buf >> 24) | ((buf >> 8) & 0xFF00) | ((buf & 0xFF00) << 8) | (buf << 24);
    buf &= 0xF0000000;
    buf |= value & 0x0FFFFFFF;
    buf = (buf >> 24) | ((buf >> 8) & 0xFF00) | ((buf & 0xFF00) << 8) | (buf << 24);
    for (; i < p_vol->bpb.num_active_FATs; i++) {
        err = PFSEC_WriteFAT(p_vol, (u8*)&buf, sector, pos, 4);
        if (err != 0 && first_err == 0) {
            first_err = err;
        }
        sector += p_vol->bpb.sectors_per_FAT;
    }
    return first_err;
}

s32 PFFAT32_WriteFATEntryWithBuf(PF2_VOLUME* p_vol, u32 cluster, u32 value, PF_CACHE_PAGE* p_page) {
    u32 offset = cluster * 4;
    u32 sector = (u16)(p_vol->bpb.active_FAT_sector + (offset >> p_vol->bpb.log2_bytes_per_sector));
    u16 pos = offset & (p_vol->bpb.bytes_per_sector - 1);
    s32 err = 0;
    s32 first_err = 0;
    u16 i;
    u32 entry;

    if (p_page->sector != sector) {
        for (i = 0; i < p_vol->bpb.num_active_FATs; i++) {
            s32 werr = PFSEC_WriteFAT(p_vol, p_page->p_buf, p_page->sector + i * p_vol->bpb.sectors_per_FAT, 0, p_vol->bpb.bytes_per_sector);
            if (werr != 0 && first_err == 0) {
                first_err = werr;
            }
        }
        err = PFSEC_ReadFAT(p_vol, p_page->p_buf, sector, 0, p_vol->bpb.bytes_per_sector);
        if (err != 0) {
            return err;
        }
        p_page->sector = sector;
    }
    entry = *(u32*)(p_page->p_buf + pos);
    entry = (entry >> 24) | ((entry >> 8) & 0xFF00) | ((entry & 0xFF00) << 8) | (entry << 24);
    entry = (value & 0x0FFFFFFF) | (entry & 0xF0000000);
    *(u32*)(p_page->p_buf + pos) = (entry >> 24) | ((entry >> 8) & 0xFF00) | ((entry & 0xFF00) << 8) | (entry << 24);
    if (err != 0) {
        first_err = err;
    }
    return first_err;
}

s32 PFFAT16_ReadFATEntry(PF2_VOLUME* p_vol, u32 cluster, u32* p_value) {
    u32 offset = cluster * 2;
    u32 sector = (u16)(p_vol->bpb.active_FAT_sector + (offset >> p_vol->bpb.log2_bytes_per_sector));
    u16 pos = offset & (p_vol->bpb.bytes_per_sector - 1);
    u16 buf;
    u16 flags = p_vol->bpb.ext_flags;
    s32 err;
    u32 fat_index = 1;
    int action;

    if (flags & 0x80) {
        fat_index = flags & 7;
    }
    do {
        err = PFSEC_ReadFAT(p_vol, (u8*)&buf, sector, pos, 2);
        if (err == 0x1000 && p_vol->p_callback != NULL) {
            action = p_vol->p_callback(p_vol->p_callback_arg);
            if (action == 0) {
                continue;
            }
            if (action == 1 && p_vol->bpb.num_active_FATs >= 2 && fat_index < p_vol->bpb.num_active_FATs) {
                fat_index++;
                sector += p_vol->bpb.sectors_per_FAT;
                continue;
            }
        }
        if (err != 0) {
            *p_value = -1;
            return err;
        }
    } while (err != 0);
    *p_value = ((buf >> 8) & 0xFF) | ((buf << 8) & 0xFF00);
    return 0;
}

s32 PFFAT32_ReadFATEntry(PF2_VOLUME* p_vol, u32 cluster, u32* p_value) {
    u32 offset = cluster * 4;
    u32 sector = (u16)(p_vol->bpb.active_FAT_sector + (offset >> p_vol->bpb.log2_bytes_per_sector));
    u16 pos = offset & (p_vol->bpb.bytes_per_sector - 1);
    u32 buf;
    s32 err;
    u32 fat_index = 1;
    int action;

    if (p_vol->bpb.ext_flags & 0x80) {
        fat_index = p_vol->bpb.ext_flags & 7;
    }
    do {
        err = PFSEC_ReadFAT(p_vol, (u8*)&buf, sector, pos, 4);
        if (err == 0x1000 && p_vol->p_callback != NULL) {
            action = p_vol->p_callback(p_vol->p_callback_arg);
            if (action == 0) {
                continue;
            }
            if (action == 1 && p_vol->bpb.num_active_FATs >= 2 && fat_index < p_vol->bpb.num_active_FATs) {
                fat_index++;
                sector += p_vol->bpb.sectors_per_FAT;
                continue;
            }
        }
        if (err != 0) {
            *p_value = -1;
            return err;
        }
    } while (err != 0);
    *p_value = ((buf >> 24) | ((buf >> 8) & 0xFF00) | ((buf & 0xFF00) << 8) | (buf << 24)) & 0x0FFFFFFF;
    return 0;
}

s32 PFFAT_GetClusterContinuousSectorInChain(PF_FFD* p_ffd, u32 initial_cluster, u32 chain_index, u32 size, u32* p_num_sector) {
    PF_VOLUME* p_vol = p_ffd->p_vol;
    u32 next_cluster = -1;
    u32 chk_clstlnk = 1;
    u32 cluster = initial_cluster;
    s32 err;

    while (next_cluster != 0) {
        err = PFFAT_ReadCluster(p_ffd, cluster, chain_index, &next_cluster, &chk_clstlnk);
        if (err != 0) {
            return err;
        }
        if (cluster + 1 != next_cluster) {
            if (next_cluster == lbl_8041B0E0[p_vol->bpb.fat_type].end_of_chain) {
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

s32 PFFAT_getContinuousSector(PF_FFD* p_ffd, u32 file_sector_index, u32 size, u32* p_sector, u32* p_num_sector) {
    PF_VOLUME* p_vol = p_ffd->p_vol;
    u32 cluster = -1;
    u32 sector;
    s32 err;

    sector = *p_sector;
    if (sector != -1) {
        cluster = ((sector - p_vol->bpb.first_data_sector) >> p_vol->bpb.log2_sectors_per_cluster) + 2;
    } else {
        err = PFFAT_GetSectorSpecified(p_ffd, file_sector_index, 0, p_sector);
        if (err != 0) {
            return err;
        }
        sector = *p_sector;
        if (sector != -1) {
            cluster = ((sector - p_vol->bpb.first_data_sector) >> p_vol->bpb.log2_sectors_per_cluster) + 2;
        }
    }

    if (sector != -1) {
        *p_num_sector = (p_vol->bpb.first_data_sector + ((cluster - 1) << p_vol->bpb.log2_sectors_per_cluster)) - sector;
    } else {
        *p_num_sector = 0;
    }

    if (*p_sector != -1 && (*p_num_sector << p_vol->bpb.log2_bytes_per_sector) < size) {
        err = PFFAT_GetClusterContinuousSectorInChain(p_ffd, cluster, (file_sector_index >> p_vol->bpb.log2_sectors_per_cluster) + 1, size, p_num_sector);
        if (err != 0) {
            return err;
        }
    }
    return 0;
}

s32 PFFAT_ReadFATSector(PF2_VOLUME* p_vol, PF_CACHE_PAGE* p_page, u32 cluster) {
    u32 offset;
    s32 err;
    u32 fat_index;
    u16 flags;

    switch (p_vol->bpb.fat_type) {
    case FAT_12:
        offset = (u16)(cluster + (cluster >> 1));
        break;
    case FAT_16:
        offset = cluster * 2;
        break;
    case FAT_32:
        offset = cluster * 4;
        break;
    default:
        return 0xF;
    }
    p_page->sector = (u16)(p_vol->bpb.active_FAT_sector + (offset >> p_vol->bpb.log2_bytes_per_sector));
    fat_index = 1;
    flags = p_vol->bpb.ext_flags;
    if (flags & 0x80) {
        fat_index = flags & 7;
    }
    do {
        err = PFSEC_ReadFAT(p_vol, p_page->p_buf, p_page->sector, 0, p_vol->bpb.bytes_per_sector);
        if (err == 0x1000 && p_vol->p_callback != NULL) {
            int action = p_vol->p_callback(p_vol->p_callback_arg);
            if (action == 0) {
                continue;
            }
            if (action == 1 && p_vol->bpb.num_active_FATs >= 2 && fat_index < p_vol->bpb.num_active_FATs) {
                fat_index++;
                p_page->sector += p_vol->bpb.sectors_per_FAT;
                continue;
            }
        }
        if (err != 0) {
            return err;
        }
    } while (err != 0);
    return err;
}

s32 PFFAT_ResetCluster(PF_VOLUME* p_vol, u32 cluster) {
    PF_CACHE_PAGE* p_page;
    u32 is_hit;
    u32 success_size;
    u32 sector;
    u32 first_sector;
    u32 i;
    s32 err;

    err = PFCACHE_AllocateDataPage(p_vol, -1, &p_page, &is_hit);
    if (err != 0) {
        return err;
    }
    first_sector = p_vol->bpb.first_data_sector + ((cluster - 2) << p_vol->bpb.log2_sectors_per_cluster);
    for (i = 0; i < p_vol->bpb.sectors_per_cluster; i++) {
        sector = first_sector + i;
        if (PFSEC_ReadData(p_vol, p_page->p_buf, sector, 0, p_vol->bpb.bytes_per_sector, &success_size, 1) != 0) {
            break;
        }
        if (success_size != p_vol->bpb.bytes_per_sector) {
            break;
        }
        VFipf_memset(p_page->p_buf, 0, p_vol->bpb.bytes_per_sector);
        if (PFSEC_WriteData(p_vol, p_page->p_buf, sector, 0, p_vol->bpb.bytes_per_sector, &success_size, 1) != 0) {
            break;
        }
        if (success_size != p_vol->bpb.bytes_per_sector) {
            break;
        }
    }
    PFCACHE_FreeDataPage(p_vol, p_page);
    return 0;
}

// TODO: clean up, give variables real names
/* jj_803E2468 */
struct Ctx {
    unsigned short unk0;
    unsigned char pad002[0x0A];
    unsigned int unkC;
    unsigned char pad010[0x08];
    unsigned short unk18;
    unsigned char pad01A[0x06];
    unsigned char unk20;
    unsigned char pad021;
    unsigned char unk22;
    unsigned char pad023[0x05];
    unsigned int unk28;
    unsigned char pad02C[0x1F84 - 0x2C];
    int unk1F84;
    unsigned char pad1F88[0x1FC0 - 0x1F88];
    int (*unk1FC0)(int);
};

inline int inline_fn(struct Ctx *arg0) {
    return arg0->unk0 - 1;
}

extern int fn_803E8DC0(void *, unsigned short *, unsigned int, unsigned int, unsigned short);

// TODO: clean up, give variables real names
int fn_803E2468(struct Ctx *arg0, int arg1, int *arg2) {
    unsigned short temp_r28;
    unsigned short var_r27;
    int var_r26;
    unsigned int var_r25;
    unsigned char buf[4];
    unsigned short temp_r9;
    unsigned short temp_r0;
    int temp_r3;
    int temp_r3_2;
    int (*temp_r12)(int);
    unsigned int temp_r0_3;
    unsigned short sp8;

    temp_r0 = (unsigned short)(arg1 + (arg1 >> 1));
    temp_r9 = arg0->unk18;
    var_r25 = 1;
    var_r27 = (unsigned short)(arg0->unk28 + ((int)temp_r0 >> arg0->unk20));
    temp_r28 = (unsigned short)(temp_r0 & inline_fn(arg0));
    if (temp_r9 & 0x80) {
        var_r25 = temp_r9 & 7;
    }

    do {
        if ((int)temp_r28 < (int)inline_fn(arg0)) {
            var_r26 = fn_803E8DC0(arg0, (unsigned short *)buf, (unsigned short)var_r27, temp_r28, 2);
        } else {
            temp_r3 = fn_803E8DC0(arg0, (unsigned short *)buf, (unsigned short)var_r27, temp_r28, 1);
            var_r26 = temp_r3;
            if (temp_r3 == 0) {
                var_r26 = fn_803E8DC0(arg0, (unsigned short *)(buf + 1),
                                      (unsigned short)var_r27 + 1, 0, 1);
            }
        }
        if (var_r26 == 0x1000) {
            temp_r12 = arg0->unk1FC0;
            if (temp_r12 != 0) {
                temp_r3_2 = temp_r12(arg0->unk1F84);
                if (temp_r3_2 == 0) {
                    continue;
                }
                if (temp_r3_2 == 1) {
                    if (arg0->unk22 >= 2 && var_r25 < arg0->unk22) {
                        var_r25 += 1;
                        var_r27 += (unsigned short)arg0->unkC;
                        continue;
                    }
                }
            }
        }
        if (var_r26 != 0) {
            *arg2 = -1;
            return var_r26;
        }
    } while (var_r26 != 0);

    sp8 = *(unsigned short *)buf;
    temp_r0_3 = ((sp8 >> 8) & 0xFF) | (((unsigned int)sp8 << 8) & 0xFF00);
    if (arg1 & 1) {
        *arg2 = (temp_r0_3 >> 4) & 0xFFF;
    } else {
        *arg2 = temp_r0_3 & 0xFFF;
    }
    return 0;
}

// TODO: clean up, give variables real names
/* jj_803E119C */
typedef struct Info { char pad0[0x10]; u32 value; char pad14[8]; u32 count; char pad20[0x14]; u32 limit; } Info;
typedef struct Record { int key; u32 value; int owner; } Record;
typedef struct Context { int owner; char pad4[4]; int *state; char padc[0x10]; int flag; char pad20[0x10]; Record *record; Info *info; } Context;
s32 PFFAT_GetClusterInChain(struct PF_FFD* p_ffd, u32 chain_index, u32 mode, u32 num_cluster, u32* locate_start, u32* locate_end);
// TODO: clean up, give variables real names
int fn_803E119C(Context *ctx, int key, int arg2, u32 *out) {
    u32 value, other;
    Info *info = ctx->info;
    u32 count = info->count;
    Record *record = ctx->record;
    int result;
    if ((u32)*ctx->state == 1) {
        if (count <= 1) { *out = -1; return 0; }
        *ctx->state = info->value;
    }
    *out = -1;
    result = PFFAT_GetClusterInChain((struct PF_FFD*)ctx, key, 2, arg2, &other, &value);
    switch (result) {
    case 0:
        if (other >= 2 && other < ctx->info->limit + 2) *out = other;
        result = 0;
        break;
    default:
        break;
    }
    if (result != 0) return result;
    if (*out == 0xFFFFFFFF) return 0;
    record->owner = ctx->owner;
    record->key = key;
    record->value = *out;
    return 0;
}

// Original pf_fat.o, pf_fat12.o, pf_fat16.o, pf_fat32.o were separate TUs; prevent cross-TU auto-inlining.
#pragma auto_inline off

s32 PFFAT_ReadCluster(PF_FFD* p_ffd, u32 cluster, u32 chain_index, u32* p_next_cluster, u32* p_chk_clstlnk) {
    s32 err;
    u32 next_cluster;

    *p_next_cluster = 0xFFFFFFFF;
    if (p_ffd->cluster_link.buffer != NULL && *p_chk_clstlnk == 1) {
        err = PFFAT_FindClusterLink(p_ffd, chain_index, p_next_cluster, p_chk_clstlnk);
        if (err != 0) {
            return err;
        }
    }
    if (p_ffd->cluster_link.buffer == NULL || *p_chk_clstlnk == 0) {
        switch (p_ffd->p_vol->bpb.fat_type) {
        case FAT_12:
            err = fn_803E2468((struct Ctx*)p_ffd->p_vol, (u16)cluster, (int*)p_next_cluster);
            break;
        case FAT_16:
            err = PFFAT16_ReadFATEntry((PF2_VOLUME*)p_ffd->p_vol, cluster, p_next_cluster);
            break;
        case FAT_32:
            err = PFFAT32_ReadFATEntry((PF2_VOLUME*)p_ffd->p_vol, cluster, p_next_cluster);
            break;
        default:
            *p_next_cluster = 0xFFFFFFFF;
            err = 15;
            break;
        }
        if (err != 0) {
            return err;
        }
        next_cluster = *p_next_cluster;
        if (next_cluster == 0) {
            return 13;
        }
        if (next_cluster == 0xFFFFFFFF) {
            return 6;
        }
        if (p_ffd->cluster_link.buffer != NULL && next_cluster != lbl_8041B0E0[p_ffd->p_vol->bpb.fat_type].end_of_chain &&
            p_ffd->cluster_link.max_count > p_ffd->cluster_link.position && p_ffd->cluster_link.max_count != 0) {
            if (chain_index == p_ffd->cluster_link.position * (p_ffd->cluster_link.interval + 1)) {
                p_ffd->cluster_link.interval_offset = 0;
                p_ffd->cluster_link.buffer[p_ffd->cluster_link.position] = next_cluster;
                p_ffd->cluster_link.position++;
            } else {
                p_ffd->cluster_link.interval_offset++;
            }
            p_ffd->cluster_link.save_index = chain_index;
        }
    }
    return 0;
}

s32 PFFAT_MakeRootDir(PF2_VOLUME* p_vol) {
    u32 num_success;
    u32 is_hit;
    PF_CACHE_PAGE* p_page;
    s32 err;
    u32 sector;
    u32 i;
    enum FatType fat_type = p_vol->bpb.fat_type;
    u32 cluster = p_vol->bpb.root_dir_cluster;
    u32 eoc = lbl_8041B0E0[fat_type].end_of_chain;

    switch (fat_type) {
    case FAT_12:
        if (eoc > 0xFFF) {
            err = 0x10;
        } else {
            err = PFFAT12_WriteFATEntry(p_vol, (u16)cluster, (u16)eoc);
        }
        break;
    case FAT_16:
        if (eoc > 0xFFFF) {
            err = 0x10;
        } else {
            err = PFFAT16_WriteFATEntry(p_vol, cluster, eoc);
        }
        break;
    case FAT_32:
        if (eoc > 0xFFFFFFF) {
            err = 0x10;
        } else {
            err = PFFAT32_WriteFATEntry(p_vol, cluster, eoc);
        }
        break;
    default:
        err = 0xF;
        break;
    }
    if (err != 0) {
        return err;
    }
    err = PFCACHE_FlushFATCache(p_vol);
    if (err != 0) {
        return err;
    }
    err = PFCACHE_AllocateDataPage((PF_VOLUME*)p_vol, 0xFFFFFFFF, &p_page, &is_hit);
    if (err != 0) {
        return err;
    }
    VFipf_memset(p_page->p_buf, 0, p_vol->bpb.bytes_per_sector);
    sector = p_vol->bpb.first_data_sector + ((p_vol->bpb.root_dir_cluster - 2) << p_vol->bpb.log2_sectors_per_cluster);
    for (i = 0; i < p_vol->bpb.sectors_per_cluster; i++) {
        err = PFSEC_WriteData((PF_VOLUME*)p_vol, p_page->p_buf, sector + i, 0, p_vol->bpb.bytes_per_sector, &num_success, 0);
        if (err != 0) {
            PFCACHE_FreeDataPage((PF_VOLUME*)p_vol, p_page);
            return err;
        }
        if (num_success != p_vol->bpb.bytes_per_sector) {
            PFCACHE_FreeDataPage((PF_VOLUME*)p_vol, p_page);
            return 0x11;
        }
    }
    PFCACHE_FreeDataPage((PF_VOLUME*)p_vol, p_page);
    return 0;
}

s32 PFFAT16_WriteFATEntry(PF2_VOLUME* p_vol, u32 cluster, u32 value) {
    u32 offset = cluster * 2;
    u16 sector = p_vol->bpb.active_FAT_sector + (offset >> p_vol->bpb.log2_bytes_per_sector);
    u16 pos = offset & (p_vol->bpb.bytes_per_sector - 1);
    u16 buf;
    u32 i;
    s32 first_err = 0;

    buf = ((value >> 8) & 0xFF) | ((value << 8) & 0xFF00);
    for (i = 0; i < p_vol->bpb.num_active_FATs; i++) {
        s32 err = PFSEC_WriteFAT(p_vol, (u8*)&buf, sector, pos, 2);
        if (err != 0 && first_err == 0) {
            first_err = err;
        }
        sector += p_vol->bpb.sectors_per_FAT;
    }
    return first_err;
}

int PFFAT_CountAllocatedClusters(PF_FFD* p_ffd, u32 size, u32* p_num_alloc_clusters) {
    u32 start_cluster;
    u32 initial_cluster;
    u32 chain_index;
    s32 err;

    *p_num_alloc_clusters = 0;
    start_cluster = *p_ffd->p_start_cluster;
    if (start_cluster < 2) {
        return 0;
    }
    if (p_ffd->last_cluster.num_last_cluster != 0) {
        *p_num_alloc_clusters = p_ffd->last_cluster.max_chain_index + 1;
        return 0;
    }
    if (p_ffd->p_hint->cluster != 0) {
        if (size <= (p_ffd->p_hint->chain_index + 1) * (p_ffd->p_vol->bpb.bytes_per_sector * p_ffd->p_vol->bpb.sectors_per_cluster)) {
            *p_num_alloc_clusters = p_ffd->p_hint->chain_index + 1;
            return 0;
        }
        *p_num_alloc_clusters = p_ffd->p_hint->chain_index;
        initial_cluster = p_ffd->p_hint->cluster;
        chain_index = p_ffd->p_hint->chain_index + 1;
        size -= p_ffd->p_hint->chain_index * (p_ffd->p_vol->bpb.bytes_per_sector * p_ffd->p_vol->bpb.sectors_per_cluster);
    } else {
        initial_cluster = start_cluster;
        chain_index = 1;
    }
    err = PFFAT_GetClusterAllocatedInChain(p_ffd, initial_cluster, chain_index, size, p_num_alloc_clusters);
    if (err != 0) {
        return err;
    }
    return 0;
}

s32 PFFAT_GetClusterAllocatedInChain(PF_FFD* p_ffd, u32 initial_cluster, u32 chain_index, u32 size, u32* p_num_clusters) {
    PF_VOLUME* p_vol = p_ffd->p_vol;
    u32 next_cluster = -1;
    u32 chk_clstlnk = 1;
    u32 cluster = initial_cluster;
    u32 bytes = 0;
    s32 err;

    while (next_cluster != 0) {
        bytes += p_vol->bpb.bytes_per_sector * p_vol->bpb.sectors_per_cluster;
        (*p_num_clusters)++;
        if (bytes >= size) break;
        err = PFFAT_ReadCluster(p_ffd, cluster, chain_index, &next_cluster, &chk_clstlnk);
        if (err != 0) return err;
        if (next_cluster == lbl_8041B0E0[p_vol->bpb.fat_type].end_of_chain) {
            p_ffd->last_cluster.num_last_cluster = cluster;
            p_ffd->last_cluster.max_chain_index = chain_index - 1;
            break;
        }
        cluster = next_cluster;
        chain_index++;
    }
    return 0;
}

static inline s32 pffat_read_entry_with_buf(PF2_VOLUME* p_vol, u32 cluster, u32* p_value, PF_CACHE_PAGE* p_page) {
    switch (p_vol->bpb.fat_type) {
    case FAT_12:
        return PFFAT12_ReadFATEntryWithBuf(p_vol, (u16)cluster, p_value, p_page);
    case FAT_16:
        return PFFAT16_ReadFATEntryWithBuf(p_vol, cluster, p_value, p_page);
    case FAT_32:
        return PFFAT32_ReadFATEntryWithBuf(p_vol, cluster, p_value, p_page);
    default:
        return 15;
    }
}

s32 PFFAT_getBeforeChain(PF2_VOLUME* p_vol, u32 cluster, u32 target_cluster, u32* p_before_cluster) {
    u32 next_cluster;
    PF_CACHE_PAGE* p_page;
    u32 is_hit;
    u32 limit = lbl_8041B0E0[p_vol->bpb.fat_type].unk4;
    s32 err;

    *p_before_cluster = -1;
    err = PFCACHE_AllocateDataPage((PF_VOLUME*)p_vol, -1, &p_page, &is_hit);
    if (err != 0) return err;
    p_page->option = 0;
    while (cluster < limit) {
        err = pffat_read_entry_with_buf(p_vol, cluster, &next_cluster, p_page);
        if (err != 0) {
            PFCACHE_FreeDataPage((PF_VOLUME*)p_vol, p_page);
            return err;
        }
        if (target_cluster == next_cluster) {
            *p_before_cluster = cluster;
            return 0;
        }
        cluster--;
        if (!(cluster >= 2 && cluster < p_vol->bpb.num_clusters + 2) && cluster < limit) {
            *p_before_cluster = target_cluster;
            return 0;
        }
    }
    PFCACHE_FreeDataPage((PF_VOLUME*)p_vol, p_page);
    return 13;
}

s32 PFFAT_FindClusterLink(PF_FFD* p_ffd, u32 chain_index, u32* p_cluster, u32* is_found) {
    u32 next_cluster = 0xFFFFFFFF;
    u32 cluster;
    u32 remaining;
    u32 index;
    s32 err;

    *is_found = 0;
    if (p_ffd->cluster_link.position == 0) return 0;
    if (p_ffd->cluster_link.save_index >= chain_index) {
        index = chain_index / (p_ffd->cluster_link.interval + 1);
        remaining = chain_index % (p_ffd->cluster_link.interval + 1);
        if (remaining == 0) {
            *p_cluster = p_ffd->cluster_link.buffer[index];
            *is_found = 1;
        } else {
            cluster = p_ffd->cluster_link.buffer[index];
            while (remaining != 0) {
                switch (p_ffd->p_vol->bpb.fat_type) {
                case FAT_12:
                    err = fn_803E2468((struct Ctx*)p_ffd->p_vol, (u16)cluster, (int*)&next_cluster);
                    break;
                case FAT_16:
                    err = PFFAT16_ReadFATEntry((PF2_VOLUME*)p_ffd->p_vol, cluster, &next_cluster);
                    break;
                case FAT_32:
                    err = PFFAT32_ReadFATEntry((PF2_VOLUME*)p_ffd->p_vol, cluster, &next_cluster);
                    break;
                default:
                    next_cluster = -1;
                    err = 15;
                    break;
                }
                if (err != 0) return err;
                cluster = next_cluster;
                if (cluster == 0) return 13;
                remaining--;
            }
            if (next_cluster == 0) return 13;
            if (next_cluster == lbl_8041B0E0[p_ffd->p_vol->bpb.fat_type].end_of_chain) return 0;
            *p_cluster = next_cluster;
            *is_found = 1;
        }
    } else {
        return 0;
    }
    return 0;
}

static inline s32 pffat_specify_cluster(PF_FFD* p_ffd, u32 chain_index, u32 may_allocate, u32* p_cluster) {
    u32 locate_start;
    u32 locate_end;
    s32 err;

    *p_cluster = 0xFFFFFFFF;
    err = PFFAT_GetClusterInChain(p_ffd, chain_index, may_allocate ? 1 : 0, 0, &locate_start, &locate_end);
    if (err != 0) return err;
    if (locate_end >= 2 && locate_end < p_ffd->p_vol->bpb.num_clusters + 2) *p_cluster = locate_end;
    return 0;
}

s32 PFFAT_GetClusterSpecified(PF_FFD* p_ffd, u32 chain_index, u32 may_allocate, u32* p_cluster) {
    PF_FAT_HINT* p_hint;
    PF_VOLUME* p_vol;
    s32 err;
    u32 found;
    u32 fat_type;

    p_vol = p_ffd->p_vol;
    fat_type = p_vol->bpb.fat_type;
    p_hint = p_ffd->p_hint;
    if (*p_ffd->p_start_cluster == 0 && !may_allocate) {
        *p_cluster = 0xFFFFFFFF;
        return 0;
    }
    if (*p_ffd->p_start_cluster == 1) {
        if (fat_type <= 1) {
            *p_cluster = 0xFFFFFFFF;
            return 0;
        }
        *p_ffd->p_start_cluster = p_vol->bpb.root_dir_cluster;
    }
    if (p_ffd->cluster_link.buffer != NULL && !may_allocate) {
        err = PFFAT_FindClusterLink(p_ffd, chain_index, p_cluster, &found);
        if (err != 0) return err;
        if (found == 1) {
            p_hint->file_version = p_ffd->file_version;
            p_hint->chain_index = chain_index;
            p_hint->cluster = *p_cluster;
            return 0;
        }
    }
    err = pffat_specify_cluster(p_ffd, chain_index, may_allocate, p_cluster);
    if (err != 0) return err;
    if (*p_cluster == 0xFFFFFFFF) return 0;
    p_hint->file_version = p_ffd->file_version;
    p_hint->chain_index = chain_index;
    p_hint->cluster = *p_cluster;
    return 0;
}

s32 PFFAT_FindClusterLinkWithBuf(PF_FFD* p_ffd, u32 chain_index, u32* p_cluster, u32* is_found, PF_CACHE_PAGE* p_page) {
    u32 next_cluster = 0xFFFFFFFF;
    u32 remaining;
    u32 cluster;
    u32 index;
    s32 err;

    *is_found = 0;
    if (p_ffd->cluster_link.position == 0) return 0;
    if (p_ffd->cluster_link.save_index >= chain_index) {
        index = chain_index / (p_ffd->cluster_link.interval + 1);
        remaining = chain_index % (p_ffd->cluster_link.interval + 1);
        if (remaining == 0) {
            *p_cluster = p_ffd->cluster_link.buffer[index];
            *is_found = 1;
        } else {
            cluster = p_ffd->cluster_link.buffer[index];
            while (remaining != 0) {
                switch (p_ffd->p_vol->bpb.fat_type) {
                case FAT_12:
                    err = PFFAT12_ReadFATEntryWithBuf((PF2_VOLUME*)p_ffd->p_vol, (u16)cluster, &next_cluster, p_page);
                    break;
                case FAT_16:
                    err = PFFAT16_ReadFATEntryWithBuf((PF2_VOLUME*)p_ffd->p_vol, cluster, &next_cluster, p_page);
                    break;
                case FAT_32:
                    err = PFFAT32_ReadFATEntryWithBuf((PF2_VOLUME*)p_ffd->p_vol, cluster, &next_cluster, p_page);
                    break;
                default:
                    err = 15;
                    break;
                }
                if (err != 0) return err;
                cluster = next_cluster;
                if (cluster == 0) return 13;
                remaining--;
            }
            if (next_cluster == 0) return 13;
            if (next_cluster == lbl_8041B0E0[p_ffd->p_vol->bpb.fat_type].end_of_chain) return 0;
            *p_cluster = next_cluster;
            *is_found = 1;
        }
    } else {
        return 0;
    }
    return 0;
}

s32 PFFAT12_WriteFATEntry(PF2_VOLUME* p_vol, u16 cluster, u16 value) {
    s32 err;
    u16 fat_offset = (u16)(cluster + (cluster >> 1));
    u16 fat_sector = (u16)(p_vol->bpb.active_FAT_sector + (fat_offset >> p_vol->bpb.log2_bytes_per_sector));
    u16 offset_in_sector = (u16)(fat_offset & (p_vol->bpb.bytes_per_sector - 1));
    u16 word;
    u8 buf[2];
    u16 i;
    s32 first_err;

    if (cluster & 1) {
        err = PFSEC_ReadFAT(p_vol, buf, fat_sector, offset_in_sector, 1);
        if (err != 0) return err;
        word = (u16)((value << 4) + (buf[0] & 0xF));
    } else {
        u16 fat_sector2 = (u16)(p_vol->bpb.active_FAT_sector + ((fat_offset + 1) >> p_vol->bpb.log2_bytes_per_sector));
        u16 offset_in_sector2 = (u16)((fat_offset + 1) & (p_vol->bpb.bytes_per_sector - 1));
        err = PFSEC_ReadFAT(p_vol, buf, fat_sector2, offset_in_sector2, 1);
        if (err != 0) return err;
        word = (u16)(((buf[0] << 8) & 0xF000) + (value & 0xFFF));
    }
    *(u16*)buf = ((word >> 8) & 0xFF) | ((word << 8) & 0xFF00);
    first_err = 0;
    for (i = 0; i < p_vol->bpb.num_active_FATs; i++) {
        if (offset_in_sector < p_vol->bpb.bytes_per_sector - 1) {
            err = PFSEC_WriteFAT(p_vol, buf, fat_sector, offset_in_sector, 2);
            if (err != 0 && first_err == 0) first_err = err;
        } else {
            err = PFSEC_WriteFAT(p_vol, buf, fat_sector, offset_in_sector, 1);
            if (err != 0 && first_err == 0) first_err = err;
            err = PFSEC_WriteFAT(p_vol, &buf[1], fat_sector + 1, 0, 1);
            if (err != 0 && first_err == 0) first_err = err;
        }
        fat_sector += p_vol->bpb.sectors_per_FAT;
    }
    return first_err;
}

s32 PFFAT_ReadClusterWithBuf(PF_FFD* p_ffd, u32 cluster, u32 chain_index, u32* p_next_cluster, u32* p_chk_clstlnk, PF_CACHE_PAGE* p_page) {
    s32 err;
    u32 next_cluster;

    *p_next_cluster = 0xFFFFFFFF;
    if (p_ffd->cluster_link.buffer != NULL && *p_chk_clstlnk == 1) {
        err = PFFAT_FindClusterLinkWithBuf(p_ffd, chain_index, p_next_cluster, p_chk_clstlnk, p_page);
        if (err != 0) {
            return err;
        }
    }
    if (p_ffd->cluster_link.buffer == NULL || *p_chk_clstlnk == 0) {
        switch (p_ffd->p_vol->bpb.fat_type) {
        case FAT_12:
            err = PFFAT12_ReadFATEntryWithBuf((PF2_VOLUME*)p_ffd->p_vol, (u16)cluster, p_next_cluster, p_page);
            break;
        case FAT_16:
            err = PFFAT16_ReadFATEntryWithBuf((PF2_VOLUME*)p_ffd->p_vol, cluster, p_next_cluster, p_page);
            break;
        case FAT_32:
            err = PFFAT32_ReadFATEntryWithBuf((PF2_VOLUME*)p_ffd->p_vol, cluster, p_next_cluster, p_page);
            break;
        default:
            err = 15;
            break;
        }
        if (err != 0) {
            return err;
        }
        next_cluster = *p_next_cluster;
        if (next_cluster == 0) {
            return 13;
        }
        if (next_cluster == 0xFFFFFFFF) {
            return 6;
        }
        if (p_ffd->cluster_link.buffer != NULL && next_cluster != lbl_8041B0E0[p_ffd->p_vol->bpb.fat_type].end_of_chain &&
            p_ffd->cluster_link.max_count > p_ffd->cluster_link.position && p_ffd->cluster_link.max_count != 0) {
            if (chain_index == p_ffd->cluster_link.position * (p_ffd->cluster_link.interval + 1)) {
                p_ffd->cluster_link.interval_offset = 0;
                p_ffd->cluster_link.buffer[p_ffd->cluster_link.position] = next_cluster;
                p_ffd->cluster_link.position++;
            } else {
                p_ffd->cluster_link.interval_offset++;
            }
            p_ffd->cluster_link.save_index = chain_index;
        }
    }
    return 0;
}

s32 PFFAT12_WriteFATEntryWithBuf(PF2_VOLUME* p_vol, u16 cluster, u16 value, PF_CACHE_PAGE* p_page);

s32 PFFAT_WriteClusterWithBuf(PF_FFD* p_ffd, u32 cluster, u32 chain_index, u32 next_cluster, u32 use_clstlnk, PF_CACHE_PAGE* p_page) {
    u32 index;
    u32 interval;
    u32 found_cluster;
    u32* buffer;
    s32 err;

    switch (p_ffd->p_vol->bpb.fat_type) {
    case FAT_12:
        if (next_cluster > 0xFFF) err = 16;
        else err = PFFAT12_WriteFATEntryWithBuf((PF2_VOLUME*)p_ffd->p_vol, (u16)cluster, (u16)next_cluster, p_page);
        break;
    case FAT_16:
        if (next_cluster > 0xFFFF) err = 16;
        else err = PFFAT16_WriteFATEntryWithBuf((PF2_VOLUME*)p_ffd->p_vol, cluster, next_cluster, p_page);
        break;
    case FAT_32:
        if (next_cluster > 0xFFFFFFF) err = 16;
        else err = PFFAT32_WriteFATEntryWithBuf((PF2_VOLUME*)p_ffd->p_vol, cluster, next_cluster, p_page);
        break;
    default:
        err = 15;
        break;
    }
    if (err != 0) return err;
    buffer = p_ffd->cluster_link.buffer;
    if (buffer != NULL && use_clstlnk == 1) {
        if (next_cluster == 0) {
            interval = p_ffd->cluster_link.interval + 1;
            index = chain_index / interval;
            if (p_ffd->cluster_link.max_count >= index) {
                if (chain_index % interval == 0) buffer[index] = 0;
                if (p_ffd->cluster_link.save_index >= chain_index) {
                    p_ffd->cluster_link.save_index = chain_index - 1;
                    p_ffd->cluster_link.position = (chain_index - 1) / (p_ffd->cluster_link.interval + 1);
                    p_ffd->cluster_link.interval_offset = (chain_index - 1) % (p_ffd->cluster_link.interval + 1);
                }
            }
        } else if (cluster != lbl_8041B0E0[p_ffd->p_vol->bpb.fat_type].end_of_chain) {
            err = PFFAT_FindClusterLinkWithBuf(p_ffd, chain_index, &found_cluster, &use_clstlnk, p_page);
            if (err != 0) return err;
            if (!use_clstlnk && p_ffd->cluster_link.max_count > p_ffd->cluster_link.position && p_ffd->cluster_link.max_count != 0) {
                if (chain_index == p_ffd->cluster_link.position * (p_ffd->cluster_link.interval + 1)) {
                    p_ffd->cluster_link.interval_offset = 0;
                    p_ffd->cluster_link.buffer[p_ffd->cluster_link.position] = cluster;
                    p_ffd->cluster_link.position++;
                } else {
                    p_ffd->cluster_link.interval_offset++;
                }
                p_ffd->cluster_link.save_index = chain_index;
            }
        }
    }
    return 0;
}

s32 PFFAT_WriteCluster(PF_FFD* p_ffd, u32 cluster, u32 chain_index, u32 next_cluster, u32 use_clstlnk) {
    u32 index;
    u32 interval;
    u32 found_cluster;
    u32* buffer;
    s32 err;

    switch (p_ffd->p_vol->bpb.fat_type) {
    case FAT_12:
        if (next_cluster > 0xFFF) err = 16;
        else err = PFFAT12_WriteFATEntry((PF2_VOLUME*)p_ffd->p_vol, (u16)cluster, (u16)next_cluster);
        break;
    case FAT_16:
        if (next_cluster > 0xFFFF) err = 16;
        else err = PFFAT16_WriteFATEntry((PF2_VOLUME*)p_ffd->p_vol, cluster, next_cluster);
        break;
    case FAT_32:
        if (next_cluster > 0xFFFFFFF) err = 16;
        else err = PFFAT32_WriteFATEntry((PF2_VOLUME*)p_ffd->p_vol, cluster, next_cluster);
        break;
    default:
        err = 15;
        break;
    }
    if (err != 0) return err;
    buffer = p_ffd->cluster_link.buffer;
    if (buffer != NULL && use_clstlnk == 1) {
        if (next_cluster == 0) {
            interval = p_ffd->cluster_link.interval + 1;
            index = chain_index / interval;
            if (p_ffd->cluster_link.max_count >= index) {
                if (chain_index % interval == 0) buffer[index] = 0;
                if (p_ffd->cluster_link.save_index >= chain_index) {
                    p_ffd->cluster_link.save_index = chain_index - 1;
                    p_ffd->cluster_link.position = (chain_index - 1) / (p_ffd->cluster_link.interval + 1);
                    p_ffd->cluster_link.interval_offset = (chain_index - 1) % (p_ffd->cluster_link.interval + 1);
                }
            }
        } else if (cluster != lbl_8041B0E0[p_ffd->p_vol->bpb.fat_type].end_of_chain) {
            err = PFFAT_FindClusterLink(p_ffd, chain_index, &found_cluster, &use_clstlnk);
            if (err != 0) return err;
            if (!use_clstlnk && p_ffd->cluster_link.max_count > p_ffd->cluster_link.position && p_ffd->cluster_link.max_count != 0) {
                if (chain_index == p_ffd->cluster_link.position * (p_ffd->cluster_link.interval + 1)) {
                    p_ffd->cluster_link.interval_offset = 0;
                    p_ffd->cluster_link.buffer[p_ffd->cluster_link.position] = cluster;
                    p_ffd->cluster_link.position++;
                } else {
                    p_ffd->cluster_link.interval_offset++;
                }
                p_ffd->cluster_link.save_index = chain_index;
            }
        }
    }
    return 0;
}

s32 PFFAT_RefreshFSINFO(PF2_VOLUME* p_vol);

s32 PFFAT_CountFreeClusters(PF2_VOLUME* p_vol, u32* p_num_free_clusters) {
    s32 err;
    u32 cluster;
    u32 value;
    u32 is_hit;
    PF_CACHE_PAGE* p_page;

    if ((p_vol->fsi_flag & 4) != 0 && p_vol->num_free_clusters != 0xFFFFFFFF) {
        *p_num_free_clusters = p_vol->num_free_clusters;
        return 0;
    }
    *p_num_free_clusters = 0;
    cluster = 2;
    err = PFCACHE_AllocateDataPage((PF_VOLUME*)p_vol, 0xFFFFFFFF, &p_page, &is_hit);
    if (err != 0) return err;
    err = PFFAT_ReadFATSector(p_vol, p_page, 2);
    if (err != 0) {
        PFCACHE_FreeDataPage((PF_VOLUME*)p_vol, p_page);
        return err;
    }
    p_page->option = 0;
    for (; cluster >= 2 && cluster < p_vol->bpb.num_clusters + 2; cluster++) {
        PF_CACHE_PAGE* p_fat_page = p_page;
        switch (p_vol->bpb.fat_type) {
        case FAT_12:
            err = PFFAT12_ReadFATEntryWithBuf(p_vol, (u16)cluster, &value, p_fat_page);
            break;
        case FAT_16:
            err = PFFAT16_ReadFATEntryWithBuf(p_vol, cluster, &value, p_fat_page);
            break;
        case FAT_32:
            err = PFFAT32_ReadFATEntryWithBuf(p_vol, cluster, &value, p_fat_page);
            break;
        default:
            err = 15;
            break;
        }
        if (err != 0) {
            PFCACHE_FreeDataPage((PF_VOLUME*)p_vol, p_page);
            return err;
        }
        if (value == 0) (*p_num_free_clusters)++;
    }
    PFCACHE_FreeDataPage((PF_VOLUME*)p_vol, p_page);
    p_vol->fsi_flag |= 4;
    p_vol->num_free_clusters = *p_num_free_clusters;
    if (p_vol->bpb.fat_type == FAT_32 && (p_vol->fsi_flag & 2) != 0) {
        if (PFFAT_RefreshFSINFO(p_vol) != 0) {
            p_vol->num_free_clusters = 0xFFFFFFFF;
            p_vol->fsi_flag &= ~4;
        }
    }
    return 0;
}

s32 PFFAT_SearchForNumFreeClusters(PF2_VOLUME* p_vol, u32 start_cluster, u32 end_cluster, u32 num_cluster, u32* p_start_free_cluster, u32* p_last_free_cluster) {
    u32 limit;
    u32 num_free;
    u32 best_start;
    u32 best_num;
    u32 wrapped;
    u32 first_cluster;
    u32 value;
    u32 is_hit;
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
    err = PFCACHE_AllocateDataPage((PF_VOLUME*)p_vol, 0xFFFFFFFF, &p_page, &is_hit);
    if (err != 0) return err;
    err = PFFAT_ReadFATSector(p_vol, p_page, start_cluster);
    if (err != 0) {
        PFCACHE_FreeDataPage((PF_VOLUME*)p_vol, p_page);
        return err;
    }
    p_page->option = 0;

    while (start_cluster <= end_cluster) {
        PF_CACHE_PAGE* p_fat_page;
        if (limit <= start_cluster) {
            start_cluster = 2;
        }
        p_fat_page = p_page;
        switch (p_vol->bpb.fat_type) {
        case FAT_12:
            err = PFFAT12_ReadFATEntryWithBuf(p_vol, (u16)start_cluster, &value, p_fat_page);
            break;
        case FAT_16:
            err = PFFAT16_ReadFATEntryWithBuf(p_vol, start_cluster, &value, p_fat_page);
            break;
        case FAT_32:
            err = PFFAT32_ReadFATEntryWithBuf(p_vol, start_cluster, &value, p_fat_page);
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
                PFCACHE_FreeDataPage((PF_VOLUME*)p_vol, p_page);
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
    PFCACHE_FreeDataPage((PF_VOLUME*)p_vol, p_page);
    return 0;
}

s32 PFCACHE_AllocateFATPage(PF_VOLUME* p_vol, u32 sector, PF_CACHE_PAGE** pp_page, u32* p_is_hit);
s32 PFCACHE_WriteFATSectorAndFreeIfNeeded(PF_VOLUME* p_vol, const u8* p_buf, u32 sector);
void PFCACHE_FreeFATPage(PF_VOLUME* p_vol, PF_CACHE_PAGE* p_page);

s32 PFFAT_InitFATRegion(PF2_VOLUME* p_vol) {
    u32 sector;
    u32 value;
    struct PF_CACHE_PAGE* p_page;
    u32 is_hit;
    s32 err;

    err = PFCACHE_AllocateFATPage((PF_VOLUME*)p_vol, -1, &p_page, &is_hit);
    if (err) {
        return err;
    }

    VFipf_memset(p_page->p_buf, 0, p_vol->bpb.bytes_per_sector);
    for (sector = p_vol->bpb.num_reserved_sectors; sector < p_vol->bpb.first_root_dir_sector; sector++) {
        err = PFCACHE_WriteFATSectorAndFreeIfNeeded((PF_VOLUME*)p_vol, p_page->p_buf, sector);
        if (err) {
            PFCACHE_FreeFATPage((PF_VOLUME*)p_vol, p_page);
            return err;
        }
    }

    PFCACHE_FreeFATPage((PF_VOLUME*)p_vol, p_page);

    value = p_vol->bpb.media | lbl_8041B0E0[p_vol->bpb.fat_type].unkC;
    switch (p_vol->bpb.fat_type) {
    case FAT_12:
        if (value > 0xFFF) err = 16;
        else err = PFFAT12_WriteFATEntry(p_vol, 0, (u16)value);
        break;
    case FAT_16:
        if (value > 0xFFFF) err = 16;
        else err = PFFAT16_WriteFATEntry(p_vol, 0, value);
        break;
    case FAT_32:
        if (value > 0xFFFFFFF) err = 16;
        else err = PFFAT32_WriteFATEntry(p_vol, 0, value);
        break;
    default:
        err = 15;
        break;
    }
    if (err) {
        return err;
    }
    value = lbl_8041B0E0[p_vol->bpb.fat_type].unk10;
    switch (p_vol->bpb.fat_type) {
    case FAT_12:
        if (value > 0xFFF) err = 16;
        else err = PFFAT12_WriteFATEntry(p_vol, 1, (u16)value);
        break;
    case FAT_16:
        if (value > 0xFFFF) err = 16;
        else err = PFFAT16_WriteFATEntry(p_vol, 1, value);
        break;
    case FAT_32:
        if (value > 0xFFFFFFF) err = 16;
        else err = PFFAT32_WriteFATEntry(p_vol, 1, value);
        break;
    default:
        err = 15;
        break;
    }
    if (err) {
        return err;
    }
    err = PFCACHE_FlushFATCache(p_vol);
    if (err) {
        return err;
    }
    return 0;
}

s32 PFFAT_DoAllocateChain(PF_FFD* p_ffd, u32 chain_len, u32 chain_index, u32* p_chain_start, u32* p_last_allocated, u32 reset_cluster);
s32 PFFAT_FreeChain(PF_FFD* p_ffd, u32 cluster, u32 chain_index, u32 size);
s32 PFFAT_RefreshFSINFO(PF2_VOLUME* p_vol);

static inline s32 PFFAT_AllocateChain(PF_FFD* p_ffd, u32 chain_len, u32 chain_index, u32* p_chain_start, u32* p_last_allocated) {
    s32 err;

    err = PFFAT_DoAllocateChain(p_ffd, chain_len, chain_index, p_chain_start, p_last_allocated, 0);
    if (err) {
        if (*p_chain_start != -1) {
            PFFAT_FreeChain(p_ffd, *p_chain_start, chain_index, -1);
        }
        *p_chain_start = -1;
        *p_last_allocated = -1;
        return err;
    }
    return 0;
}

s32 PFFAT_GetClusterInChain(struct PF_FFD* p_ffd, u32 chain_index, u32 mode, u32 num_cluster, u32* locate_start, u32* locate_end) {
    s32 err;
    u32 bad;
    u32 eoc1;
    u32 current_cluster;
    u32 next_cluster;
    u32 trace_cnt;
    u32 append_cnt;
    u32 search_index;
    u32* p_chain_start;
    u32 chain_start;
    u32 is_hit;
    u32 check_use;
    struct PF_CACHE_PAGE* p_page;

    bad = lbl_8041B0E0[p_ffd->p_vol->bpb.fat_type].unk0;
    eoc1 = lbl_8041B0E0[p_ffd->p_vol->bpb.fat_type].unk4;
    *locate_end = -1;
    *locate_start = -1;

    if (p_ffd->last_access_cluster.chain_index != 0 && p_ffd->last_access_cluster.chain_index <= chain_index) {
        chain_start = p_ffd->last_access_cluster.cluster;
        p_chain_start = &chain_start;
        trace_cnt = chain_index - p_ffd->last_access_cluster.chain_index;
        search_index = p_ffd->last_access_cluster.chain_index + 1;
    } else {
        p_chain_start = p_ffd->p_start_cluster;
        trace_cnt = chain_index;
        search_index = 1;
    }

    current_cluster = *p_chain_start;
    next_cluster = *p_chain_start;

    if (mode == 2) {
        if (trace_cnt != 0 && num_cluster != 0) {
            trace_cnt += num_cluster - 1;
        } else {
            trace_cnt = num_cluster;
        }
        append_cnt = trace_cnt;
        if (p_ffd->last_cluster.num_last_cluster != 0) {
            trace_cnt = 0;
            append_cnt = p_ffd->last_cluster.max_chain_index + num_cluster - chain_index;
            current_cluster = p_ffd->last_cluster.num_last_cluster;
            next_cluster = lbl_8041B0E0[p_ffd->p_vol->bpb.fat_type].end_of_chain;
            search_index = p_ffd->last_cluster.max_chain_index + 1;
        }
    } else {
        if (mode == 1 && current_cluster == 0) {
            trace_cnt++;
        }
        append_cnt = trace_cnt;
    }

    if (trace_cnt != 0) {
        err = PFCACHE_AllocateDataPage(p_ffd->p_vol, -1, &p_page, &is_hit);
        if (err) return err;
        err = PFFAT_ReadFATSector((PF2_VOLUME*)p_ffd->p_vol, p_page, current_cluster);
        if (err) {
            PFCACHE_FreeDataPage(p_ffd->p_vol, p_page);
            return err;
        }
        p_page->option = 0;
        check_use = 1;

        while (trace_cnt-- != 0 && current_cluster < bad) {
            if (current_cluster != 0) {
                err = PFFAT_ReadClusterWithBuf(p_ffd, current_cluster, search_index, &next_cluster, &check_use, p_page);
                if (err != 0 && next_cluster != (u32)-1) {
                    PFCACHE_FreeDataPage(p_ffd->p_vol, p_page);
                    return err;
                }
            } else {
                search_index--;
                next_cluster = lbl_8041B0E0[p_ffd->p_vol->bpb.fat_type].end_of_chain;
            }

            if ((next_cluster < 2 || next_cluster >= (p_ffd->p_vol->bpb.num_clusters + 2)) && next_cluster < eoc1) {
                PFCACHE_FreeDataPage(p_ffd->p_vol, p_page);
                return 14;
            }

            if (mode != 0 && next_cluster >= eoc1) {
                break;
            }

            current_cluster = next_cluster;
            search_index++;
        }
        append_cnt = trace_cnt;
        PFCACHE_FreeDataPage(p_ffd->p_vol, p_page);
    }

    if (mode != 0 && next_cluster >= eoc1) {
        *locate_start = 0;
        err = PFFAT_AllocateChain(p_ffd, append_cnt + 1, search_index, locate_start, &next_cluster);
        if (err) {
            return err;
        }
        if (next_cluster == -1) {
            return 6;
        }
        if (*p_chain_start == 0) {
            *p_chain_start = *locate_start;
        } else {
            err = PFFAT_WriteCluster(p_ffd, current_cluster, search_index - 1, *locate_start, 1);
            if (err) {
                return err;
            }
        }
        current_cluster = next_cluster;
    }

    *locate_end = current_cluster;
    return 0;
}

static inline s32 PFFAT_UpdateFATEntry(PF2_VOLUME* p_vol, PF_CACHE_PAGE* p_page) {
    u16 i;
    u32 sector = p_page->sector;
    s32 first_err = 0;
    for (i = 0; i < p_vol->bpb.num_active_FATs; i++) {
        s32 err = PFSEC_WriteFAT(p_vol, p_page->p_buf, sector, 0, p_vol->bpb.bytes_per_sector);
        if (err != 0 && first_err == 0) first_err = err;
        sector += p_vol->bpb.sectors_per_FAT;
    }
    return first_err;
}

s32 PFFAT_DoAllocateChain(struct PF_FFD* p_ffd, u32 chain_len, u32 chain_index, u32* p_chain_start, u32* p_last_allocated, u32 reset_cluster) {
    PF2_VOLUME* p_vol;
    u32 eoc2;
    enum FatType fat_type;
    u32 first_allocated_cluster;
    u32 last_allocated_cluster;
    u32 start_cluster;
    s32 err;
    u32 is_hit;
    struct PF_CACHE_PAGE* p_page;

    u32 i;

    p_vol = (PF2_VOLUME*)p_ffd->p_vol;
    fat_type = p_vol->bpb.fat_type;
    eoc2 = lbl_8041B0E0[fat_type].end_of_chain;
    *p_chain_start = -1;
    *p_last_allocated = -1;
    last_allocated_cluster = -1;

    err = PFFAT_SearchForNumFreeClusters(p_vol, p_vol->last_free_cluster, -1, chain_len, &first_allocated_cluster, &last_allocated_cluster);
    if (err) {
        return err;
    }
    if (first_allocated_cluster == -1) {
        return 0;
    }

    if (reset_cluster) {
        err = PFFAT_ResetCluster((PF_VOLUME*)p_vol, first_allocated_cluster);
        if (err) return err;
    }
    err = PFCACHE_AllocateDataPage((PF_VOLUME*)p_vol, -1, &p_page, &is_hit);
    if (err) return err;
    err = PFFAT_ReadFATSector(p_vol, p_page, first_allocated_cluster);
    if (err) {
        PFCACHE_FreeDataPage((PF_VOLUME*)p_vol, p_page);
        return err;
    }

    p_page->option = 0;
    for (i = first_allocated_cluster; i < last_allocated_cluster; i++) {
        err = PFFAT_WriteClusterWithBuf(p_ffd, i, chain_index, i + 1, 1, p_page);
        if (err) {
            PFCACHE_FreeDataPage((PF_VOLUME*)p_vol, p_page);
            return err;
        }
        if ((p_vol->fsi_flag & 4) && p_vol->num_free_clusters != -1 && p_vol->num_free_clusters != 0) {
            if (p_vol->bpb.fat_type != FAT_32 || last_allocated_cluster != p_vol->bpb.root_dir_cluster) {
                p_vol->num_free_clusters--;
            }
        }
        if (*p_chain_start == -1) {
            *p_chain_start = i;
        }
        *p_last_allocated = i;
        chain_index++;
    }

    if (last_allocated_cluster != -1) {
        err = PFFAT_WriteClusterWithBuf(p_ffd, last_allocated_cluster, chain_index, eoc2, 1, p_page);
        if (err) {
            PFCACHE_FreeDataPage((PF_VOLUME*)p_vol, p_page);
            return err;
        }
        if (*p_chain_start == -1) {
            *p_chain_start = i;
        }
        *p_last_allocated = i;
        if (p_vol->num_free_clusters != -1 && p_vol->num_free_clusters != 0) {
            if (p_vol->bpb.fat_type != FAT_32 || last_allocated_cluster != p_vol->bpb.root_dir_cluster) {
                p_vol->num_free_clusters--;
            }
        }
    }

    p_vol->last_free_cluster = last_allocated_cluster + 1;
    p_ffd->last_cluster.num_last_cluster = last_allocated_cluster;
    p_ffd->last_cluster.max_chain_index = chain_index;
    *p_last_allocated = last_allocated_cluster;

    err = PFFAT_UpdateFATEntry(p_vol, p_page);
    if (err != 0) {
        PFCACHE_FreeDataPage((PF_VOLUME*)p_vol, p_page);
        return err;
    }
    PFCACHE_FreeDataPage((PF_VOLUME*)p_vol, p_page);

    if (p_vol->bpb.fat_type == FAT_32 && (p_vol->fsi_flag & 2)) {
        if (PFFAT_RefreshFSINFO(p_vol)) {
            p_vol->num_free_clusters = -1;
            p_vol->fsi_flag &= ~4;
        }
    }
    return 0;
}
