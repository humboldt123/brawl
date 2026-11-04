// WiiConnect24 SDK: Schedule.
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
 * scheduler (/dev/net/kd/request)
 * ------------------------------------------------------------------------------------------- */

#define NWC24_IOCTL_SUSPEND_SCHEDULER 1
#define NWC24_IOCTL_TRY_SUSPEND_SCHEDULER 2
#define NWC24_IOCTL_RESUME_SCHEDULER 3
#define NWC24_IOCTL_GENERATE_USER_ID 15

typedef struct ScdCommonBuffer {
    u32 word0;
} ScdCommonBuffer;

#pragma pack(push, 1)
typedef struct ScdCommonResult {
    s32 result;
    union {
        u64 userid;
        s32 exResult;
    };
    u32 word8;
} ScdCommonResult;
#pragma pack(pop)

#define SCD_IOCTL_IN ((ScdCommonBuffer *)nwc24ScdCommonBuffer)
#define SCD_IOCTL_OUT ((ScdCommonResult *)nwc24ScdCommonResult)

static u32 nwc24ScdInitialized = 0;
static s32 nwc24ScdSuspendCnt = 0;
static s32 nwc24ScdOpenCnt = 0;

static OSMutex nwc24ScdCommandMutex;
static OSMutex nwc24ScdCounterMutex;

static u8 nwc24ScdCommonBuffer[32] __attribute__((aligned(32)));
static u8 nwc24ScdCommonResult[32] __attribute__((aligned(32)));


static NWC24Err ExecSuspendScheduler(void);
static NWC24Err ExecTrySuspendScheduler(u32 arg0);
static NWC24Err ExecResumeScheduler(void);
static NWC24Err ExecNoParamCommand(const char *user, s32 type, NWC24Err *exResultOut);

static void InitScdMutex(void);

static void ScdLockRight(void);
static BOOL ScdTryLockRight(void);
static void ScdUnlockRight(void);

static void LockCounters(void);
static BOOL TryLockCounters(void);
static void UnlockCounters(void);

static NWC24Err ScdCheckCallingStatus(const char *user, BOOL block);

static inline NWC24Err ScdCheckCallingStatusStrict(void);

s32 NWC24SuspendScheduler(void) {
    s32 count;

    LockCounters();
    {
        count = ExecSuspendScheduler();
        if (count >= 0) {
            nwc24ScdSuspendCnt++;
            count -= nwc24ScdOpenCnt;
        }
    }
    UnlockCounters();

    return count;
}

s32 NWC24ResumeScheduler(void) {
    s32 count;

    LockCounters();
    {
        if (nwc24ScdOpenCnt > 0 && nwc24ScdSuspendCnt == 0) {
            count = 0;
        } else {
            count = ExecResumeScheduler();

            if (nwc24ScdSuspendCnt > 0) {
                nwc24ScdSuspendCnt--;
                count -= nwc24ScdOpenCnt;
            }
        }
    }
    UnlockCounters();

    return count;
}

NWC24Err NWC24iGetSchedulerStat(void *stat, u32 size) {
    s32 fd;
    NWC24Err result;
    NWC24Err close;

    if (size > 0x100) {
        return NWC24_ERR_OVERFLOW;
    }

    if (!OSGetCurrentThread()) {
        return NWC24_ERR_FATAL;
    }

    ScdLockRight();
    {
        result = NWC24_OPEN_DEVICE("/dev/net/kd/request", &fd, 0);

        if (result >= 0) {
            result = NWC24_IOCTL_DEVICE(fd, 0x1E, NULL, 0, stat, size);

            if (result >= 0) {
                result = *(s32 *)stat;
            }

            close = NWC24_CLOSE_DEVICE(fd);
            if (result >= 0) {
                result = close;
            }
        }

        // HYPOTHESIS: word 3 of the stat block carries the "new mail" flags
        if (size > 0x10) {
            NWC24iSetNewMsgArrived(((u32 *)stat)[3]);
        }
    }
    ScdUnlockRight();

    return result;
}

