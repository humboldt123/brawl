// SIBios: the serial interface driver, i.e. the gamecube controller ports. one transfer can be in flight
// at a time, others queue up in a per channel packet with a time to fire. polling is done by the hardware
// itself, this code only reads the results back and keeps the "what is plugged in" type word fresh.
#include "revolution/os.h"
#include "revolution/si.h"

extern u32 fn_801E9B4C(void);
extern u32 __PADFixBits;
extern BOOL SITransfer(s32 chan, void *output, u32 outputBytes, void *input, u32 inputBytes, SICallback callback,
                       OSTime fire);
extern BOOL SIIsChanBusy(s32 chan);
extern u32 SIGetStatus(s32 chan);
extern BOOL SIEnablePollingInterrupt(BOOL enable);


#define SI_CHAN_NONE (-1)
#define SI_MAX_CHAN 4

// register file at 0xCD006400, as words: 3 per channel (out, in hi, in lo), then poll, comcsr, sr, the io buffer
// starts at word 0x20
vu32 SI_REGS[0x40] : 0xCD006400;

#define SI_CHAN_OUT(chan) SI_REGS[(chan)*3]
#define SI_CHAN_IN_HI(chan) SI_REGS[(chan)*3 + 1]
#define SI_CHAN_IN_LO(chan) SI_REGS[(chan)*3 + 2]
#define SI_POLL SI_REGS[12]
#define SI_COMCSR SI_REGS[13]
#define SI_SR SI_REGS[14]
#define SI_IOBUF(i) SI_REGS[0x20 + (i)]

#define SI_COMCSR_TSTART 0x00000001
#define SI_COMCSR_COMERR 0x20000000
#define SI_COMCSR_RDSTINT 0x10000000
#define SI_COMCSR_RDSTINTMSK 0x08000000
#define SI_COMCSR_TCINT 0x80000000
#define SI_COMCSR_TCINTMSK 0x40000000

#define SI_ERROR_UNDER_RUN 0x0001
#define SI_ERROR_OVER_RUN 0x0002
#define SI_ERROR_COLLISION 0x0004
#define SI_ERROR_NO_RESPONSE 0x0008
#define SI_ERROR_BUSY 0x0080

#define SI_WIRELESS_LITE 0x00040000
#define SI_WIRELESS_CONT 0x00080000

typedef struct SIControl {
    s32 chan;
    u32 poll;
    u32 inputBytes;
    void *input;
    SICallback callback;
} SIControl;

const char *__SIVersion = "<< RVL_SDK - SI \trelease build: Nov 30 2006 03:31:44 (0x4199_60831) >>";

static SIControl Si = {SI_CHAN_NONE, 0, 0, NULL, NULL};
static u32 Type[SI_MAX_CHAN] = {SI_ERROR_NO_RESPONSE, SI_ERROR_NO_RESPONSE, SI_ERROR_NO_RESPONSE,
                                SI_ERROR_NO_RESPONSE};

static SIPacket Packet[SI_MAX_CHAN];
static OSTime XferTime[SI_MAX_CHAN];
static OSTime TypeTime[SI_MAX_CHAN];
static SITypeAndStatusCallback TypeCallback[SI_MAX_CHAN][4];
static OSAlarm Alarm[SI_MAX_CHAN];
static u32 InputBuffer[SI_MAX_CHAN][2];
static vu32 InputBufferValid[SI_MAX_CHAN];
static vu32 InputBufferVcount[SI_MAX_CHAN];
static void (*RDSTHandler[4])(s16, OSContext *);

static void GetTypeCallback(s32 chan, u32 sr, OSContext *context);
static void AlarmHandler(OSAlarm *alarm, OSContext *context);
static BOOL __SITransfer(s32 chan, void *output, u32 outputBytes, void *input, u32 inputBytes,
                         SICallback callback);

BOOL SIBusy(void) {
    return Si.chan != SI_CHAN_NONE ? TRUE : FALSE;
}

BOOL SIIsChanBusy(s32 chan) {
    return (Packet[chan].chan != SI_CHAN_NONE || Si.chan == chan) ? TRUE : FALSE;
}

