// WiiConnect24 SDK: Time.
#include "revolution.h"
#include "revolution/nwc24.h"
#include "revolution/nwc24/NWC24Internal.h"
#include "revolution/sc.h"
#include "revolution/vf.h"
#include <cstring>
#include <cstdarg>

// BYTES: preserve calls across SDK source boundaries; original inline helpers
// are marked inline explicitly.
#pragma auto_inline off

/* ---------------------------------------------------------------------------------------------
 * time (/dev/net/kd/time, rtc counter bookkeeping)
 * ------------------------------------------------------------------------------------------- */

#define NWC24_IOCTL_SET_RTC_COUNTER 23
#define NWC24_IOCTL_GET_TIME_DIFFERENCE 24

typedef struct TimeCommonBuffer {
    u32 rtc;
    u32 flags;
} TimeCommonBuffer;

#pragma pack(push, 1)
typedef struct TimeCommonResult {
    s32 result;
    s64 diff;
} TimeCommonResult;
#pragma pack(pop)

#define TIME_IOCTL_IN ((TimeCommonBuffer *)nwc24TimeCommonBuffer)
#define TIME_IOCTL_OUT ((TimeCommonResult *)nwc24TimeCommonResult)

static BOOL nwc24TimeInitialized = FALSE;
static u32 nwc24TimeRtc = 0;
static s64 nwc24TimeDifference = 0;

static u8 nwc24TimeCommonResult[32] __attribute__((aligned(32)));
static u8 nwc24TimeCommonBuffer[32] __attribute__((aligned(32)));
static OSMutex nwc24TimeCommandMutex __attribute__((aligned(32)));

static NWC24Err GetRTC(u32 *rtc);
static NWC24Err TimeCheckCallingStatus(const char *user);
static void TimeInitMutex(void);
static void TimeLockRight(void);
static void TimeUnlockRight(void);

// BYTES: explicit zero initialization retains the SDK formatter scratch
// before the mutex and IPC buffers in the original pooled storage.
u8 nwc24TimeUnusedBuf[128] = {0};

NWC24Err NWC24iGetUniversalTime(s64 *timeOut) {
    static s64 whenCached = 0;

    NWC24Err result;

    if (timeOut == NULL) {
        nwc24TimeDifference = 0;
        return NWC24_ERR_INVALID_VALUE;
    }

    if (whenCached == 0 || whenCached + OS_TIMER_CLOCK <= __OSGetSystemTime() || nwc24TimeRtc == 0) {
        result = NWC24iGetTimeDifference(&nwc24TimeDifference);
        if (result < 0) {
            return result;
        }

        result = GetRTC(&nwc24TimeRtc);
        if (result != NWC24_OK) {
            return result;
        }

        whenCached = __OSGetSystemTime();
    }

    *timeOut = nwc24TimeDifference + nwc24TimeRtc;
    return NWC24_OK;
}

NWC24Err NWC24iGetTimeDifference(s64 *diffOut) {
    s32 fd;
    NWC24Err result;
    NWC24Err close;

    result = TimeCheckCallingStatus(__FUNCTION__);
    if (result < 0) {
        return result;
    }

    TimeLockRight();
    {
        result = NWC24_OPEN_DEVICE("/dev/net/kd/time", &fd, 0);

        if (result >= 0) {
            result = NWC24_IOCTL_DEVICE(fd, NWC24_IOCTL_GET_TIME_DIFFERENCE, NULL, 0, &nwc24TimeCommonResult,
                                        sizeof(nwc24TimeCommonResult));

            if (result >= 0) {
                result = TIME_IOCTL_OUT->result;

                if (result == 0 && diffOut != NULL) {
                    *diffOut = TIME_IOCTL_OUT->diff;
                }
            }

            close = NWC24_CLOSE_DEVICE(fd);
            if (result >= 0) {
                result = close;
            }
        }
    }
    TimeUnlockRight();

    return result;
}

NWC24Err NWC24iSetRtcCounter(u32 rtc, u32 flags) {
    s32 fd;
    NWC24Err result;
    NWC24Err close;

    result = TimeCheckCallingStatus(__FUNCTION__);
    if (result < 0) {
        return result;
    }

    TimeLockRight();
    {
        result = NWC24_OPEN_DEVICE("/dev/net/kd/time", &fd, 0);

        if (result >= 0) {
            TIME_IOCTL_IN->rtc = rtc;
            TIME_IOCTL_IN->flags = flags;

            result = NWC24_IOCTL_DEVICE(fd, NWC24_IOCTL_SET_RTC_COUNTER, &nwc24TimeCommonBuffer,
                                        sizeof(nwc24TimeCommonBuffer), &nwc24TimeCommonResult,
                                        sizeof(nwc24TimeCommonResult));

            if (result >= 0) {
                result = TIME_IOCTL_OUT->result;
            }

            close = NWC24_CLOSE_DEVICE(fd);
            if (result >= 0) {
                result = close;
            }
        }
    }
    TimeUnlockRight();

    return result;
}

NWC24Err NWC24iSynchronizeRtcCounter(BOOL forceSave) {
    u32 rtc;
    NWC24Err result;

    result = GetRTC(&rtc);
    if (result != NWC24_OK) {
        return result;
    }

    return NWC24iSetRtcCounter(rtc, forceSave ? 1 : 0);
}

static inline NWC24Err GetRTC(u32 *rtc) {
    u32 status;

    do {
        status = SCCheckStatus();

        if (status == SC_STATUS_FATAL) {
            return NWC24_ERR_FATAL;
        }
    } while (status != SC_STATUS_OK);

    *rtc = OSTicksToSeconds(OSGetTime()) - SCGetCounterBias();
    return NWC24_OK;
}

static inline NWC24Err TimeCheckCallingStatus(const char *user) {
#pragma unused(user)
    if (!OSGetCurrentThread()) {
        return NWC24_ERR_FATAL;
    }

    return NWC24_OK;
}

static inline void TimeInitMutex(void) {
    BOOL enabled = OSDisableInterrupts();

    if (!nwc24TimeInitialized) {
        OSInitMutex(&nwc24TimeCommandMutex);

        memset(&nwc24TimeCommonBuffer, 0, sizeof(nwc24TimeCommonBuffer));
        memset(&nwc24TimeCommonResult, 0, sizeof(nwc24TimeCommonResult));

        nwc24TimeInitialized = TRUE;
    }

    OSRestoreInterrupts(enabled);
}

static inline void TimeLockRight(void) {
    if (!nwc24TimeInitialized) {
        TimeInitMutex();
    }

    OSLockMutex(&nwc24TimeCommandMutex);
}

static inline void TimeUnlockRight(void) {
    OSUnlockMutex(&nwc24TimeCommandMutex);
}
