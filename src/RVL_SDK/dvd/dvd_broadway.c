// dvd_broadway: the ppc side shim that talks to /dev/di through ipc. every DVDLow* call fills one of a
// small ring of command blocks, fires an async ioctl and hands a context to the completion callback.
// the whole original tu is here (strings for the dead stripped calls are still in the pool), the
// functions nothing in the game uses are drafted too so the string/bss order lines up.
#define bool _Bool
#include "private/iostypes.h"
#include "private/iosrestypes.h"
#include "private/iosresclt.h"
#include "revolution/dvd.h"
#include "revolution/esp.h"
#include "revolution/ipc.h"
#include "revolution/os.h"
#include "revolution/sc.h"
#include <cstring>

// BYTES: integer-0 NULL, so pointer tests against NULL compile as signed cmpwi as in retail. Not header-wide:
// making revolution/types.h define NULL 0 for C changes no other SDK unit except OSExec and nwc24, whose
// retail compares are cmplwi ((void*)0), so the NULL spelling differs per original source file.
#undef NULL
#define NULL 0

#define NUM_COMMAND_BLOCKS 4
#define PATH_BUF_LEN 32
#define H3_HASH_SIZE (96 * 1024)
#define SECTOR_SIZE 2048
#define NUM_IO_VECS 10
#define NUM_DVD_CONTEXTS 4
#define CB_CONTEXT_MAGIC 0xfeebdaed

#define IS_IPC_MEM_ALIGNED(addr) (((u32)(addr) & 0x1F) == 0)

// command numbers for /dev/di. the 0x70+ ones are pseudo commands the ios side picks apart itself
#define DI_INQUIRY_CMD 0x12
#define DI_REPORT_KEY_CMD 0xA4
#define DI_SEEK_CMD 0xAB
#define DI_READ_DVD_CMD 0xD0
#define DI_READ_DVD_CONFIG_CMD 0xD1
#define DI_STOP_LASER_CMD 0xD2
#define DI_OFFSET_CMD 0xD9
#define DI_READ_DISC_INFO_CMD 0xDA
#define DI_REQUEST_DISC_STATUS_CMD 0xDB
#define DI_REQUEST_RETRY_NUMBER_CMD 0xDC
#define DI_SET_MAXIMUM_ROTATION_CMD 0xDD
#define DI_SER_MEAS_CONTROL_CMD 0xDF
#define DI_REQUEST_ERROR_CMD 0xE0
#define DI_STOP_MOTOR_CMD 0xE3
#define DI_AUDIO_BUF_CONFIG_CMD 0xE4
#define DI_READ_DISK_ID_CMD 0x70
#define DI_READ_RVL_CMD 0x71
#define DI_WAIT_FOR_COVER_CLOSE_CMD 0x79
#define DI_GET_COVER_REGISTER_CMD 0x7A
#define DI_NOTIFY_RESET_CMD 0x7E
#define DI_SET_SPINUP_FLAG_CMD 0x7F
#define DI_READ_DVD_PHYSICAL_CMD 0x80
#define DI_READ_DVD_COPYRIGHT_CMD 0x81
#define DI_READ_DVD_DISC_KEY_CMD 0x82
#define DI_GET_LENGTH_CMD 0x83
#define DI_GET_IMM_BUFFER_CMD 0x84
#define DI_MASK_COVER_INTERRUPT 0x85
#define DI_CLEAR_COVER_INTERRUPT 0x86
#define DI_UNMASK_STATUS_INTERRUPTS 0x87
#define DI_GET_COVER_STATUS_CMD 0x88
#define DI_UNMASK_COVER_INTERRUPT 0x89
#define DI_RESET_CMD 0x8A
#define DI_OPEN_PARTITION_CMD 0x8B
#define DI_CLOSE_PARTITION_CMD 0x8C
#define DI_UNENCRYPTED_READ_CMD 0x8D
#define DI_DVD_VIDEO_ENABLE_CMD 0x8E
#define DI_GET_NO_DISC_OPEN_PARAMS_CMD 0x90
#define DI_NO_DISC_OPEN_PARTITION_CMD 0x91
#define DI_GET_NO_DISC_BUFFER_SIZES 0x92
#define DI_OPEN_PART_TMD_TICKET_CMD 0x93
#define DI_OPEN_PART_TMD_TICKET_VIEW_CMD 0x94
#define DI_GET_STATUS_REGISTER_CMD 0x95

#define DVD_INTTYPE_TC 0x1
#define DVD_INTTYPE_BRK 0x8

typedef enum callbackType {
    BOGUS_TYPE = 0,
    TRANSACTION_CB,
    COVER_CB,
    COVER_REG_CB
} callbackType_t;

// padded out to a full cache line so a context never shares one with anything else
typedef struct dvdContext {
    DVDLowCallback callback; // 0x00
    callbackType_t callbackType; // 0x04
    bool inUse; // 0x08
    u32 contextMagic; // 0x0C
    u32 contextNum; // 0x10
    u32 pad[3]; // 0x14
} dvdContext_t;

extern s32 IPCCltInit(void);

static volatile bool requestInProgress = false;
static bool breakRequested;
static bool callbackInProgress = false;

s32 DiFD = -1;

static int freeCommandBuf = 0;
static diCommand_t* diCommand;
static char* pathBuf;
static bool allocateStructures();
static void* ddrAllocAligned32(const int size);

static int freeDvdContext = 0;
static bool dvdContextsInited = false;
static dvdContext_t dvdContexts[NUM_DVD_CONTEXTS] __attribute__((aligned(32)));

static diRegVals_t diRegValCache __attribute__((aligned(32)));

static u32 registerBuf[8] __attribute__((aligned(32)));
static u32 coverRegister[8] __attribute__((aligned(32)));
static u32 statusRegister[8] __attribute__((aligned(32)));
static u32 coverStatus[8] __attribute__((aligned(32)));
static s32 lastTicketError[8] __attribute__((aligned(32)));

static u32 readLength;
static u32 spinUpValue;

static void* ddrAllocAligned32(const int size) {
    void *low, *high;

    if ((size & 0x1F) != 0) {
        OSReport("(ddrAllocAligned32) size is not a multiple of 32 !!! \n");
        return NULL;
    }
    low = IPCGetBufferLo();
    high = IPCGetBufferHi();
    if (((u32)low & 0x1F) != 0) {
        low = (void*)(((u32)low + 31) & 0x1F);
    }
    if ((u32)low + size > (u32)high) {
        OSReport("(ddrAllocAligned32) Not enough space to allocate %d bytes\n", size);
    }
    IPCSetBufferLo((void*)((u32)low + size));
    return low;
}

static bool allocateStructures() {
    if ((diCommand = ddrAllocAligned32(sizeof(diCommand_t) * NUM_COMMAND_BLOCKS)) == NULL) {
        OSReport("Allocation of diCommand blocks failed\n");
        return false;
    }
    if ((pathBuf = ddrAllocAligned32(PATH_BUF_LEN)) == NULL) {
        OSReport("Allocation of pathBuf failed\n");
        return false;
    }
    return true;
}

void initDvdContexts(void);
void initDvdContexts(void) {
    u32 i;
    for (i = 0; i < NUM_DVD_CONTEXTS; i++) {
        dvdContexts[i].callback = NULL;
        dvdContexts[i].callbackType = BOGUS_TYPE;
        dvdContexts[i].inUse = false;
        dvdContexts[i].contextMagic = CB_CONTEXT_MAGIC;
        dvdContexts[i].contextNum = i;
    }
    freeDvdContext = 0;
}