static u32 CompleteTransfer(void) {
    u32 sr;
    u32 i;
    u32 rLen;
    u8 *data;

    sr = SI_SR;
    SI_COMCSR = (SI_COMCSR | SI_COMCSR_TCINT) & ~SI_COMCSR_TSTART;

    if (Si.chan != SI_CHAN_NONE) {
        XferTime[Si.chan] = __OSGetSystemTime();

        data = (u8 *)Si.input;
        rLen = Si.inputBytes / 4;
        for (i = 0; i < rLen; data += 4, i++) {
            *(u32 *)data = SI_REGS[0x20 + i];
        }

        rLen = Si.inputBytes & 3;
        if (rLen) {
            u32 temp = SI_IOBUF(i);
            for (i = 0; i < rLen; i++) {
                *data++ = (u8)(temp >> ((3 - i) * 8));
            }
        }

        if (SI_COMCSR & SI_COMCSR_COMERR) {
            sr >>= (3 - Si.chan) * 8;
            sr &= 0xF;

            if ((sr & SI_ERROR_NO_RESPONSE) && !(Type[Si.chan] & SI_ERROR_BUSY)) {
                Type[Si.chan] = SI_ERROR_NO_RESPONSE;
            }

            if (sr == 0) {
                sr = SI_ERROR_COLLISION;
            }
        } else {
            TypeTime[Si.chan] = __OSGetSystemTime();
            sr = 0;
        }

        Si.chan = SI_CHAN_NONE;
    }

    return sr;
}

static void SIInterruptHandler(s16 interrupt, OSContext *context) {
    u32 reg;

    reg = SI_COMCSR;

    if ((reg & (SI_COMCSR_TCINT | SI_COMCSR_TCINTMSK)) == (SI_COMCSR_TCINT | SI_COMCSR_TCINTMSK)) {
        s32 chan;
        s32 next;
        u32 sr;
        s32 i;
        SICallback callback;
        SIPacket *packet;

        chan = Si.chan;
        sr = CompleteTransfer();

        callback = Si.callback;
        Si.callback = NULL;

        next = chan;
        for (i = 0; i < SI_MAX_CHAN; i++) {
            next++;
            next %= SI_MAX_CHAN;
            packet = &Packet[next];
            if (packet->chan != SI_CHAN_NONE && packet->fire <= __OSGetSystemTime()) {
                if (__SITransfer(packet->chan, packet->output, packet->outputBytes, packet->input,
                                 packet->inputBytes, packet->callback)) {
                    OSCancelAlarm(&Alarm[next]);
                    packet->chan = SI_CHAN_NONE;
                }
                break;
            }
        }

        if (callback) {
            callback(chan, sr, context);
        }

        SI_SR = SI_SR & (0x0F000000 >> (chan * 8));

        if (Type[chan] == SI_ERROR_BUSY && !SIIsChanBusy(chan)) {
            static u32 cmdTypeAndStatus = 0;

            SITransfer(chan, &cmdTypeAndStatus, 1, &Type[chan], 3, GetTypeCallback,
                       OSMicrosecondsToTicks(65));
        }
    }

    if ((reg & (SI_COMCSR_RDSTINT | SI_COMCSR_RDSTINTMSK)) == (SI_COMCSR_RDSTINT | SI_COMCSR_RDSTINTMSK)) {
        s32 i;
        u32 vcount;
        u32 x;

        vcount = fn_801E9B4C() + 1;
        x = (Si.poll >> 16) & 0x3FF;

        for (i = 0; i < SI_MAX_CHAN; i++) {
            u32 sr;
            BOOL ok;
            BOOL enabled;

            enabled = OSDisableInterrupts();
            sr = SI_SR >> ((3 - i) * 8);
            if ((sr & SI_ERROR_NO_RESPONSE) && !(Type[i] & SI_ERROR_BUSY)) {
                Type[i] = SI_ERROR_NO_RESPONSE;
            }
            OSRestoreInterrupts(enabled);

            if (sr & 0x20) {
                InputBuffer[i][0] = SI_CHAN_IN_HI(i);
                InputBuffer[i][1] = SI_CHAN_IN_LO(i);
                InputBufferValid[i] = TRUE;
                ok = TRUE;
            } else {
                ok = FALSE;
            }

            if (ok) {
                InputBufferVcount[i] = vcount;
            }
        }

        for (i = 0; i < SI_MAX_CHAN; i++) {
            if (Si.poll & (0x80000000 >> (24 + i))) {
                if (InputBufferVcount[i] == 0 || InputBufferVcount[i] + (x / 2) < vcount) {
                    return;
                }
            }
        }

        for (i = 0; i < SI_MAX_CHAN; i++) {
            InputBufferVcount[i] = 0;
        }

        for (i = 0; i < SI_MAX_CHAN; i++) {
            if (RDSTHandler[i]) {
                RDSTHandler[i](interrupt, context);
            }
        }
    }
}

