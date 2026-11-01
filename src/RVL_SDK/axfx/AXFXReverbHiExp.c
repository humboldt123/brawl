// AXFXReverbHiExp: the reverb effect itself. every channel (left, right, surround) runs the input through a
// pre delay line, three parallel comb filters, two allpass filters, a low pass for damping and a last allpass,
// after which the three channels are cross mixed. AXFXReverbHi in front of this just maps simple knobs onto
// the parameters here.
#include "revolution/axfx.h"
#include "revolution/os.h"
#include <cmath>
#include <cstring>

// BYTES: integer-0 NULL, so pointer tests against NULL compile as signed cmpwi as in retail. Not header-wide:
// making revolution/types.h define NULL 0 for C changes no other SDK unit except OSExec and nwc24, whose
// retail compares are cmplwi ((void*)0), so the NULL spelling differs per original source file.
#undef NULL
#define NULL 0

#define AX_SAMPLES_PER_FRAME 96
#define SAMPLE_RATE_F 32000.0f

// lengths (in samples) of the early reflection taps, three per mode, then the matching gains
static u32 __EarlyLengths[8][3] = {
    {0x9D, 0x1DF, 0x33D}, {0x13D, 0x329, 0x45D}, {0x1DF, 0x3AD, 0x5CF}, {0x281, 0x4EB, 0x79D},
    {0x31D, 0x683, 0xA13}, {0x3C7, 0x76D, 0xB57}, {0x463, 0x883, 0xD55}, {0x4FF, 0x9AD, 0xF31},
};

static f32 __EarlyGains[8][3] = {
    {0.4f, -1.0f, 0.3f}, {0.5f, -0.95f, 0.3f}, {0.6f, -0.9f, 0.3f}, {0.75f, -0.85f, 0.3f},
    {-0.9f, 0.8f, 0.3f}, {-1.0f, 0.7f, 0.3f}, {-1.0f, 0.7f, 0.3f}, {-1.0f, 0.7f, 0.3f},
};

// per fused mode: three comb lengths, two allpass lengths, three last allpass lengths. the last row holds the
// maximum of every column and is what the buffers get sized from.
static u32 __FusedLengths[7][8] = {
    {0x6FD, 0x7CF, 0x91D, 0x1B1, 0x95, 0x2F, 0x49, 0x43},
    {0x95, 0x125, 0x1C1, 0xFB, 0x67, 0x2F, 0x49, 0x43},
    {0x3B3, 0x551, 0x5FB, 0x1B1, 0x89, 0x2F, 0x49, 0x43},
    {0x4FF, 0x5FB, 0x7B5, 0x1FD, 0x95, 0x2F, 0x49, 0x43},
    {0x5FB, 0x737, 0x8F9, 0x233, 0xB3, 0x2F, 0x49, 0x43},
    {0x71F, 0x935, 0xA85, 0x23B, 0x89, 0x2F, 0x49, 0x43},
    {0x71F, 0x935, 0xA85, 0x23B, 0xB3, 0x2F, 0x49, 0x43},
};

static BOOL __AllocDelayLine(AXFX_REVERBHI_EXP *fx);
static void fn_802030E4(AXFX_REVERBHI_EXP *fx);
static void __FreeDelayLine(AXFX_REVERBHI_EXP *fx);
static BOOL __InitParams(AXFX_REVERBHI_EXP *fx);

u32 AXFXReverbHiExpGetMemSize(AXFX_REVERBHI_EXP *fx) {
    s32 num = __EarlyLengths[7][2];

    num += (s32)(SAMPLE_RATE_F * fx->preDelayTimeMax);
    num += __FusedLengths[6][0];
    num += __FusedLengths[6][1];
    num += __FusedLengths[6][2];
    num += __FusedLengths[6][3];
    num += __FusedLengths[6][4];
    num *= 3;
    num += __FusedLengths[6][5];
    num += __FusedLengths[6][6];
    num += __FusedLengths[6][7];

    return num * 4;
}