// grabs the next slot of the context ring and sanity checks it first. spins forever if the ring
// is corrupt, same as the original apparently does
static inline dvdContext_t* newContext(const DVDLowCallback callback, const callbackType_t type) {
    int returnIndex;

    if ((volatile bool)dvdContexts[freeDvdContext].inUse == true) {
        OSReport("(newContext) ERROR: freeDvdContext.inUse (#%d) is true\n", freeDvdContext);
        OSReport("(newContext) Now spinning in infinite loop\n");
        while (1) {
        }
    }

    if ((volatile u32)dvdContexts[freeDvdContext].contextMagic != CB_CONTEXT_MAGIC) {
        OSReport("(newContext) Something overwrote the context magic - spinning \n");
        while (1) {
        }
    }

    dvdContexts[freeDvdContext].callback = callback;
    dvdContexts[freeDvdContext].callbackType = type;
    dvdContexts[freeDvdContext].inUse = true;

    returnIndex = freeDvdContext;

    freeDvdContext++;
    if (freeDvdContext >= NUM_DVD_CONTEXTS) {
        freeDvdContext = 0;
    }
    return dvdContexts + returnIndex;
}

static inline void nextCommandBuf(int* bufNum) {
    (*bufNum)++;
    if (*bufNum >= NUM_COMMAND_BLOCKS) {
        *bufNum = 0;
    }
}

static IOSError doTransactionCallback(IOSError ret, void* context) {
    dvdContext_t* dvdContext;

    dvdContext = (dvdContext_t*)context;

    if ((volatile u32)dvdContext->contextMagic != CB_CONTEXT_MAGIC) {
        OSReport("(doTransactionCallback) Error - context mangled!\n");
        dvdContext->contextMagic = CB_CONTEXT_MAGIC;
        goto out;
    }

    requestInProgress = false;

    if ((volatile DVDLowCallback)dvdContext->callback != NULL) {
        s32 callbackArg;

        callbackInProgress = true;
        callbackArg = ret;
        if (breakRequested == true) {
            breakRequested = false;
            callbackArg |= DVD_INTTYPE_BRK;
        }
        if (callbackArg & DVD_INTTYPE_TC) {
            readLength = 0;
        }
        dvdContext->callback((u32)callbackArg);
        callbackInProgress = false;
    }

out:
    dvdContext->inUse = false;
    return 0;
}

static IOSError doCoverCallback(IOSError ret, void* context) {
    dvdContext_t* dvdContext;

    requestInProgress = false;

    dvdContext = (dvdContext_t*)context;
    if ((volatile u32)dvdContext->contextMagic != CB_CONTEXT_MAGIC) {
        OSReport("(doCoverCallback) Error - context mangled!\n");
        dvdContext->contextMagic = CB_CONTEXT_MAGIC;
        goto out;
    }

    if ((volatile DVDLowCallback)dvdContext->callback != NULL) {
        s32 callbackArg;

        callbackInProgress = true;
        callbackArg = ret;
        if (breakRequested == true) {
            breakRequested = false;
            callbackArg |= DVD_INTTYPE_BRK;
        }
        dvdContext->callback((u32)callbackArg);
        callbackInProgress = false;
    }

out:
    dvdContext->inUse = false;
    return 0;
}

static IOSError doPrepareCoverRegisterCallback(IOSError ret, void* context) {
    dvdContext_t* dvdContext;

    requestInProgress = false;

    diRegValCache.CoverRegVal = registerBuf[0];

    dvdContext = (dvdContext_t*)context;
    if ((volatile u32)dvdContext->contextMagic != CB_CONTEXT_MAGIC) {
        OSReport("(doTransactionCallback) Error - context mangled!\n");
        dvdContext->contextMagic = CB_CONTEXT_MAGIC;
        goto out;
    }

    if ((volatile DVDLowCallback)dvdContext->callback != NULL) {
        s32 callbackArg;

        callbackInProgress = true;
        callbackArg = ret;
        if (breakRequested == true) {
            breakRequested = false;
            callbackArg |= DVD_INTTYPE_BRK;
        }
        dvdContext->callback((u32)callbackArg);
        callbackInProgress = false;
    }
out:
    dvdContext->inUse = false;
    return 0;
}

static bool DVDLowInitCalled = false;

bool DVDLowFinalize(void) {
    IOSError retVal;

    retVal = IOS_Close(DiFD);

    if (retVal != IOS_ERROR_OK) {
        OSReport("(DVDLowFinish) Error: IOS_Close failed\n");
        return false;
    }

    DVDLowInitCalled = false;

    return true;
}

bool DVDLowInit(void) {
    IOSError retVal;

    if ((sizeof(diCommand_t) & 0x1F) != 0) {
        return false;
    }

    if ((sizeof(diRegVals_t) & 0x1F) != 0) {
        return false;
    }

    if ((sizeof(dvdContext_t) & 0x1F) != 0) {
        return false;
    }

    if (DVDLowInitCalled == false) {
        DVDLowInitCalled = true;
        retVal = IPCCltInit();
        if (retVal != IOS_ERROR_OK) {
            OSReport("IPCCltInit returned error: %d\n", retVal);
            return false;
        }

        if (allocateStructures() == false) {
            return false;
        }
        if (dvdContextsInited == false) {
            initDvdContexts();
            dvdContextsInited = true;
        }
    }

    strncpy(pathBuf, "/dev/di", PATH_BUF_LEN);
    DiFD = IOS_Open(pathBuf, 0);

    if (DiFD >= 0) {
        return true;
    } else {
        switch (DiFD) {
        case IOS_ERROR_NOEXISTS:
            OSReport("(DVDLowInit) Error: IOS_Open failed - pathname '/dev/di' does not exist\n");
            return false;
            break;

        case IOS_ERROR_ACCESS:
            OSReport("(DVDLowInit) Error: IOS_Open failed - calling thread lacks permission\n");
            return false;
            break;

        case IOS_ERROR_MAX:
            OSReport("(DVDLowInit) Error: IOS_Open failed - connection limit has been reached\n");
            return false;
            break;

        default:
            OSReport("(DVDLowInit) IOS_Open failed, errorcode = %d\n", DiFD);
            return false;
            break;
        }
    }
}

bool DVDLowReadDiskID(DVDDiskID* diskID, DVDLowCallback callback) {
    dvdContext_t* dvdContext;
    IOSError rv;

    requestInProgress = true;

    dvdContext = newContext(callback, TRANSACTION_CB);

    if (diskID == NULL) {
        OSReport("@@@@@@ WARNING - Calling DVDLowReadDiskId with NULL ptr\n");
    }

    nextCommandBuf(&freeCommandBuf);
    diCommand[freeCommandBuf].theCommand = DI_READ_DISK_ID_CMD;

    rv = IOS_IoctlAsync(DiFD, DI_READ_DISK_ID_CMD, &(diCommand[freeCommandBuf]), sizeof(diCommand_t), diskID, sizeof(DVDDiskID),
                        doTransactionCallback, dvdContext);
    if (rv != IOS_ERROR_OK) {
        OSReport("@@@ (DVDLowReadDiskID) IOS_IoctlAsync returned error: %d\n", rv);
        dvdContext->inUse = false;
        return false;
    }
    return true;
}

static IOSIoVector ioVec[NUM_IO_VECS] __attribute__((aligned(32)));

