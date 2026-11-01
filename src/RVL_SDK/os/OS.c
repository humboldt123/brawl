// OS: the main os bring-up. fpr init, console type detection, arena clearing, exception vector install and
// the OSInit sequence that wakes every other os module up in order.
#include "revolution/os.h"
#include "revolution/os/OSBootInfo.h"
#include "revolution/dvd.h"
#include "revolution/exi.h"
#include "revolution/ipc.h"
#include "revolution/pad.h"
#include "revolution/sc.h"
#include "revolution/si.h"
#include "revolution/db.h"
#include "revolution/base/PPCArch.h"
#include <cstring>

// BYTES: integer-0 NULL, so pointer tests against NULL compile as signed cmpwi as in retail. Not header-wide:
// making revolution/types.h define NULL 0 for C changes no other SDK unit except OSExec and nwc24, whose
// retail compares are cmplwi ((void*)0), so the NULL spelling differs per original source file.
#undef NULL
#define NULL 0

#define LOWMEM(off) ((void *)(0x80000000 + (off)))
#define MAKE_DVD_DEVICE_CODE_(x) ((1 << 15) | (x))
#define MB_(x) ((x) >> 20)

typedef struct OSBI2 {
    u8 pad0[0xC];
    u32 debugFlag;
    u8 pad1[0x14];
    u32 padSpec;
} OSBI2;

extern u32 __PADSpec;
extern u8 _stack_addr[];
extern u8 __ArenaLo[];
extern u8 __ArenaHi[];
extern BOOL __OSInReboot;
extern s32 ESP_InitLib(void);
extern s32 ESP_GetTitleId(u64 *);
extern s32 ESP_CloseLib(void);
extern BOOL __DVDCheckDevice(void);
extern void __OSInitPlayTime(void);
extern void OSReturnToMenu(void);
extern BOOL __DVDLongFileNameFlag;
extern void EnableMetroTRKInterrupts(void);
extern void SIInit(void);
extern u32 OSGetConsoleSimulatedMem1Size(void);
extern u32 OSGetConsoleSimulatedMem2Size(void);

OSExecParams __OSRebootParams;
static DVDDriveInfo DriveInfo __attribute__((aligned(32)));
static DVDCommandBlock DriveBlock;

s64 __OSStartTime;
static OSBootInfo* BootInfo;
static u32* BI2DebugFlag;
static u32 BI2DebugFlagHolder;

static char GameNameBuffer[5];

static f64 ZeroF;     // for lfd
static f32 ZeroPS[2]; // for psl_l

static __OSExceptionHandler* OSExceptionTable;

static BOOL AreWeInitialized;
BOOL __OSIsGcam;
BOOL __OSInNandBoot;
BOOL __OSInIPL;

const char* __OSVersion =
    "<< RVL_SDK - OS \trelease build: Sep  7 2007 02:14:12 (0x4199_60831) >>";

static void OSExceptionInit(void);
void OSDefaultExceptionHandler(__OSException, OSContext *);

void __OSDBINTSTART(void);
void __OSDBINTEND(void);
void __OSDBJUMPSTART(void);
void __OSDBJUMPDEST(void);
void __OSDBJUMPEND(void);
void __OSEVStart(void);
void __DBVECTOR(void);
void __OSEVSetNumber(void);
void __OSEVEnd(void);