BOOL AXFXReverbHiExpInit(AXFX_REVERBHI_EXP *fx) {
    BOOL enabled;

    enabled = OSDisableInterrupts();
    fx->active = 1;

    if (fx->preDelayTimeMax < 0.0f) {
        AXFXReverbHiExpShutdown(fx);
        OSRestoreInterrupts(enabled);
        return FALSE;
    }

    fx->earlyMaxLength = __EarlyLengths[7][2];
    fx->preDelayMaxLength = (u32)(SAMPLE_RATE_F * fx->preDelayTimeMax);
    fx->combMaxLength[0] = __FusedLengths[6][0];
    fx->combMaxLength[1] = __FusedLengths[6][1];
    fx->combMaxLength[2] = __FusedLengths[6][2];
    fx->allpassMaxLength[0] = __FusedLengths[6][3];
    fx->allpassMaxLength[1] = __FusedLengths[6][4];
    fx->lastAllpassMaxLength[0] = __FusedLengths[6][5];
    fx->lastAllpassMaxLength[1] = __FusedLengths[6][6];
    fx->lastAllpassMaxLength[2] = __FusedLengths[6][7];

    if (!__AllocDelayLine(fx)) {
        AXFXReverbHiExpShutdown(fx);
        OSRestoreInterrupts(enabled);
        return FALSE;
    }

    fn_802030E4(fx);

    if (!__InitParams(fx)) {
        AXFXReverbHiExpShutdown(fx);
        OSRestoreInterrupts(enabled);
        return FALSE;
    }

    fx->active &= ~1;
    OSRestoreInterrupts(enabled);
    return TRUE;
}

BOOL AXFXReverbHiExpSettings(AXFX_REVERBHI_EXP *fx) {
    BOOL enabled;

    enabled = OSDisableInterrupts();
    fx->active |= 1;
    AXFXReverbHiExpShutdown(fx);

    if (!AXFXReverbHiExpInit(fx)) {
        AXFXReverbHiExpShutdown(fx);
        OSRestoreInterrupts(enabled);
        return FALSE;
    }

    fx->active |= 2;
    fx->active &= ~1;
    OSRestoreInterrupts(enabled);
    return TRUE;
}

void AXFXReverbHiExpShutdown(AXFX_REVERBHI_EXP *fx) {
    BOOL enabled;

    enabled = OSDisableInterrupts();
    fx->active |= 1;
    __FreeDelayLine(fx);
    OSRestoreInterrupts(enabled);
}

