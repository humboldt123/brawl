// OSExec: the part of the os that boots other code. packs launch arguments, talks to the es module to
// start titles (menu included), and boots a dol from the disc or from nand.
// HYPOTHESIS: the retail source had several more entry points (the OSLaunch* family) that nothing in brawl
// calls, so the linker stripped them. their error strings are still sitting in .data, so cheap stand-ins
// are kept below (marked MATCH-ONLY) purely so the string pool comes out in the same order.
#include "revolution/os.h"
#include "revolution/os/OSExec.h"
#include "revolution/os/OSExecParams.h"
#include "revolution/os/OSNandbootinfo.h"
#include "revolution/os/OSPlayTime.h"
#include "revolution/os/OSReset.h"
#include "revolution/os/OSStateFlags.h"
#include "revolution/os/OSStateTM.h"
#include "revolution/os/OSCache.h"
#include "revolution/os/OSInterrupt.h"
#include "revolution/os/OSThread.h"
#include "revolution/esp.h"
#include "revolution/dvd.h"
#include "revolution/nand.h"
#include "revolution/sc.h"
#include <cstring>
#include <cstdio>

extern s32 ESP_ListTitleContents(ESTitleId titleId, u32 *cids, u32 *count);

// not declared by the headers
extern bool DVDLowFinalize(void);
extern void DVDResume(void);
extern void __DVDPrepareResetAsync(void (*cb)(void));
extern void IPCReInit(void);
extern void IPCCltReInit(void);
extern void __OSInitIPCBuffer(void);
extern s32 NANDGetHomeDir(char *out);
extern s32 fn_8020D7BC(void *out);
extern void OSGetSaveRegion(void **start, void **end);
extern BOOL __OSInIPL;
extern size_t wcslen(const wchar_t *s);

#define OS_APP_TYPE_NAND 0x81
#define OS_APP_TYPE_DVD 0x80

#define MENU_TITLE_HI 1
#define MENU_TITLE_LO 2
#define TICKET_VIEW_SIZE 0xD8

// low memory words the boot code reads and writes
OSExecParams *OS_DOL_EXEC_PARAMS_PTR : 0x800030F0;
vs32 OS_DISC_OFFSET : 0x800030F4;
vu32 OS_LAUNCH_PARTITION_TYPE : 0x80003194;
vu32 OS_LAUNCH_PARTITION_OFFSET : 0x80003198;
vu32 OS_LAUNCH_ARG : 0x8000318C;
vu32 OS_LAUNCH_ARG2 : 0x80003190;
vu8 OS_LAUNCH_APP_TYPE : 0x80003186;

// sbss is laid out backwards in c, so these are declared top down
static vs32 ResetDone;
BOOL __OSInReboot;
static vu32 DiscResult;
static u32 DolBaseOffset;
u32 __OSNextPartitionType;

static DVDDiskID DiscId;

typedef struct PartitionGroups {
    u32 count0;
    u32 table0;
    u32 count1;
    u32 table1;
    u8 pad[0x10];
} PartitionGroups;

typedef struct PartitionEntry {
    u32 offset;
    u32 type;
} PartitionEntry;

// BYTES: the original reads the disc through a helper whose locals show up as dead constant stores,
// and checks the command status with a macro that calls the getter twice
#define STATUS_OK(b) (DVDGetCommandBlockStatus(b) <= 2 && DVDGetCommandBlockStatus(b) >= 0)

static inline void ReadDiscWords(void *dst, s32 len, s32 wordOffset) {
    DVDCommandBlock block;
    struct {
        u8 tmp[4];
    } unused = {0xFF, 0xFF, 0xFF, 0};

    DVDReadAbsAsyncPrio(&block, dst, len, wordOffset, NULL, 0);
    while (DVDGetCommandBlockStatus(&block) != 0) {
        if (!STATUS_OK(&block)) {
            __OSReturnToMenuForError();
        }
    }
}

// the offset of the dol area on the disc, read once from the first bytes the boot loader left behind
static inline u32 GetDolBaseOffset(void) {
    u32 base = DolBaseOffset;

    if (base != 0) {
        return base;
    }

    if (OS_DISC_OFFSET != 0) {
        s32 *buf = (s32 *)OSAllocFromMEM1ArenaLo(0x40, 0x20);
        ReadDiscWords(buf, 0x40, OS_DISC_OFFSET >> 2);
        base = (OS_DISC_OFFSET + buf[0x38 / 4]) >> 2;
        DolBaseOffset = base;
    } else {
        base = 0x910;
        DolBaseOffset = base;
    }

    return base;
}