bool DVDLowOpenPartition(const u32 partitionWordOffset, const ESTicket* const eTicket, const u32 numCertBytes,
                         const u8* const certificates, ESTitleMeta* tmd, DVDLowCallback callback) {
    dvdContext_t* dvdContext;
    IOSError rv;

    if (eTicket != NULL && !IS_IPC_MEM_ALIGNED(eTicket)) {
        OSReport("(DVDLowOpenPartition) eTicket memory is unaligned\n");
        return false;
    }
    if (certificates != NULL && !IS_IPC_MEM_ALIGNED(certificates)) {
        OSReport("(DVDLowOpenPartition) certificates memory is unaligned\n");
        return false;
    }
    if (tmd != NULL && !IS_IPC_MEM_ALIGNED(tmd)) {
        OSReport("(DVDLowOpenPartition) certificates memory is unaligned\n");
        return false;
    }

    requestInProgress = true;
    dvdContext = newContext(callback, TRANSACTION_CB);

    nextCommandBuf(&freeCommandBuf);
    diCommand[freeCommandBuf].theCommand = DI_OPEN_PARTITION_CMD;
    diCommand[freeCommandBuf].arg[0] = partitionWordOffset;

    // input vectors
    ioVec[0].base = (u8*)&(diCommand[freeCommandBuf]);
    ioVec[0].length = sizeof(diCommand_t);

    ioVec[1].base = (u8*)eTicket;
    if (eTicket == NULL) {
        ioVec[1].length = 0;
    } else {
        ioVec[1].length = sizeof(ESTicket);
    }

    ioVec[2].base = (u8*)certificates;
    if (certificates == NULL) {
        ioVec[2].length = 0;
    } else {
        ioVec[2].length = numCertBytes;
    }

    // output vectors
    ioVec[3].base = (u8*)tmd;
    ioVec[3].length = sizeof(ESTitleMeta);

    ioVec[4].base = (u8*)&(lastTicketError[0]);
    ioVec[4].length = sizeof(lastTicketError);

    rv = IOS_IoctlvAsync(DiFD, DI_OPEN_PARTITION_CMD, 3, 2, ioVec, doTransactionCallback, dvdContext);
    if (rv != IOS_ERROR_OK) {
        OSReport("@@@ (DVDLowOpenPartition) IOS_IoctlvAsync returned error: %d\n", rv);
        dvdContext->inUse = false;
        return false;
    }
    return true;
}

bool DVDLowOpenPartitionWithTmdAndTicket(const u32 partitionWordOffset, const ESTicket* const eTicket, const u32 numTmdBytes,
                                         const ESTitleMeta* const tmd, const u32 numCertBytes, const u8* const certificates,
                                         DVDLowCallback callback) {
    dvdContext_t* dvdContext;
    IOSError rv;

    if (eTicket != NULL && !IS_IPC_MEM_ALIGNED(eTicket)) {
        OSReport("(%s) eTicket memory is unaligned\n", __FUNCTION__);
        return false;
    }
    if (certificates != NULL && !IS_IPC_MEM_ALIGNED(certificates)) {
        return false;
    }
    if (tmd == NULL) {
        OSReport("(%s) tmd parameter cannot be NULL\n", __FUNCTION__);
        return false;
    } else if (!IS_IPC_MEM_ALIGNED(tmd)) {
        OSReport("(%s) tmd memory is unaligned\n", __FUNCTION__);
        return false;
    }

    if (eTicket == NULL) {
        OSReport("(%s) eTicket parameter cannot be NULL\n", __FUNCTION__);
        return false;
    } else if (!IS_IPC_MEM_ALIGNED(eTicket)) {
        OSReport("(%s) eTicket memory is unaligned\n", __FUNCTION__);
        return false;
    }

    requestInProgress = true;
    dvdContext = newContext(callback, TRANSACTION_CB);

    nextCommandBuf(&freeCommandBuf);
    diCommand[freeCommandBuf].theCommand = DI_OPEN_PART_TMD_TICKET_CMD;
    diCommand[freeCommandBuf].arg[0] = partitionWordOffset;

    ioVec[0].base = (u8*)&(diCommand[freeCommandBuf]);
    ioVec[0].length = sizeof(diCommand_t);

    ioVec[1].base = (u8*)eTicket;
    ioVec[1].length = sizeof(ESTicket);

    ioVec[2].base = (u8*)tmd;
    ioVec[2].length = numTmdBytes;

    ioVec[3].base = (u8*)certificates;
    if (certificates == NULL) {
        ioVec[3].length = 0;
    } else {
        ioVec[3].length = numCertBytes;
    }

    ioVec[4].base = (u8*)&(lastTicketError[0]);
    ioVec[4].length = sizeof(lastTicketError);

    rv = IOS_IoctlvAsync(DiFD, DI_OPEN_PART_TMD_TICKET_CMD, 4, 1, ioVec, doTransactionCallback, dvdContext);
    if (rv != IOS_ERROR_OK) {
        OSReport("@@@ (DVDLowOpenPartition) IOS_IoctlvAsync returned error: %d\n", rv);
        dvdContext->inUse = false;
        return false;
    }
    return true;
}

bool DVDLowOpenPartitionWithTmdAndTicketView(const u32 partitionWordOffset, const ESTicketView* const eTicketView,
                                             const u32 numTmdBytes, const ESTitleMeta* const tmd, const u32 numCertBytes,
                                             const u8* const certificates, DVDLowCallback callback) {
    dvdContext_t* dvdContext;
    IOSError rv;

    if (certificates != NULL && !IS_IPC_MEM_ALIGNED(certificates)) {
        return false;
    }
    if (tmd == NULL) {
        OSReport("(%s) tmd parameter cannot be NULL\n", __FUNCTION__);
        return false;
    } else if (!IS_IPC_MEM_ALIGNED(tmd)) {
        OSReport("(%s) tmd memory is unaligned\n", __FUNCTION__);
        return false;
    }

    if (eTicketView == NULL) {
        OSReport("(%s) eTicketView parameter cannot be NULL\n", __FUNCTION__);
        return false;
    } else if (!IS_IPC_MEM_ALIGNED(eTicketView)) {
        OSReport("(%s) eTicketView memory is unaligned\n", __FUNCTION__);
        return false;
    }

    requestInProgress = true;
    dvdContext = newContext(callback, TRANSACTION_CB);

    nextCommandBuf(&freeCommandBuf);
    diCommand[freeCommandBuf].theCommand = DI_OPEN_PART_TMD_TICKET_VIEW_CMD;
    diCommand[freeCommandBuf].arg[0] = partitionWordOffset;

    ioVec[0].base = (u8*)&(diCommand[freeCommandBuf]);
    ioVec[0].length = sizeof(diCommand_t);

    ioVec[1].base = (u8*)eTicketView;
    ioVec[1].length = sizeof(ESTicketView);

    ioVec[2].base = (u8*)tmd;
    ioVec[2].length = numTmdBytes;

    ioVec[3].base = (u8*)certificates;
    if (certificates == NULL) {
        ioVec[3].length = 0;
    } else {
        ioVec[3].length = numCertBytes;
    }

    ioVec[4].base = (u8*)&(lastTicketError[0]);
    ioVec[4].length = sizeof(lastTicketError);

    rv = IOS_IoctlvAsync(DiFD, DI_OPEN_PART_TMD_TICKET_VIEW_CMD, 4, 1, ioVec, doTransactionCallback, dvdContext);
    if (rv != IOS_ERROR_OK) {
        OSReport("@@@ (DVDLowOpenPartition) IOS_IoctlvAsync returned error: %d\n", rv);
        dvdContext->inUse = false;
        return false;
    }
    return true;
}