void AXFXReverbHiExpCallback(AXFX_BUFFERUPDATE *update, AXFX_REVERBHI_EXP *fx) {
    s32 *upd[3];
    f32 mixed[3];
    f32 chanOut[3];
    s32 *busIn[3];
    s32 *outL;
    s32 *outR;
    s32 *outS;
    f32 lpf;
    f32 lpfInv;
    f32 fused;
    f32 xtalk;
    f32 allpass;
    int i;
    int c;

    if (fx->active) {
        fx->active &= ~2;
        return;
    }

    upd[0] = update->left;
    upd[1] = update->right;
    upd[2] = update->surround;

    if (fx->busIn != NULL) {
        busIn[0] = fx->busIn->left;
        busIn[1] = fx->busIn->right;
        busIn[2] = fx->busIn->surround;
    }

    if (fx->busOut != NULL) {
        outL = fx->busOut->left;
        outR = fx->busOut->right;
        outS = fx->busOut->surround;
    }

    lpf = fx->lpfCoef;
    lpfInv = 1.0f - lpf;
    fused = 0.6f * fx->fusedGain;
    xtalk = 0.5f * fx->crosstalk;
    allpass = fx->allpassCoef;

    for (i = 0; i < AX_SAMPLES_PER_FRAME; i++) {
        for (c = 0; c < 3; c++) {
            f32 in;
            f32 early;
            f32 delayed;
            f32 sum;
            f32 old;
            f32 val;
            f32 lastOld;
            f32 lastNew;

            if (fx->busIn != NULL) {
                s32 *bus = busIn[c];
                in = (f32)(*upd[c] + *bus);
                busIn[c] = bus + 1;
            } else {
                in = (f32)*upd[c];
            }

            early = fx->earlyCoef[0] * fx->earlyLine[c][fx->earlyPos[0]] +
                    fx->earlyCoef[1] * fx->earlyLine[c][fx->earlyPos[1]] +
                    fx->earlyCoef[2] * fx->earlyLine[c][fx->earlyPos[2]];

            if (fx->preDelayLength != 0) {
                delayed = fx->preDelayLine[c][fx->preDelayPos];
                fx->preDelayLine[c][fx->preDelayPos] = in;
            } else {
                delayed = in;
            }

            sum = 0.0f;

            old = fx->combLine[c][0][fx->combPos[0]];
            sum += old;
            fx->combLine[c][0][fx->combPos[0]] = delayed + old * fx->combCoef[0];

            old = fx->combLine[c][1][fx->combPos[1]];
            sum += old;
            fx->combLine[c][1][fx->combPos[1]] = delayed + old * fx->combCoef[1];

            old = fx->combLine[c][2][fx->combPos[2]];
            sum += old;
            fx->combLine[c][2][fx->combPos[2]] = delayed + old * fx->combCoef[2];

            old = fx->allpassLine[c][0][fx->allpassPos[0]];
            val = sum + old * allpass;
            fx->allpassLine[c][0][fx->allpassPos[0]] = val;
            val = old - val * allpass;

            old = fx->allpassLine[c][1][fx->allpassPos[1]];
            lastNew = val + old * allpass;
            fx->allpassLine[c][1][fx->allpassPos[1]] = lastNew;
            val = old - lastNew * allpass;

            val = lpfInv * val + lpf * fx->lastLpfOut[c];
            fx->lastLpfOut[c] = val;

            lastOld = fx->lastAllpassLine[c][fx->lastAllpassPos[c]];
            lastNew = val + lastOld * allpass;
            fx->lastAllpassLine[c][fx->lastAllpassPos[c]] = lastNew;
            val = lastOld - lastNew * allpass;
            fx->lastAllpassPos[c]++;
            if (fx->lastAllpassPos[c] >= fx->lastAllpassLength[c]) {
                fx->lastAllpassPos[c] = 0;
            }

            chanOut[c] = val * fused + early;
        }

        mixed[0] = chanOut[0] + (chanOut[1] + chanOut[2]) * xtalk;
        mixed[1] = chanOut[1] + (chanOut[0] + chanOut[2]) * xtalk;
        mixed[2] = chanOut[2] + (chanOut[0] + chanOut[1]) * xtalk;

        *upd[0]++ = (s32)(mixed[0] * fx->outGain);
        *upd[1]++ = (s32)(mixed[1] * fx->outGain);
        *upd[2]++ = (s32)(mixed[2] * fx->outGain);

        if (fx->busOut != NULL) {
            *outL++ = (s32)(mixed[0] * fx->sendGain);
            *outR++ = (s32)(mixed[1] * fx->sendGain);
            *outS++ = (s32)(mixed[2] * fx->sendGain);
        }

        fx->earlyPos[0]++;
        if (fx->earlyPos[0] >= fx->earlyLength) {
            fx->earlyPos[0] = 0;
        }
        fx->earlyPos[1]++;
        if (fx->earlyPos[1] >= fx->earlyLength) {
            fx->earlyPos[1] = 0;
        }
        fx->earlyPos[2]++;
        if (fx->earlyPos[2] >= fx->earlyLength) {
            fx->earlyPos[2] = 0;
        }

        if (fx->preDelayLength != 0) {
            fx->preDelayPos++;
        if (fx->preDelayPos >= fx->preDelayLength) {
                fx->preDelayPos = 0;
            }
        }

        fx->combPos[0]++;
        if (fx->combPos[0] >= fx->combLength[0]) {
            fx->combPos[0] = 0;
        }
        fx->combPos[1]++;
        if (fx->combPos[1] >= fx->combLength[1]) {
            fx->combPos[1] = 0;
        }
        fx->combPos[2]++;
        if (fx->combPos[2] >= fx->combLength[2]) {
            fx->combPos[2] = 0;
        }

        fx->allpassPos[0]++;
        if (fx->allpassPos[0] >= fx->allpassLength[0]) {
            fx->allpassPos[0] = 0;
        }
        fx->allpassPos[1]++;
        if (fx->allpassPos[1] >= fx->allpassLength[1]) {
            fx->allpassPos[1] = 0;
        }
    }
}

