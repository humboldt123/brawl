// nand: the file level wrappers on top of the isfs ipc calls. every public call checks nandIsInitialized,
// builds an absolute path where it needs one and turns the isfs result into a NAND_RESULT_*.
#include "revolution/nand.h"
#include <cstdio>
#include <cstring>

// BYTES: integer-0 NULL, so pointer tests against NULL compile as signed cmpwi as in retail. Not header-wide:
// making revolution/types.h define NULL 0 for C changes no other SDK unit except OSExec and nwc24, whose
// retail compares are cmplwi ((void*)0), so the NULL spelling differs per original source file.
#undef NULL
#define NULL 0

#define IPC_RESULT_OK 0
#define IPC_RESULT_INVALID -101
#define IPC_RESULT_ACCESS -102

#define IPC_SEEK_BEG 0
#define IPC_SEEK_CUR 1
#define IPC_SEEK_END 2

#define NAND_SEEK_BEG 0
#define NAND_SEEK_CUR 1
#define NAND_SEEK_END 2

// isfs calls the shared header does not list; the arguments are the same ones the original passes
extern s32 ISFS_CreateFile(const u8* path, u32 attr, u32 ownerAcc, u32 groupAcc, u32 othersAcc);
extern s32 ISFS_CreateDir(const u8* path, u32 attr, u32 ownerAcc, u32 groupAcc, u32 othersAcc);
extern s32 ISFS_Rename(const u8* from, const u8* to);
extern s32 ISFS_GetFileStats(s32 fd, ISFSFileStats* stats);
extern s32 ISFS_GetFileStatsAsync(s32 fd, ISFSFileStats* stats, ISFSCallback cb, void* arg);
extern s32 ISFS_GetAttr(const u8* path, u32* ownerId, u16* groupId, u32* attr, u32* ownerAcc, u32* groupAcc, u32* othersAcc);
extern s32 ISFS_SetAttr(const u8* path, u32 ownerId, u16 groupId, u32 attr, u32 ownerAcc, u32 groupAcc, u32 othersAcc);
extern s32 ISFS_SetAttrAsync(const u8* path, u32 ownerId, u16 groupId, u32 attr, u32 ownerAcc, u32 groupAcc, u32 othersAcc,
                             ISFSCallback cb, void* arg);
extern BOOL nandIsUnderPrivatePath(const char* path);

typedef void (*NANDAsyncCb)(s32 result, NANDCommandBlock* block);

#define MEMCLR(x) memset((x), 0, sizeof(*(x)))

static void nandSplitPerm(u8 perm, u32* ownerPerm, u32* groupPerm, u32* otherPerm);
static void nandGetStatusCallback(s32 result, void* arg);
static void nandGetFileStatusAsyncCallback(s32 result, void* arg);
static BOOL nandInspectPermission(u8 perm);

static s32 nandCreate(const char* path, u8 perm, u8 attr, NANDCommandBlock* block, BOOL async, BOOL priv) {
    char absPath[64] = {0};
    u32 ownerPerm = 0;
    u32 groupPerm = 0;
    u32 otherPerm = 0;

    nandGenerateAbsPath(absPath, path);

    if (!priv && nandIsPrivatePath(absPath)) {
        return IPC_RESULT_ACCESS;
    }

    if (!nandInspectPermission(perm)) {
        return IPC_RESULT_INVALID;
    }

    nandSplitPerm(perm, &ownerPerm, &groupPerm, &otherPerm);

    if (async) {
        return ISFS_CreateFileAsync((u8*)absPath, attr, ownerPerm, groupPerm, otherPerm, nandCallback, block);
    } else {
        return ISFS_CreateFile((u8*)absPath, attr, ownerPerm, groupPerm, otherPerm);
    }
}

s32 NANDCreate(const char* path, u8 perm, u8 attr) {
    if (!nandIsInitialized()) {
        return NAND_RESULT_FATAL_ERROR;
    }

    return nandConvertErrorCode(nandCreate(path, perm, attr, NULL, FALSE, FALSE));
}