bool DVDLowGetNoDiscBufferSizes(const u32 partitionWordOffset, u32* numTmdBytes, u32* numCertBytes, DVDLowCallback callback) {
    dvdContext_t* dvdContext;
    IOSError rv;

    if (numTmdBytes == NULL || numCertBytes == NULL) {
        OSReport("(%s) Error: NULL pointer argument\n", __FUNCTION__);
        return false;
    }

    if (!IS_IPC_MEM_ALIGNED(numTmdBytes)) {
        OSReport("(%s) numTmdBytes memory is unaligned\n", __FUNCTION__);
        return false;
    }
    if (!IS_IPC_MEM_ALIGNED(numCertBytes)) {
        OSReport("(%s) certificates memory is unaligned\n", __FUNCTION__);
        return false;
    }

    requestInProgress = true;
    dvdContext = newContext(callback, TRANSACTION_CB);

    nextCommandBuf(&freeCommandBuf);
    diCommand[freeCommandBuf].theCommand = DI_GET_NO_DISC_BUFFER_SIZES;
    diCommand[freeCommandBuf].arg[0] = partitionWordOffset;

    ioVec[0].base = (u8*)&(diCommand[freeCommandBuf]);
    ioVec[0].length = sizeof(diCommand_t);

    ioVec[1].base = (u8*)numTmdBytes;
    ioVec[1].length = sizeof(u32);

    ioVec[2].base = (u8*)numCertBytes;
    ioVec[2].length = sizeof(u32);

    rv = IOS_IoctlvAsync(DiFD, DI_GET_NO_DISC_BUFFER_SIZES, 1, 2, ioVec, doTransactionCallback, dvdContext);
    if (rv != IOS_ERROR_OK) {
        OSReport("@@@ (%s) IOS_IoctlvAsync returned error: %d\n", __FUNCTION__, rv);
        dvdContext->inUse = false;
        return false;
    }
    return true;
}

bool DVDLowGetNoDiscOpenPartitionParams(const u32 partitionWordOffset, ESTicket* eTicket, u32* numTmdBytes, ESTitleMeta* tmd,
                                        u32* numCertBytes, u8* certificates, u32* dataWordOffset, u8* h3HashPtr,
                                        DVDLowCallback callback) {
    dvdContext_t* dvdContext;
    IOSError rv;

    if (eTicket == NULL || numTmdBytes == NULL || tmd == NULL || numCertBytes == NULL || certificates == NULL ||
        dataWordOffset == NULL || h3HashPtr == NULL) {
        OSReport("(%s) Error: NULL pointer argument\n", __FUNCTION__);
        return false;
    }

    if (!IS_IPC_MEM_ALIGNED(eTicket) || !IS_IPC_MEM_ALIGNED(numTmdBytes) || !IS_IPC_MEM_ALIGNED(tmd) ||
        !IS_IPC_MEM_ALIGNED(numCertBytes) || !IS_IPC_MEM_ALIGNED(certificates) || !IS_IPC_MEM_ALIGNED(dataWordOffset) ||
        !IS_IPC_MEM_ALIGNED(h3HashPtr)) {
        OSReport("(%s) pointer argument is unaligned\n", __FUNCTION__);
        return false;
    }

    requestInProgress = true;
    dvdContext = newContext(callback, TRANSACTION_CB);

    nextCommandBuf(&freeCommandBuf);
    diCommand[freeCommandBuf].theCommand = DI_GET_NO_DISC_OPEN_PARAMS_CMD;
    diCommand[freeCommandBuf].arg[0] = partitionWordOffset;

    // input vectors
    ioVec[0].base = (u8*)&(diCommand[freeCommandBuf]);
    ioVec[0].length = sizeof(diCommand_t);

    ioVec[1].base = (u8*)numTmdBytes;
    ioVec[1].length = sizeof(u32);

    ioVec[2].base = (u8*)numCertBytes;
    ioVec[2].length = sizeof(u32);

    // output vectors
    ioVec[3].base = (u8*)eTicket;
    ioVec[3].length = sizeof(ESTicket);

    ioVec[4].base = (u8*)numTmdBytes;
    ioVec[4].length = sizeof(u32);

    ioVec[5].base = (u8*)tmd;
    ioVec[5].length = *numTmdBytes;

    ioVec[6].base = (u8*)numCertBytes;
    ioVec[6].length = sizeof(u32);

    ioVec[7].base = certificates;
    ioVec[7].length = *numCertBytes;

    ioVec[8].base = (u8*)dataWordOffset;
    ioVec[8].length = sizeof(u32);

    ioVec[9].base = h3HashPtr;
    ioVec[9].length = H3_HASH_SIZE;

    rv = IOS_IoctlvAsync(DiFD, DI_GET_NO_DISC_OPEN_PARAMS_CMD, 3, 7, ioVec, doTransactionCallback, dvdContext);
    if (rv != IOS_ERROR_OK) {
        OSReport("@@@ (%s) IOS_IoctlvAsync returned error: %d\n", __FUNCTION__, rv);
        dvdContext->inUse = false;
        return false;
    }
    return true;
}

bool DVDLowNoDiscOpenPartition(const ESTicket* const eTicket, const u32 numTmdBytes, const ESTitleMeta* const tmd,
                               const u32 numCertBytes, const u8* const certificates, const u32 dataWordOffset,
                               const u8* const h3HashPtr, DVDLowCallback callback) {
    dvdContext_t* dvdContext;
    IOSError rv;

    if (eTicket == NULL || tmd == NULL || certificates == NULL) {
        OSReport("(%s) Error: NULL pointer argument\n", __FUNCTION__);
        return false;
    }

    if (!IS_IPC_MEM_ALIGNED(eTicket) || !IS_IPC_MEM_ALIGNED(tmd) || !IS_IPC_MEM_ALIGNED(certificates) ||
        !IS_IPC_MEM_ALIGNED(dataWordOffset) || !IS_IPC_MEM_ALIGNED(h3HashPtr)) {
        OSReport("(%s) pointer argument is unaligned\n", __FUNCTION__);
        return false;
    }

    requestInProgress = true;
    dvdContext = newContext(callback, TRANSACTION_CB);

    nextCommandBuf(&freeCommandBuf);
    diCommand[freeCommandBuf].theCommand = DI_NO_DISC_OPEN_PARTITION_CMD;
    diCommand[freeCommandBuf].arg[0] = dataWordOffset;

    ioVec[0].base = (u8*)&(diCommand[freeCommandBuf]);
    ioVec[0].length = sizeof(diCommand_t);

    ioVec[1].base = (u8*)eTicket;
    ioVec[1].length = sizeof(ESTicket);

    ioVec[2].base = (u8*)tmd;
    ioVec[2].length = numTmdBytes;

    ioVec[3].base = (u8*)certificates;
    ioVec[3].length = numCertBytes;

    ioVec[4].base = (u8*)h3HashPtr;
    ioVec[4].length = H3_HASH_SIZE;

    ioVec[5].base = (u8*)&(lastTicketError[0]);
    ioVec[5].length = sizeof(u32);

    rv = IOS_IoctlvAsync(DiFD, DI_NO_DISC_OPEN_PARTITION_CMD, 5, 1, ioVec, doTransactionCallback, dvdContext);
    if (rv != IOS_ERROR_OK) {
        OSReport("@@@ (%s) IOS_IoctlvAsync returned error: %d\n", __FUNCTION__, rv);
        dvdContext->inUse = false;
        return false;
    }
    return true;
}

bool DVDLowClosePartition(DVDLowCallback callback) {
    dvdContext_t* dvdContext;
    IOSError rv;

    nextCommandBuf(&freeCommandBuf);
    diCommand[freeCommandBuf].theCommand = DI_CLOSE_PARTITION_CMD;

    requestInProgress = true;
    dvdContext = newContext(callback, TRANSACTION_CB);

    rv = IOS_IoctlAsync(DiFD, DI_CLOSE_PARTITION_CMD, &(diCommand[freeCommandBuf]), sizeof(diCommand_t), NULL, 0,
                        doTransactionCallback, dvdContext);

    if (rv != IOS_ERROR_OK) {
        OSReport("@@@ (DVDLowClosePartition) IOS_IoctlAsync returned error: %d\n", rv);
        dvdContext->inUse = false;
        return false;
    }
    return true;
}

