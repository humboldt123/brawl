#ifndef FAT_PF2_H
#define FAT_PF2_H

// The second PrFile2 build in main.dol: the SD card file system behind the FA
// API. It is the same eSOL code as the VF copy (include/FAT/fat_vf.h) but
// configured differently (four contexts; its PF_VOLUME is 0x1FC8 bytes and
// its volume set keeps last_error at 0x40), so only the layouts that are
// evidenced as unchanged are shared with the VF headers. The other handles
// stay opaque. Our symbols do not name most of these functions; the PrFile2
// API names are noted next to the placeholders (HYPOTHESIS: by behavior).
#include <types.h>
#include <revolution/FA/FA.h>
#include <revolution/FA/FAFstat.h>
#include <ogws/revolution/VF/vf_struct.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PF2_VOLUME PF2_VOLUME;
typedef struct PF2_FILE PF2_FILE;
typedef struct PF2_DIR PF2_DIR;
typedef struct PF2_STR PF2_STR;
typedef struct PF2_DIR_ENT PF2_DIR_ENT;
typedef struct PF2_ENT_ITER PF2_ENT_ITER;
typedef struct PF2_DEV_INF PF2_DEV_INF;

// PrFile2 critical section (pf_critical): a recursive lock owned by a context
typedef struct PF_CRITICAL {
    u32 unk0;
    s32 count;      // 0x4 recursion depth
    s32 resource;   // 0x8 lock resource id
    u32 owner;      // 0xC owning context
} PF_CRITICAL;

// FA callbacks (lbl_80598080): notified with a drive letter
typedef struct FACallbacks {
    void (*unk0)(s8 drv_char);
    void (*unk4)(s8 drv_char);
    u32 unk8[4];
} FACallbacks;
extern FACallbacks lbl_80598080;

// pf_apicmn
s32 fn_803EB48C(s32 err);                                    // PFAPI_convertError
int PFAPI_convertReturnValue(int err);
s32 PFSTR_InitStr(PF_STR* p_str, const char* s, u32 code_mode);
int fn_803EB5D8(PF2_DIR* dir);                               // pf_closedir
int fn_803EB5FC(void);                                       // pf_errnum
u16 fn_803D68B8(PF_STR* p_str);                              // PFSTR_StrLen
s32 PFSTR_StrCmp(const PF_STR* p_str, const s8* s);
u32 PFSTR_StrNumChar(PF_STR* p_str, u32 target);
void PFSTR_MoveStrPos(PF_STR* p_str, s16 num_s8);
void PFSTR_ToUpperNStr(PF_STR* p_str, u16 num, s8* dest);
int fn_803EB620(PF2_FILE* file);                             // pf_fclose
u32 fn_803EB6E0(void* buf, u32 size, u32 count, PF2_FILE* file);  // pf_fread
int fn_803EB708(PF2_FILE* file, s32 offset, s32 origin);     // pf_fseek
int fn_803EB72C(const char* pattern, int attr, FAEntryInfo* dta);  // pf_fsfirst
int fn_803EB7A0(FAEntryInfo* dta);                           // pf_fsnext
int fn_803EB7C4(const char* path, FAStat* st);               // pf_fstat
u32 fn_803EB824(void* buf, u32 size, u32 count, PF2_FILE* file);  // pf_fwrite
int fn_803EB84C(s8 drv_char);                                // pf_format
int fn_803EB874(void* config, void* param);                  // pf_init_prfile2 (both arguments unused)
int fn_803EB8B4(s8 drv_char);                                // pf_mount
int fn_803EB8F8(const char* path);                           // pf_mkdir
int fn_803EB948(s8 drv_char);                                // pf_detach
PF2_DIR* fn_803EB970(const char* path);                      // pf_opendir
int fn_803EB9C8(const char* path);                           // pf_remove
s32 fn_803EBA18(s8 drv_char);                                // pf_unmount

// implementation layer
// HYPOTHESIS: the search state of the directory-find calls (PrFile2's PF_DTA), as in fat_vf.h;
// only the volume pointer is known here.
typedef struct PF2_DTA {
    u32 unk0;
    u32 unk4;
    struct PF2_VOLUME* p_vol;  // 0x8
    u8 unkC[0x434];
} PF2_DTA;

