// AXCL: the dsp command list. every audio frame the list is rebuilt to tell the dsp which buffers to mix, which aux
// buses to apply and where the finished frame goes. also holds the output mode and the return volumes.
#include "revolution/ax.h"
#include "revolution/os.h"

// BYTES: integer-0 NULL, so pointer tests against NULL compile as signed cmpwi as in retail. Not header-wide:
// making revolution/types.h define NULL 0 for C changes no other SDK unit except OSExec and nwc24, whose
// retail compares are cmplwi ((void*)0), so the NULL spelling differs per original source file.
#undef NULL
#define NULL 0

#define AX_MAX_VOLUME 0x8000

#define AX_OUTPUT_STEREO 0
#define AX_OUTPUT_SURROUND 1
#define AX_OUTPUT_DPL2 2

// compression parameters
#define THRESHOLD 32768
#define RELEASE_FRAMES 10

// two command lists are kept, one is being built while the dsp reads the other
#define LIST_MAX 2
#define AX_CL_SIZE 0x80

extern u16 __AXCompressorTable[];

extern void *__AXGetStudio(void);
extern void *__AXGetPBs(void);
extern void __AXGetAuxAInput(void **out);
extern void __AXGetAuxAInputDpl2(void **out);
extern void __AXGetAuxAOutput(void **out);
extern void __AXGetAuxAOutputDpl2R(void **out);
extern void __AXGetAuxAOutputDpl2Ls(void **out);
extern void __AXGetAuxAOutputDpl2Rs(void **out);
extern void __AXGetAuxBInput(void **out);
extern void __AXGetAuxBInputDpl2(void **out);
extern void __AXGetAuxBOutput(void **out);
extern void __AXGetAuxBOutputDpl2R(void **out);
extern void __AXGetAuxBOutputDpl2Ls(void **out);
extern void __AXGetAuxBOutputDpl2Rs(void **out);
extern void __AXGetAuxCInput(void **out);
extern void __AXGetAuxCOutput(void **out);

// every write goes through the global cursor, the compiler never keeps it in a register
#define LIST_WRITE_16(x)    \
    do {                    \
        *__AXClWrite = (x); \
        __AXClWrite++;      \
    } while (0)

#define LIST_WRITE_32(x)                 \
    do {                                 \
        LIST_WRITE_16(((u32)(x)) >> 16); \
        LIST_WRITE_16((x));              \
    } while (0)

// opcodes of the command list the dsp interprets
enum {
    COMMAND_SETUP,
    COMMAND_ADD_TO_LR,
    COMMAND_SUB_TO_LR,
    COMMAND_ADD_SUB_TO_LR,
    COMMAND_PROCESS,
    COMMAND_MIX_AUXA,
    COMMAND_MIX_AUXB,
    COMMAND_MIX_AUXC,
    COMMAND_UPL_AUXA_MIX_LRSC,
    COMMAND_UPL_AUXB_MIX_LRSC,
    COMMAND_COMPRESSOR,
    COMMAND_OUTPUT,
    COMMAND_OUTPUT_DPL2,
    COMMAND_WM_OUTPUT,
    COMMAND_END
};

static u32 __AXCommandListPosition;

static u16 *__AXClWrite;
u32 __AXClMode;

static u32 __AXCommandListCycles;
static u8 __AXCommandList[AX_CL_SIZE * LIST_MAX];

static BOOL __AXCompressor;

static volatile u16 __AXMasterVolume;
static u16 __AXAuxAVolume;
static u16 __AXAuxBVolume;
static u16 __AXAuxCVolume;

u32 __AXGetCommandListCycles(void) {
    return __AXCommandListCycles;
}

void *__AXGetCommandListAddress(void) {
    void *list = __AXCommandList + __AXCommandListPosition * AX_CL_SIZE;

    __AXCommandListPosition++;
    __AXCommandListPosition %= LIST_MAX;
    __AXClWrite = (u16 *)(__AXCommandList + __AXCommandListPosition * AX_CL_SIZE);

    return list;
}

