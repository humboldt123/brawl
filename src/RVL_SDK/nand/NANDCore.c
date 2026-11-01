// NANDCore: shared nand state (current/home directory, init), path helpers and the isfs error mapping.
#include "revolution/esp.h"
#include "revolution/nand.h"
#include "revolution/os.h"
#include <cstdio>
#include <cstring>

// BYTES: integer-0 NULL, so pointer tests against NULL compile as signed cmpwi as in retail. Not header-wide:
// making revolution/types.h define NULL 0 for C changes no other SDK unit except OSExec and nwc24, whose
// retail compares are cmplwi ((void*)0), so the NULL spelling differs per original source file.
#undef NULL
#define NULL 0

#define IPC_RESULT_OK 0
#define IPC_RESULT_ACCESS -102
#define IPC_RESULT_INVALID -101

#define NAND_LIB_UNINITIALIZED 0
#define NAND_LIB_INITIALIZING 1
#define NAND_LIB_INITIALIZED 2

typedef void (*NANDAsyncCb)(s32 result, NANDCommandBlock* block);

extern s32 ISFS_OpenLib(void);
extern s32 ISFS_ReadDir(const u8* path, u8* names, u32* count);
extern s32 ISFS_ReadDirAsync(const u8* path, u8* names, u32* count, ISFSCallback cb, void* arg);
extern s32 ISFS_ShutdownAsync(ISFSCallback cb, void* arg);
extern void OSReport(const char* fmt, ...);
extern s64 OSGetTime(void);
extern BOOL nandOnShutdown(BOOL final, u32 event);

static void nandShutdownCallback(s32 result, void* arg);
static void nandGetTypeCallback(s32 result, void* arg);
static void nandChangeDirCallback(s32 result, void* arg);

const char* __NANDVersion = "<< RVL_SDK - NAND \trelease build: May  8 2007 13:01:45 (0x4199_60831) >>";

static s32 s_libState = NAND_LIB_UNINITIALIZED;
static char s_currentDir[64] __attribute__((aligned(32))) = "/";
static OSShutdownFunctionInfo s_shutdownFuncInfo = {nandOnShutdown, 255, NULL, NULL};

static char s_homeDir[64] __attribute__((aligned(32)));

void nandRemoveTailToken(char* newp, const char* oldp) {
    int i;

    if (oldp[0] == '/' && oldp[1] == '\0') {
        newp[0] = '/';
        newp[1] = '\0';
    } else {
        for (i = strlen(oldp) - 1; i >= 0; i--) {
            if (oldp[i] == '/') {
                if (i != 0) {
                    strncpy(newp, oldp, i);
                    newp[i] = '\0';
                    break;
                } else {
                    newp[0] = '/';
                    newp[1] = '\0';
                    break;
                }
            }
        }
    }
}

void nandGetHeadToken(char* head, char* rest, const char* path) {
    int i;

    for (i = 0; i <= strlen(path); i++) {
        if (path[i] == '/') {
            strncpy(head, path, i);
            head[i] = '\0';

            if (path[i + 1] == '\0') {
                *rest = '\0';
            } else {
                strcpy(rest, path + i + 1);
            }
            break;
        } else if (path[i] == '\0') {
            strncpy(head, path, i);
            head[i] = '\0';
            *rest = '\0';
            break;
        }
    }
}

void nandGetRelativeName(char* name, const char* path) {
    int i;

    if (strcmp("/", path) == 0) {
        strcpy(name, "");
    } else {
        for (i = strlen(path) - 1; i >= 0; i--) {
            if (path[i] == '/') {
                break;
            }
        }

        strcpy(name, path + i + 1);
    }
}

void nandConvertPath(char* abs, const char* dir, const char* rel) {
    char head[128];
    char rest[128];
    char notail[128];
    char newdir[128];

    if (strlen(rel) == 0) {
        strcpy(abs, dir);
        return;
    }

    nandGetHeadToken(head, rest, rel);

    if (strcmp(head, ".") == 0) {
        nandConvertPath(abs, dir, rest);
    } else if (strcmp(head, "..") == 0) {
        nandRemoveTailToken(notail, dir);
        nandConvertPath(abs, notail, rest);
    } else if (head[0] != '\0') {
        if (strcmp(dir, "/") == 0) {
            sprintf(newdir, "/%s", head);
        } else {
            sprintf(newdir, "%s/%s", dir, head);
        }
        nandConvertPath(abs, newdir, rest);
    } else {
        strcpy(abs, dir);
    }
}

BOOL nandIsRelativePath(const char* path) {
    return *path == '/' ? FALSE : TRUE;
}

BOOL nandIsPrivatePath(const char* path) {
    size_t len = sizeof("/shared2") - 1;
    return strncmp(path, "/shared2", len) == 0 ? TRUE : FALSE;
}

BOOL nandIsUnderPrivatePath(const char* path) {
    size_t len = sizeof("/shared2/") - 1;

    if (strncmp(path, "/shared2/", len) == 0 && path[len] != '\0') {
        return TRUE;
    }

    return FALSE;
}