BOOL SIEnablePollingInterrupt(BOOL enable) {
    BOOL enabled;
    BOOL rc;
    u32 reg;
    s32 i;

    enabled = OSDisableInterrupts();
    reg = SI_COMCSR;
    rc = (reg & SI_COMCSR_RDSTINTMSK) ? TRUE : FALSE;

    if (enable) {
        reg |= SI_COMCSR_RDSTINTMSK;
        for (i = 0; i < SI_MAX_CHAN; i++) {
            InputBufferVcount[i] = 0;
        }
    } else {
        reg &= ~SI_COMCSR_RDSTINTMSK;
    }

    reg &= ~(SI_COMCSR_TCINT | SI_COMCSR_TSTART);
    SI_COMCSR = reg;
    OSRestoreInterrupts(enabled);
    return rc;
}

BOOL SIUnregisterPollingHandler(void (*handler)(s16, OSContext *)) {
    BOOL enabled;
    s32 i;

    enabled = OSDisableInterrupts();
    for (i = 0; i < SI_MAX_CHAN; i++) {
        if (RDSTHandler[i] == handler) {
            RDSTHandler[i] = NULL;
            for (i = 0; i < SI_MAX_CHAN; i++) {
                if (RDSTHandler[i]) {
                    break;
                }
            }
            if (i == SI_MAX_CHAN) {
                SIEnablePollingInterrupt(FALSE);
            }
            OSRestoreInterrupts(enabled);
            return TRUE;
        }
    }
    OSRestoreInterrupts(enabled);
    return FALSE;
}

void SIInit(void) {
    static BOOL Initialized = FALSE;

    if (Initialized) {
        return;
    }

    OSRegisterVersion(__SIVersion);

    Packet[0].chan = Packet[1].chan = Packet[2].chan = Packet[3].chan = SI_CHAN_NONE;

    Si.poll = 0;
    SISetSamplingRate(0);

    while (SI_COMCSR & SI_COMCSR_TSTART)
        ;
    SI_COMCSR = SI_COMCSR_TCINT;

    __OSSetInterruptHandler(0x14, SIInterruptHandler);
    __OSUnmaskInterrupts(0x800);

    SIGetType(0);
    SIGetType(1);
    SIGetType(2);
    SIGetType(3);

    Initialized = TRUE;
}

static BOOL __SITransfer(s32 chan, void *output, u32 outputBytes, void *input, u32 inputBytes,
                         SICallback callback) {
    BOOL enabled;
    u32 rLen;
    u32 i;
    u32 sr;
    union {
        struct {
            u32 tcint : 1;
            u32 tcintmsk : 1;
            u32 comerr : 1;
            u32 rdstint : 1;
            u32 rdstintmsk : 1;
            u32 pad2 : 4;
            u32 outlength : 7;
            u32 pad1 : 1;
            u32 inlength : 7;
            u32 pad0 : 5;
            u32 channel : 2;
            u32 tstart : 1;
        };
        u32 reg;
    } comcsr;

    enabled = OSDisableInterrupts();
    if (Si.chan != SI_CHAN_NONE) {
        OSRestoreInterrupts(enabled);
        return FALSE;
    }

    sr = SI_SR;
    sr &= 0x0F000000 >> (8 * chan);
    SI_SR = sr;

    Si.chan = chan;
    Si.callback = callback;
    Si.inputBytes = inputBytes;
    Si.input = input;

    rLen = (outputBytes + 3) / 4;
    for (i = 0; i < rLen; i++) {
        SI_IOBUF(i) = ((u32 *)output)[i];
    }

    comcsr.reg = SI_COMCSR;
    comcsr.tcint = TRUE;
    comcsr.tcintmsk = callback != NULL;
    comcsr.outlength = outputBytes == 128 ? 0 : outputBytes;
    comcsr.inlength = inputBytes == 128 ? 0 : inputBytes;
    comcsr.channel = chan;
    comcsr.tstart = TRUE;
    SI_COMCSR = comcsr.reg;

    OSRestoreInterrupts(enabled);
    return TRUE;
}

u32 SIGetStatus(s32 chan) {
    BOOL enabled;
    u32 sr;

    enabled = OSDisableInterrupts();
    sr = SI_SR;
    sr >>= (3 - chan) * 8;
    if ((sr & SI_ERROR_NO_RESPONSE) && !(Type[chan] & SI_ERROR_BUSY)) {
        Type[chan] = SI_ERROR_NO_RESPONSE;
    }
    OSRestoreInterrupts(enabled);
    return sr;
}

void SISetCommand(s32 chan, u32 command) {
    SI_CHAN_OUT(chan) = command;
}

void SITransferCommands(void) {
    SI_SR = 0x80000000;
}