void __AXNextFrame(void *surround, void *lr, void *rmt) {
    void *imm;
    void *backup;

    __AXCommandListCycles = 7811;
    backup = __AXClWrite;
    imm = __AXGetStudio();

    LIST_WRITE_16(COMMAND_SETUP);
    LIST_WRITE_32((u32)imm);
    __AXCommandListCycles += 4126;

    switch (__AXClMode) {
    case AX_OUTPUT_STEREO:
        LIST_WRITE_16(COMMAND_ADD_TO_LR);
        LIST_WRITE_32((u32)surround);
        __AXCommandListCycles += 733;
        break;
    case AX_OUTPUT_SURROUND:
        LIST_WRITE_16(COMMAND_SUB_TO_LR);
        LIST_WRITE_32((u32)surround);
        __AXCommandListCycles += 829;
        break;
    case AX_OUTPUT_DPL2:
        LIST_WRITE_16(COMMAND_ADD_SUB_TO_LR);
        LIST_WRITE_32((u32)surround);
        __AXCommandListCycles += 925;
        break;
    }

    imm = __AXGetPBs();
    LIST_WRITE_16(COMMAND_PROCESS);
    LIST_WRITE_32((u32)imm);

    if (__AXClMode == AX_OUTPUT_DPL2) {
        __AXGetAuxAInput(&imm);
        if (imm != NULL) {
            LIST_WRITE_16(COMMAND_UPL_AUXA_MIX_LRSC);
            LIST_WRITE_16(__AXAuxAVolume);
            LIST_WRITE_32((u32)imm);

            __AXGetAuxAInputDpl2(&imm);
            LIST_WRITE_32((u32)imm);

            __AXGetAuxAOutput(&imm);
            LIST_WRITE_32((u32)imm);

            __AXGetAuxAOutputDpl2R(&imm);
            LIST_WRITE_32((u32)imm);

            __AXGetAuxAOutputDpl2Ls(&imm);
            LIST_WRITE_32((u32)imm);

            __AXGetAuxAOutputDpl2Rs(&imm);
            LIST_WRITE_32((u32)imm);

            __AXCommandListCycles += 3036;
        }

        __AXGetAuxBInput(&imm);
        if (imm != NULL) {
            LIST_WRITE_16(COMMAND_UPL_AUXB_MIX_LRSC);
            LIST_WRITE_16(__AXAuxBVolume);
            LIST_WRITE_32((u32)imm);

            __AXGetAuxBInputDpl2(&imm);
            LIST_WRITE_32((u32)imm);

            __AXGetAuxBOutput(&imm);
            LIST_WRITE_32((u32)imm);

            __AXGetAuxBOutputDpl2R(&imm);
            LIST_WRITE_32((u32)imm);

            __AXGetAuxBOutputDpl2Ls(&imm);
            LIST_WRITE_32((u32)imm);

            __AXGetAuxBOutputDpl2Rs(&imm);
            LIST_WRITE_32((u32)imm);

            __AXCommandListCycles += 3036;
        }
    } else {
        __AXGetAuxAInput(&imm);
        if (imm != NULL) {
            LIST_WRITE_16(COMMAND_MIX_AUXA);
            LIST_WRITE_16(__AXAuxAVolume);
            LIST_WRITE_32((u32)imm);

            __AXGetAuxAOutput(&imm);
            LIST_WRITE_32((u32)imm);

            __AXCommandListCycles += 2235;
        }

        __AXGetAuxBInput(&imm);
        if (imm != NULL) {
            LIST_WRITE_16(COMMAND_MIX_AUXB);
            LIST_WRITE_16(__AXAuxBVolume);
            LIST_WRITE_32((u32)imm);

            __AXGetAuxBOutput(&imm);
            LIST_WRITE_32((u32)imm);

            __AXCommandListCycles += 2235;
        }

        __AXGetAuxCInput(&imm);
        if (imm != NULL) {
            LIST_WRITE_16(COMMAND_MIX_AUXC);
            LIST_WRITE_16(__AXAuxCVolume);
            LIST_WRITE_32((u32)imm);

            __AXGetAuxCOutput(&imm);
            LIST_WRITE_32((u32)imm);

            __AXCommandListCycles += 2235;
        }
    }

    if (__AXCompressor) {
        LIST_WRITE_16(COMMAND_COMPRESSOR);
        LIST_WRITE_16(THRESHOLD);
        LIST_WRITE_16(RELEASE_FRAMES);
        LIST_WRITE_32((u32)__AXCompressorTable);
        __AXCommandListCycles += 1850;
    }

    LIST_WRITE_16(COMMAND_WM_OUTPUT);
    LIST_WRITE_32(((u32 *)rmt)[0]);
    LIST_WRITE_32(((u32 *)rmt)[1]);
    LIST_WRITE_32(((u32 *)rmt)[2]);
    LIST_WRITE_32(((u32 *)rmt)[3]);
    __AXCommandListCycles += 409;

    if (__AXClMode == AX_OUTPUT_DPL2) {
        LIST_WRITE_16(COMMAND_OUTPUT_DPL2);
        LIST_WRITE_16(__AXMasterVolume);
        LIST_WRITE_32((u32)surround);
        LIST_WRITE_32((u32)lr);
        __AXCommandListCycles += 1195;
    } else {
        LIST_WRITE_16(COMMAND_OUTPUT);
        LIST_WRITE_16(__AXMasterVolume);
        LIST_WRITE_32((u32)surround);
        LIST_WRITE_32((u32)lr);
        __AXCommandListCycles += 1172;
    }

    LIST_WRITE_16(COMMAND_END);
    __AXCommandListCycles += 30;

    DCFlushRange(backup, AX_CL_SIZE);
}

void __AXClInit(void) {
    __AXClMode = AX_OUTPUT_STEREO;
    __AXCommandListPosition = 0;
    __AXClWrite = (u16 *)__AXCommandList;
    __AXCompressor = TRUE;

    __AXAuxCVolume = __AXAuxBVolume = __AXAuxAVolume = __AXMasterVolume = AX_MAX_VOLUME;
}

void __AXClQuit(void) {
}

void AXSetMode(u32 mode) {
    __AXClMode = mode;
}

u16 AXGetAuxAReturnVolume(void) {
    return __AXAuxAVolume;
}

u16 AXGetAuxBReturnVolume(void) {
    return __AXAuxBVolume;
}

u16 AXGetAuxCReturnVolume(void) {
    return __AXAuxCVolume;
}

void AXSetMasterVolume(u16 volume) {
    if (volume > AX_MAX_VOLUME) {
        volume = AX_MAX_VOLUME;
    }

    __AXMasterVolume = volume;
}

void AXSetAuxAReturnVolume(u16 volume) {
    __AXAuxAVolume = volume;
}

void AXSetAuxBReturnVolume(u16 volume) {
    __AXAuxBVolume = volume;
}

void AXSetAuxCReturnVolume(u16 volume) {
    __AXAuxCVolume = volume;
}