asm void __OSFPRInit(void) {
    nofralloc

    // Set FP available bit
    mfmsr r3
    ori r3, r3, 0x2000
    mtmsr r3

    // Check if paired-singles are enabled in HID2
    mfspr r3, 0x398
    rlwinm. r3, r3, 3, 31, 31
    beq paired_singles_disabled

    lis r3, ZeroPS@ha
    addi r3, r3, ZeroPS@l
    psq_l f0, 0(r3), 0, 0
    ps_mr f1, f0
    ps_mr f2, f0
    ps_mr f3, f0
    ps_mr f4, f0
    ps_mr f5, f0
    ps_mr f6, f0
    ps_mr f7, f0
    ps_mr f8, f0
    ps_mr f9, f0
    ps_mr f10, f0
    ps_mr f11, f0
    ps_mr f12, f0
    ps_mr f13, f0
    ps_mr f14, f0
    ps_mr f15, f0
    ps_mr f16, f0
    ps_mr f17, f0
    ps_mr f18, f0
    ps_mr f19, f0
    ps_mr f20, f0
    ps_mr f21, f0
    ps_mr f22, f0
    ps_mr f23, f0
    ps_mr f24, f0
    ps_mr f25, f0
    ps_mr f26, f0
    ps_mr f27, f0
    ps_mr f28, f0
    ps_mr f29, f0
    ps_mr f30, f0
    ps_mr f31, f0

paired_singles_disabled:
    lfd f0, ZeroF
    fmr f1, f0
    fmr f2, f0
    fmr f3, f0
    fmr f4, f0
    fmr f5, f0
    fmr f6, f0
    fmr f7, f0
    fmr f8, f0
    fmr f9, f0
    fmr f10, f0
    fmr f11, f0
    fmr f12, f0
    fmr f13, f0
    fmr f14, f0
    fmr f15, f0
    fmr f16, f0
    fmr f17, f0
    fmr f18, f0
    fmr f19, f0
    fmr f20, f0
    fmr f21, f0
    fmr f22, f0
    fmr f23, f0
    fmr f24, f0
    fmr f25, f0
    fmr f26, f0
    fmr f27, f0
    fmr f28, f0
    fmr f29, f0
    fmr f30, f0
    fmr f31, f0
    mtfsf 0xff, f0

    blr
}

static void DisableWriteGatherPipe(void) {
    PPCMthid2(PPCMfhid2() & ~0x40000000);
}

u32 __OSGetHollywoodRev(void) {
    return *(u32*)0x80003138;
}

void __OSGetIOSRev(OSIOSRev* rev) {
    u32 version = *(u32*)0xC0003140;
    u32 builddate = *(u32*)0xC0003144;

    rev->reserved = version >> 24 & 0xFF;
    rev->major = version >> 16 & 0xFF;
    rev->minor = version >> 8 & 0xFF;
    rev->micro = version >> 0 & 0xFF;

    rev->month = (builddate >> 16 & 0xF) + (builddate >> 20 & 0xF) * 10;
    rev->date = (builddate >> 8 & 0xF) + (builddate >> 12 & 0xF) * 10;
    rev->year =
        (builddate >> 0 & 0xF) + (builddate >> 4 & 0xF) * 10 + 2000;
}

u32 OSGetConsoleType(void) {
    u32 hollywood;
    u32 mem2size;

    if (BootInfo == NULL || BootInfo->consoleType == 0) {
        return 0x10000002;
    }

    hollywood = __OSGetHollywoodRev();

    if ((*(volatile u16 *)0x800030E6) & (1 << 15)) {
        switch ((*(volatile u16 *)0x800030E6) & ~(1 << 15)) {
        case 0x0002:
        case 0x0003:
        case 0x0203:
            switch (hollywood) {
            case 0x00000000:
                return 0x00000010;
            case 0x00000001:
                return 0x00000011;
            case 0x00000002:
                return 0x00000012;
            case 0x00000010:
                return 0x00000020;
            case 0x00000011:
                return 0x00000021;
            }

            if (hollywood > 0x00000011) {
                return 0x00000021;
            }

        case 0x0202:
        case 0x0201:
            switch (hollywood) {
            case 0x00000000:
                return 0x10000010;
            case 0x00000001:
                return 0x10000011;
            case 0x00000002:
                return 0x10000012;
            case 0x00000010:
                return 0x10000020;
            case 0x00000011:
                return 0x10000021;
            }

            if (hollywood > 0x00000011) {
                return 0x10000021;
            }

        case 0x0300:
            return 0x100;
        }
    }

    mem2size = OSGetPhysicalMem2Size();

    switch (hollywood) {
    case 0x00000000:
        if (mem2size == 0x4000000) {
            return 0x00000010;
        }
        return 0x10000010;
    case 0x00000001:
        if (mem2size == 0x4000000) {
            return 0x00000011;
        }
        return 0x10000011;
    case 0x00000002:
        if (mem2size == 0x4000000) {
            return 0x00000012;
        }
        return 0x10000012;
    case 0x00000010:
        if (mem2size == 0x4000000) {
            return 0x00000020;
        }
        return 0x10000020;
    case 0x00000011:
        if (mem2size == 0x4000000) {
            return 0x00000021;
        }
        return 0x10000021;
    }

    if (hollywood > 0x00000011) {
        if (mem2size == 0x4000000) {
            return 0x00000021;
        }
        return 0x10000021;
    }

    return BootInfo->consoleType;
}