static BOOL fn_801D8D34(char *addr, int argc, char **argv) {
    int n;
    char *top;
    char **newArgv;
    u32 j;
    int len;

    memset(addr, 0, 0x2000);
    if (argc == 0) {
        ((u32 *)addr)[2] = 0;
    } else {
        n = argc;
        top = addr + 0x2000;
        while (--argc >= 0) {
            char *s = argv[argc];
            len = strlen(s) + 1;
            top -= len;
            strcpy(top, s);
            argv[argc] = (char *)(top - addr);
        }

        newArgv = (char **)(addr + (((u32)(top - addr)) & ~3)) - (n + 1);
        for (j = 0; j < n + 1; j++) {
            newArgv[j] = argv[j];
        }

        newArgv[-1] = (char *)n;
        ((u32 *)addr)[2] = (u32)((char *)(newArgv - 1) - addr);
    }

    return TRUE;
}

// writes the bytes of a utf-16 string out as hex digits
static BOOL fn_801D8EB4(char *dst, const char *src) {
    int k;

    if (src != NULL) {
        while (src[0] != 0 || src[1] != 0) {
            for (k = 0; k < 4; k++) {
                int odd = k & 1;
                int mask = odd ? 0x0F : 0xF0;
                int n = (src[0] & mask) >> (u8)(odd ? 0 : 4);

                if (n >= 0 && n < 10) {
                    *dst = '0' + n;
                } else if (n >= 10 && n < 16) {
                    *dst = 'a' + (n - 10);
                } else {
                    return FALSE;
                }

                dst++;
                if (odd) {
                    src++;
                }
            }
        }

        *dst = 0;
        return TRUE;
    }

    return FALSE;
}

// same as fn_801D8D34 but every even entry from the third on is utf-16 and gets stored as hex
static BOOL fn_801D900C(char *addr, int argc, char **argv) {
    int n;
    char *top;
    char **newArgv;
    u32 j;
    int len;

    memset(addr, 0, 0x2000);
    if (argc == 0) {
        ((u32 *)addr)[2] = 0;
    } else {
        n = argc;
        top = addr + 0x2000;
        while (--argc >= 0) {
            if (argc < 2 || argc % 2 != 0) {
                char *s = argv[argc];
                len = strlen(s) + 1;
                top -= len;
                strcpy(top, s);
                argv[argc] = (char *)(top - addr);
            } else {
                char *s = argv[argc];
                len = wcslen((wchar_t *)s) * 4 + 1;
                top -= len;
                fn_801D8EB4(top, s);
                argv[argc] = (char *)(top - addr);
            }
        }

        newArgv = (char **)(addr + (((u32)(top - addr)) & ~3)) - (n + 1);
        for (j = 0; j < n + 1; j++) {
            newArgv[j] = argv[j];
        }

        newArgv[-1] = (char *)n;
        ((u32 *)addr)[2] = (u32)((char *)(newArgv - 1) - addr);
    }

    return TRUE;
}

// jumps to the entry point of whatever was just loaded (a real call in the original, not inlined)
#pragma push
#pragma auto_inline off
static void fn_801D91D8(register void (*entry)(void)) {
    ICFlashInvalidate();
    asm {
        sync
        isync
        mtctr entry
        bctr
    }
}
#pragma pop

static void fn_801D9214(void) {
    ResetDone = 1;
}

void __OSGetExecParams(OSExecParams *out) {
    if (OS_DOL_EXEC_PARAMS_PTR >= (OSExecParams *)0x80000000) {
        memcpy(out, OS_DOL_EXEC_PARAMS_PTR, sizeof(OSExecParams));
    } else {
        out->valid = FALSE;
    }
}

static void fn_801D9244(u32 result) {
    DiscResult = result;
}

#define WAIT_DISC(line)                                                  \
    do {                                                                 \
        while (DiscResult == 0)                                          \
            ;                                                            \
        if (DiscResult != 1) {                                           \
            OSReport("\nOSExec(): Failed to exec %d in %d\n", DiscResult, line); \
            __OSReturnToMenuForError();                                  \
        }                                                                \
    } while (0)