BOOL nandIsInitialized(void) {
    return s_libState == NAND_LIB_INITIALIZED ? TRUE : FALSE;
}

// the release build keeps this as an empty stub
#pragma dont_inline on
void nandReportErrorCode(s32 result) {
#pragma unused(result)
}
#pragma dont_inline reset

s32 nandConvertErrorCode(const s32 result) {
    int i;
    char msg1[128] __attribute__((aligned(64)));
    char msg2[128] __attribute__((aligned(64)));

    // isfs result and the nand result it turns into, pairwise
    const s32 errorMap[] = {0,    0,   -102, -1,  -103, -4,  -114, -5,  -105, -6,  -116, -15, -101, -8,  -108, -9,  -109, -10,
                            -107, -11, -110, -16, -106, -12, -115, -13, -104, -64, -111, -14, -117, -64, -118, -3,  -119, -128,
                            -1,   -1,  -2,   -6,  -3,   -64, -4,   -8,  -5,   -64, -6,   -12, -7,   -64, -8,   -3,  -9,   -64,
                            -10,  -64, -11,  -64, -12,  -5,  -13,  -64, -14,  -64, -15,  -64, -16,  -64, -17,  -64, -18,  -64,
                            -19,  -64, -20,  -64, -21,  -64, -22,  -2,  -23,  -64};

    i = 0;

    if (result >= 0) {
        return result;
    }

    for (; i < 82; i += 2) {
        if (result == errorMap[i]) {
            if (result == -114 || result == -116 || result == -117 || result == -9 || result == -12) {
                sprintf(msg1, "ISFS error code: %d", result);
                NANDLoggingAddMessageAsync(NULL, msg1);
            }
            nandReportErrorCode(result);
            return errorMap[i + 1];
        }
    }

    OSReport("CAUTION!  Unexpected error code [%d] was found.\n", result);
    sprintf(msg2, "ISFS unexpected error code: %d", result);
    NANDLoggingAddMessageAsync(NULL, msg2);
    nandReportErrorCode(result);
    return NAND_RESULT_UNKNOWN;
}

void nandGenerateAbsPath(char* abs, const char* rel) {
    s32 len;

    if (strlen(rel) == 0) {
        strcpy(abs, "");
    } else if (nandIsRelativePath(rel)) {
        nandConvertPath(abs, s_currentDir, rel);
    } else {
        strcpy(abs, rel);
        len = strlen(abs);
        if (len != 0 && abs[len - 1] == '/' && len - 1 != 0) {
            abs[len - 1] = '\0';
        }
    }
}

void nandGetParentDirectory(char* dir, const char* path) {
    int i;

    for (i = strlen(path); i >= 0; i--) {
        if (path[i] == '/') {
            break;
        }
    }

    if (i == 0) {
        strcpy(dir, "/");
    } else {
        strncpy(dir, path, i);
        dir[i] = '\0';
    }
}

s32 NANDInit(void) {
    s32 result;
    ESTitleId tid;
    BOOL enabled;

    enabled = OSDisableInterrupts();

    if (s_libState == NAND_LIB_INITIALIZING) {
        OSRestoreInterrupts(enabled);
        return NAND_RESULT_BUSY;
    } else if (s_libState == NAND_LIB_INITIALIZED) {
        OSRestoreInterrupts(enabled);
        return NAND_RESULT_OK;
    }

    s_libState = NAND_LIB_INITIALIZING;
    OSRestoreInterrupts(enabled);

    result = ISFS_OpenLib();
    if (result == IPC_RESULT_OK) {
        result = ESP_InitLib();

        if (result == IPC_RESULT_OK) {
            result = ESP_GetTitleId(&tid);
        }

        if (result == IPC_RESULT_OK) {
            result = ESP_GetDataDir(tid, s_homeDir);
        }

        if (result == IPC_RESULT_OK) {
            strcpy(s_currentDir, s_homeDir);
        }

        ESP_CloseLib();

        if (result != IPC_RESULT_OK) {
            OSReport("Failed to set home directory.\n");
        }

        OSRegisterShutdownFunction(&s_shutdownFuncInfo);
        enabled = OSDisableInterrupts();
        s_libState = NAND_LIB_INITIALIZED;
        OSRestoreInterrupts(enabled);
        OSRegisterVersion(__NANDVersion);
        return NAND_RESULT_OK;
    } else {
        enabled = OSDisableInterrupts();
        s_libState = NAND_LIB_UNINITIALIZED;
        OSRestoreInterrupts(enabled);
        return nandConvertErrorCode(result);
    }
}

BOOL nandOnShutdown(BOOL final, u32 event) {
    if (!final) {
        if (event == 2) {
            volatile BOOL shutdown = FALSE;
            s64 start = OSGetTime();
            ISFS_ShutdownAsync(nandShutdownCallback, (void*)&shutdown);

            while (OSTicksToMilliseconds(OSGetTime() - start) < 500) {
                if (shutdown) {
                    break;
                }
            }
        }

        return TRUE;
    }

    return TRUE;
}

