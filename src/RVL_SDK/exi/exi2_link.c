// exi2 link: talks to the debugger hardware hanging off exi channel 2 (the ndev/usb gecko style
// box). the mailbox + ram window protocol is bit banged here, the interrupt handlers at the top just
// forward to whatever callback the debug stub installed.
#include <types.h>

// BYTES: in the original these live in another file, so the debugger driver below never inlines them.
// has to be said before the headers declare them
extern "C" {
void __DBEXIInit(void) __attribute__((never_inline));
BOOL __EXI2Imm(void *mem, s32 size, u32 type) __attribute__((never_inline));
BOOL __DBEXIReadRam(u32 cmd, void *mem, s32 size) __attribute__((never_inline));
BOOL __DBEXIWriteRam(u32 cmd, const void *mem, s32 size) __attribute__((never_inline));
}

#include <revolution/EXI.h>
#include <revolution/NdevExi2AD.h>
#include <revolution/OS.h>

// openrvl headers want c++ (bool), keep c linkage
extern "C" {

// BYTES: in the original these live in another file, so the debugger driver below never inlines them

// ---- debugger driver ----

// sbss order in the original, c++ lays these out in declaration order
static OSInterruptHandler __DBMtrCallback;
static OSInterruptHandler __DBDbgCallback;
static u8 __DBEXIInputFlag;
static u32 __DBRecvMail;
static u32 __DBRecvDataSize;

static u8 __DBReadUSB_CSR(void);
static void __DBWaitForSendMail(void);

void __DBMtrHandler(s16 type, OSContext *ctx) {
    __DBEXIInputFlag = TRUE;

    if (__DBMtrCallback != NULL) {
        __DBMtrCallback(0, ctx);
    }
}

void __DBIntrHandler(s16 type, OSContext *ctx) {
    PI_HW_REGS[PI_INTSR] = PI_INTSR_DEBUG;
    if (__DBDbgCallback != NULL) {
        __DBDbgCallback(type, ctx);
    }
}

static void __DBCheckMailBox(void) {
    u8 csr = __DBReadUSB_CSR();

    if (!(csr & 0x8)) {
        u32 mail;

        __DBReadMailbox(&mail);
        if (ODEMUIsValidMail(mail)) {
            __DBRecvMail = mail;
            __DBRecvDataSize = ODEMUGetSize(mail);
            __DBEXIInputFlag = TRUE;
        }
    }
}

static u8 __DBReadUSB_CSR(void) {
    u8 val;

    __DBEXIReadReg(0x34000000, &val, sizeof(val));
    return val;
}

void DBInitComm(u8 **flagOut, OSInterruptHandler handler) {
    BOOL enabled = OSDisableInterrupts();

    *flagOut = &__DBEXIInputFlag;
    __DBMtrCallback = handler;
    __DBEXIInit();

    OSRestoreInterrupts(enabled);
}

void DBInitInterrupts(void) {
    __OSMaskInterrupts(OS_INTR_MASK(OS_INTR_EXI_2_EXI) | OS_INTR_MASK(OS_INTR_EXI_2_TC));
    __OSMaskInterrupts(OS_INTR_MASK(OS_INTR_PI_DEBUG));
    __DBDbgCallback = __DBMtrHandler;
    __OSSetInterruptHandler(OS_INTR_PI_DEBUG, __DBIntrHandler);
    __OSUnmaskInterrupts(OS_INTR_MASK(OS_INTR_PI_DEBUG));
}

u32 DBQueryData(void) {
    __DBEXIInputFlag = FALSE;

    if (__DBRecvDataSize == 0) {
        BOOL enabled = OSDisableInterrupts();

        __DBCheckMailBox();
        OSRestoreInterrupts(enabled);
    }

    return __DBRecvDataSize;
}

BOOL DBRead(void *dst, u32 size) {
    BOOL enabled = OSDisableInterrupts();

    __DBRead(ODEMUGetPc2NngcOffset(__DBRecvMail) + 0x1000, dst, ROUND_UP(size, 4));
    __DBRecvDataSize = 0;
    __DBEXIInputFlag = FALSE;

    OSRestoreInterrupts(enabled);
    return FALSE;
}

BOOL DBWrite(const void *src, u32 size) {
    static u8 l_byOffsetCounter = 128;

    BOOL enabled = OSDisableInterrupts();
    u32 ofs;
    u32 mail;

    __DBWaitForSendMail();

    // the two halves of the window get used one after the other
    ofs = 0;
    if (!(++l_byOffsetCounter & 0x1)) {
        ofs = 0;
    } else {
        ofs = 0x800;
    }

    while (__DBWrite(ofs, src, ROUND_UP(size, 4)) == 0) {
    }

    __DBWaitForSendMail();

    mail = ODEMUGenMailData(l_byOffsetCounter, size);
    while (__DBWriteMailbox(mail) == 0) {
    }

    __DBWaitForSendMail();

    OSRestoreInterrupts(enabled);
    return FALSE;
}

static void __DBWaitForSendMail(void) {
    while (__DBReadUSB_CSR() & 0x4) {
    }
}

void DBOpen(void) {}

void DBClose(void) {}

// ---- exi channel 2 access ----

static BOOL __EXI2Select(void) {
    u32 cpr = EXI_CHAN_PARAMS[EXI_CHAN_2].cpr;

    // MISMATCH-RISK: 0x40 is the low bit of the clock field, a CS1B bit was probably what they were after
    EXI_CHAN_PARAMS[EXI_CHAN_2].cpr =
        ((cpr & (EXI_CPR_EXIINTMASK | EXI_CPR_TCINTMASK | EXI_CPR_EXTINTMASK)) | 0x40 | EXI_CPR_CS0B);

    return TRUE;
}

static BOOL __EXI2Deselect(void) {
    u32 cpr = EXI_CHAN_PARAMS[EXI_CHAN_2].cpr;

    EXI_CHAN_PARAMS[EXI_CHAN_2].cpr = cpr & (EXI_CPR_EXIINTMASK | EXI_CPR_TCINTMASK | EXI_CPR_EXTINTMASK);

    return TRUE;
}

static BOOL __EXI2Sync(void) {
    while (EXI_CHAN_PARAMS[EXI_CHAN_2].cr & EXI_CR_TSTART) {
    }

    return TRUE;
}

BOOL __EXI2Imm(void *mem, s32 size, u32 type) {
    int i;

    if (type != EXI_READ) {
        u32 imm = 0;

        for (i = 0; i < size; i++) {
            const u8 *bmem = (const u8 *)mem;
            imm |= bmem[i] << (3 - i) * 8;
        }

        EXI_CHAN_PARAMS[EXI_CHAN_2].data = imm;
    }

    EXI_CHAN_PARAMS[EXI_CHAN_2].cr = type << 2 | EXI_CR_TSTART | (size - 1) * 16;

    __EXI2Sync();

    if (type == EXI_READ) {
        u32 imm = EXI_CHAN_PARAMS[EXI_CHAN_2].data;
        u8 *bmem = (u8 *)mem;

        for (i = 0; i < size; i++, bmem++) {
            *bmem = imm >> (3 - i) * 8;
        }
    }

    return TRUE;
}

void __DBEXIInit(void) {
    u32 val0, val1;

    __OSMaskInterrupts(OS_INTR_MASK(OS_INTR_EXI_2_EXI) | OS_INTR_MASK(OS_INTR_EXI_2_TC));
    while ((EXI_CHAN_PARAMS[EXI_CHAN_2].cr & EXI_CR_TSTART) == 1) {
    }
    EXI_CHAN_PARAMS[EXI_CHAN_2].cpr = 0;

    val0 = 0xB4000000;
    val1 = 0xD4000000;

    __EXI2Select();
    __EXI2Imm(&val0, 4, EXI_WRITE);
    __EXI2Sync();

    __EXI2Imm(&val1, 4, EXI_WRITE);
    __EXI2Sync();
    __EXI2Deselect();
}

BOOL __DBEXIReadReg(u32 cmd, void *mem, s32 size) {
    BOOL error = FALSE;
    u32 read_val = 0;

    error = error | !__EXI2Select();
    error = error | !__EXI2Imm(&cmd, sizeof(cmd), EXI_WRITE);
    error = error | !__EXI2Sync();
    error = error | !__EXI2Imm(&read_val, sizeof(read_val), EXI_READ);
    error = error | !__EXI2Sync();
    error = error | !__EXI2Deselect();

    switch (size) {
    case 1:
        *(u8 *)mem = read_val >> 24;
        break;
    case 2:
        *(u16 *)mem = read_val >> 24 | read_val >> 8 & 0xFF00;
        break;
    default:
        *(u32 *)mem = __EXISwap32(read_val);
        break;
    }

    return error == FALSE;
}

BOOL __DBEXIWriteReg(u32 cmd, const void *mem, s32 size) {
    BOOL error = FALSE;
    u32 write_val = 0;

    switch (size) {
    case 1:
        write_val = *(u8 *)mem << 24;
        break;
    case 2:
        write_val = *(u16 *)mem << 24 | (*(u16 *)mem & 0xFF00) << 8;
        break;
    default:
        write_val = __EXISwap32(*(u32 *)mem);
        break;
    }

    error = error | !__EXI2Select();
    error = error | !__EXI2Imm(&cmd, sizeof(cmd), EXI_WRITE);
    error = error | !__EXI2Sync();
    error = error | !__EXI2Imm(&write_val, sizeof(write_val), EXI_WRITE);
    error = error | !__EXI2Sync();
    error = error | !__EXI2Deselect();

    return error == FALSE;
}

BOOL __DBEXIReadRam(u32 cmd, void *mem, s32 size) {
    BOOL error = FALSE;
    u32 read_val;
    u32 *lmem = (u32 *)mem;

    error = error | !__EXI2Select();
    error = error | !__EXI2Imm(&cmd, sizeof(cmd), EXI_WRITE);
    error = error | !__EXI2Sync();

    for (; size > 0; size -= 4) {
        error = error | !__EXI2Imm(&read_val, sizeof(read_val), EXI_READ);
        error = error | !__EXI2Sync();
        *lmem++ = read_val;
    }

    error = error | !__EXI2Deselect();

    return error == FALSE;
}

BOOL __DBEXIWriteRam(u32 cmd, const void *mem, s32 size) {
    BOOL error = FALSE;
    u32 *lmem = (u32 *)mem;

    error = error | !__EXI2Select();
    error = error | !__EXI2Imm(&cmd, sizeof(cmd), EXI_WRITE);
    error = error | !__EXI2Sync();

    for (; size > 0; size -= 4) {
        u32 write_val = *lmem++;

        error = error | !__EXI2Imm(&write_val, sizeof(write_val), EXI_WRITE);
        error = error | !__EXI2Sync();
    }

    error = error | !__EXI2Deselect();

    return error == FALSE;
}


}