bool DVDLowUnencryptedRead(void* destAddr, u32 length, u32 wordOffset, DVDLowCallback callback) {
    dvdContext_t* dvdContext;
    IOSError rv;

    requestInProgress = true;
    dvdContext = newContext(callback, TRANSACTION_CB);

    readLength = length;

    nextCommandBuf(&freeCommandBuf);
    diCommand[freeCommandBuf].theCommand = DI_UNENCRYPTED_READ_CMD;
    diCommand[freeCommandBuf].arg[0] = length;
    diCommand[freeCommandBuf].arg[1] = wordOffset;

    rv = IOS_IoctlAsync(DiFD, DI_UNENCRYPTED_READ_CMD, &(diCommand[freeCommandBuf]), sizeof(diCommand_t), destAddr, length,
                        doTransactionCallback, dvdContext);
    if (rv != IOS_ERROR_OK) {
        OSReport("@@@ (DVDLowUnencryptedRead) IOS_IoctlAsync returned error: %d\n", rv);
        dvdContext->inUse = false;
        return false;
    }
    return true;
}

bool DVDLowStopMotor(bool eject, bool saving, DVDLowCallback callback) {
    dvdContext_t* dvdContext;
    IOSError rv;

    requestInProgress = true;
    dvdContext = newContext(callback, TRANSACTION_CB);

    nextCommandBuf(&freeCommandBuf);
    diCommand[freeCommandBuf].theCommand = DI_STOP_MOTOR_CMD;
    diCommand[freeCommandBuf].arg[0] = eject;
    diCommand[freeCommandBuf].arg[1] = saving;

    rv = IOS_IoctlAsync(DiFD, DI_STOP_MOTOR_CMD, &(diCommand[freeCommandBuf]), sizeof(diCommand_t), &diRegValCache,
                        sizeof(diRegVals_t), doTransactionCallback, dvdContext);
    if (rv != IOS_ERROR_OK) {
        OSReport("@@@ (DVDLowStopMotor) IOS_IoctlAsync returned error: %d\n", rv);
        dvdContext->inUse = false;
        return false;
    }
    return true;
}

bool DVDLowWaitForCoverClose(DVDLowCallback callback) {
    dvdContext_t* dvdContext;
    IOSError rv;

    requestInProgress = true;
    dvdContext = newContext(callback, COVER_CB);

    nextCommandBuf(&freeCommandBuf);
    diCommand[freeCommandBuf].theCommand = DI_WAIT_FOR_COVER_CLOSE_CMD;

    rv = IOS_IoctlAsync(DiFD, DI_WAIT_FOR_COVER_CLOSE_CMD, &(diCommand[freeCommandBuf]), sizeof(diCommand_t), NULL, 0,
                        doCoverCallback, dvdContext);
    if (rv != IOS_ERROR_OK) {
        OSReport("@@@ (DVDLowWaitForCoverClose) IOS_IoctlAsync returned error: %d\n", rv);
        dvdContext->inUse = false;
        return false;
    }
    return true;
}

bool DVDLowInquiry(DVDDriveInfo* info, DVDLowCallback callback) {
    dvdContext_t* dvdContext;
    IOSError rv;

    requestInProgress = true;
    dvdContext = newContext(callback, TRANSACTION_CB);

    nextCommandBuf(&freeCommandBuf);
    diCommand[freeCommandBuf].theCommand = DI_INQUIRY_CMD;

    rv = IOS_IoctlAsync(DiFD, DI_INQUIRY_CMD, &(diCommand[freeCommandBuf]), sizeof(diCommand_t), info, sizeof(DVDDriveInfo),
                        doTransactionCallback, dvdContext);
    if (rv != IOS_ERROR_OK) {
        OSReport("@@@ (DVDLowInquiry) IOS_IoctlAsync returned error: %d\n", rv);
        dvdContext->inUse = false;
        return false;
    }
    return true;
}

bool DVDLowRequestError(DVDLowCallback callback) {
    dvdContext_t* dvdContext;
    IOSError rv;

    requestInProgress = true;
    dvdContext = newContext(callback, TRANSACTION_CB);

    nextCommandBuf(&freeCommandBuf);
    diCommand[freeCommandBuf].theCommand = DI_REQUEST_ERROR_CMD;

    rv = IOS_IoctlAsync(DiFD, DI_REQUEST_ERROR_CMD, &(diCommand[freeCommandBuf]), sizeof(diCommand_t), &diRegValCache,
                        sizeof(diRegVals_t), doTransactionCallback, dvdContext);
    if (rv != IOS_ERROR_OK) {
        OSReport("@@@ (DVDLowRequestError) IOS_IoctlAsync returned error: %d\n", rv);
        dvdContext->inUse = false;
        return false;
    }
    return true;
}

// this one never goes down to the iop, it only remembers the value for the next reset
bool DVDLowSetSpinupFlag(u32 spinup) {
    spinUpValue = spinup;
    return true;
}

bool DVDLowNotifyReset(void) {
    IOSError rv;

    if (callbackInProgress == true) {
        OSReport("(DVDLowSetSpinupFlag): Synch functions can't be called in callbacks\n");
        return false;
    }

    nextCommandBuf(&freeCommandBuf);
    diCommand[freeCommandBuf].theCommand = DI_NOTIFY_RESET_CMD;

    rv = IOS_Ioctl(DiFD, DI_NOTIFY_RESET_CMD, &(diCommand[freeCommandBuf]), sizeof(diCommand_t), NULL, 0);

    if (rv != IOS_ERROR_OK) {
        OSReport("@@@ (DVDLowNotifyReset) IOS_IoctlAsync returned error: %d\n", rv);
        return false;
    }
    return true;
}

bool DVDLowReset(DVDLowCallback callback) {
    dvdContext_t* dvdContext;
    IOSError rv;

    requestInProgress = true;
    dvdContext = newContext(callback, TRANSACTION_CB);

    nextCommandBuf(&freeCommandBuf);
    diCommand[freeCommandBuf].theCommand = DI_RESET_CMD;
    diCommand[freeCommandBuf].arg[0] = spinUpValue;

    rv = IOS_IoctlAsync(DiFD, DI_RESET_CMD, &(diCommand[freeCommandBuf]), sizeof(diCommand_t), NULL, 0, doTransactionCallback,
                        dvdContext);
    if (rv != IOS_ERROR_OK) {
        OSReport("@@@ (DVDLowReset) IOS_IoctlAsync returned error: %d\n", rv);
        dvdContext->inUse = false;
        return false;
    }
    return true;
}

bool DVDLowAudioBufferConfig(BOOL enable, u32 size, DVDLowCallback callback) {
    dvdContext_t* dvdContext;
    IOSError rv;

    requestInProgress = true;
    dvdContext = newContext(callback, TRANSACTION_CB);

    nextCommandBuf(&freeCommandBuf);
    diCommand[freeCommandBuf].theCommand = DI_AUDIO_BUF_CONFIG_CMD;
    diCommand[freeCommandBuf].arg[0] = enable;
    diCommand[freeCommandBuf].arg[1] = size;

    rv = IOS_IoctlAsync(DiFD, DI_AUDIO_BUF_CONFIG_CMD, &(diCommand[freeCommandBuf]), sizeof(diCommand_t), &diRegValCache,
                        sizeof(diRegVals_t), doTransactionCallback, dvdContext);
    if (rv != IOS_ERROR_OK) {
        OSReport("@@@ (DVDLowAudioBufferConfig) IOS_IoctlAsync returned error: %d\n", rv);
        dvdContext->inUse = false;
        return false;
    }
    return true;
}