NWC24Err NWC24iSetScriptMode(u32 mode) {
    s32 fd;
    NWC24Err result;
    NWC24Err close;

    result = ScdCheckCallingStatusStrict();
    if (result < 0) {
        return result;
    }

    ScdLockRight();
    {
        memset(nwc24ScdCommonBuffer, 0, sizeof(nwc24ScdCommonBuffer));

        result = NWC24_OPEN_DEVICE("/dev/net/kd/request", &fd, 0);

        if (result >= 0) {
            SCD_IOCTL_IN->word0 = mode;

            result = NWC24_IOCTL_DEVICE(fd, 0x22, nwc24ScdCommonBuffer, sizeof(nwc24ScdCommonBuffer),
                                        nwc24ScdCommonResult, sizeof(nwc24ScdCommonResult));

            if (result >= 0) {
                result = SCD_IOCTL_OUT->result;
            }

            close = NWC24_CLOSE_DEVICE(fd);
            if (result >= 0) {
                result = close;
            }
        }
    }
    ScdUnlockRight();

    return result;
}

NWC24Err NWC24iRequestGenerateUserId(NWC24UserId *idOut, u32 *arg1) {
    s32 fd;
    NWC24Err result;
    NWC24Err close;

    result = ScdCheckCallingStatus(__FUNCTION__, TRUE);
    if (result < 0) {
        return result;
    }

    ScdLockRight();
    {
        result = NWC24_OPEN_DEVICE("/dev/net/kd/request", &fd, 0);

        if (result >= 0) {
            result = NWC24_IOCTL_DEVICE(fd, NWC24_IOCTL_GENERATE_USER_ID, NULL, 0, nwc24ScdCommonResult,
                                        sizeof(nwc24ScdCommonResult));

            if (result >= 0) {
                result = SCD_IOCTL_OUT->result;

                if (result == NWC24_OK || result == NWC24_ERR_ID_GENERATED || result == NWC24_ERR_ID_REGISTERED) {
                    if (idOut != NULL) {
                        *idOut = SCD_IOCTL_OUT->userid;
                    }

                    if (arg1 != NULL) {
                        *arg1 = SCD_IOCTL_OUT->word8;
                    }
                }
            }

            close = NWC24_CLOSE_DEVICE(fd);
            if (result >= 0) {
                result = close;
            }
        }
    }
    ScdUnlockRight();

    return result;
}

NWC24Err NWC24iExecCmd6(NWC24Err *out) {
    return ExecNoParamCommand(NULL, 6, out);
}

NWC24Err NWC24iExecCmd7(NWC24Err *out) {
    return ExecNoParamCommand(NULL, 7, out);
}

NWC24Err NWC24iExecCmd8(void) {
    return ExecNoParamCommand(NULL, 8, NULL);
}

NWC24Err NWC24iExecCmd9(void) {
    return ExecNoParamCommand(NULL, 9, NULL);
}

NWC24Err NWC24iTrySuspendForOpenLib(void) {
    NWC24Err result;

    if (!TryLockCounters()) {
        return NWC24_ERR_MUTEX;
    }

    result = ExecTrySuspendScheduler(0);
    if (result >= 0) {
        nwc24ScdOpenCnt++;
        result = NWC24_OK;
    }

    UnlockCounters();
    return result;
}

NWC24Err NWC24iResumeForCloseLib(void) {
    NWC24Err result;

    LockCounters();
    {
        result = ExecResumeScheduler();
        if (result >= 0) {
            nwc24ScdOpenCnt--;
            result = NWC24_OK;
        }
    }
    UnlockCounters();

    return result;
}

static NWC24Err ExecSuspendScheduler(void) {
    return ExecNoParamCommand(NULL, NWC24_IOCTL_SUSPEND_SCHEDULER, NULL);
}

static NWC24Err ExecTrySuspendScheduler(u32 arg0) {
    s32 fd;
    NWC24Err result;
    NWC24Err close;

    result = ScdCheckCallingStatus(__FUNCTION__, TRUE);
    if (result < 0) {
        return result;
    }

    if (!ScdTryLockRight()) {
        return NWC24_ERR_MUTEX;
    }

    result = NWC24_OPEN_DEVICE("/dev/net/kd/request", &fd, 0);

    if (result >= 0) {
        SCD_IOCTL_IN->word0 = arg0;

        result = NWC24_IOCTL_DEVICE(fd, NWC24_IOCTL_TRY_SUSPEND_SCHEDULER, nwc24ScdCommonBuffer,
                                    sizeof(nwc24ScdCommonBuffer), nwc24ScdCommonResult, sizeof(nwc24ScdCommonResult));

        if (result >= 0) {
            result = SCD_IOCTL_OUT->result;
        }

        close = NWC24_CLOSE_DEVICE(fd);
        if (close < 0) {
            result = close;
        }
    }

    ScdUnlockRight();
    return result;
}

