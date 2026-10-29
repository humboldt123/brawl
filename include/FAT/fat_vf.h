#ifndef FAT_FAT_VF_H
#define FAT_FAT_VF_H

// Typed view of the VF library (Nintendo's wrapper around eSOL PrFile2) for the
// FAT units. The structure layouts are the DWARF-derived ones of the Wii Sports
// decompilation (include/ogws/revolution/VF/vf_struct.h); Brawl links a later
// build of the same library, and the sizes below match its .bss objects.
#include <stddef.h>
#include <types.h>
#include <revolution/OS/OSMutex.h>
#include <ogws/revolution/VF/vf_struct.h>
#include <ogws/revolution/VF/pf_cache.h>
#include <ogws/revolution/VF/pf_dir.h>
#include <ogws/revolution/VF/pf_driver.h>
#include <ogws/revolution/VF/pf_entry.h>
#include <ogws/revolution/VF/pf_fat.h>
#include <ogws/revolution/VF/pf_fat12.h>
#include <ogws/revolution/VF/pf_fat16.h>
#include <ogws/revolution/VF/pf_fat32.h>
#include <ogws/revolution/VF/pf_fatfs.h>
#include <ogws/revolution/VF/pf_file.h>
#include <ogws/revolution/VF/pf_path.h>
#include <ogws/revolution/VF/pf_sector.h>
#include <ogws/revolution/VF/pf_service.h>
#include <ogws/revolution/VF/pf_str.h>
#include <ogws/revolution/VF/pf_system.h>
#include <ogws/revolution/VF/pf_api_util.h>
#include <ogws/revolution/VF/pf_volume.h>
#include <ogws/revolution/VF/pdm_bpb.h>
#include <ogws/revolution/VF/pdm_disk.h>
#include <ogws/revolution/VF/pdm_dskmng.h>
#include <ogws/revolution/VF/pdm_mbr.h>
#include <ogws/revolution/VF/pdm_partition.h>
#include <ogws/revolution/VF/nand_drv.h>
#include <ogws/revolution/VF/d_common.h>
#include <ogws/revolution/VF/d_hash.h>
#include <ogws/revolution/VF/d_vf_sys.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FAT_VF_CHECK_LAYOUT(cond) __static_assert(cond, #cond)

FAT_VF_CHECK_LAYOUT(sizeof(PF_CACHE_PAGE) == 0x28);
FAT_VF_CHECK_LAYOUT(sizeof(PF_DIR_ENT) == 0x240);
FAT_VF_CHECK_LAYOUT(sizeof(PF_SFD) == 0x290);
FAT_VF_CHECK_LAYOUT(sizeof(PF_SDD) == 0x280);
FAT_VF_CHECK_LAYOUT(sizeof(PF_FILE) == 0x30);
FAT_VF_CHECK_LAYOUT(sizeof(PF_ENT_ITER) == 0x70);
FAT_VF_CHECK_LAYOUT(sizeof(PF_VOLUME) == 0x1898);
FAT_VF_CHECK_LAYOUT(offsetof(PF_VOLUME, cache) == 0x15E8);
FAT_VF_CHECK_LAYOUT(offsetof(PF_VOLUME, flags) == 0x1878);
FAT_VF_CHECK_LAYOUT(offsetof(PF_VOLUME, p_part) == 0x188C);
FAT_VF_CHECK_LAYOUT(sizeof(PF_VOLUME_SET) == 0x27FB8);
FAT_VF_CHECK_LAYOUT(offsetof(PF_VOLUME_SET, volumes) == 0x48);
FAT_VF_CHECK_LAYOUT(sizeof(PDM_DISK) == 0x34);
FAT_VF_CHECK_LAYOUT(sizeof(PDM_PARTITION) == 0x2C);
FAT_VF_CHECK_LAYOUT(sizeof(PDM_DISK_SET) == 0xB64);
FAT_VF_CHECK_LAYOUT(offsetof(PDM_DISK_SET, partition) == 0x6EC);
FAT_VF_CHECK_LAYOUT(sizeof(VF_DRIVE_INFO) == 0x14);
FAT_VF_CHECK_LAYOUT(sizeof(VF_HANDLE_TYPE) == 0x140);

// HYPOTHESIS: the search state of the directory-find calls (PrFile2's PF_DTA);
// only the fields this unit touches are known.
typedef struct PF_DTA {
    u32 unk0;
    u32 unk4;
    struct PF_VOLUME* p_vol;  // 0x8
    u32 start_cluster;        // 0xC
    u32 index;                // 0x10
    u32 status;               // 0x14
    u8 unk18[3];
    s8 attr;                  // 0x1B
    u8 pattern[0x428];        // 0x1C
    u8 unk444[4];
} PF_DTA;
FAT_VF_CHECK_LAYOUT(offsetof(PF_DTA, pattern) == 0x1C);

// One entry of the per-FAT-type value table (lbl_804169B0); the third word is
// what VFiPFFAT_GetValueOfEOC2 returns.
typedef struct PF_FAT_VALUES {
    u32 unk0;
    u32 unk4;
    u32 eoc2;
    u32 unkC;
    u32 unk10;
} PF_FAT_VALUES;
extern const PF_FAT_VALUES lbl_804169B0[3];