bool DVDLowBreak(void) {
    if (requestInProgress == false) {
        return true;
    }
    breakRequested = true;
    while (requestInProgress == true) {
    }
    return true;
}

DVDLowCallback DVDLowClearCallback(void) {
    DVDLowCallback oldCallback;
    int contextIndex;
    int i;

    contextIndex = freeDvdContext - 1;
    if (contextIndex < 0) {
        contextIndex = NUM_DVD_CONTEXTS - 1;
    }
    oldCallback = dvdContexts[contextIndex].callback;

    for (i = 0; i < NUM_DVD_CONTEXTS; i++) {
        dvdContexts[i].callback = NULL;
        dvdContexts[i].inUse = false;
    }
    return oldCallback;
}

u32 DVDLowGetCoverStatus(void) {
    IOSError rv;

    if (callbackInProgress == true) {
        OSReport("(DVDLowGetCoverStatus): Synch functions can't be called in callbacks\n");
        return false;
    }

    nextCommandBuf(&freeCommandBuf);
    diCommand[freeCommandBuf].theCommand = DI_GET_COVER_STATUS_CMD;

    rv = IOS_Ioctl(DiFD, DI_GET_COVER_STATUS_CMD, &(diCommand[freeCommandBuf]), sizeof(diCommand_t), coverStatus,
                   sizeof(u32) * 8);

    if (rv != IOS_ERROR_OK) {
        OSReport("@@@ (DVDLowGetCoverStatus) IOS_Ioctl returned error: %d\n", rv);
        return 0xdeaddead;
    }
    return coverStatus[0];
}

bool DVDLowReadDvd(u32 strm, u32 retry, void* destAddr, u32 lengthInSectors, u32 lsn, DVDLowCallback callback) {
    dvdContext_t* dvdContext;
    IOSError rv;

    requestInProgress = true;
    dvdContext = newContext(callback, TRANSACTION_CB);

    nextCommandBuf(&freeCommandBuf);
    diCommand[freeCommandBuf].theCommand = DI_READ_DVD_CMD;

    if (strm == 0) {
        diCommand[freeCommandBuf].arg[0] = 0x0;
    } else {
        diCommand[freeCommandBuf].arg[0] = 0x1;
    }
    if (retry == 0) {
        diCommand[freeCommandBuf].arg[1] = 0x0;
    } else {
        diCommand[freeCommandBuf].arg[1] = 0x1;
    }
    diCommand[freeCommandBuf].arg[2] = lengthInSectors;
    diCommand[freeCommandBuf].arg[3] = lsn;

    readLength = lengthInSectors * SECTOR_SIZE;

    rv = IOS_IoctlAsync(DiFD, DI_READ_DVD_CMD, &(diCommand[freeCommandBuf]), sizeof(diCommand_t), destAddr,
                        lengthInSectors * SECTOR_SIZE, doTransactionCallback, dvdContext);
    if (rv != IOS_ERROR_OK) {
        OSReport("@@@ (DVDLowReadDVD) IOS_IoctlAsync returned error: %d\n", rv);
        dvdContext->inUse = false;
        return false;
    }
    return true;
}

bool DVDLowReadDvdConfig(bool set, u32 type, u32 config, DVDLowCallback callback) {
    dvdContext_t* dvdContext;
    IOSError rv;

    requestInProgress = true;
    dvdContext = newContext(callback, TRANSACTION_CB);

    nextCommandBuf(&freeCommandBuf);
    diCommand[freeCommandBuf].theCommand = DI_READ_DVD_CONFIG_CMD;
    diCommand[freeCommandBuf].arg[0] = set;
    diCommand[freeCommandBuf].arg[1] = type;
    diCommand[freeCommandBuf].arg[2] = config;

    rv = IOS_IoctlAsync(DiFD, DI_READ_DVD_CONFIG_CMD, &(diCommand[freeCommandBuf]), sizeof(diCommand_t), &diRegValCache,
                        sizeof(diRegVals_t), doTransactionCallback, dvdContext);
    if (rv != IOS_ERROR_OK) {
        OSReport("@@@ (DVDLowReadDVDConfig) IOS_IoctlAsync returned error: %d\n", rv);
        dvdContext->inUse = false;
        return false;
    }
    return true;
}

bool DVDLowReadDvdCopyright(u32 layer, DVDLowCallback callback) {
    dvdContext_t* dvdContext;
    IOSError rv;

    requestInProgress = true;
    dvdContext = newContext(callback, TRANSACTION_CB);

    nextCommandBuf(&freeCommandBuf);
    diCommand[freeCommandBuf].theCommand = DI_READ_DVD_COPYRIGHT_CMD;
    diCommand[freeCommandBuf].arg[0] = layer;

    rv = IOS_IoctlAsync(DiFD, DI_READ_DVD_COPYRIGHT_CMD, &(diCommand[freeCommandBuf]), sizeof(diCommand_t), &diRegValCache,
                        sizeof(diRegVals_t), doTransactionCallback, dvdContext);
    if (rv != IOS_ERROR_OK) {
        OSReport("@@@ (DVDLowReadDvdCopyright) IOS_IoctlAsync returned error: %d\n", rv);
        dvdContext->inUse = false;
        return false;
    }
    return true;
}

bool DVDLowReadDvdPhysical(DVDVideoPhysical* physical, u32 layer, DVDLowCallback callback) {
    dvdContext_t* dvdContext;
    IOSError rv;

    requestInProgress = true;
    dvdContext = newContext(callback, TRANSACTION_CB);

    nextCommandBuf(&freeCommandBuf);
    diCommand[freeCommandBuf].theCommand = DI_READ_DVD_PHYSICAL_CMD;
    diCommand[freeCommandBuf].arg[0] = layer;

    if ((sizeof(DVDVideoPhysical) & 0x1F) != 0) {
        OSReport("Size of DVDVideoPhysical is not a multiple of 32!\n");
        return false;
    }

    rv = IOS_IoctlAsync(DiFD, DI_READ_DVD_PHYSICAL_CMD, &(diCommand[freeCommandBuf]), sizeof(diCommand_t), physical,
                        sizeof(DVDVideoPhysical), doTransactionCallback, dvdContext);

    if (rv != IOS_ERROR_OK) {
        OSReport("@@@ (DVDLowReadDvdPhysical) IOS_IoctlAsync returned error: %d\n", rv);
        dvdContext->inUse = false;
        return false;
    }
    return true;
}

bool DVDLowReadDvdDiscKey(DVDVideoDiscKey* diskKey, u32 layer, DVDLowCallback callback) {
    dvdContext_t* dvdContext;
    IOSError rv;

    requestInProgress = true;
    dvdContext = newContext(callback, TRANSACTION_CB);

    nextCommandBuf(&freeCommandBuf);
    diCommand[freeCommandBuf].theCommand = DI_READ_DVD_DISC_KEY_CMD;
    diCommand[freeCommandBuf].arg[0] = layer;

    rv = IOS_IoctlAsync(DiFD, DI_READ_DVD_DISC_KEY_CMD, &(diCommand[freeCommandBuf]), sizeof(diCommand_t), diskKey,
                        sizeof(DVDVideoDiscKey), doTransactionCallback, dvdContext);
    if (rv != IOS_ERROR_OK) {
        OSReport("@@@ (DVDLowReadDvdDiscKey) IOS_IoctlAsync returned error: %d\n", rv);
        dvdContext->inUse = false;
        return false;
    }
    return true;
}

