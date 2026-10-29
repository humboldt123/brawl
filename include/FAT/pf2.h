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
int fn_803EBE4C(void);                                       // FAErrnum
u16 fn_803D68B8(PF_STR* p_str);                              // PFSTR_StrLen
int fn_803EB620(PF2_FILE* file);                             // pf_fclose
u32 fn_803EB6E0(void* buf, u32 size, u32 count, PF2_FILE* file);  // pf_fread
int fn_803EB708(PF2_FILE* file, s32 offset, s32 origin);     // pf_fseek
int fn_803EB72C(const char* pattern, int attr, FAEntryInfo* dta);  // pf_fsfirst
int fn_803EB7A0(FAEntryInfo* dta);                           // pf_fsnext
int fn_803EB7C4(const char* path, FAStat* st);               // pf_fstat
u32 fn_803EB824(void* buf, u32 size, u32 count, PF2_FILE* file);  // pf_fwrite
int fn_803EB84C(s8 drv_char);                                // pf_format
int fn_803EB874(void);                                       // pf_init_prfile2
int fn_803EB8B4(s8 drv_char);                                // pf_mount
int fn_803EB8F8(const char* path);                           // pf_mkdir
int fn_803EB948(s8 drv_char);                                // pf_detach
PF2_DIR* fn_803EB970(const char* path);                      // pf_opendir
int fn_803EB9C8(const char* path);                           // pf_remove
s32 fn_803EBA18(s8 drv_char);                                // pf_unmount

// implementation layer
int fn_803DC120(FAEntryInfo* dta);                           // PFDIR_fsnext
int fn_803DC358(PF2_DIR* dir);                               // PFDIR_closedir
int fn_803DD390(PF_FFD* p_ffd, PF2_DIR_ENT* p_ent, u32 index, PF2_STR* pattern, u8 attr_required, u8 attr_unwanted, u32* p_lpos, u32* p_ppos);  // PFENT_findEntryPos
int fn_803DD674(PF2_DIR_ENT* p_ent, u8 num_entries, PF_FFD* p_ffd, u32* p_next_chain, PF2_STR* p_filename, u32* p_pos);  // PFENT_allocateEntryPos
int fn_803DE800(PF2_ENT_ITER* iter, PF2_DIR_ENT* ent, PF2_VOLUME* vol, PF2_STR* path, u32 wildcard, u32 no_look_last_token);
u32 fn_803E12B4(PF_FFD* p_ffd, u32 index, u32 mode, u32 size, u32* p_cluster);  // PFFAT_GetClusterInChain
int fn_803E3610(void);                                       // PFFATFS_initializeFATFS
int fn_803E5774(PF2_FILE* file);                             // PFFILE_fclose
int fn_803E5A50(void* buf, u32 size, u32 count, PF2_FILE* file, u32* count_read);   // PFFILE_fread
int fn_803E5B8C(void* buf, u32 size, u32 count, PF2_FILE* file, u32* count_written);  // PFFILE_fwrite
int fn_803E5D00(PF2_FILE* file, s32 offset, s32 origin);    // PFFILE_fseek
int fn_803E5F58(PF2_STR* path, PF2_STR* dir_path, PF2_STR* filename, u32 wildcard);  // PFPATH_DoSplitPath
int fn_803EA188(void);                                       // PFVOL_errnum
int fn_803EA198(s8 drv_char);                                // PFVOL_format
int fn_803EA788(s8 drv_char);                                // PFVOL_detach
int fn_803EAE34(s8 drv_char, int* result);                   // PFVOL_mount
void fn_803EBBD8(void);                                      // PFSYS_initializeSYS
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
void fn_803EBB4C(void);                                      // PFSYS_initFileLock
int fn_803EBB58(PF_FILE* p_file);                            // PFSYS_finalizeFileLock

// pf_driver
int fn_803D79B0(PF2_VOLUME* p_vol);                          // PFDRV_mount
int fn_803D8878(PF2_VOLUME* p_vol, const u8* param);         // PFDRV_format

// pf_code / pf_str
s32 fn_803EAEAC(const s8* src, u16* dst);                    // CP932_OEM2Unicode
s32 fn_803EB068(const u16* src, s8* dst);                    // CP932_Unicode2OEM
void fn_803D65A4(unsigned int width, short* oem_width, short* uni_width);  // PFCODE_Divide_Width
u32 VFipf_strlen(const char* s);                              // pf_strlen
s32 VFipf_w_strlen(const u16* s);                               // pf_wcslen
s32 VFipf_strcmp(const char* s1, const char* s2);             // pf_strcmp

#ifdef __cplusplus
}
#endif

#endif