// reloads ios for the partition of the next title and opens that partition on the disc
static void fn_801D924C(void) {
    u32 viewCount;
    ESTicketView *view;
    PartitionGroups *groups;
    ESTicketView *views;
    u32 memHi;
    s32 err;
    PartitionEntry *table;
    u32 iosLo;
    PartitionEntry *entry;
    u32 memLo;
    u32 iosHi;
    u32 tmdSize;
    PartitionEntry *part;
    ESTitleMeta *tmd;
    s32 i;

    err = -1;
    viewCount = 1;
    tmdSize = 0;
    groups = (PartitionGroups *)OSAllocFromMEM1ArenaLo(0x20, 0x20);
    table = (PartitionEntry *)OSAllocFromMEM1ArenaLo(0x800, 0x20);
    tmd = (ESTitleMeta *)OSAllocFromMEM1ArenaLo(0x4A00, 0x40);
    view = (ESTicketView *)OSAllocFromMEM1ArenaLo(0xE0, 0x20);

    if (__OSNextPartitionType == OS_LAUNCH_PARTITION_TYPE && OS_LAUNCH_PARTITION_OFFSET != 0) {
        err = ESP_InitLib();
        if (err == 0) {
            err = ESP_DiGetTicketView(NULL, view);
        }
        if (err == 0) {
            err = ESP_DiGetTmd(NULL, &tmdSize);
        }
        if (err == 0) {
            err = ESP_DiGetTmd(tmd, &tmdSize);
        }
        ESP_CloseLib();

        if (OSPlayTimeIsLimited()) {
            __OSPlayTimeType type = (__OSPlayTimeType)0;
            u32 time = -1;

            __OSGetPlayTime(view, &type, &time);
            if (time == 0) {
                __OSWriteExpiredFlag();
                __OSReturnToMenuForError();
            }
        }
    }

    if (err == 0) {
        part = table;
        table[0].type = OS_LAUNCH_PARTITION_TYPE;
        table[0].offset = OS_LAUNCH_PARTITION_OFFSET;
    } else {
        DiscResult = 0;
        DVDLowClosePartition(fn_801D9244);
        WAIT_DISC(895);

        DiscResult = 0;
        DVDLowUnencryptedRead(groups, 0x20, 0x10000, fn_801D9244);
        WAIT_DISC(905);

        DiscResult = 0;
        DVDLowUnencryptedRead(table, 0x800, groups->table0, fn_801D9244);
        WAIT_DISC(917);

        part = NULL;
        for (i = 0, entry = table; (u8)i < groups->count0; entry++, i++) {
            if (entry->type == __OSNextPartitionType) {
                part = entry;
            }
        }

        if (part == NULL) {
            for (i = 0, entry = table + 4; (u8)i < groups->count1; entry++, i++) {
                if (entry->type == __OSNextPartitionType) {
                    part = entry;
                }
            }
        }

        if (part == NULL) {
            OSReport("\nOSExec(): The specified game doesn't exist in the disc\n");
            __OSReturnToMenuForError();
        }

        OS_LAUNCH_PARTITION_TYPE = part->type;
        OS_LAUNCH_PARTITION_OFFSET = part->offset;

        DiscResult = 0;
        if (*(vu8 *)0x80003187 == OS_APP_TYPE_DVD) {
            DVDLowOpenPartitionWithTmdAndTicketView(part->offset, view, tmdSize, tmd, 0, NULL,
                                                    fn_801D9244);
        } else {
            DVDLowOpenPartition(part->offset, NULL, 0, NULL, tmd, fn_801D9244);
        }
        WAIT_DISC(978);

        DiscResult = 0;
        DVDLowClosePartition(fn_801D9244);
        WAIT_DISC(988);
    }

    iosHi = *(u32 *)((u8 *)tmd + 0x184);
    iosLo = *(u32 *)((u8 *)tmd + 0x188);

    err = ESP_InitLib();
    if (err != 0) {
        OSReport("\nOSExec(): Failed to exec %d in %d\n", err, 999);
        __OSHotResetForError();
    }

    err = ESP_GetTicketViews(((u64)iosHi << 32) | iosLo, NULL, &viewCount);
    if (viewCount != 1 || err != 0) {
        OSReport("\nOSExec(): Failed to exec %d in %d\n", err, 1008);
        __OSHotResetForError();
    }

    views = (ESTicketView *)OSAllocFromMEM1ArenaLo(((viewCount * TICKET_VIEW_SIZE) + 0x1F) & ~0x1F, 0x20);
    err = ESP_GetTicketViews(((u64)iosHi << 32) | iosLo, views, &viewCount);
    if (err != 0) {
        OSReport("\nOSExec(): Failed to exec %d in %d\n", err, 1017);
        __OSHotResetForError();
    }

    DVDLowFinalize();
    memHi = *(vu32 *)0x8000311C;
    memLo = *(vu32 *)0x80003120;
    DCStoreRange((void *)0x80003100, 0x100);

    err = ESP_LaunchTitle(((u64)iosHi << 32) | iosLo, views);
    if (err != 0) {
        OSReport("\nOSExec(): Failed to exec %d in %d\n", err, 1033);
        __OSHotResetForError();
    }

    ESP_CloseLib();
    DCInvalidateRange((void *)0x80003100, 0x100);

    if (memHi < *(vu32 *)0x8000311C) {
        u32 cur = *(vu32 *)0x8000311C;
        *(vu32 *)0x80003120 = memHi - (cur - *(vu32 *)0x80003120);
        *(vu32 *)0x80003128 = memHi - (cur - *(vu32 *)0x80003128);
        *(vu32 *)0x80003130 = memHi - (cur - *(vu32 *)0x80003130);
        *(vu32 *)0x80003134 = memHi - (cur - *(vu32 *)0x80003134);
        *(vu32 *)0x8000311C = memHi;
    }

    if (memLo < *(vu32 *)0x80003120) {
        __OSInitMemoryProtection();
    }

    __OSInitIPCBuffer();
    IPCReInit();
    IPCCltReInit();
    DVDLowInit();

    DiscResult = 0;
    DVDLowReadDiskID(&DiscId, fn_801D9244);
    WAIT_DISC(1077);

    DiscResult = 0;
    if (*(vu8 *)0x80003187 == OS_APP_TYPE_DVD) {
        DVDLowOpenPartitionWithTmdAndTicketView(part->offset, view, tmdSize, tmd, 0, NULL,
                                                fn_801D9244);
    } else {
        DVDLowOpenPartition(part->offset, NULL, 0, NULL, tmd, fn_801D9244);
    }
    WAIT_DISC(1100);
}