static void MemClear(void* mem, u32 size) {
    void* flush = (0x40000 < size) ? (u32)mem + (size - 0x40000) : mem;
    DCZeroRange(mem, size);
    DCFlushRange(flush, 0x40000);
}

static void ClearArena(void) __attribute__((never_inline));
static void ClearArena(void) {
    // System reset
    if (!((OSGetResetCode() >> 31) & 1)) {
        MemClear(OSGetArenaLo(), (u32)OSGetArenaHi() - (u32)OSGetArenaLo());
    }
    // Region doesn't exist, or begins somewhere outside of MEM1
    else if (__OSRebootParams.regionStart == NULL ||
             !OSIsMEM1Region(__OSRebootParams.regionStart)) {
        MemClear(OSGetArenaLo(), (u32)OSGetArenaHi() - (u32)OSGetArenaLo());
    }
    // Region begins after arena begins
    else if (OSGetArenaLo() < __OSRebootParams.regionStart) {
        // Region is outside of arena
        if (OSGetArenaHi() <= __OSRebootParams.regionStart) {
            MemClear(OSGetArenaLo(), (u32)OSGetArenaHi() - (u32)OSGetArenaLo());
        }
        // Region begins in arena
        else {
            // Clear arena area before region
            MemClear(OSGetArenaLo(),
                     (u32)__OSRebootParams.regionStart - (u32)OSGetArenaLo());

            // Region ends inside arena
            if (OSGetArenaHi() > __OSRebootParams.regionEnd) {
                // Clear arena area after region
                MemClear(__OSRebootParams.regionEnd,
                         (u32)OSGetArenaHi() - (u32)__OSRebootParams.regionEnd);
            }
        }
    }
}

static void ClearMEM2Arena(void) __attribute__((never_inline));
static void ClearMEM2Arena(void) {
    // System reset
    if (!((OSGetResetCode() >> 31) & 1)) {
        MemClear(OSGetMEM2ArenaLo(),
                 (u32)OSGetMEM2ArenaHi() - (u32)OSGetMEM2ArenaLo());
    }
    // Region doesn't exist, or begins somewhere outside of MEM2
    else if (__OSRebootParams.regionStart == NULL ||
             !OSIsMEM2Region(__OSRebootParams.regionStart)) {
        MemClear(OSGetMEM2ArenaLo(),
                 (u32)OSGetMEM2ArenaHi() - (u32)OSGetMEM2ArenaLo());
    }
    // Region begins after arena begins
    else if (OSGetMEM2ArenaLo() < __OSRebootParams.regionStart) {
        // Region is outside of arena
        if (OSGetMEM2ArenaHi() <= __OSRebootParams.regionStart) {
            MemClear(OSGetMEM2ArenaLo(),
                     (u32)OSGetMEM2ArenaHi() - (u32)OSGetMEM2ArenaLo());
        }
        // Region begins in arena
        else {
            // Clear arena area before region
            MemClear(OSGetMEM2ArenaLo(), (u32)__OSRebootParams.regionStart -
                                             (u32)OSGetMEM2ArenaLo());
            // Region ends inside arena
            if (OSGetMEM2ArenaHi() > __OSRebootParams.regionEnd) {
                // Clear arena area after region
                MemClear(__OSRebootParams.regionEnd,
                         (u32)OSGetMEM2ArenaHi() -
                             (u32)__OSRebootParams.regionEnd);
            }
        }
    }
}

static void InquiryCallback(s32 result, DVDCommandBlock* block) {
#pragma unused(result)

    switch (block->state) {
    case DVD_STATE_END:
        (*(volatile u16*)0x800030E6) = MAKE_DVD_DEVICE_CODE_(DriveInfo.deviceCode);
        break;
    default:
        (*(volatile u16*)0x800030E6) = 0x0001;
        break;
    }
}