// VFiPFAPI_convertError's PrFile2 -> VF error table.
extern s32 lbl_80492730[0xA0 / 4];

// HYPOTHESIS: d_hash name table entry; dHash_InitHashTable clears key and used.
typedef struct dHashEntry {
    u16 key;
    u8 unk2[14];
    u8 used;
    u8 arg;
} dHashEntry;
FAT_VF_CHECK_LAYOUT(sizeof(dHashEntry) == 0x12);
extern dHashEntry lbl_80563360[31];

typedef s32 (*VFSysCheckExistPrfFileFunc)(struct VF_HANDLE_DEVICE*, const s8*, void*, u32);
extern VFSysCheckExistPrfFileFunc lbl_804927D0[4];

// HYPOTHESIS: the time stamp callback fills the current date and time.
typedef void (*VFTimeStampCallback)(struct PF_SYS_DATE*, struct PF_SYS_TIME*);

// Path separators used by the directory and volume calls.
extern s8 lbl_8059FE30[4];
extern s8 lbl_8059FE60[2];

// HYPOTHESIS: the PrFile2 volume set (VFipf_vol_set in OpenRVL's pf_volume.h) and
// the disk manager's disk/partition set; kept under their address labels.
extern PF_VOLUME_SET lbl_8053B248;
extern PDM_DISK_SET lbl_8053A6E0;

// The VF system layer: its API mutex, per-drive information and handle table.
extern OSMutex lbl_80563200;
extern VF_DRIVE_INFO lbl_80563590[26];
extern PF_SYSTEM_SET lbl_805A1200;  // VFiPF_InitLockFile
extern u32 lbl_805A1208;              // VFIsAvailable
extern u32 lbl_805A120C;              // HYPOTHESIS: API mutex initialized
extern u32 lbl_805A1210;              // number of handles in lbl_805A1220
extern s32 lbl_805A1214;              // last VF error
extern VF_HANDLE_TYPE* lbl_805A1220;  // handle table
extern VFTimeStampCallback lbl_805A1224;