void __OSLaunchMenu(void) {
    u32 count = 1;
    ESTicketView *views;
    s32 err;

    OSSetArenaLo((void *)0x81280000);
    OSSetArenaHi((void *)0x812F0000);

    err = ESP_InitLib();
    if (err != 0) {
        return;
    }

    err = ESP_GetTicketViews(((u64)MENU_TITLE_HI << 32) | MENU_TITLE_LO, NULL, &count);
    if (count != 1 || err != 0) {
        return;
    }

    views = (ESTicketView *)OSAllocFromMEM1ArenaLo(((count * TICKET_VIEW_SIZE) + 0x1F) & ~0x1F, 0x20);
    if (ESP_GetTicketViews(((u64)MENU_TITLE_HI << 32) | MENU_TITLE_LO, views, &count) != 0) {
        return;
    }

    if (ESP_LaunchTitle(((u64)MENU_TITLE_HI << 32) | MENU_TITLE_LO, views) != 0) {
        return;
    }

    while (TRUE) {
    }
}

// true when the group/company of the title's nand data matches the disc (or the home dir)
static BOOL fn_801D997C(ESTitleId tid, BOOL useDisc) {
    char home[0x44];
    char path[0x44];
    NANDStatus status;
    u16 want;
    u16 have;

    if (NANDGetHomeDir(home) != 0) {
        return FALSE;
    }

    if (NANDGetStatus(home, &status) != 0) {
        return FALSE;
    }

    want = status.groupId;
    if (want == 2 && OSGetAppType() == OS_APP_TYPE_DVD && (OSGetConsoleType() & 0xF0000000) == 0x10000000) {
        want = *(u16 *)((u8 *)DVDGetCurrentDiskID() + 4);
    }

    if (useDisc) {
        have = *(u16 *)((u8 *)DVDGetCurrentDiskID() + 4);
    } else {
        sprintf(path, "/title/%08x/%08x/data", (u32)((tid >> 32) & 0xFFFFFFFF), (u32)(tid & 0xFFFFFFFF));
        if (NANDGetStatus(path, &status) != 0) {
            return FALSE;
        }
        have = status.groupId;
    }

    return want == have;
}

// picks the ticket view with the most play time left (or most remaining launches)
static s32 fn_801D9A90(ESTicketView *views, u32 count) {
    ESTicketView *v;
    u32 i;
    s32 best = 0;
    BOOL foundPermanent = FALSE;
    u32 bestPermanent = 0;
    u32 bestTime = 0;

    if (views == NULL) {
        OSReport("NULL pointer detected: line %d in %s\n", 1346, "OSExec.c");
        return -0x3F9;
    }

    for (v = views, i = 0; i < count; v++, i++) {
        u32 time;
        __OSPlayTimeType type;
        s32 r = __OSGetPlayTime(v, &type, &time);

        if (r > 0) {
            continue;
        }
        if (r != 0) {
            return r;
        }

        switch (type) {
        case OSPLAYTIME_PERMANENT: {
            u16 mask;
            u32 j;
            u32 n = 0;

            if (!foundPermanent) {
                best = i;
                foundPermanent = TRUE;
            }

            mask = *(u8 *)((u8 *)v + 0x18) | (*(u8 *)((u8 *)v + 0x19) << 8);
            for (j = 0; j < 16; j++) {
                if (mask & (1 << j)) {
                    n++;
                }
            }

            if (n > bestPermanent) {
                best = i;
                bestPermanent = n;
            }
            break;
        }
        case OSPLAYTIME_LIMITED:
            if (!foundPermanent && time > bestTime) {
                best = i;
                bestTime = time;
            }
            break;
        case OSPLAYTIME_OTHER:
            break;
        }
    }

    if (!foundPermanent && bestTime == 0) {
        return -1;
    }

    return best;
}