static void CheckTargets(void) {
    switch (*(u8*)0x8000315C) {
    case 0x81:
        OSReport("OS ERROR: boot program is not for RVL target. Please use "
                 "correct boot program.\n");
OSPanic("OS.c", 1156, "Failed to run app");
        break;
    case 0x80:
    default:
        break;
    }

    switch (*(u8*)0x8000315D) {
    case 0x81:
        OSReport("OS ERROR: apploader[D].img is not for RVL target. Please use "
                 "correct apploader[D].img.\n");
OSPanic("OS.c", 1174, "Failed to run app");
        break;
    case 0x80:
    default:
        break;
    }
}

static void ReportOSInfo(void) {
    u32 type;
    OSIOSRev rev;
    u32 category;
    u32 tdev;
    u32 mb;
    void *lo;
    void *hi;

    OSReport("\nRevolution OS\n");
    OSReport("Kernel built : %s %s\n", "Sep  7 2007", "02:14:12");

    OSReport("Console Type : ");
    type = OSGetConsoleType();
    category = type & 0xF0000000;
    switch (category) {
    case 0x00000000:
        switch (type) {
        case 0x00000011:
            OSReport("Pre-production board 1\n");
            break;
        case 0x00000012:
            OSReport("Pre-production board 2-1\n");
            break;
        case 0x00000020:
            OSReport("Pre-production board 2-2\n");
            break;
        case 0x00000100:
            OSReport("RVA 1\n");
            break;
        default:
            OSReport("Retail %d\n", type);
            break;
        }
        break;
    case 0x10000000:
        switch (type) {
        case 0x10000021:
            OSReport("NDEV 2.1\n");
            break;
        case 0x10000020:
            OSReport("NDEV 2.0\n");
            break;
        case 0x10000012:
            OSReport("NDEV 1.2\n");
            break;
        case 0x10000011:
            OSReport("NDEV 1.1\n");
            break;
        case 0x10000010:
            OSReport("NDEV 1.0\n");
            break;
        case 0x10000008:
            OSReport("Revolution Emulator\n");
            break;
        default:
            OSReport("Emulation platform (%08x)\n", type);
            break;
        }
        break;
    case 0x20000000:
        tdev = type & 0x0FFFFFFF;
        OSReport("TDEV-based emulation HW%d\n", tdev - 3);
        break;
    default:
        OSReport("%08x\n", type);
        break;
    }

    __OSGetIOSRev(&rev);
    OSReport("Firmware     : %d.%d.%d ", rev.major, rev.minor, rev.micro);
    OSReport("(%d/%d/%d)\n", rev.month, rev.date, rev.year);

    mb = (u32)(OSGetConsoleSimulatedMem1Size() + OSGetConsoleSimulatedMem2Size()) >> 20;
    OSReport("Memory %d MB\n", mb);
    hi = OSGetMEM1ArenaHi();
    lo = OSGetMEM1ArenaLo();
    OSReport("MEM1 Arena : 0x%x - 0x%x\n", lo, hi);
    hi = OSGetMEM2ArenaHi();
    lo = OSGetMEM2ArenaLo();
    OSReport("MEM2 Arena : 0x%x - 0x%x\n", lo, hi);
}

extern u64 fn_801E3844(void);
extern s32 ESP_SetUid(u64);