static BOOL __AllocDelayLine(AXFX_REVERBHI_EXP *fx) {
    u32 c;
    u32 i;

    for (c = 0; c < 3; c++) {
        fx->earlyLine[c] = (f32 *)__AXFXAlloc(fx->earlyMaxLength * sizeof(f32));
        if (fx->earlyLine[c] == NULL) {
            return FALSE;
        }

        if (fx->preDelayMaxLength != 0) {
            fx->preDelayLine[c] = (f32 *)__AXFXAlloc(fx->preDelayMaxLength * sizeof(f32));
            if (fx->preDelayLine[c] == NULL) {
                return FALSE;
            }
        } else {
            fx->preDelayLine[c] = NULL;
        }

        for (i = 0; i < 3; i++) {
            fx->combLine[c][i] = (f32 *)__AXFXAlloc(fx->combMaxLength[i] * sizeof(f32));
            if (fx->combLine[c][i] == NULL) {
                return FALSE;
            }
        }

        for (i = 0; i < 2; i++) {
            fx->allpassLine[c][i] = (f32 *)__AXFXAlloc(fx->allpassMaxLength[i] * sizeof(f32));
            if (fx->allpassLine[c][i] == NULL) {
                return FALSE;
            }
        }

        fx->lastAllpassLine[c] = (f32 *)__AXFXAlloc(fx->lastAllpassMaxLength[c] * sizeof(f32));
        if (fx->lastAllpassLine[c] == NULL) {
            return FALSE;
        }
    }

    return TRUE;
}

static void fn_802030E4(AXFX_REVERBHI_EXP *fx) {
    u32 c;
    u32 i;

    for (c = 0; c < 3; c++) {
        if (fx->earlyLine[c] != NULL) {
            memset(fx->earlyLine[c], 0, fx->earlyMaxLength * sizeof(f32));
        }

        if (fx->preDelayLine[c] != NULL) {
            memset(fx->preDelayLine[c], 0, fx->preDelayMaxLength * sizeof(f32));
        }

        for (i = 0; i < 3; i++) {
            if (fx->combLine[c][i] != NULL) {
                memset(fx->combLine[c][i], 0, fx->combMaxLength[i] * sizeof(f32));
            }
        }

        for (i = 0; i < 2; i++) {
            if (fx->allpassLine[c][i] != NULL) {
                memset(fx->allpassLine[c][i], 0, fx->allpassMaxLength[i] * sizeof(f32));
            }
        }

        if (fx->lastAllpassLine[c] != NULL) {
            memset(fx->lastAllpassLine[c], 0, fx->lastAllpassMaxLength[c] * sizeof(f32));
        }
    }
}

static void __FreeDelayLine(AXFX_REVERBHI_EXP *fx) {
    u32 c;
    u32 i;

    for (c = 0; c < 3; c++) {
        if (fx->earlyLine[c] != NULL) {
            __AXFXFree(fx->earlyLine[c]);
            fx->earlyLine[c] = NULL;
        }

        if (fx->preDelayLine[c] != NULL) {
            __AXFXFree(fx->preDelayLine[c]);
            fx->preDelayLine[c] = NULL;
        }

        for (i = 0; i < 3; i++) {
            if (fx->combLine[c][i] != NULL) {
                __AXFXFree(fx->combLine[c][i]);
                fx->combLine[c][i] = NULL;
            }
        }

        for (i = 0; i < 2; i++) {
            if (fx->allpassLine[c][i] != NULL) {
                __AXFXFree(fx->allpassLine[c][i]);
                fx->allpassLine[c][i] = NULL;
            }
        }

        if (fx->lastAllpassLine[c] != NULL) {
            __AXFXFree(fx->lastAllpassLine[c]);
            fx->lastAllpassLine[c] = NULL;
        }
    }
}