// relaunches the current title with its own ticket view
void fn_801D9C80(void) {
    ESTitleId tid __attribute__((aligned(32)));
    ESTicketView *view;
    ESTicketView *views;
    u32 count = 1;
    s32 err;

    OSSetArenaLo((void *)0x81280000);
    OSSetArenaHi((void *)0x812F0000);

    err = ESP_InitLib();
    if (err != 0) {
        __OSReturnToMenuForError();
    }

    err = ESP_GetTitleId(&tid);
    if (err != 0) {
        __OSReturnToMenuForError();
    }

    view = (ESTicketView *)OSAllocFromMEM1ArenaLo(0xE0, 0x20);
    if (view == NULL) {
        __OSReturnToMenuForError();
    }

    memset(view, 0, 0xE0);
    err = ESP_DiGetTicketView(NULL, view);
    if (err == -0x3F9) {
        err = ESP_GetTicketViews(tid, NULL, &count);
        if (err != 0) {
            __OSReturnToMenuForError();
        }

        views = (ESTicketView *)OSAllocFromMEM1ArenaLo(((count * TICKET_VIEW_SIZE) + 0x1F) & ~0x1F, 0x20);
        if (views == NULL) {
            __OSReturnToMenuForError();
        }

        err = ESP_GetTicketViews(tid, views, &count);
        if (err != 0) {
            __OSReturnToMenuForError();
        }

        memcpy(view, views, TICKET_VIEW_SIZE);
    } else if (err != 0) {
        __OSReturnToMenuForError();
    } else if (OSPlayTimeIsLimited()) {
        __OSPlayTimeType type = (__OSPlayTimeType)0;
        u32 time = -1;

        __OSGetPlayTime(view, &type, &time);
        if (time == 0) {
            __OSWriteExpiredFlag();
            __OSReturnToMenuForError();
        }
    }

    err = ESP_LaunchTitle(tid, view);
    if (err != 0) {
        __OSReturnToMenuForError();
    }

    while (TRUE) {
    }
}

// launches a title by looking up its best ticket view
static void fn_801D9E10(ESTitleId tid) {
    u32 count = 1;
    ESTicketView *view;
    ESTicketView *views;
    s32 best;

    if (ESP_InitLib() != 0) {
        return;
    }

    view = (ESTicketView *)OSAllocFromMEM1ArenaLo(0xE0, 0x20);
    if (view == NULL) {
        return;
    }

    memset(view, 0, 0xE0);
    if (ESP_GetTicketViews(tid, NULL, &count) != 0) {
        return;
    }

    views = (ESTicketView *)OSAllocFromMEM1ArenaLo(((count * TICKET_VIEW_SIZE) + 0x1F) & ~0x1F, 0x20);
    if (views == NULL) {
        return;
    }

    if (ESP_GetTicketViews(tid, views, &count) != 0) {
        return;
    }

    best = fn_801D9A90(views, count);
    if (best < 0) {
        return;
    }

    memcpy(view, &views[best], TICKET_VIEW_SIZE);
    if (ESP_LaunchTitle(tid, view) != 0) {
        return;
    }

    while (TRUE) {
    }
}

#define IS_MENU_TITLE(tid) ((tid) == (((u64)MENU_TITLE_HI << 32) | MENU_TITLE_LO))