void OSInit(void) {
    OSBI2* bi2;
    void* mem1lo;
    void* mem1hi;
    void* mem2lo;
    void* mem2hi;
    u64 titleId;
    u64 playing;
    s32 ret;

    if (!AreWeInitialized) {
        AreWeInitialized = TRUE;

        __OSStartTime = __OSGetSystemTime();
        OSDisableInterrupts();
        __OSGetExecParams(&__OSRebootParams);

        PPCMtmmcr0(0);
        PPCMtmmcr1(0);
        PPCMtpmc1(0);
        PPCMtpmc2(0);
        PPCMtpmc3(0);
        PPCMtpmc4(0);
        PPCMthid4(0x83900000);
        PPCDisableSpeculation();
        PPCSetFpNonIEEEMode();

        BootInfo = (OSBootInfo*)0x80000000;
        BI2DebugFlag = NULL;
        __DVDLongFileNameFlag = FALSE;

        // dvd bi2 block
        bi2 = *(OSBI2**)0x800000F4;
        if (bi2 != NULL) {
            BI2DebugFlag = &bi2->debugFlag;
            __PADSpec = bi2->padSpec;
            *(u8*)0x800030E8 = *BI2DebugFlag;
            *(u8*)0x800030E9 = __PADSpec;
        } else if (BootInfo->arenaHi != NULL) {
            BI2DebugFlagHolder = *(u8*)0x800030E8;
            BI2DebugFlag = &BI2DebugFlagHolder;
            __PADSpec = *(u8*)0x800030E9;
        }

        __DVDLongFileNameFlag = TRUE;

        mem1lo = *(void**)0x8000310C;
        if (mem1lo == NULL) {
            if (OSIsMEM1Region(__ArenaLo)) {
                mem1lo = BootInfo->arenaLo == NULL ? __ArenaLo : BootInfo->arenaLo;

                // the program stack is followed by a debug stack, if nothing is attached reuse it
                if (BootInfo->arenaLo == NULL && BI2DebugFlag != NULL && *BI2DebugFlag < 2) {
                    mem1lo = (void*)(((u32)_stack_addr + 31) & ~31);
                }
            } else {
                mem1lo = (void*)0x80004000;
            }
        }
        OSSetMEM1ArenaLo(mem1lo);

        mem1hi = *(void**)0x80003110;
        if (mem1hi == NULL) {
            mem1hi = BootInfo->arenaHi == NULL ? __ArenaHi : BootInfo->arenaHi;
        }
        OSSetMEM1ArenaHi(mem1hi);

        mem2lo = *(void**)0x80003124;
        if (mem2lo != NULL) {
            if (OSIsMEM2Region(__ArenaLo)) {
                mem2lo = __ArenaLo;

                if (BI2DebugFlag != NULL && *BI2DebugFlag < 2) {
                    mem2lo = (void*)(((u32)_stack_addr + 31) & ~31);
                }
            } else if (mem2lo >= (void*)0x90000000 && mem2lo < (void*)0x90000800) {
                // first 2k of mem2 is spoken for
                mem2lo = (void*)0x90000800;
            }
            OSSetMEM2ArenaLo(mem2lo);
        }

        mem2hi = *(void**)0x80003128;
        if (mem2hi != NULL) {
            OSSetMEM2ArenaHi(mem2hi);
        }

        __OSInitIPCBuffer();
        OSExceptionInit();
        __OSInitSystemCall();
        __OSInitAlarm();
        __OSModuleInit();
        __OSInterruptInit();
        __OSContextInit();
        __OSCacheInit();
        EXIInit();
        SIInit();
        __OSInitSram();
        __OSThreadInit();
        __OSInitAudioSystem();

        DisableWriteGatherPipe();

        if (!__OSInIPL) {
            __OSInitMemoryProtection();
        }

        ReportOSInfo();
        OSRegisterVersion(__OSVersion);

        if (BI2DebugFlag != NULL && *BI2DebugFlag >= 2) {
            EnableMetroTRKInterrupts();
        }

        if (!__OSInNandBoot && !__OSInReboot) {
            ClearArena();
            ClearMEM2Arena();
        }
        OSEnableInterrupts();
        IPCCltInit();

        if (!__OSInNandBoot && !__OSInReboot) {
            __OSInitSTM();

            SCInit();
            while (SCCheckStatus() == SC_STATUS_BUSY) {
                ;
            }

            __OSInitNet();
        }

        if (!__OSInIPL) {
            CheckTargets();
            DVDInit();

            if (__OSIsGcam) {
                (*(volatile u16*)0x800030E6) = MAKE_DVD_DEVICE_CODE_(0x1000);
            } else if ((*(volatile u16*)0x800030E6) == 0) {
                DCInvalidateRange(&DriveInfo, sizeof(DVDDriveInfo));
                DVDInquiryAsync(&DriveBlock, &DriveInfo, InquiryCallback);
            }

            if (OSGetAppType() == 0x80 && !__OSInReboot && !__DVDCheckDevice()) {
                OSReturnToMenu();
            }
        }

        if (!__OSInIPL && !__OSInNandBoot) {
            __OSInitPlayTime();
        }

        if (!__OSInIPL && !__OSInNandBoot && !__OSInReboot) {
            __OSStartPlayRecord();
        }

        if (OSGetResetCode() == 0xA0000000) {
            ret = ESP_InitLib();
            if (ret == 0) {
                ret = ESP_GetTitleId(&titleId);
            }

            if (ret == 0) {
                playing = fn_801E3844();
                if (playing != 0 && playing != titleId) {
                    ESP_SetUid(playing);
                }
            }

            ESP_CloseLib();
        }
    }
}