bool DVDLowReportKey(DVDVideoReportKey* reportKey, u32 format, u32 lsn, DVDLowCallback callback) {
    dvdContext_t* dvdContext;
    IOSError rv;

    requestInProgress = true;
    dvdContext = newContext(callback, TRANSACTION_CB);

    nextCommandBuf(&freeCommandBuf);
    diCommand[freeCommandBuf].theCommand = DI_REPORT_KEY_CMD;

    // callers pass the format already shifted into hardware position, the ios side wants it plain
    diCommand[freeCommandBuf].arg[0] = format >> 16;
    diCommand[freeCommandBuf].arg[1] = lsn;

    rv = IOS_IoctlAsync(DiFD, DI_REPORT_KEY_CMD, &(diCommand[freeCommandBuf]), sizeof(diCommand_t), reportKey,
                        sizeof(DVDVideoReportKey), doTransactionCallback, dvdContext);
    if (rv != IOS_ERROR_OK) {
        OSReport("@@@ (DVDLowReportKey) IOS_IoctlAsync returned error: %d\n", rv);
        dvdContext->inUse = false;
        return false;
    }
    return true;
}

bool DVDLowOffset(u32 subcmd, u32 offset_4_byte, DVDLowCallback callback) {
    dvdContext_t* dvdContext;
    IOSError rv;

    requestInProgress = true;
    dvdContext = newContext(callback, TRANSACTION_CB);

    nextCommandBuf(&freeCommandBuf);
    diCommand[freeCommandBuf].theCommand = DI_OFFSET_CMD;

    if (subcmd == 0) {
        diCommand[freeCommandBuf].arg[0] = 0x0;
    } else {
        diCommand[freeCommandBuf].arg[0] = 0x1;
    }
    diCommand[freeCommandBuf].arg[1] = offset_4_byte;

    rv = IOS_IoctlAsync(DiFD, DI_OFFSET_CMD, &(diCommand[freeCommandBuf]), sizeof(diCommand_t), &diRegValCache,
                        sizeof(diRegVals_t), doTransactionCallback, dvdContext);
    if (rv != IOS_ERROR_OK) {
        OSReport("@@@ (DVDLowOffset) IOS_IoctlAsync returned error: %d\n", rv);
        dvdContext->inUse = false;
        return false;
    }
    return true;
}

bool DVDLowStopLaser(DVDLowCallback callback) {
    dvdContext_t* dvdContext;
    IOSError rv;

    requestInProgress = true;
    dvdContext = newContext(callback, TRANSACTION_CB);

    nextCommandBuf(&freeCommandBuf);
    diCommand[freeCommandBuf].theCommand = DI_STOP_LASER_CMD;

    rv = IOS_IoctlAsync(DiFD, DI_STOP_LASER_CMD, &(diCommand[freeCommandBuf]), sizeof(diCommand_t), &diRegValCache,
                        sizeof(diRegVals_t), doTransactionCallback, dvdContext);
    if (rv != IOS_ERROR_OK) {
        OSReport("@@@ (DVDLowStopLaser) IOS_IoctlAsync returned error: %d\n", rv);
        dvdContext->inUse = false;
        return false;
    }
    return true;
}

bool DVDLowReadDiskBca(DVDDiskBca* diskBca, DVDLowCallback callback) {
    dvdContext_t* dvdContext;
    IOSError rv;

    requestInProgress = true;
    dvdContext = newContext(callback, TRANSACTION_CB);

    nextCommandBuf(&freeCommandBuf);
    diCommand[freeCommandBuf].theCommand = DI_READ_DISC_INFO_CMD;

    rv = IOS_IoctlAsync(DiFD, DI_READ_DISC_INFO_CMD, &(diCommand[freeCommandBuf]), sizeof(diCommand_t), diskBca,
                        sizeof(DVDDiskBca), doTransactionCallback, dvdContext);
    if (rv != IOS_ERROR_OK) {
        OSReport("@@@ (DVDLowReadDiskBca) IOS_IoctlAsync returned error: %d\n", rv);
        dvdContext->inUse = false;
        return false;
    }
    return true;
}

bool DVDLowSerMeasControl(DVDLowDriveSer* ser, bool clear, bool enable, DVDLowCallback callback) {
    dvdContext_t* dvdContext;
    IOSError rv;

    requestInProgress = true;
    dvdContext = newContext(callback, TRANSACTION_CB);

    nextCommandBuf(&freeCommandBuf);
    diCommand[freeCommandBuf].theCommand = DI_SER_MEAS_CONTROL_CMD;
    diCommand[freeCommandBuf].arg[0] = clear;
    diCommand[freeCommandBuf].arg[1] = enable;

    rv = IOS_IoctlAsync(DiFD, DI_SER_MEAS_CONTROL_CMD, &(diCommand[freeCommandBuf]), sizeof(diCommand_t), ser,
                        sizeof(DVDLowDriveSer), doTransactionCallback, dvdContext);
    if (rv != IOS_ERROR_OK) {
        OSReport("@@@ (DVDLowSerMeasControl) IOS_IoctlAsync returned error: %d\n", rv);
        dvdContext->inUse = false;
        return false;
    }
    return true;
}

bool DVDLowRequestDiscStatus(DVDLowCallback callback) {
    dvdContext_t* dvdContext;
    IOSError rv;

    requestInProgress = true;
    dvdContext = newContext(callback, TRANSACTION_CB);

    nextCommandBuf(&freeCommandBuf);
    diCommand[freeCommandBuf].theCommand = DI_REQUEST_DISC_STATUS_CMD;

    rv = IOS_IoctlAsync(DiFD, DI_REQUEST_DISC_STATUS_CMD, &(diCommand[freeCommandBuf]), sizeof(diCommand_t), &diRegValCache,
                        sizeof(diRegVals_t), doTransactionCallback, dvdContext);

    if (rv != IOS_ERROR_OK) {
        OSReport("@@@ (DVDLowRequestDiscStatus) IOS_IoctlAsync returned error: %d\n", rv);
        dvdContext->inUse = false;
        return false;
    }
    return true;
}

bool DVDLowRequestRetryNumber(DVDLowCallback callback) {
    dvdContext_t* dvdContext;
    IOSError rv;

    requestInProgress = true;
    dvdContext = newContext(callback, TRANSACTION_CB);

    nextCommandBuf(&freeCommandBuf);
    diCommand[freeCommandBuf].theCommand = DI_REQUEST_RETRY_NUMBER_CMD;

    rv = IOS_IoctlAsync(DiFD, DI_REQUEST_RETRY_NUMBER_CMD, &(diCommand[freeCommandBuf]), sizeof(diCommand_t), &diRegValCache,
                        sizeof(diRegVals_t), doTransactionCallback, dvdContext);
    if (rv != IOS_ERROR_OK) {
        OSReport("@@@ (DVDLowRequestRetryNumber) IOS_IoctlAsync returned error: %d\n", rv);
        dvdContext->inUse = false;
        return false;
    }
    return true;
}

bool DVDLowSetMaximumRotation(u32 subcmd, DVDLowCallback callback) {
    dvdContext_t* dvdContext;
    IOSError rv;

    requestInProgress = true;
    dvdContext = newContext(callback, TRANSACTION_CB);

    nextCommandBuf(&freeCommandBuf);
    diCommand[freeCommandBuf].theCommand = DI_SET_MAXIMUM_ROTATION_CMD;
    diCommand[freeCommandBuf].arg[0] = (subcmd >> 16) & 0x3;

    rv = IOS_IoctlAsync(DiFD, DI_SET_MAXIMUM_ROTATION_CMD, &(diCommand[freeCommandBuf]), sizeof(diCommand_t), NULL, 0,
                        doTransactionCallback, dvdContext);

    if (rv != IOS_ERROR_OK) {
        OSReport("@@@ (DVDLowSetMaxRotation) IOS_IoctlAsync returned error: %d\n", rv);
        dvdContext->inUse = false;
        return false;
    }
    return true;
}