// starts a title through the es module. argValue and argv go into the nandboot info so the next title can
// read them back, check turns on the region/company sanity checks.
static void fn_801D9F2C(ESTitleId tid, u32 argValue, char **argv, BOOL check) {
    u32 tmdSize = 0;
    ESTmdView *tmd = NULL;
    s32 err;

    err = ESP_InitLib();
    if (err != 0) {
        __OSReturnToMenuForError();
    }

    err = ESP_GetTmdView(tid, NULL, &tmdSize);
    if (err != 0) {
        __OSReturnToMenuForError();
    }

    tmd = (ESTmdView *)OSAllocFromMEM1ArenaLo((tmdSize + 0x1F) & ~0x1F, 0x40);
    if (tmd == NULL) {
        __OSReturnToMenuForError();
    }

    err = ESP_GetTmdView(tid, tmd, &tmdSize);
    if (err != 0) {
        __OSReturnToMenuForError();
    }

    {
        u32 contents = 0;
        u32 views = 0;
        BOOL installed;
        ESTitleId ios = tmd->head.sysVersion;

        if (ESP_GetTicketViews(ios, NULL, &views) != 0 || views != 1) {
            installed = FALSE;
        } else if (ESP_ListTitleContents(ios, NULL, &contents) != 0 || contents == 0) {
            installed = FALSE;
        } else {
            installed = TRUE;
        }

        if (!installed) {
            OSReport("OSLaunchTitle(): Firmware is not installed\n");
            __OSReturnToMenuForError();
        }
    }

    if (check) {
        u32 region;
        BOOL regionOk;

        if (!fn_801D997C(tid, FALSE)) {
            OSReport("OSLaunchTitle(): Company code is not correct\n");
            __OSReturnToMenuForError();
        }

        region = *(u32 *)&tmd->head.reserved[0];
        if (region == 3) {
            regionOk = TRUE;
        } else if (region == (u32)SCGetProductGameRegion()) {
            regionOk = TRUE;
        } else {
            regionOk = FALSE;
        }

        if (!regionOk) {
            OSReport("OSLaunchTitle(): Country code is not correct\n");
            __OSReturnToMenuForError();
        }
    }

    {
        BOOL spinup;

        if (tmd->head.reserved[0x18] & 1) {
            spinup = TRUE;
        } else {
            spinup = FALSE;
        }

        if (!spinup) {
            if (__DVDGetCoverStatus() == 2) {
                __DVDResetWithNoSpinup();
            }
        }
    }

    if (argv || argValue) {
        OSNandbootInfo *info;
        u8 *buf;
        OSStateFlags flags;
        ESTitleId cur __attribute__((aligned(32)));
        char name[0x18];
        char **newArgv;
        int n;
        int i;

        buf = (u8 *)OSAllocFromMEM1ArenaLo(0x2000, 0x40);
        info = (OSNandbootInfo *)(buf + 0xFE0);
        sprintf(name, "%016llx", tid);

        n = 0;
        if (argv) {
            while (argv[n]) {
                n++;
            }
        }

        n++;
        newArgv = (char **)OSAllocFromMEM1ArenaLo((n + 1) * 4, 1);
        newArgv[0] = name;
        for (i = 1; i < n; i++) {
            newArgv[i] = argv[i - 1];
        }

        fn_801D8D34((char *)buf, n, newArgv);

        if (__OSInIPL) {
            cur = ((u64)MENU_TITLE_HI << 32) | MENU_TITLE_LO;
        } else {
            err = ESP_InitLib();
            if (err != 0) {
                OSReport("\nOSExec(): Failed to exec %d in %d\n", err, 1719);
                __OSHotResetForError();
            }

            err = ESP_GetTitleId(&cur);
            if (err != 0) {
                OSReport("\nOSExec(): Failed to exec %d in %d\n", err, 1727);
                __OSHotResetForError();
            }
        }

        info->LastTitleId = cur;
        info->ArgOffset = ((u32 *)buf)[2];
        info->LastAppType = OSGetAppType();
        if (IS_MENU_TITLE(tid)) {
            info->ReturnType = 4;
        } else {
            info->ReturnType = 2;
        }
        info->ArgValue = argValue;
        __OSCreateNandbootInfo();
        __OSWriteNandbootInfo(info);

        __OSReadStateFlags(&flags);
        flags.lastShutdown = 3;
        if (IS_MENU_TITLE(tid)) {
            flags.lastDiscState = __OSGetDiscState(flags.lastDiscState);
            flags.menuMode = 2;
        }
        __OSWriteStateFlags(&flags);
    }

    OSDisableScheduler();
    __OSShutdownDevices(6);
    OSEnableScheduler();
    fn_801D9E10(tid);
    __OSLaunchMenu();
}

void __OSReturnToMenul(u32 argValue, const char *arg0, ...) {
    va_list ap;
    char **argv;
    int n;

    __OSUnRegisterStateEvent();
    OSSetArenaLo((void *)0x81280000);
    OSSetArenaHi((void *)0x812F0000);

    argv = (char **)OSAllocFromMEM1ArenaLo(4, 0x1000);
    n = 0;
    va_start(ap, arg0);
    while ((argv[n++] = (char *)arg0) != NULL) {
        arg0 = va_arg(ap, const char *);
    }
    va_end(ap);

    fn_801D9F2C(((u64)MENU_TITLE_HI << 32) | MENU_TITLE_LO, argValue, argv, FALSE);
    __OSReturnToMenuForError();
}

// MATCH-ONLY: nothing calls these in brawl so the linker drops them, but their strings stay in .data and
// the pool order depends on them. the bodies are guesses that just touch the right messages in order.
void __OSLaunchTitlevForSystem(void) {
    OSReport("__OSLaunchTitlevForSystem(): only system can call this function\n");
}

void __OSLaunchTitlelForSystem(void) {
    OSReport("__OSLaunchTitlelForSystem(): only system can call this function\n");
}

void OSLaunchDisk(void) {
    OSReport("OSLaunchDisk(): You can't call this API from DVD application.  \n");
    OSReport("OSLaunchDisk(): You must call DVDPrepareDisk before launching disc.\n");
    OSReport("OSLaunchDisk(): Specified id is different from one specified to DVDPrepareDisk.\n");
    OSReport("0000000000000000");
}