PF2_VOLUME* PFPATH_GetVolumeFromPath(PF_STR* p_path);
s32 PFVOL_CheckForRead(PF2_VOLUME* p_vol);
void PFDIR_FinalizeAllDirs(PF2_VOLUME* p_vol);
s32 PFDIR_p_fsnext(PF2_DTA* dta);
s32 PFDIR_p_fsfirst(PF2_VOLUME* p_vol, PF_STR* p_pattern, u8 attr, PF2_DTA* dta);
s32 PFDIR_p_fstat(PF2_VOLUME* p_vol, PF_STR* p_path, void* p_stat);
s32 PFDIR_p_opendir(PF2_VOLUME* p_vol, PF_STR* p_path, PF2_DIR** pp_dir);
int PFDIR_fsnext(PF2_DTA* dta);
int PFDIR_fsfirst(PF_STR* p_pattern, u8 attr, PF2_DTA* dta);
int PFDIR_fstat(PF_STR* p_path, void* p_stat);
int PFDIR_opendir(PF_STR* p_path, PF2_DIR** pp_dir);
void PFCLUSTER_UpdateLastAccessCluster(PF_FILE* p_file, u32 sector);
int PFDIR_closedir(PF2_DIR* dir);
s32 PFVOL_CheckForWrite(PF2_VOLUME* p_vol);
s32 PFDIR_p_mkdir(PF2_VOLUME* p_vol, PF_STR* p_path, u32 unk2, u32 unk3);
s32 PFCACHE_FlushFATCache(PF2_VOLUME* p_vol);
s32 PFCACHE_FlushDataCacheSpecific(PF2_VOLUME* p_vol, void* signature);
int PFDIR_mkdir(PF_STR* p_path);
s32 PFENT_findEntryPos(PF_FFD* p_ffd, PF2_DIR_ENT* p_ent, u32 index, PF2_STR* pattern, u8 attr_required, u8 attr_unwanted, u32* p_lpos, u32* p_ppos);
s32 PFENT_allocateEntryPos(PF2_DIR_ENT* p_ent, u8 num_entries, PF_FFD* p_ffd, u32* p_next_chain, PF2_STR* p_filename, u32* p_pos);
int fn_803DE800(PF2_ENT_ITER* iter, PF2_DIR_ENT* ent, PF2_VOLUME* vol, PF2_STR* path, u32 wildcard, u32 no_look_last_token);
u32 fn_803E12B4(PF_FFD* p_ffd, u32 index, u32 mode, u32 size, u32* p_cluster);  // PFFAT_GetClusterInChain
u32 PFFAT_GetSectorSpecified(PF_FFD* p_ffd, u32 index, u32 may_allocate, u32* p_cluster);
int PFFAT_GetSectorAllocated(PF_FFD* p_ffd, u32 index, u32 size, u32* p_cluster);
void PFFAT_InitHint(PF_FAT_HINT* p_hint);
int PFFAT_ResetFFD(PF_FFD* p_ffd, u32* p_start_cluster);
int PFFAT_InitFFD(PF_FFD* p_ffd, PF_FAT_HINT* p_hint, PF_VOLUME* p_vol, u32* p_start_cluster);
int PFFAT_FinalizeFFD(PF_FFD* p_ffd);
int fn_803E3610(void);                                       // PFFATFS_initializeFATFS
void PFFILE_Cursor_MoveToEnd(PF2_FILE* p_file);
void PFFILE_Cursor_MoveToClusterEnd(PF2_FILE* p_file, u32 size);
int PFFILE_p_fread(PF2_VOLUME* p_vol, u8* p_buf, u32 size, u32 count, PF2_FILE* p_file, u32* p_count_read);
void PFFILE_FinalizeAllFiles(PF2_VOLUME* p_vol);
int PFFILE_remove(PF_STR* p_path_str);
int PFFILE_fopen(PF_STR* p_path_str, s32 mode, PF2_FILE** pp_file);
int PFFILE_Cursor_Read(PF2_FILE* p_file, u8* p_buf, u32 size, u32* p_size_read);
int PFFILE_p_fopen(PF2_VOLUME* p_vol, PF_STR* p_path_str, s32 mode, PF2_FILE** pp_file);
int PFFILE_p_remove(PF2_VOLUME* p_vol, PF_STR* p_path_str);
int PFFAT_CountAllocatedClusters(PF_FFD* p_ffd, u32 size, u32* p_num_alloc_clusters);
int PFFILE_p_fwrite(PF2_VOLUME* p_vol, u8* p_buf, u32 size, u32 count, PF2_FILE* p_file, u32* p_count_written);
u8 PFENT_getcurrentDateTimeForEnt(s16* p_date, s16* p_time);
u32 PFFILE_Cursor_AdvanceToRead(PF2_FILE* p_file, u32 n, u32 sector);
PF2_VOLUME* PFPATH_GetVolumeFromPath(PF_STR* p_path);
void PF_enterCritical(void* unk);
void PF_exitCritical(void* unk);
int fn_803E5774(PF2_FILE* file);                           // PFFILE_fclose
int fn_803E5A50(void* buf, u32 size, u32 count, PF2_FILE* file, u32* count_read);   // PFFILE_fread
int fn_803E5B8C(void* buf, u32 size, u32 count, PF2_FILE* file, u32* count_written);  // PFFILE_fwrite
int fn_803E5D00(PF2_FILE* file, s32 offset, s32 origin);    // PFFILE_fseek
int fn_803E5F58(PF2_STR* path, PF2_STR* dir_path, PF2_STR* filename, u32 wildcard);  // PFPATH_DoSplitPath
int fn_803EA188(void);                                       // PFVOL_errnum
int fn_803EA198(s8 drv_char);                                // PFVOL_format
int fn_803EA788(s8 drv_char);                                // PFVOL_detach
int fn_803EAE34(s8 drv_char, int* result);                   // PFVOL_mount
void fn_803EBBD8(void);                                      // PFSYS_initializeSYS
int pfd_sddrv_registar_callback(void (*insert_cb)(s8 drv_char), void (*eject_cb)(s8 drv_char));
// pfk: the kernel shim over RVL OS threads and semaphores; a semaphore
// resource is the address of its OSSemaphore.
typedef struct PFK_SEMAPHORE_INFO {
    u32 initial;               // 0x0
    u32 max;                   // 0x4
    struct OSSemaphore* sem;   // 0x8
} PFK_SEMAPHORE_INFO;