s32 NANDPrivateCreate(const char* path, u8 perm, u8 attr) {
    if (!nandIsInitialized()) {
        return NAND_RESULT_FATAL_ERROR;
    }

    return nandConvertErrorCode(nandCreate(path, perm, attr, NULL, FALSE, TRUE));
}

s32 NANDPrivateCreateAsync(const char* path, u8 perm, u8 attr, NANDCallback callback, NANDCommandBlock* block) {
    if (!nandIsInitialized()) {
        return NAND_RESULT_FATAL_ERROR;
    }

    block->callback = callback;
    return nandConvertErrorCode(nandCreate(path, perm, attr, block, TRUE, TRUE));
}

static s32 nandDelete(const char* path, NANDCommandBlock* block, BOOL async, BOOL priv) {
    char absPath[64] = {0};

    nandGenerateAbsPath(absPath, path);

    if (!priv && nandIsPrivatePath(absPath)) {
        return IPC_RESULT_ACCESS;
    }

    if (async) {
        return ISFS_DeleteAsync((u8*)absPath, nandCallback, block);
    } else {
        return ISFS_Delete((u8*)absPath);
    }
}

s32 NANDDelete(const char* path) {
    if (!nandIsInitialized()) {
        return NAND_RESULT_FATAL_ERROR;
    }

    return nandConvertErrorCode(nandDelete(path, NULL, FALSE, FALSE));
}

s32 NANDPrivateDelete(const char* path) {
    if (!nandIsInitialized()) {
        return NAND_RESULT_FATAL_ERROR;
    }

    return nandConvertErrorCode(nandDelete(path, NULL, FALSE, TRUE));
}

s32 NANDPrivateDeleteAsync(const char* path, NANDCallback callback, NANDCommandBlock* block) {
    if (!nandIsInitialized()) {
        return NAND_RESULT_FATAL_ERROR;
    }

    block->callback = callback;
    return nandConvertErrorCode(nandDelete(path, block, TRUE, TRUE));
}

s32 NANDRead(NANDFileInfo* info, void* buf, u32 len) {
    if (!nandIsInitialized()) {
        return NAND_RESULT_FATAL_ERROR;
    }

    return nandConvertErrorCode(ISFS_Read(info->fileDescriptor, (u8*)buf, len));
}

s32 NANDReadAsync(NANDFileInfo* info, void* buf, u32 len, NANDCallback callback, NANDCommandBlock* block) {
    if (!nandIsInitialized()) {
        return NAND_RESULT_FATAL_ERROR;
    }

    block->callback = callback;
    return nandConvertErrorCode(ISFS_ReadAsync(info->fileDescriptor, (u8*)buf, len, nandCallback, block));
}

s32 NANDWrite(NANDFileInfo* info, const void* buf, u32 len) {
    if (!nandIsInitialized()) {
        return NAND_RESULT_FATAL_ERROR;
    }

    return nandConvertErrorCode(ISFS_Write(info->fileDescriptor, (const u8*)buf, len));
}

s32 NANDWriteAsync(NANDFileInfo* info, const void* buf, u32 len, NANDCallback callback, NANDCommandBlock* block) {
    if (!nandIsInitialized()) {
        return NAND_RESULT_FATAL_ERROR;
    }

    block->callback = callback;
    return nandConvertErrorCode(ISFS_WriteAsync(info->fileDescriptor, (const u8*)buf, len, nandCallback, block));
}

static s32 nandSeek(s32 fd, s32 offset, s32 whence, NANDCommandBlock* block, BOOL async) {
    u32 mode = (u32)-1;

    switch (whence) {
    case NAND_SEEK_BEG:
        mode = IPC_SEEK_BEG;
        break;
    case NAND_SEEK_CUR:
        mode = IPC_SEEK_CUR;
        break;
    case NAND_SEEK_END:
        mode = IPC_SEEK_END;
        break;
    }

    if (async) {
        return ISFS_SeekAsync(fd, offset, mode, nandCallback, block);
    } else {
        return ISFS_Seek(fd, offset, mode);
    }
}