static BOOL __InitParams(AXFX_REVERBHI_EXP *fx) {
    u32 i;

    if (fx->earlyMode >= 8) {
        return FALSE;
    }

    if (fx->preDelayTime < 0.0f || fx->preDelayTime > fx->preDelayTimeMax) {
        return FALSE;
    }

    if (fx->fusedMode >= 6) {
        return FALSE;
    }

    if (fx->fusedTime < 0.0f) {
        return FALSE;
    }

    if (fx->coloration < 0.0f || fx->coloration > 1.0f) {
        return FALSE;
    }

    if (fx->damping < 0.0f || fx->damping > 1.0f) {
        return FALSE;
    }

    if (fx->crosstalk < 0.0f || fx->crosstalk > 1.0f) {
        return FALSE;
    }

    if (fx->earlyGain < 0.0f || fx->earlyGain > 1.0f) {
        return FALSE;
    }

    if (fx->fusedGain < 0.0f || fx->fusedGain > 1.0f) {
        return FALSE;
    }

    if (fx->outGain < 0.0f || fx->outGain > 1.0f) {
        return FALSE;
    }

    if (fx->sendGain < 0.0f || fx->sendGain > 1.0f) {
        return FALSE;
    }

    fx->earlyLength = __EarlyLengths[fx->earlyMode][2];
    fx->earlyPos[0] = fx->earlyLength - __EarlyLengths[fx->earlyMode][0];
    fx->earlyCoef[0] = 0.6f * (fx->earlyGain * __EarlyGains[fx->earlyMode][0]);
    fx->earlyPos[1] = fx->earlyLength - __EarlyLengths[fx->earlyMode][1];
    fx->earlyCoef[1] = 0.6f * (fx->earlyGain * __EarlyGains[fx->earlyMode][1]);
    fx->earlyPos[2] = fx->earlyLength - __EarlyLengths[fx->earlyMode][2];
    fx->earlyCoef[2] = 0.6f * (fx->earlyGain * __EarlyGains[fx->earlyMode][2]);

    fx->preDelayPos = 0;
    fx->preDelayLength = (u32)(SAMPLE_RATE_F * fx->preDelayTime);

    for (i = 0; i < 3; i++) {
        u32 len;

        fx->combPos[i] = 0;
        len = __FusedLengths[fx->fusedMode][i];
        fx->combLength[i] = len;
        fx->combCoef[i] = (f32)pow(10.0, -3.0f * (f32)len / (SAMPLE_RATE_F * fx->fusedTime));
    }

    fx->allpassPos[0] = 0;
    fx->allpassLength[0] = __FusedLengths[fx->fusedMode][3];
    fx->allpassPos[1] = 0;
    fx->allpassLength[1] = __FusedLengths[fx->fusedMode][4];
    fx->lastAllpassPos[0] = 0;
    fx->lastAllpassLength[0] = __FusedLengths[fx->fusedMode][5];
    fx->lastAllpassPos[1] = 0;
    fx->lastAllpassLength[1] = __FusedLengths[fx->fusedMode][6];
    fx->lastAllpassPos[2] = 0;
    fx->lastAllpassLength[2] = __FusedLengths[fx->fusedMode][7];
    fx->allpassCoef = fx->coloration;

    fx->lpfCoef = 1.0f - fx->damping;
    if (fx->lpfCoef > 0.95f) {
        fx->lpfCoef = 0.95f;
    }

    fx->lastLpfOut[0] = 0.0f;
    fx->lastLpfOut[1] = 0.0f;
    fx->lastLpfOut[2] = 0.0f;
    return TRUE;
}