// Functions of this library that our symbols do not name yet. HYPOTHESIS names
// from the Wii Sports VF headers, by address order.
int fn_803A962C(int c);                                    // VFipf_toupper
void* fn_803A9644(void* dst, const void* src, u32 length); // VFipf_memcpy
void* fn_803A976C(void* dst, int c, u32 length);           // VFipf_memset
u32 fn_803A9864(const s8* s);                              // VFipf_strlen
s8* fn_803A9884(s8* dst, const s8* src);                   // VFipf_strcpy
s32 fn_803A98A8(u8* s1, u8* s2);                           // VFipf_strcmp
s32 fn_803A98E0(u8* s1, u8* s2, u32 length);               // VFipf_strncmp
int VFiPFCODE_Combine_Width(int oem_width, int uni_width);
void VFiPFCODE_Divide_Width(unsigned int width, short* oem_width, short* uni_width);
u16 fn_803A9948(const u8* buf);                            // VFiPF_GET_LE_U16
u32 fn_803A9A78(struct PF_STR* p_str);                     // VFiPFSTR_GetCodeMode
void fn_803A9A80(struct PF_STR* p_str, s8* p_local);       // VFiPFSTR_SetLocalStr
const s8* fn_803A9AA8(struct PF_STR* p_str, u32 target);   // VFiPFSTR_GetStrPos
s32 fn_803A9FF8(const u16* s);
u16* fn_803AA01C(u16* dst, const u16* src);
s32 fn_803AA040(u16* s1, u16* s2, s32 length);
s32 fn_803AB6EC(struct PDM_DISK* p_disk, struct PDM_DISK* lp_disk);        // VFipdm_disk_do_get_permission
s32 fn_803AB8C4(struct PDM_INIT_DISK* p_init_disk_tbl, struct PDM_DISK** pp_disk); // VFipdm_disk_open_disk
s32 fn_803ABA30(struct PDM_DISK* p_disk);                                  // VFipdm_disk_close_disk
s32 fn_803B11A4(struct PF_VOLUME* p_vol, struct PF_STR* p_path);
s32 fn_803B161C(PF_DTA* p_dta);
s32 fn_803B19A0(struct PF_VOLUME* p_vol, struct PF_STR* p_path, s8 attr, PF_DTA* p_dta);
s32 fn_803B1C78(PF_DTA* p_dta);
s32 fn_803B0920(struct PF_VOLUME* p_vol, struct PF_STR* p_path, u32 unk2, u32 unk3);
s32 fn_803B1E20(struct PF_STR* p_path);
s32 fn_803B1F80(struct PF_STR* p_path);
s32 VFiPFENT_RecalcEntryIterator(struct PF_ENT_ITER* p_iter, u32 may_allocate);
s32 VFiPFENT_ITER_IteratorInitialize(struct PF_ENT_ITER* p_iter, u32 index_start_from);
s32 VFiPFENT_ITER_Advance(struct PF_ENT_ITER* p_iter, u32 may_allocate);
s32 VFiPFENT_ITER_GetEntryOfPath(struct PF_ENT_ITER* p_iter, struct PF_DIR_ENT* p_ent, struct PF_VOLUME* p_vol, struct PF_STR* p_path, u32 no_look_last_token);
s32 fn_803B4910(struct PF_ENT_ITER* p_iter, struct PF_DIR_ENT* p_ent, struct PF_VOLUME* p_vol, struct PF_STR* p_path, u32 wildcard, u32 no_look_last_token);
s32 fn_803B5644(struct PF_ENT_ITER* p_iter, struct PF_DIR_ENT* p_ent, struct PF_VOLUME* p_vol, struct PF_STR* p_path);
s32 fn_803BD16C(struct PF_STR* p_path, struct PF_STR* p_dir_path, struct PF_STR* p_filename, u32 wildcard); // VFiPFPATH_DoSplitPath
s32 fn_803BE3D0(struct PF_STR* p_path, struct PF_STR* p_dir_path, struct PF_STR* p_filename);
void fn_803BF90C(u8* pattern, u8* unk, struct PF_STR* p_filename);
s32 fn_803C1F84(s8 drv_char);
s32 VFiPFCODE_CP932_OEM2Unicode(const s8* cp932_src, u16* uc_dst);
s32 VFiPFCODE_CP932_Unicode2OEM(const u16* uc_src, s8* cp932_dst);
s32 VFiPFCODE_CP932_OEMCharWidth(const s8* buf);
s32 VFiPFCODE_CP932_isOEMMBchar(u32 value, s32 type);
s32 VFiPFCODE_CP932_UnicodeCharWidth(const u16* buf);
s32 fn_803C29DC(const s8* path);
struct PF_FILE* fn_803C2A2C(const s8* path);
s32 fn_803C2BB8(s8 drv_char);
s32 fn_803C2CA0(PF_DTA* p_dta);
s32 fn_803C2DB4(const s8* path);
s32 VFipf2_detach(s8 drv_char);
s32 VFipf2_fclose(struct PF_FILE* p_file);
s32 VFipf2_fseek(struct PF_FILE* p_file, s32 offset, s32 origin);
s32 VFipf2_unmount(s8 drv_char, u32 mode);
s8* VF_path2handleidx(s32* o_handle_idx_p, const s8* i_path_p);
s32 fn_803C63FC(void* unk0, s8* path, u32 unk2);
s32 fn_803C64EC(void* unk0, s32 handle_idx, s8* path, u32 unk3);
void fn_803C68D8(void);
// Per-drive NAND access functions (normal or private namespace).
typedef struct VFNandFuncs {
    s32 (*create)(const char* path, u8 perm, u8 attr);
    s32 (*open)(const char* path, struct NANDFileInfo* info, u8 accType);
    s32 (*createDir)(const char* path, u8 perm, u8 attr);
    s32 (*remove)(const char* path);
} VFNandFuncs;
FAT_VF_CHECK_LAYOUT(sizeof(VFNandFuncs) == 0x10);
extern VFNandFuncs lbl_80563798[26];

// NAND retry policy: number of attempts and the delay between them (ms)
extern s32 lbl_805A1228;
extern s32 lbl_805A122C;
extern struct PDM_FUNCTBL lbl_8041AC68;  // nanddrv function table
s32 fn_803CACB4(PDM_DISK_TBL* p_disk_tbl, u32 ui_ext);  // VFi_nanddrv_init_drv_tbl

s32 fn_803C8090(const char* path, u8 perm, u8 attr);                    // VFi_NandCreate
s32 VFi_NandPrivateCreate(const char* path, u8 perm, u8 attr);
s32 fn_803C8210(const char* path);                                      // VFi_NandDelete
s32 VFi_NandPrivateDelete(const char* path);
s32 fn_803C8420(const char* path, struct NANDFileInfo* info, u8 accType);  // VFi_NandOpen
s32 VFi_NandPrivateOpen(const char* path, struct NANDFileInfo* info, u8 accType);
s32 fn_803C8660(const char* path, u8 perm, u8 attr);                    // VFi_NandCreateDir
s32 VFi_NandPrivateCreateDir(const char* path, u8 perm, u8 attr);
s32 fn_803CACCC(u32 num_blocks, u8* buf, u32 block, u32 bps, u32* p_num_success, struct PDM_DISK* p_disk);  // nanddrv_physical_read
s32 fn_803CAA94(struct PDM_DISK* p_disk, const u8* p_buf, u32 block, u32 num_blocks, u32* p_num_success);  // nanddrv_pwrite
s32 fn_803CABD4(struct PDM_DISK* p_disk);  // nanddrv_unmount
s32 fn_803CAC0C(struct PDM_DISK* p_disk, struct PDM_DISK_INFO* p_disk_info);  // nanddrv_get_disk_info
s32 dCommon_FlushFromVol(struct PF_VOLUME* p_vol, int i_setLastDeviceError);

#ifdef __cplusplus
}
#endif

#endif
