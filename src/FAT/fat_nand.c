#include <types.h>
#include <FAT/fat_vf.h>
#include <revolution/OS/OSTime.h>
#include <revolution/OS/OSThread.h>

// HYPOTHESIS: untyped placeholders for the zero-initialized .bss of this unit, sizes and order from the original layout.
VFNandFuncs lbl_80563798[26] = { 0 };

// Function bodies below are derived from https://github.com/jj-link/brawl.

void fn_803C9168(int, int, int, int, int);

void fn_803C9ECC(int a, int b, int c) {
    fn_803C9168(a, b, c, 3, -10);
}

s32 nanddrv_format(PDM_DISK* p_disk, const u8* param) {
    return p_disk ? 0 : -20;
}

s32 nanddrv_pread(PDM_DISK* p_disk, u8* p_buf, u32 block, u32 num_blocks, u32* p_num_success) {
    *p_num_success = 0;
    if (p_disk == NULL || p_buf == NULL || num_blocks == 0 || p_num_success == NULL) {
        return -20;
    }
    return fn_803CACCC(num_blocks, p_buf, block, 0x200, p_num_success, p_disk);
}

s32 nanddrv_BuildUpFSInfoSector(u8* buf) {
    return dCommon_MakeFsInfoSec(buf, 0x200) == 0 ? 0 : -0x15;
}

s32 nanddrv_init(PDM_DISK* p_disk) {
    if (p_disk == NULL) {
        return -0x14;
    }
    dCommon_setFatTypeToDisk(p_disk, 1);
    VFipdm_disk_notify_media_insert(p_disk);
    return 0;
}

s32 fn_803CAA94(PDM_DISK* p_disk, const u8* p_buf, u32 block, u32 num_blocks, u32* p_num_success) {
    *p_num_success = 0;
    if (p_disk == NULL || p_buf == NULL || num_blocks == 0 || p_num_success == NULL) {
        return -20;
    }
    return nanddrv_physical_write(num_blocks, p_buf, block, 0x200, p_num_success, p_disk);
}

s32 fn_803CABD4(PDM_DISK* p_disk) {
    if (p_disk == NULL) {
        return -0x14;
    }
    dCommon_setFatTypeToDisk(p_disk, 1);
    return 0;
}

s32 fn_803CAC0C(PDM_DISK* p_disk, PDM_DISK_INFO* p_disk_info) {
    u32 total;

    if (p_disk == NULL || p_disk_info == NULL) {
        return -0x14;
    }
    total = (dCommon_getFileSizeFromDisk(p_disk) - 0x1F) >> 9;
    p_disk_info->total_sectors = total;
    p_disk_info->cylinders = total / 255 / 63 >> 9;
    p_disk_info->heads = 0xFF;
    p_disk_info->sectors_per_track = 0x3F;
    p_disk_info->bytes_per_sector = 0x200;
    p_disk_info->media_attr = 0;
    p_disk_info->format_param = NULL;
    return 0;
}

s32 fn_803C8210(const char* path) {
    s32 count = lbl_805A1228;
    s32 result = 0;

    while (count-- > 0) {
        result = NANDDelete(path);
        if (result != -3 && result != -2) {
            return result;
        }
        OSSleepTicks(OS_MSEC_TO_TICKS((s64)lbl_805A122C));
    }
    return result;
}

s32 VFi_NandPrivateDelete(const char* path) {
    s32 count = lbl_805A1228;
    s32 result = 0;

    while (count-- > 0) {
        result = NANDPrivateDelete(path);
        if (result != -3 && result != -2) {
            return result;
        }
        OSSleepTicks(OS_MSEC_TO_TICKS((s64)lbl_805A122C));
    }
    return result;
}

s32 VFi_NandClose(NANDFileInfo* info) {
    s32 count = lbl_805A1228;
    s32 result = 0;

    while (count-- > 0) {
        result = NANDClose(info);
        if (result != -3 && result != -2) {
            return result;
        }
        OSSleepTicks(OS_MSEC_TO_TICKS((s64)lbl_805A122C));
    }
    return result;
}

s32 VFi_NandGetLength(NANDFileInfo* info, u32* length) {
    s32 count = lbl_805A1228;
    s32 result = 0;

    while (count-- > 0) {
        result = NANDGetLength(info, length);
        if (result != -3 && result != -2) {
            return result;
        }
        OSSleepTicks(OS_MSEC_TO_TICKS((s64)lbl_805A122C));
    }
    return result;
}

void VFi_NandSetNANDFuncNormal(u32 i_handleIdx) {
    if (i_handleIdx < 26) {
        lbl_80563798[i_handleIdx].create = fn_803C8090;
        lbl_80563798[i_handleIdx].open = fn_803C8420;
        lbl_80563798[i_handleIdx].createDir = fn_803C8660;
        lbl_80563798[i_handleIdx].remove = fn_803C8210;
    }
}

void VFi_NandSetNANDFuncEx(u32 i_handleIdx) {
    if (i_handleIdx < 26) {
        lbl_80563798[i_handleIdx].create = VFi_NandPrivateCreate;
        lbl_80563798[i_handleIdx].open = VFi_NandPrivateOpen;
        lbl_80563798[i_handleIdx].createDir = VFi_NandPrivateCreateDir;
        lbl_80563798[i_handleIdx].remove = VFi_NandPrivateDelete;
    }
}

s32 fn_803CACB4(PDM_DISK_TBL* p_disk_tbl, u32 ui_ext) {
    p_disk_tbl->ui_ext = ui_ext;
    p_disk_tbl->p_func = &lbl_8041AC68;
    return 0;
}