u32 SISetXY(u32 x, u32 y) {
    u32 poll;
    BOOL enabled;

    poll = x << 16;
    poll |= y << 8;

    enabled = OSDisableInterrupts();
    Si.poll &= ~0x03FFFF00;
    Si.poll |= poll;
    poll = Si.poll;
    SI_POLL = poll;
    OSRestoreInterrupts(enabled);
    return poll;
}

u32 SIEnablePolling(u32 poll) {
    BOOL enabled;
    u32 en;

    if (poll == 0) {
        return Si.poll;
    }

    enabled = OSDisableInterrupts();
    poll >>= 24;
    en = poll & 0xF0;
    poll &= (en >> 4) | 0x03FFFFF0;
    Si.poll &= ~(en >> 4);
    Si.poll |= poll;
    poll = Si.poll;

    SI_SR = 0x80000000;
    SI_POLL = poll;
    OSRestoreInterrupts(enabled);
    return poll;
}

u32 SIDisablePolling(u32 poll) {
    BOOL enabled;

    if (poll == 0) {
        return Si.poll;
    }

    enabled = OSDisableInterrupts();
    poll = Si.poll & ~((poll >> 24) & 0xF0);
    SI_POLL = poll;
    Si.poll = poll;
    OSRestoreInterrupts(enabled);
    return poll;
}

BOOL SIGetResponse(s32 chan, void *data) {
    BOOL enabled;
    BOOL valid;
    u32 sr;

    enabled = OSDisableInterrupts();
    sr = SIGetStatus(chan);

    if (sr & 0x20) {
        InputBuffer[chan][0] = SI_CHAN_IN_HI(chan);
        InputBuffer[chan][1] = SI_CHAN_IN_LO(chan);
        InputBufferValid[chan] = TRUE;
    }

    valid = InputBufferValid[chan];
    InputBufferValid[chan] = FALSE;
    if (valid) {
        ((u32 *)data)[0] = InputBuffer[chan][0];
        ((u32 *)data)[1] = InputBuffer[chan][1];
    }

    OSRestoreInterrupts(enabled);
    return valid;
}

static void AlarmHandler(OSAlarm *alarm, OSContext *context) {
    s32 chan;
    SIPacket *packet;

    chan = alarm - Alarm;
    packet = &Packet[chan];
    if (packet->chan != SI_CHAN_NONE) {
        if (__SITransfer(packet->chan, packet->output, packet->outputBytes, packet->input,
                         packet->inputBytes, packet->callback)) {
            packet->chan = SI_CHAN_NONE;
        }
    }
}

BOOL SITransfer(s32 chan, void *output, u32 outputBytes, void *input, u32 inputBytes, SICallback callback,
                OSTime fireTime) {
    BOOL enabled;
    SIPacket *packet = &Packet[chan];
    OSTime now;
    OSTime fire;

    enabled = OSDisableInterrupts();
    if (packet->chan != SI_CHAN_NONE || Si.chan == chan) {
        OSRestoreInterrupts(enabled);
        return FALSE;
    }

    now = __OSGetSystemTime();
    if (fireTime == 0) {
        fire = now;
    } else {
        fire = XferTime[chan] + fireTime;
    }

    if (now < fire) {
        OSSetAlarm(&Alarm[chan], fire - now, AlarmHandler);
    } else if (__SITransfer(chan, output, outputBytes, input, inputBytes, callback)) {
        OSRestoreInterrupts(enabled);
        return TRUE;
    }

    packet->chan = chan;
    packet->output = output;
    packet->outputBytes = outputBytes;
    packet->input = input;
    packet->inputBytes = inputBytes;
    packet->callback = callback;
    packet->fire = fire;

    OSRestoreInterrupts(enabled);
    return TRUE;
}

#define SI_TYPE_MASK 0x18000000
#define SI_TYPE_GC 0x08000000
#define SI_GC_WIRELESS 0x80000000
#define SI_WIRELESS_RECEIVED 0x40000000
#define SI_WIRELESS_IR 0x04000000
#define SI_WIRELESS_FIX_ID 0x00100000
#define SI_WIRELESS_ID_MASK 0x00CFFF00

// BYTES: the loop that hands the finished type to whoever asked is repeated verbatim in the retail
// code, once per exit of the wireless id fixing below
#define RUN_TYPE_CALLBACKS()                                  \
    do {                                                      \
        SITypeAndStatusCallback *cbs = TypeCallback[chan];    \
        s32 i;                                                \
        u32 cbType = Type[chan];                              \
        for (i = 0; i < 4; i++) {                             \
            SITypeAndStatusCallback cb = cbs[i];              \
            if (cb) {                                         \
                cbs[i] = NULL;                                \
                cb(chan, cbType);                             \
            }                                                 \
        }                                                     \
    } while (0)