static NWC24Err ExecResumeScheduler(void) {
    return ExecNoParamCommand(NULL, NWC24_IOCTL_RESUME_SCHEDULER, NULL);
}

static NWC24Err ExecNoParamCommand(const char *user, s32 type, NWC24Err *exResultOut) {
    s32 fd;
    NWC24Err result;
    NWC24Err close;

    if (!OSGetCurrentThread()) {
        return NWC24_ERR_FATAL;
    }

    ScdLockRight();
    {
        result = NWC24iOpenResourceManager(user, "/dev/net/kd/request", &fd, 0);

        if (result >= 0) {
            result = NWC24iIoctlResourceManager(user, fd, type, NULL, 0, nwc24ScdCommonResult,
                                                sizeof(nwc24ScdCommonResult));

            if (result >= 0) {
                result = SCD_IOCTL_OUT->result;

                if (result == NWC24_ERR_FAILED || result == NWC24_ERR_CONFIG_NETWORK) {
                    if (exResultOut != NULL) {
                        *exResultOut = SCD_IOCTL_OUT->exResult;
                    }
                }
            }

            close = NWC24iCloseResourceManager(user, fd);
            if (close < 0) {
                result = close;
            }
        }
    }
    ScdUnlockRight();

    return result;
}


// like ScdCheckCallingStatus but also refuses while the lib open is blocked
static inline NWC24Err ScdCheckCallingStatusStrict(void) {
    if (!OSGetCurrentThread()) {
        return NWC24_ERR_FATAL;
    }

    if (NWC24IsMsgLibOpened() || NWC24IsMsgLibOpenedByTool()) {
        return NWC24_ERR_LIB_OPENED;
    }

    if (NWC24IsMsgLibOpenBlocking()) {
        return NWC24_ERR_BUSY;
    }

    return NWC24_OK;
}


// HYPOTHESIS: plain "no parameter" scheduler commands 6..9. only the socket library (so.c) calls
// them, around its startup/cleanup, so they are probably "tell kd the network is going up/down".


static inline void InitScdMutex(void) {
    BOOL enabled = OSDisableInterrupts();

    if (!(nwc24ScdInitialized & 1)) {
        OSInitMutex(&nwc24ScdCommandMutex);
        OSInitMutex(&nwc24ScdCounterMutex);

        memset(nwc24ScdCommonBuffer, 0, sizeof(nwc24ScdCommonBuffer));
        memset(nwc24ScdCommonResult, 0, sizeof(nwc24ScdCommonResult));

        nwc24ScdInitialized |= 1;
    }

    OSRestoreInterrupts(enabled);
}

static inline void ScdLockRight(void) {
    if (!(nwc24ScdInitialized & 1)) {
        InitScdMutex();
    }

    OSLockMutex(&nwc24ScdCommandMutex);
}

static inline BOOL ScdTryLockRight(void) {
    if (!(nwc24ScdInitialized & 1)) {
        InitScdMutex();
    }

    return OSTryLockMutex(&nwc24ScdCommandMutex);
}

static inline void ScdUnlockRight(void) {
    OSUnlockMutex(&nwc24ScdCommandMutex);
}

static inline void LockCounters(void) {
    if (!(nwc24ScdInitialized & 1)) {
        InitScdMutex();
    }

    OSLockMutex(&nwc24ScdCounterMutex);
}

static inline BOOL TryLockCounters(void) {
    if (!(nwc24ScdInitialized & 1)) {
        InitScdMutex();
    }

    return OSTryLockMutex(&nwc24ScdCounterMutex);
}

static inline void UnlockCounters(void) {
    OSUnlockMutex(&nwc24ScdCounterMutex);
}

static inline NWC24Err ScdCheckCallingStatus(const char *user, BOOL block) {
#pragma unused(user)
#pragma unused(block)
    if (!OSGetCurrentThread()) {
        return NWC24_ERR_FATAL;
    }

    if (NWC24IsMsgLibOpened() || NWC24IsMsgLibOpenedByTool()) {
        return NWC24_ERR_LIB_OPENED;
    }

    return NWC24_OK;
}


// Unused scheduler-status reply buffer retained in the SDK object.
u8 nwc24ScdStatBuf[256] __attribute__((aligned(32)));