extern int DVDLayoutFormatDummy;
extern u32 __DVDLayoutFormat;

// boots a dol: runs the disc loader (ipl, reboot or nand flavours), then jumps to the entry point
static void fn_801DA4DC(s32 dolOffset, u32 restartCode, void *regionStart, void *regionEnd, BOOL argsUseDefault,
                        int argc, char **argv) {
    OSExecParams *params;
    u32 *hdr;

    OSDisableInterrupts();

    if (__OSInReboot) {
        __OSNextPartitionType = OS_LAUNCH_PARTITION_TYPE;
    }

    params = (OSExecParams *)OSAllocFromMEM1ArenaLo(0x1C, 1);
    params->valid = TRUE;
    params->restartCode = restartCode;
    params->regionStart = regionStart;
    params->regionEnd = regionEnd;
    params->argsUseDefault = argsUseDefault;

    if (argsUseDefault == 0) {
        params->argsAddr = OSAllocFromMEM1ArenaLo(0x2000, 1);
        if (__OSNextPartitionType == 2 && !__OSInReboot) {
            fn_801D900C((char *)params->argsAddr, argc, argv);
        } else {
            fn_801D8D34((char *)params->argsAddr, argc, argv);
        }
    }

    DVDInit();
    DVDSetAutoInvalidation(TRUE);
    DVDResume();
    ResetDone = 0;
    __DVDPrepareResetAsync(fn_801D9214);
    __OSMaskInterrupts(0xFFFFFFF0);
    __OSUnmaskInterrupts(0x10);
    OSEnableInterrupts();

    while (ResetDone != 1) {
    }

    fn_801D924C();

    if (restartCode == 0xA0000000 && !__OSInReboot) {
        ESTitleId tid __attribute__((aligned(32)));

        if (ESP_InitLib() != 0) {
            return;
        }
        if (ESP_GetTitleId(&tid) != 0) {
            return;
        }
        if (ESP_CloseLib() != 0) {
            return;
        }

        snprintf((char *)params->argsAddr + ((u32 *)argv)[1], 0x11, "%016llx", tid);
    }

    hdr = (u32 *)OSAllocFromMEM1ArenaLo(0x20, 0x20);
    ReadDiscWords(hdr, 0x20, GetDolBaseOffset());
    ReadDiscWords((void *)0x81200000, (hdr[5] + 0x1F) & ~0x1F, GetDolBaseOffset() + 8);
    ICInvalidateRange((void *)0x81200000, (hdr[5] + 0x1F) & ~0x1F);

    if (strncmp((char *)hdr, "2004/02/01", 10) > 0) {
        void (*init)(void *);
        int (*main)(void **, u32 *, u32 *);
        void *(*close)(void);
        void *entry;
        OSExecParams *copy;
        void *dst;
        u32 size;
        u32 offset;

        if (dolOffset == -1) {
            dolOffset = ((hdr[5] + 0x20) >> 2) + GetDolBaseOffset();
        }

        params->bootDol = dolOffset;
        ((void (*)(void *, void *, void *))hdr[4])(&init, &main, &close);

        copy = (OSExecParams *)OSAllocFromMEM1ArenaLo(0x1C, 1);
        memcpy(copy, params, 0x1C);
        OS_DOL_EXEC_PARAMS_PTR = copy;

        init(OSReport);
        OSSetArenaLo(copy);

        while (main(&dst, &size, &offset)) {
            ReadDiscWords(dst, size, offset >> __DVDLayoutFormat);
        }

        entry = close();

        *(vu32 *)0x80003180 = *(vu32 *)0x80000000;
        *(vu8 *)0x80003184 = 0x80;
        copy = (OSExecParams *)OSAllocFromMEM1ArenaLo(0x1C, 1);
        memcpy(copy, params, 0x1C);
        OS_DOL_EXEC_PARAMS_PTR = copy;
        *(vu32 *)0xCC003024 = 7;
        OSDisableInterrupts();
        fn_801D91D8((void (*)(void))entry);
    } else {
        *(vu32 *)0x812FDFF0 = (u32)regionStart;
        *(vu32 *)0x812FDFEC = (u32)regionEnd;
        *(vu8 *)0x800030E2 = 1;

        ReadDiscWords((void *)0x81330000, (hdr[6] + 0x1F) & ~0x1F, ((hdr[5] + 0x20) >> 2) + GetDolBaseOffset());
        ICInvalidateRange((void *)0x81330000, (hdr[6] + 0x1F) & ~0x1F);
        OSDisableInterrupts();
        ICFlashInvalidate();
        fn_801D91D8((void (*)(void))0x81330000);
    }
}