static void GetTypeCallback(s32 chan, u32 error, OSContext *context) {
    static u32 cmdFixDevice[SI_MAX_CHAN];
    u32 type;
    u32 id;
    u32 fix;

    Type[chan] &= ~SI_ERROR_BUSY;
    Type[chan] |= error;
    TypeTime[chan] = __OSGetSystemTime();

    type = Type[chan];

    fix = __PADFixBits & (0x80000000 >> chan);
    __PADFixBits &= ~(0x80000000 >> chan);

    if ((error & 0xF) || (type & SI_TYPE_MASK) != SI_TYPE_GC || !(type & SI_GC_WIRELESS) ||
        (type & SI_WIRELESS_IR)) {
        OSSetWirelessID(chan, 0);
        RUN_TYPE_CALLBACKS();
        return;
    }

    id = (u32)(OSGetWirelessID(chan) << 8);

    if (fix && (id & SI_WIRELESS_FIX_ID)) {
        cmdFixDevice[chan] = 0x4E000000 | (id & SI_WIRELESS_ID_MASK) | SI_WIRELESS_FIX_ID;
        Type[chan] = SI_ERROR_BUSY;
        SITransfer(chan, &cmdFixDevice[chan], 3, &Type[chan], 3, GetTypeCallback, 0);
        return;
    }

    if (type & SI_WIRELESS_FIX_ID) {
        if ((id & SI_WIRELESS_ID_MASK) != (type & SI_WIRELESS_ID_MASK)) {
            if (!(id & SI_WIRELESS_FIX_ID)) {
                id = (type & SI_WIRELESS_ID_MASK) | SI_WIRELESS_FIX_ID;
                OSSetWirelessID(chan, (u16)((id >> 8) & 0xFFFF));
            }

            cmdFixDevice[chan] = 0x4E000000 | id;
            Type[chan] = SI_ERROR_BUSY;
            SITransfer(chan, &cmdFixDevice[chan], 3, &Type[chan], 3, GetTypeCallback, 0);
            return;
        }
    } else if (type & SI_WIRELESS_RECEIVED) {
        id = (type & SI_WIRELESS_ID_MASK) | SI_WIRELESS_FIX_ID;
        OSSetWirelessID(chan, (u16)((id >> 8) & 0xFFFF));

        cmdFixDevice[chan] = 0x4E000000 | id;
        Type[chan] = SI_ERROR_BUSY;
        SITransfer(chan, &cmdFixDevice[chan], 3, &Type[chan], 3, GetTypeCallback, 0);
        return;
    } else {
        OSSetWirelessID(chan, 0);
    }

    RUN_TYPE_CALLBACKS();
}

u32 SIGetType(s32 chan) {
    static u32 cmdTypeAndStatus = 0;
    BOOL enabled;
    u32 type;
    OSTime diff;

    enabled = OSDisableInterrupts();
    type = Type[chan];
    diff = __OSGetSystemTime() - TypeTime[chan];

    if (Si.poll & (0x80 >> chan)) {
        if (type != SI_ERROR_NO_RESPONSE) {
            TypeTime[chan] = __OSGetSystemTime();
            OSRestoreInterrupts(enabled);
            return type;
        }
        Type[chan] = SI_ERROR_BUSY;
        type = SI_ERROR_BUSY;
    } else if (diff <= OSMillisecondsToTicks(50) && type != SI_ERROR_NO_RESPONSE) {
        OSRestoreInterrupts(enabled);
        return type;
    } else if (diff <= OSMillisecondsToTicks(75)) {
        Type[chan] = SI_ERROR_BUSY;
    } else {
        Type[chan] = SI_ERROR_BUSY;
        type = SI_ERROR_BUSY;
    }

    TypeTime[chan] = __OSGetSystemTime();
    SITransfer(chan, &cmdTypeAndStatus, 1, &Type[chan], 3, GetTypeCallback, OSMicrosecondsToTicks(65));
    OSRestoreInterrupts(enabled);
    return type;
}

u32 SIGetTypeAsync(s32 chan, SITypeAndStatusCallback callback) {
    BOOL enabled;
    u32 type;

    enabled = OSDisableInterrupts();
    type = SIGetType(chan);

    if (Type[chan] & SI_ERROR_BUSY) {
        s32 i;

        for (i = 0; i < 4; i++) {
            if (TypeCallback[chan][i] == callback) {
                break;
            }
            if (TypeCallback[chan][i] == NULL) {
                TypeCallback[chan][i] = callback;
                break;
            }
        }
    } else {
        callback(chan, type);
    }

    OSRestoreInterrupts(enabled);
    return type;
}

u32 __PADFixBits;