s32 NANDSeek(NANDFileInfo* info, s32 offset, s32 whence) {
    if (!nandIsInitialized()) {
        return NAND_RESULT_FATAL_ERROR;
    }

    return nandConvertErrorCode(nandSeek(info->fileDescriptor, offset, whence, NULL, FALSE));
}

s32 NANDSeekAsync(NANDFileInfo* info, s32 offset, s32 whence, NANDCallback callback, NANDCommandBlock* block) {
    if (!nandIsInitialized()) {
        return NAND_RESULT_FATAL_ERROR;
    }

    block->callback = callback;
    return nandConvertErrorCode(nandSeek(info->fileDescriptor, offset, whence, block, TRUE));
}

static s32 nandReadDir(const char* path, char* names, u32* count) {
    char absPath[64] = {0};

    nandGenerateAbsPath(absPath, path);

    if (nandIsPrivatePath(absPath)) {
        return IPC_RESULT_ACCESS;
    }

    return ISFS_ReadDir((u8*)absPath, (u8*)names, count);
}

s32 NANDReadDir(const char* path, char* names, u32* count) {
    if (!nandIsInitialized()) {
        return NAND_RESULT_FATAL_ERROR;
    }

    return nandConvertErrorCode(nandReadDir(path, names, count));
}

static s32 nandCreateDir(const char* path, u8 perm, u8 attr, NANDCommandBlock* block, BOOL async, BOOL priv) {
    char absPath[64] = {0};
    u32 ownerPerm, groupPerm, otherPerm;

    nandGenerateAbsPath(absPath, path);

    if (!priv && nandIsPrivatePath(absPath)) {
        return IPC_RESULT_ACCESS;
    }

    if (!nandInspectPermission(perm)) {
        return IPC_RESULT_INVALID;
    }

    ownerPerm = 0;
    groupPerm = 0;
    otherPerm = 0;
    nandSplitPerm(perm, &ownerPerm, &groupPerm, &otherPerm);

    if (async) {
        return ISFS_CreateDirAsync((u8*)absPath, attr, ownerPerm, groupPerm, otherPerm, nandCallback, block);
    } else {
        return ISFS_CreateDir((u8*)absPath, attr, ownerPerm, groupPerm, otherPerm);
    }
}

s32 NANDCreateDir(const char* path, u8 perm, u8 attr) {
    if (!nandIsInitialized()) {
        return NAND_RESULT_FATAL_ERROR;
    }

    return nandConvertErrorCode(nandCreateDir(path, perm, attr, NULL, FALSE, FALSE));
}

s32 NANDPrivateCreateDir(const char* path, u8 perm, u8 attr) {
    if (!nandIsInitialized()) {
        return NAND_RESULT_FATAL_ERROR;
    }

    return nandConvertErrorCode(nandCreateDir(path, perm, attr, NULL, FALSE, TRUE));
}

s32 NANDPrivateCreateDirAsync(const char* path, u8 perm, u8 attr, NANDCallback callback, NANDCommandBlock* block) {
    if (!nandIsInitialized()) {
        return NAND_RESULT_FATAL_ERROR;
    }

    block->callback = callback;
    return nandConvertErrorCode(nandCreateDir(path, perm, attr, block, TRUE, TRUE));
}

static s32 nandGetFileStatus(s32 fd, u32* lengthOut, u32* positionOut) {
    ISFSFileStats stats __attribute__((aligned(32)));
    s32 result;

    result = ISFS_GetFileStats(fd, &stats);
    if (result == IPC_RESULT_OK) {
        if (lengthOut != NULL) {
            *lengthOut = stats.size;
        }

        if (positionOut != NULL) {
            *positionOut = stats.offset;
        }
    }

    return result;
}