void __OSBootDol(u32 dolOffset, u32 restartCode, const char **argv) {
    void *regionStart;
    void *regionEnd;
    char name[0x20];
    char **newArgv;
    int n;
    int i;

    OSGetSaveRegion(&regionStart, &regionEnd);
    sprintf(name, "%d", dolOffset);

    n = 0;
    if (argv) {
        while (argv[n]) {
            n++;
        }
    }

    newArgv = (char **)OSAllocFromMEM1ArenaLo((n + 2) * 4, 1);
    newArgv[0] = name;
    for (i = 1; i < n + 1; i++) {
        newArgv[i] = (char *)argv[i - 1];
    }

    fn_801DA4DC(-1, restartCode, regionStart, regionEnd, FALSE, n + 1, newArgv);
}

// guesses a title launch from a (hi, lo) title id, goes through the dol loader in the end
static void fn_801DAF04(ESTitleId titleId, u32 argValue, char **argv) {
    u8 appType = OSGetAppType();
    char path[0x20];
    u8 scratch[0x20];
    ESTitleId tid;
    NANDStatus status;
    int n;
    char **newArgv;
    OSNandbootInfo *info;
    s32 err;
    int i;

    if (appType == OS_APP_TYPE_NAND) {
        OSReport("OSLaunchPartition(): You can't call this API from NAND application.  \n");
        OSPanic("OSExec.c", 2841, "");
    }

    if (titleId != 0 && (u32)(titleId >> 32) != 0x10000 && (u32)(titleId >> 32) != 0x10004) {
        OSReport("OSLaunchPartition(): Specified title ID is not DISC application.  \n");
        OSPanic("OSExec.c", 2850, "");
    }

    OS_LAUNCH_ARG = argValue;
    OS_LAUNCH_ARG2 = 0;
    OS_LAUNCH_APP_TYPE = appType;

    if (titleId != 0) {
        sprintf(path, "/title/%08x/%08x", (u32)(titleId >> 32), (u32)titleId);
        err = NANDGetStatus(path, &status);
        switch (err) {
        case 0:
            break;
        case -12:
            if (fn_8020D7BC(scratch) != 0) {
                OSReport("\nOSLaunchPartition(): Failed to get free i-nodes number\n");
                __OSHotResetForError();
            }

            if (0x17FF - *(u32 *)(scratch + 0x18) < 5) {
                OSReport("\nOSLaunchPartition(): There are not enough i-nodes to launch next partition\n");
                __OSHotResetForError();
            }
            break;
        default:
            OSReport("\nOSLaunchPartition(): Failed to get nand status\n");
            __OSHotResetForError();
            break;
        }
    }

    sprintf(path, "0000000000000000");

    n = 0;
    if (argv) {
        while (argv[n]) {
            n++;
        }
    }

    newArgv = (char **)OSAllocFromMEM1ArenaLo((n + 2) * 4, 1);
    newArgv[0] = path;
    for (i = 1; i < n + 1; i++) {
        newArgv[i] = argv[i - 1];
    }
    newArgv[n + 1] = NULL;

    info = (OSNandbootInfo *)OSAllocFromMEM1ArenaLo(0x1020, 0x40);
    memset(info, 0, 0x1020);

    err = ESP_InitLib();
    if (err != 0) {
        OSReport("\nOSExec(): Failed to exec %d in %d\n", err, 2925);
        __OSHotResetForError();
    }

    err = ESP_GetTitleId(&tid);
    if (err != 0) {
        OSReport("\nOSExec(): Failed to exec %d in %d\n", err, 2933);
        __OSHotResetForError();
    }

    err = ESP_CloseLib();
    if (err != 0) {
        OSReport("\nOSExec(): Failed to exec %d in %d\n", err, 2941);
        __OSHotResetForError();
    }

    info->LastTitleId = tid;
    info->LastAppType = appType;
    __OSCreateNandbootInfo();
    __OSWriteNandbootInfo(info);

    __OSNextPartitionType = (u32)titleId;
    __OSBootDol(0, 0xA0000000, (const char **)newArgv);

    OSDisableScheduler();
    __OSShutdownDevices(1);
    __OSHotResetForError();
}

void fn_801DB2B8(ESTitleId tid, u32 argValue, char *arg0, ...) {
    va_list ap;
    char **argv;
    int n;

    __OSStopPlayRecord();
    __OSUnRegisterStateEvent();
    OSDisableScheduler();
    __OSShutdownDevices(6);
    OSEnableScheduler();
    OSDisableInterrupts();
    OSSetArenaLo((void *)0x81280000);
    OSSetArenaHi((void *)0x812F0000);

    argv = (char **)OSAllocFromMEM1ArenaLo(4, 0x1000);
    n = 0;
    va_start(ap, arg0);
    while ((argv[n++] = arg0) != NULL) {
        arg0 = va_arg(ap, char *);
    }
    va_end(ap);

    fn_801DAF04(tid, argValue, argv);
}