static void nandShutdownCallback(s32 result, void* arg) {
#pragma unused(result)

    *(BOOL*)arg = TRUE;
}

static s32 nandChangeDir(const char* path, NANDCommandBlock* block, BOOL async, BOOL priv) {
    if (async) {
        nandGenerateAbsPath(block->absPath, path);

        if (!priv && nandIsPrivatePath(block->absPath)) {
            return IPC_RESULT_ACCESS;
        }

        return ISFS_ReadDirAsync((u8*)block->absPath, NULL, &block->num, nandChangeDirCallback, block);
    } else {
        u32 numFiles = 0;
        char absPath[64] = {0};
        s32 result;
        BOOL enabled;

        nandGenerateAbsPath(absPath, path);

        if (!priv && nandIsPrivatePath(absPath)) {
            return IPC_RESULT_ACCESS;
        }

        result = ISFS_ReadDir((u8*)absPath, NULL, &numFiles);
        if (result == IPC_RESULT_OK) {
            enabled = OSDisableInterrupts();
            strcpy(s_currentDir, absPath);
            OSRestoreInterrupts(enabled);
        }

        return result;
    }
}

s32 NANDChangeDir(const char* path) {
    if (!nandIsInitialized()) {
        return NAND_RESULT_FATAL_ERROR;
    }

    return nandConvertErrorCode(nandChangeDir(path, NULL, FALSE, FALSE));
}

static void nandChangeDirCallback(s32 result, void* arg) {
    NANDCommandBlock* block = (NANDCommandBlock*)arg;
    BOOL enabled;

    if (result == IPC_RESULT_OK) {
        enabled = OSDisableInterrupts();
        strcpy(s_currentDir, block->absPath);
        OSRestoreInterrupts(enabled);
    }

    ((NANDAsyncCb)block->callback)(nandConvertErrorCode(result), block);
}

s32 NANDGetCurrentDir(char* out) {
    BOOL enabled;

    if (!nandIsInitialized()) {
        return NAND_RESULT_FATAL_ERROR;
    }

    enabled = OSDisableInterrupts();
    strcpy(out, s_currentDir);
    OSRestoreInterrupts(enabled);
    return NAND_RESULT_OK;
}

s32 NANDGetHomeDir(char* out) {
    if (!nandIsInitialized()) {
        return NAND_RESULT_FATAL_ERROR;
    }

    strcpy(out, s_homeDir);
    return NAND_RESULT_OK;
}

void nandCallback(s32 result, void* arg) {
    NANDCommandBlock* block = (NANDCommandBlock*)arg;
    ((NANDAsyncCb)block->callback)(nandConvertErrorCode(result), block);
}

static s32 nandGetType(const char* path, u8* type, NANDCommandBlock* block, BOOL async, BOOL priv) {
    if (strlen(path) == 0) {
        return IPC_RESULT_INVALID;
    }

    if (async) {
        nandGenerateAbsPath(block->absPath, path);

        if (!priv && nandIsUnderPrivatePath(block->absPath)) {
            return IPC_RESULT_ACCESS;
        }

        block->type = type;
        return ISFS_ReadDirAsync((u8*)block->absPath, NULL, &block->num, nandGetTypeCallback, block);
    } else {
        char absPath[64] = {0};
        u32 numFiles;
        s32 result;

        nandGenerateAbsPath(absPath, path);

        if (!priv && nandIsUnderPrivatePath(absPath)) {
            return IPC_RESULT_ACCESS;
        }

        numFiles = 0;
        result = ISFS_ReadDir((u8*)absPath, NULL, &numFiles);
        if (result == IPC_RESULT_OK || result == IPC_RESULT_ACCESS) {
            *type = 2;
            return IPC_RESULT_OK;
        } else if (result == IPC_RESULT_INVALID) {
            *type = 1;
            return IPC_RESULT_OK;
        }

        return result;
    }
}

s32 NANDPrivateGetTypeAsync(const char* path, u8* type, NANDCallback callback, NANDCommandBlock* block) {
    if (!nandIsInitialized()) {
        return NAND_RESULT_FATAL_ERROR;
    }

    block->callback = callback;
    return nandConvertErrorCode(nandGetType(path, type, block, TRUE, TRUE));
}

static void nandGetTypeCallback(s32 result, void* arg) {
    NANDCommandBlock* block = (NANDCommandBlock*)arg;

    if (result == IPC_RESULT_OK || result == IPC_RESULT_ACCESS) {
        *block->type = 2;
        result = IPC_RESULT_OK;
    } else if (result == IPC_RESULT_INVALID) {
        *block->type = 1;
        result = IPC_RESULT_OK;
    }

    ((NANDAsyncCb)block->callback)(nandConvertErrorCode(result), block);
}

const char* nandGetHomeDir(void) {
    return s_homeDir;
}