s32 NANDGetLength(NANDFileInfo* info, u32* length) {
    if (!nandIsInitialized()) {
        return NAND_RESULT_FATAL_ERROR;
    }

    return nandConvertErrorCode(nandGetFileStatus(info->fileDescriptor, length, NULL));
}

s32 NANDTell(NANDFileInfo* info, u32* position) {
    if (!nandIsInitialized()) {
        return NAND_RESULT_FATAL_ERROR;
    }

    return nandConvertErrorCode(nandGetFileStatus(info->fileDescriptor, NULL, position));
}

static s32 nandGetFileStatusAsync(s32 fd, NANDCommandBlock* block) {
    // the stats land in a 32 byte aligned scratch area carved out of the block's path buffer
    return ISFS_GetFileStatsAsync(fd, (ISFSFileStats*)(((u32)block->absPath + 31) & ~31), nandGetFileStatusAsyncCallback, block);
}

static void nandGetFileStatusAsyncCallback(s32 result, void* arg) {
    NANDCommandBlock* block = arg;
    ISFSFileStats* stats = (ISFSFileStats*)(((u32)block->absPath + 31) & ~31);

    if (result == IPC_RESULT_OK) {
        if (block->length != NULL) {
            *block->length = stats->size;
        }

        if (block->pos != NULL) {
            *block->pos = stats->offset;
        }
    }

    ((NANDAsyncCb)block->callback)(nandConvertErrorCode(result), arg);
}

s32 NANDGetLengthAsync(NANDFileInfo* info, u32* lengthOut, NANDCallback callback, NANDCommandBlock* block) {
    if (!nandIsInitialized()) {
        return NAND_RESULT_FATAL_ERROR;
    }

    block->callback = callback;
    block->length = lengthOut;
    block->pos = NULL;
    return nandConvertErrorCode(nandGetFileStatusAsync(info->fileDescriptor, block));
}

#pragma dont_inline on
static void nandComposePerm(u8* out, u32 ownerPerm, u32 groupPerm, u32 otherPerm) {
    u32 perm = 0;

    if (ownerPerm & NAND_ACCESS_READ) {
        perm |= NAND_PERM_RUSR;
    }

    if (ownerPerm & NAND_ACCESS_WRITE) {
        perm |= NAND_PERM_WUSR;
    }

    if (groupPerm & NAND_ACCESS_READ) {
        perm |= NAND_PERM_RGRP;
    }

    if (groupPerm & NAND_ACCESS_WRITE) {
        perm |= NAND_PERM_WGRP;
    }

    if (otherPerm & NAND_ACCESS_READ) {
        perm |= NAND_PERM_ROTH;
    }

    if (otherPerm & NAND_ACCESS_WRITE) {
        perm |= NAND_PERM_WOTH;
    }

    *out = perm;
}
#pragma dont_inline reset

static void nandSplitPerm(u8 perm, u32* ownerPerm, u32* groupPerm, u32* otherPerm) {
    *ownerPerm = 0;
    *groupPerm = 0;
    *otherPerm = 0;

    if (perm & NAND_PERM_RUSR) {
        *ownerPerm |= NAND_ACCESS_READ;
    }

    if (perm & NAND_PERM_WUSR) {
        *ownerPerm |= NAND_ACCESS_WRITE;
    }

    if (perm & NAND_PERM_RGRP) {
        *groupPerm |= NAND_ACCESS_READ;
    }

    if (perm & NAND_PERM_WGRP) {
        *groupPerm |= NAND_ACCESS_WRITE;
    }

    if (perm & NAND_PERM_ROTH) {
        *otherPerm |= NAND_ACCESS_READ;
    }

    if (perm & NAND_PERM_WOTH) {
        *otherPerm |= NAND_ACCESS_WRITE;
    }
}