// Physical addresses
// (Must be defined down here because of data pooling)
static u32 __OSExceptionLocations[15] = {
    0x0100, 0x0200, 0x0300, 0x0400, 0x0500, 0x0600, 0x0700, 0x0800,
    0x0900, 0x0C00, 0x0D00, 0x0F00, 0x1300, 0x1400, 0x1700};

static void OSExceptionInit(void) {
    u32* dst;
    u8 i;
    u32* pInst;
    u32 inst;
    void* vectorCode;
    u32 vectorSize;
    int j;
    u32* code;

    // Instruction that will be modified
    pInst = (u32*)__OSEVSetNumber;
    inst = *pInst;

    // OS exception vector
    vectorCode = __OSEVStart;
    vectorSize = (u32)__OSEVEnd - (u32)__OSEVStart;

    dst = (u32*)(void*)0x80000060;
    // Code is empty if DB integrator has not yet been installed
    if (*dst == 0) {
        DBPrintf("Installing OSDBIntegrator\n");
        memcpy(dst, __OSDBINTSTART, (u32)__OSDBINTEND - (u32)__OSDBINTSTART);
        DCFlushRangeNoSync(dst, (u32)__OSDBINTEND - (u32)__OSDBINTSTART);
        __sync();
        ICInvalidateRange(dst, (u32)__OSDBINTEND - (u32)__OSDBINTSTART);
    }

    for (i = 0; i < 15; i++) {
        if (BI2DebugFlag != NULL && *BI2DebugFlag >= 2 &&
            __DBIsExceptionMarked(i)) {
            DBPrintf(">>> OSINIT: exception %d commandeered by TRK\n", i);
        } else {
            // Modify li instruction with exception ID
            *pInst = inst | i;

            if (__DBIsExceptionMarked(i)) {
                DBPrintf(">>> OSINIT: exception %d vectored to debugger\n", i);
                memcpy(__DBVECTOR, __OSDBJUMPSTART,
                       (u32)__OSDBJUMPEND - (u32)__OSDBJUMPSTART);
            } else {
                code = (u32*)__DBVECTOR;
                for (j = 0; j < (u32)__OSDBJUMPEND - (u32)__OSDBJUMPSTART;
                     j += sizeof(u32), code++) {
                    // Write nop
                    *code = 0x60000000;
                }
            }

            dst = (void*)(__OSExceptionLocations[i] + 0x80000000);
            memcpy(dst, vectorCode, vectorSize);
            DCFlushRangeNoSync(dst, vectorSize);
            __sync();
            ICInvalidateRange(dst, vectorSize);
        }
    }

    OSExceptionTable =
        (__OSExceptionHandler*)0x80003000;
    for (i = 0; i < 15; i++) {
        __OSSetExceptionHandler(i, OSDefaultExceptionHandler);
    }

    // Restore original instruction
    *pInst = inst;
    DBPrintf("Exceptions initialized...\n");
}

static asm void __OSDBIntegrator(void) {
    nofralloc

    entry __OSDBINTSTART

    // Access OS debug interface
    li r5, 0x40

    // Save exception hook return address
    mflr r3
    stw r3, 0xC(r5)

    // Load exception hook
    lwz r3, 0x8(r5)
    // Physical -> Cached
    oris r3, r3, 0x8000
    mtlr r3

    // Setup MMU
    li r3, 0x30
    mtmsr r3

    // Call exception hook
    blr

    entry __OSDBINTEND
}

static asm void __OSDBJump(void){
    nofralloc

    entry __OSDBJUMPSTART

    bl __OSDBJUMPDEST

    entry __OSDBJUMPEND
}

__OSExceptionHandler
    __OSSetExceptionHandler(u8 type, __OSExceptionHandler handler) {
    __OSExceptionHandler old = OSExceptionTable[type];
    OSExceptionTable[type] = handler;
    return old;
}