int fn_803EC27C(u32* context);                               // get the current context (thread)
s32 fn_803EC2F8(PFK_SEMAPHORE_INFO* info);                   // create a semaphore
int fn_803EC394(s32 resource);                               // delete a semaphore
int fn_803EC3D4(s32 resource);                               // lock
int fn_803EC418(s32 resource);                               // unlock

// PrFile2 file-lock bookkeeping (pf_system): the number of file locks in use
extern u32 lbl_805A1260;
void fn_803EBA40(PF_CRITICAL* critical);                      // PF_initCritical
void fn_803EBB4C(void);                                      // PFSYS_initFileLock
int fn_803EBB58(PF_FILE* p_file);                            // PFSYS_finalizeFileLock

// pf_driver
int fn_803D79B0(PF2_VOLUME* p_vol);                          // PFDRV_mount
int fn_803D8878(PF2_VOLUME* p_vol, const u8* param);         // PFDRV_format

typedef struct PDM_CALLBACKS PDM_CALLBACKS;
void PFDRV_MediaInsert(u32 part);
void PFDRV_MediaEject(u32 part);

// pf_code / pf_str
s32 fn_803EAEAC(const s8* src, u16* dst);                    // CP932_OEM2Unicode
s32 fn_803EB068(const u16* src, s8* dst);                    // CP932_Unicode2OEM
void fn_803D65A4(unsigned int width, short* oem_width, short* uni_width);  // PFCODE_Divide_Width
u32 VFipf_strlen(const char* s);                              // pf_strlen
s32 VFipf_w_strlen(const u16* s);                               // pf_wcslen
s32 VFipf_strcmp(const char* s1, const char* s2);             // pf_strcmp
void* VFipf_memcpy(void* dst, void* src, u32 length);         // pf_memcpy