static s32 nandGetStatus(const char* path, NANDStatus* status, NANDCommandBlock* block, BOOL async, BOOL priv) {
    s32 result;
    u32 attr;
    u32 ownerPerm, groupPerm, otherPerm;
    char absPath[64] = {0};

    nandGenerateAbsPath(absPath, path);

    if (!priv && nandIsUnderPrivatePath(absPath)) {
        return IPC_RESULT_ACCESS;
    }

    if (async) {
        return ISFS_GetAttrAsync((u8*)absPath, &status->ownerId, &status->groupId, &block->attr, &block->ownerAcc,
                                 &block->groupAcc, &block->othersAcc, nandGetStatusCallback, block);
    } else {
        attr = 0;
        ownerPerm = 0;
        groupPerm = 0;
        otherPerm = 0;

        result = ISFS_GetAttr((u8*)absPath, &status->ownerId, &status->groupId, &attr, &ownerPerm, &groupPerm, &otherPerm);

        if (result == IPC_RESULT_OK) {
            nandComposePerm(&status->permission, ownerPerm, groupPerm, otherPerm);
            status->attribute = attr;
        }

        return result;
    }
}

static void nandGetStatusCallback(s32 result, void* arg) {
    NANDCommandBlock* block = (NANDCommandBlock*)arg;
    NANDStatus* status;

    if (result == IPC_RESULT_OK) {
        status = (NANDStatus*)block->status;
        status->attribute = block->attr;
        nandComposePerm(&status->permission, block->ownerAcc, block->groupAcc, block->othersAcc);
    }

    ((NANDAsyncCb)block->callback)(nandConvertErrorCode(result), block);
}

s32 NANDGetStatus(const char* path, NANDStatus* status) {
    if (!nandIsInitialized()) {
        return NAND_RESULT_FATAL_ERROR;
    }

    return nandConvertErrorCode(nandGetStatus(path, status, NULL, FALSE, FALSE));
}

s32 NANDPrivateGetStatus(const char* path, NANDStatus* status) {
    if (!nandIsInitialized()) {
        return NAND_RESULT_FATAL_ERROR;
    }

    return nandConvertErrorCode(nandGetStatus(path, status, NULL, FALSE, TRUE));
}

s32 NANDPrivateGetStatusAsync(const char* path, NANDStatus* status, NANDCallback callback, NANDCommandBlock* block) {
    if (!nandIsInitialized()) {
        return NAND_RESULT_FATAL_ERROR;
    }

    block->callback = callback;
    block->status = status;
    return nandConvertErrorCode(nandGetStatus(path, status, block, TRUE, TRUE));
}

static s32 nandSetStatus(const char* path, const NANDStatus* status, NANDCommandBlock* block, BOOL async, BOOL priv) {
    char absPath[64] = {0};
    u32 ownerPerm, groupPerm, otherPerm;

    nandGenerateAbsPath(absPath, path);

    if (!priv && nandIsPrivatePath(absPath)) {
        return IPC_RESULT_ACCESS;
    }

    if (!nandInspectPermission(status->permission)) {
        return IPC_RESULT_INVALID;
    }

    ownerPerm = 0;
    groupPerm = 0;
    otherPerm = 0;
    nandSplitPerm(status->permission, &ownerPerm, &groupPerm, &otherPerm);

    if (async) {
        return ISFS_SetAttrAsync((u8*)absPath, status->ownerId, status->groupId, status->attribute, ownerPerm, groupPerm,
                                 otherPerm, nandCallback, block);
    } else {
        return ISFS_SetAttr((u8*)absPath, status->ownerId, status->groupId, status->attribute, ownerPerm, groupPerm,
                            otherPerm);
    }
}

s32 NANDSetStatus(const char* path, const NANDStatus* status) {
    if (!nandIsInitialized()) {
        return NAND_RESULT_FATAL_ERROR;
    }

    return nandConvertErrorCode(nandSetStatus(path, status, NULL, FALSE, FALSE));
}

void NANDSetUserData(NANDCommandBlock* block, void* data) {
    block->userData = data;
}

void* NANDGetUserData(NANDCommandBlock* block) {
    return block->userData;
}

static BOOL nandInspectPermission(u8 perm) {
    return perm & NAND_PERM_RUSR;
}