__OSExceptionHandler __OSGetExceptionHandler(u8 type) {
    return OSExceptionTable[type];
}

static asm void OSExceptionVector(void) {
    nofralloc

    entry __OSEVStart

    mtsprg0 r4
    
    // Current OS context (physical address)
    lwz r4, 0x000000C0(0)
    stw r3, OSContext.gpr[3](r4)
    mfsprg0 r3
    stw r3, OSContext.gpr[4](r4)
    stw r5, OSContext.gpr[5](r4)
    lhz r3, OSContext.state(r4)
    ori r3, r3, 0x2
    sth r3, OSContext.state(r4)
    mfcr r3
    stw r3, OSContext.cr(r4)
    mflr r3
    stw r3, OSContext.lr(r4)
    mfctr r3

    entry __OSDBJUMPDEST

    stw r3, OSContext.ctr(r4)
    mfxer r3
    stw r3, OSContext.xer(r4)
    mfsrr0 r3
    stw r3, OSContext.srr0(r4)
    mfsrr1 r3
    stw r3, OSContext.srr1(r4)
    mr r5, r3

    entry __DBVECTOR

    nop
    mfmsr r3
    ori r3, r3, 0x30
    mtsrr1 r3

    entry __OSEVSetNumber

    li r3, 0
    // Currrent OS context (cached address)
    lwz r4, 0x000000D4(r0)
    rlwinm. r5, r5, 0, 30, 30
    bne lbl_800ECF70
    lis r5, OSDefaultExceptionHandler@ha
    addi r5, r5, OSDefaultExceptionHandler@l
    mtsrr0 r5
    rfi

lbl_800ECF70:
    rlwinm r5, r3, 2, 22, 29
    lwz r5, 0x3000(r5)
    mtsrr0 r5
    rfi

    entry __OSEVEnd

    nop
}

asm void OSDefaultExceptionHandler(u8 type, register OSContext* ctx) {
    nofralloc

    stw r0, ctx->gpr[0]
    stw r1, ctx->gpr[1]
    stw r2, ctx->gpr[2]
    stmw r6, ctx->gpr[6]

    mfspr r0, GQR1
    stw r0, ctx->gqr[1]
    mfspr r0, GQR2
    stw r0, ctx->gqr[2]
    mfspr r0, GQR3
    stw r0, ctx->gqr[3]
    mfspr r0, GQR4
    stw r0, ctx->gqr[4]
    mfspr r0, GQR5
    stw r0, ctx->gqr[5]
    mfspr r0, GQR6
    stw r0, ctx->gqr[6]
    mfspr r0, GQR7
    stw r0, ctx->gqr[7]

    mfdsisr r5
    mfdar r6

    stwu r1, -8(r1)
    b __OSUnhandledException
}

void __OSPSInit(void) {
    PPCMthid2(PPCMfhid2() | (1 << 31) | 0x20000000);
    ICFlashInvalidate();
    asm {
        sync
        li r3, 0
        mtgqr0 r3
        mtgqr1 r3
        mtgqr2 r3
        mtgqr3 r3
        mtgqr4 r3
        mtgqr5 r3
        mtgqr6 r3
        mtgqr7 r3
    }
}

u32 __OSGetDIConfig(void) {
    return (*(volatile u32*)0xCD006024) & 0xFF;
}

void OSRegisterVersion(const char* ver) {
    OSReport("%s\n", ver);
}

// Must be defined down here because of data pooling
static const char* AppGameNameForSysMenu = "HAEA";

const char* OSGetAppGamename(void) {
    int i;
    const char* name = (const char*)0x80003194;

    if (__OSInIPL) {
        name = AppGameNameForSysMenu;
    } else {
        char first = *(char*)0x80003194;

        // anything that is not 0-9 / A-Z means the second copy is not valid, use the first
        if (!(first >= '0' && (first <= '9' || first >= 'A') && first <= 'Z')) {
            name = (const char*)0x80003180;
        }
    }

    for (i = 0; i < 4; i++) {
        GameNameBuffer[i] = name[i];
    }
    GameNameBuffer[4] = '\0';

    return GameNameBuffer;
}

const u8 OSGetAppType(void) {
    if (__OSInIPL) {
        return 0x40;
    }

    return *(u8*)0x80003184;
}