// pf_entry
int PFENT_findEntry(PF_FFD* p_ffd, PF2_DIR_ENT* p_ent, u32 index, PF2_STR* pattern, u8 attr_required, u8 attr_unwanted);
int PFENT_allocateEntry(PF2_DIR_ENT* p_ent, u8 num_entries, PF_FFD* p_ffd, u32* p_next_chain, PF2_STR* p_filename);
int PFENT_ITER_GetEntryOfPattern(PF2_ENT_ITER* iter, PF2_DIR_ENT* ent, PF2_VOLUME* vol, PF2_STR* path);
void PFENT_LoadShortNameFromBuf(PF2_DIR_ENT* p_ent, const u8* buf);
u8 PFENT_CalcCheckSum(PF2_DIR_ENT* p_ent);
s32 PFENT_GetRootDir(PF2_VOLUME* p_vol, PF2_DIR_ENT* p_ent);
s32 PFENT_ITER_IteratorInitialize(PF2_ENT_ITER* p_iter, u32 index_start_from);
s32 PFENT_RecalcEntryIterator(PF2_ENT_ITER* p_iter, u32 may_allocate);
s32 PFENT_MakeRootDir(PF2_VOLUME* p_vol);
s32 PFENT_ITER_FindEntry(PF2_ENT_ITER* p_iter, PF2_DIR_ENT* p_ent, PF_STR* p_path, u32 wildcard, u32 no_look_last_token, u32* p_pos);
s32 PFENT_ITER_DoFindEntry(PF2_ENT_ITER* p_iter, PF2_DIR_ENT* p_ent, PF_STR* p_path, u32 wildcard, u32 no_look_last_token, u32* p_pos);
int PFSTR_StrNCmp(PF_STR* p_str, const s8* s, u32 a, u32 b, u32 c);
s32 PFVOL_GetCurrentDir(PF2_VOLUME* p_vol, PF2_DIR_ENT* p_ent);
s32 PFFAT_MakeRootDir(PF2_VOLUME* p_vol);
s32 PFCACHE_AllocateDataPage(PF_VOLUME* p_vol, u32 sector, PF_CACHE_PAGE** pp_page, u32* p_is_hit);
void PFCACHE_FreeDataPage(PF_VOLUME* p_vol, PF_CACHE_PAGE* p_page);
s32 PFSEC_WriteData(PF_VOLUME* p_vol, const u8* p_buf, u32 sector, u16 offset, u32 size, u32* p_success_size, u32 set_sig);
s32 PFPATH_getShortName(s8* short_name, const u8* pDirEntry, u8 attr);
s32 PFPATH_putShortName(u8* pDirEntry, const s8* short_name, u8 attr);
s32 PFSEC_ReadData(PF_VOLUME* p_vol, u8* p_buf, u32 sector, u16 offset, u32 size, u32* p_success_size, u32 set_sig);
s32 PFSEC_ReadFAT(PF2_VOLUME* p_vol, u8* p_buf, u32 sector, u16 offset, u16 size);
s32 PFSEC_WriteFAT(PF2_VOLUME* p_vol, const u8* p_buf, u32 sector, u16 offset, u16 size);
s32 PFFAT16_WriteFATEntryWithBuf(PF2_VOLUME* p_vol, u32 cluster, u32 value, PF_CACHE_PAGE* p_page);
s32 PFFAT32_WriteFATEntry(PF2_VOLUME* p_vol, u32 cluster, u32 value);
s32 PFFAT16_ReadFATEntry(PF2_VOLUME* p_vol, u32 cluster, u32* p_value);
s32 PFFAT32_ReadFATEntry(PF2_VOLUME* p_vol, u32 cluster, u32* p_value);
s32 PFFAT32_WriteFATEntryWithBuf(PF2_VOLUME* p_vol, u32 cluster, u32 value, PF_CACHE_PAGE* p_page);
s32 PFFAT_GetClusterInChain(PF_FFD* p_ffd, u32 chain_index, u32 mode, u32 num_cluster, u32* p_start, u32* p_end);
s32 PFFAT_ReadCluster(PF_FFD* p_ffd, u32 cluster, u32 chain_index, u32* p_next_cluster, u32* p_chk_clstlnk);
s32 PFFAT_ResetCluster(PF_VOLUME* p_vol, u32 cluster);
void* VFipf_memset(void* dst, int c, u32 length);
s32 PFFAT_ReadFATSector(PF2_VOLUME* p_vol, PF_CACHE_PAGE* p_page, u32 cluster);
s32 PFFAT_getContinuousSector(PF_FFD* p_ffd, u32 file_sector_index, u32 size, u32* p_sector, u32* p_num_sector);
s32 PFFAT_GetClusterContinuousSectorInChain(PF_FFD* p_ffd, u32 initial_cluster, u32 chain_index, u32 size, u32* p_num_sector);

#ifdef __cplusplus
}
#endif

#endif