bool DVDLowRead(void* destAddr, u32 length, u32 wordOffset, DVDLowCallback callback) {
    dvdContext_t* dvdContext;
    IOSError rv;

    if (((u32)destAddr & 0x1F) != 0) {
        OSReport("(DVDLowRead): ERROR - destAddr buffer is not 32 byte aligned\n");
        return false;
    }

    requestInProgress = true;
    dvdContext = newContext(callback, TRANSACTION_CB);

    readLength = length;

    nextCommandBuf(&freeCommandBuf);
    diCommand[freeCommandBuf].theCommand = DI_READ_RVL_CMD;
    diCommand[freeCommandBuf].arg[0] = length;
    diCommand[freeCommandBuf].arg[1] = wordOffset;

    rv = IOS_IoctlAsync(DiFD, DI_READ_RVL_CMD, &(diCommand[freeCommandBuf]), sizeof(diCommand_t), destAddr, length,
                        doTransactionCallback, dvdContext);
    if (rv != IOS_ERROR_OK) {
        OSReport("@@@ (DVDLowRead) IOS_IoctlAsync returned error: %d\n", rv);
        dvdContext->inUse = false;
        return false;
    }
    return true;
}

bool DVDLowSeek(u32 wordOffset, DVDLowCallback callback) {
    dvdContext_t* dvdContext;
    IOSError rv;

    requestInProgress = true;
    dvdContext = newContext(callback, TRANSACTION_CB);

    nextCommandBuf(&freeCommandBuf);
    diCommand[freeCommandBuf].theCommand = DI_SEEK_CMD;
    diCommand[freeCommandBuf].arg[0] = wordOffset;

    rv = IOS_IoctlAsync(DiFD, DI_SEEK_CMD, &(diCommand[freeCommandBuf]), sizeof(diCommand_t), NULL, 0, doTransactionCallback,
                        dvdContext);
    if (rv != IOS_ERROR_OK) {
        OSReport("@@@ (DVDLowSeek) IOS_IoctlAsync returned error: %d\n", rv);
        dvdContext->inUse = false;
        return false;
    }
    return true;
}

// the blocking version of the cover register read, DVDLowGetCoverRegister below only hands back the cache
u32 DVDLowGetCoverReg(void) {
    IOSError rv;

    if (callbackInProgress == true) {
        OSReport("(DVDLowGetCoverReg): Synch functions can't be called in callbacks\n");
        return false;
    }

    nextCommandBuf(&freeCommandBuf);
    diCommand[freeCommandBuf].theCommand = DI_GET_COVER_REGISTER_CMD;

    rv = IOS_Ioctl(DiFD, DI_GET_COVER_REGISTER_CMD, &(diCommand[freeCommandBuf]), sizeof(diCommand_t), coverRegister,
                   sizeof(u32) * 8);
    if (rv != IOS_ERROR_OK) {
        OSReport("@@@ (DVDLowGetCoverReg) IOS_Ioctl returned error: %d\n", rv);
        return false;
    }
    return coverRegister[0];
}

u32 DVDLowGetCoverRegister(void) {
    return diRegValCache.CoverRegVal;
}

u32 DVDLowGetStatusRegister(void) {
    return statusRegister[0];
}

bool DVDLowPrepareCoverRegister(DVDLowCallback callback) {
    dvdContext_t* dvdContext;
    IOSError rv;

    nextCommandBuf(&freeCommandBuf);
    diCommand[freeCommandBuf].theCommand = DI_GET_COVER_REGISTER_CMD;

    requestInProgress = true;
    dvdContext = newContext(callback, TRANSACTION_CB);

    rv = IOS_IoctlAsync(DiFD, DI_GET_COVER_REGISTER_CMD, &(diCommand[freeCommandBuf]), sizeof(diCommand_t), registerBuf,
                        sizeof(registerBuf), doPrepareCoverRegisterCallback, dvdContext);
    if (rv != IOS_ERROR_OK) {
        OSReport("@@@ (DVDLowPrepareCoverRegsiter) IOS_IoctlAsync returned error: %d\n", rv);
        dvdContext->inUse = false;
        return false;
    }
    return true;
}

bool DVDLowPrepareStatusRegister(DVDLowCallback callback) {
    dvdContext_t* dvdContext;
    IOSError rv;

    nextCommandBuf(&freeCommandBuf);
    diCommand[freeCommandBuf].theCommand = DI_GET_STATUS_REGISTER_CMD;

    requestInProgress = true;
    dvdContext = newContext(callback, TRANSACTION_CB);

    rv = IOS_IoctlAsync(DiFD, DI_GET_STATUS_REGISTER_CMD, &(diCommand[freeCommandBuf]), sizeof(diCommand_t), statusRegister,
                        sizeof(statusRegister), doTransactionCallback, dvdContext);
    if (rv != IOS_ERROR_OK) {
        OSReport("@@@ (DVDLowPrepareStatusRegsiter) IOS_IoctlAsync returned error: %d\n", rv);
        dvdContext->inUse = false;
        return false;
    }
    return true;
}

u32 DVDLowGetLength(void) {
    return readLength;
}

u32 DVDLowGetImmBufferReg(void) {
    return diRegValCache.ImmRegVal;
}

bool DVDLowUnmaskStatusInterrupts(void) {
    return true;
}

bool DVDLowMaskCoverInterrupt(void) {
    return true;
}

bool DVDLowUnmaskCoverInterrupt(void) {
    return true;
}

bool DVDLowClearCoverInterrupt(DVDLowCallback callback) {
    dvdContext_t* dvdContext;
    IOSError rv;

    nextCommandBuf(&freeCommandBuf);
    diCommand[freeCommandBuf].theCommand = DI_CLEAR_COVER_INTERRUPT;

    requestInProgress = true;
    dvdContext = newContext(callback, TRANSACTION_CB);

    rv = IOS_IoctlAsync(DiFD, DI_CLEAR_COVER_INTERRUPT, &(diCommand[freeCommandBuf]), sizeof(diCommand_t), NULL, 0,
                        doTransactionCallback, dvdContext);
    if (rv != IOS_ERROR_OK) {
        OSReport("@@@ (DVDLowClearCoverInterrupt) IOS_IoctlAsync returned error: %d\n", rv);
        dvdContext->inUse = false;
        return false;
    }
    return true;
}

s32 DVDLowGetLastEticketError(void) {
    return lastTicketError[0];
}

bool __DVDLowTestAlarm(const OSAlarm* alarm) {
#pragma unused(alarm)
    return false;
}

bool DVDLowEnableDvdVideo(const bool enable, DVDLowCallback callback) {
    dvdContext_t* dvdContext;
    IOSError rv;

    nextCommandBuf(&freeCommandBuf);
    diCommand[freeCommandBuf].theCommand = DI_DVD_VIDEO_ENABLE_CMD;
    diCommand[freeCommandBuf].arg[0] = enable;

    requestInProgress = true;
    dvdContext = newContext(callback, TRANSACTION_CB);

    rv = IOS_IoctlAsync(DiFD, DI_DVD_VIDEO_ENABLE_CMD, &(diCommand[freeCommandBuf]), sizeof(diCommand_t), NULL, 0,
                        doTransactionCallback, dvdContext);
    if (rv != IOS_ERROR_OK) {
        OSReport("@@@ (DVDLowEnableDvdVideo) IOS_IoctlAsync returned error: %d\n", rv);
        dvdContext->inUse = false;
        return false;
    }
    return true;
}
