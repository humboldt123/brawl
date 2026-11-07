#include "revolution.h"
#include "revolution/nwc24.h"
#include "revolution/nwc24/NWC24Internal.h"
#include "revolution/sc.h"
#include "revolution/vf.h"
#include <cstring>
#include <cstdarg>

// BYTES: preserve the SDK's explicit inline helpers.
#pragma auto_inline off

/* ---------------------------------------------------------------------------------------------
 * manage (open/close the lib, version registration, mail-arrived flags)
 * ------------------------------------------------------------------------------------------- */

#define MANAGE_ERROR_CODE_BASE 109000

enum {
    NWC24_LIB_CLOSED,
    NWC24_LIB_OPENED,
    NWC24_LIB_OPENED_BY_TOOL,
    NWC24_LIB_BLOCKED
};

enum {
    NWC24_FAIL_SFL = (1 << 0),
    NWC24_FAIL_DL_TASK = (1 << 1),
    NWC24_FAIL_FATAL = (1 << 2)
};

const char *__NWC24Version = "<< RVL_SDK - NWC24 \trelease build: Jun 28 2007 18:29:32 (0x4199_60831) >>";

NWC24iWork *NWC24WorkP = NULL;

static int Opened = NWC24_LIB_CLOSED;
static u32 YouGotMail = 0;
static u32 GlobalErrorCode = 0;
static BOOL Registered = FALSE;

static inline void Register_i(void);
static inline BOOL IsOpened_i(void);
static inline BOOL IsBlocking_i(void);
static inline void SetErrorCode_i(u32 code);
static NWC24Err NWC24OpenLibInternal(NWC24iWork *work, int state);

// the small manage helpers get inlined into NWC24OpenLibInternal, but other files call the real ones
static inline void Register_i(void) {
    if (Registered) {
        return;
    }

    OSRegisterVersion(__NWC24Version);
    Registered = TRUE;
}

void NWC24iRegister(void) {
    Register_i();
}

NWC24Err NWC24OpenLib(void *work) {
    if (Opened == NWC24_LIB_OPENED_BY_TOOL) {
        return NWC24_ERR_BUSY;
    }

    return NWC24OpenLibInternal((NWC24iWork *)work, NWC24_LIB_OPENED);
}

static NWC24Err NWC24OpenLibInternal(NWC24iWork *work, int state) {
    NWC24Err result;
    NWC24Err failErr;
    u32 failFlag;

    SetErrorCode_i(NWC24_OK);

    if (!VFIsAvailable()) {
        return NWC24_ERR_FATAL;
    }

    if (IsOpened_i()) {
        return NWC24_ERR_LIB_OPENED;
    }

    if (IsBlocking_i()) {
        return NWC24_ERR_BUSY;
    }

    if (work == NULL) {
        return NWC24_ERR_NULL;
    }

    if ((u32)work % 32 != 0) {
        return NWC24_ERR_ALIGNMENT;
    }

    result = NWC24iTrySuspendForOpenLib();
    if (result == NWC24_OK) {
        Register_i();

        YouGotMail &= ~(1 << NWC24_MSGTYPE_RVL_MENU_SHARED);
        NWC24WorkP = work;

        NWC24InitBase64Table(NWC24WorkP->base64Work);

        failFlag = 0;
        failErr = NWC24_OK;

        result = NWC24iConfigOpen();
        if (result != NWC24_OK) {
            failErr = result;
            failFlag |= NWC24_FAIL_FATAL;
        }

        result = NWC24iOpenMBox();
        if (result != NWC24_OK) {
            failErr = result;
            failFlag |= NWC24_FAIL_FATAL;
        }

        result = NWC24iOpenFriendList();
        if (result != NWC24_OK) {
            failErr = result;
            failFlag |= NWC24_FAIL_FATAL;
        }

        result = NWC24iOpenSecretFriendList();
        if (result != NWC24_OK) {
            failErr = result;

            if (result == NWC24_ERR_FILE_NOEXISTS) {
                failFlag |= NWC24_FAIL_FATAL;
            } else {
                failFlag |= NWC24_FAIL_SFL;
            }
        }

        result = NWC24iOpenDlTaskList();
        if (result < 0) {
            failErr = result;

            if (result == NWC24_ERR_FILE_NOEXISTS) {
                failFlag |= NWC24_FAIL_FATAL;
            } else {
                failFlag |= NWC24_FAIL_DL_TASK;
            }
        }

        if (failFlag == (NWC24_FAIL_SFL | NWC24_FAIL_DL_TASK)) {
            failErr = NWC24_ERR_OLD_SYSTEM;
        }

        if (failFlag != 0) {
            NWC24WorkP = NULL;
            NWC24iResumeForCloseLib();
            result = failErr;
        } else {
            Opened = state;
            return NWC24_OK;
        }
    }

    switch (result) {
    case NWC24_ERR_OLD_SYSTEM:
    case NWC24_ERR_FILE_BROKEN:
    case NWC24_ERR_INTERNAL_VF:
    case NWC24_ERR_INTERNAL_IPC:
    case NWC24_ERR_NAND_CORRUPT:
    case NWC24_ERR_INPROGRESS:
    case NWC24_ERR_BUSY:
    case NWC24_ERR_MUTEX:
    case NWC24_ERR_FILE_OTHER:
    case NWC24_ERR_FILE_NOEXISTS:
    case NWC24_ERR_FILE_WRITE:
    case NWC24_ERR_FILE_READ:
    case NWC24_ERR_FILE_CLOSE:
    case NWC24_ERR_FILE_OPEN:
    case NWC24_ERR_BROKEN:
    case NWC24_ERR_FATAL:
        SetErrorCode_i(result - MANAGE_ERROR_CODE_BASE);
        break;
    }

    return result;
}

NWC24Err NWC24CloseLib(void) {
    s32 result;

    if (Opened != NWC24_LIB_OPENED) {
        return NWC24_ERR_LIB_NOT_OPENED;
    }

    result = NWC24iConfigFlush();
    if (result != NWC24_OK) {
        return result;
    }

    result = NWC24iCloseDlTaskList();
    if (result < 0) {
        return result;
    }

    result = NWC24iResumeForCloseLib();
    if (result != NWC24_OK) {
        return result;
    }

    NWC24WorkP = NULL;
    Opened = NWC24_LIB_CLOSED;
    return result;
}

static inline BOOL IsOpened_i(void) {
    return Opened == NWC24_LIB_OPENED;
}

BOOL NWC24IsMsgLibOpened(void) {
    return IsOpened_i();
}

BOOL NWC24IsMsgLibOpenedByTool(void) {
    return Opened == NWC24_LIB_OPENED_BY_TOOL;
}

static inline BOOL IsBlocking_i(void) {
    return Opened == NWC24_LIB_BLOCKED;
}

BOOL NWC24IsMsgLibOpenBlocking(void) {
    return IsBlocking_i();
}

NWC24Err NWC24BlockOpenMsgLib(BOOL block) {
    s32 result;
    BOOL enabled;

    result = NWC24_OK;
    enabled = OSDisableInterrupts();

    if (block) {
        if (Opened == NWC24_LIB_CLOSED) {
            Opened = NWC24_LIB_BLOCKED;
        } else if (Opened == NWC24_LIB_OPENED) {
            result = NWC24_ERR_LIB_OPENED;
        } else {
            result = NWC24_ERR_BUSY;
        }
    } else {
        if (Opened == NWC24_LIB_BLOCKED) {
            Opened = NWC24_LIB_CLOSED;
        } else {
            result = NWC24_ERR_LIB_NOT_OPENED;
        }
    }

    OSRestoreInterrupts(enabled);
    return result;
}

NWC24Err NWC24iSetNewMsgArrived(u32 flags) {
    YouGotMail |= flags;
    return NWC24_OK;
}

static inline void SetErrorCode_i(u32 code) {
    GlobalErrorCode = code;
}

NWC24Err fn_80399BB8(s32 *stage);
/* ---------------------------------------------------------------------------------------------
 * system status checks (works out whether the scheduler is allowed to run, and what to tell the user)
 * ------------------------------------------------------------------------------------------- */

// plain sc items that live in the sc unit, none of them have names yet
extern u32 fn_80210064(void);
extern u32 fn_802100A0(void);
extern u32 fn_802100DC(void);
extern BOOL fn_8020FC90(void *dst);
extern s32 NCDiGetEnabledConfigList(u32 *a, u32 *b, u32 *c);

static NWC24Err fn_8039A69C(s32 code, u32 mode, u32 *delay);
static NWC24Err fn_8039A53C(s32 *outCode, u32 mode);

// HYPOTHESIS: reads the id creation stage out of the config

// checks a string: not null and not empty, no longer than maxlen, at least minlen

// strlcpy, hands back the number of chars copied

NWC24Err fn_8039A350(u32 mode) {
    NWC24Err result;
    u8 parental[0x4A];
    u32 sp18;
    u32 sp14;
    u32 sp10;
    s32 stage;
    BOOL forScheduler;
    u32 eula;
    u32 status;
    u32 isEnabled;
    u32 sp08;
    u32 restrictions;
    NWC24Err scheduler;

    GlobalErrorCode = 0;

    if (Opened != NWC24_LIB_OPENED) {
        result = NWC24_ERR_LIB_NOT_OPENED;
        goto end;
    }

    result = -39;

    do {
        status = SCCheckStatus();
    } while (status == 1);

    if (status == 2) {
        GlobalErrorCode = -112 - MANAGE_ERROR_CODE_BASE;
        goto end;
    }

    isEnabled = fn_802100DC() & 1;
    eula = fn_802100A0();
    restrictions = fn_80210064() & 2;
    fn_8020FC90(parental);

    if (isEnabled == 0) {
        GlobalErrorCode = -139 - MANAGE_ERROR_CODE_BASE;
        goto end;
    }

    if (eula == 0) {
        GlobalErrorCode = -107 - MANAGE_ERROR_CODE_BASE;
        goto end;
    }

    forScheduler = mode & 1;
    if (forScheduler && (parental[0] & 0x80) && restrictions != 0) {
        GlobalErrorCode = -107 - MANAGE_ERROR_CODE_BASE;
        goto end;
    }

    result = -31;
    if (NCDiGetEnabledConfigList(&sp18, &sp14, &sp10) < 0) {
        GlobalErrorCode = -133 - MANAGE_ERROR_CODE_BASE;
        goto end;
    }

    if (sp18 == 0 && sp14 == 0 && sp10 == 0) {
        GlobalErrorCode = -50299;
        goto end;
    }

    if (forScheduler) {
        fn_80399BB8(&stage);
        if (stage != NWC24_IDCS_REGISTERED) {
            GlobalErrorCode = -144 - MANAGE_ERROR_CODE_BASE;
            goto end;
        }
    }

    sp08 = 0;
    scheduler = fn_8039A53C((s32 *)&sp08, mode);
    if (scheduler < 0) {
        GlobalErrorCode = sp08;
        result = scheduler;
        goto end;
    }

    if ((result = NWC24iMBoxCheck(0, 0)) == NWC24_ERR_FULL) {
        GlobalErrorCode = -106 - MANAGE_ERROR_CODE_BASE;
    }

    if (result != NWC24_ERR_FULL) {
        result = NWC24_OK;
    }

end:
    return result;
}

s32 NWC24GetErrorCode(void) {
    return GlobalErrorCode;
}

void NWC24iSetErrorCode(u32 code) {
    SetErrorCode_i(code);
}

// walks the scheduler's recent error list (newest first) until the delays add up to a minute
static NWC24Err fn_8039A53C(s32 *outCode, u32 mode) {
    u32 count;
    u32 n;
    u32 *stat = (u32 *)NWC24WorkP->WORK_0x400;
    u32 sum = 0;
    u32 delay;
    NWC24Err result;
    u32 *entry;
    u32 i;
    s32 idx;

    *outCode = 0;
    result = NWC24iGetSchedulerStat(stat, 0x100);

    if (result == NWC24_ERR_FATAL || result == NWC24_ERR_NOMEM || result == -42) {
        *outCode = result - 0x1AA2C - 1 + 1;
        return NWC24_ERR_FATAL;
    }

    if (result != NWC24_OK) {
        return NWC24_OK;
    }

    if (mode & 1) {
        if ((stat[0x44 / 4] & 3) == 3) {
            return NWC24_OK;
        }
    } else if (mode & 2) {
        if ((stat[0x48 / 4] & 0xF) == 0xF) {
            return NWC24_OK;
        }
    }

    count = stat[0x14 / 4];
    n = 32;
    if (count < 32) {
        n = count;
    }

    idx = (s32)(count + 31) % 32;
    for (i = 0; i < n; i++) {
        entry = &stat[idx];
        result = fn_8039A69C(entry[0x80 / 4], mode, &delay);
        if (result != NWC24_OK) {
            sum += delay;
            if (sum >= 60) {
                *outCode = entry[0x80 / 4];
                return result;
            }
        }

        idx--;
        if (idx < 0) {
            idx = 31;
        }
    }

    *outCode = 0;
    return NWC24_OK;
}

// HYPOTHESIS: turns one scheduler error code (a six digit number, digits mean something) into how long the
// user should be told to wait and whether it counts as a network or server problem
static NWC24Err fn_8039A69C(s32 code, u32 mode, u32 *delay) {
    s8 d0, d1, d2, d3, d4, d5;
    s32 n;
    s32 low;

    *delay = 0;

    n = -code;
    d0 = n % 10;
    n /= 10;
    d1 = n % 10;
    n /= 10;
    d2 = n % 10;
    n /= 10;
    d3 = n % 10;
    n /= 10;
    d4 = n % 10;
    n /= 10;
    d5 = n % 10;

    if (d5 == 0 && d4 == 5) {
        *delay = 60;
        return -31;
    }

    if (d3 == 7) {
        if (!(mode & 2)) {
            return NWC24_OK;
        }
    } else if (!(mode & 1)) {
        return NWC24_OK;
    }

    if (d5 == 1 && d4 == 1) {
        if (d2 == 1) {
            return NWC24_OK;
        }

        if (d2 == 4 && d1 == 0 && d0 == 7) {
            *delay = 60;
            return -31;
        }

        if (!(mode & 1)) {
            return NWC24_OK;
        }

        *delay = 30;
        return -32;
    } else if (d5 == 1 && d4 == 0) {
        if (!(mode & 1)) {
            return NWC24_OK;
        }

        low = d0 + d1 * 10;
        switch (d2) {
        case 0:
            if (low >= 70) {
                return NWC24_OK;
            }

            *delay = 60;
            switch (low) {
            case 0:
            case 1:
            case 2:
            case 3:
                return -1;
            case 30:
            case 31:
                return -31;
            case 32:
                return -32;
            }
            return NWC24_OK;
        case 2:
            switch (-low) {
            case -1:
                *delay = 60;
                return -1;
            case -11:
            case -45:
                return NWC24_OK;
            case -31:
            case -44:
            case -33:
                *delay = 60;
                return -31;
            case -32:
                *delay = 20;
                return -32;
            }
            return NWC24_OK;
        case 3:
            switch (low) {
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
                *delay = 30;
                return -31;
            case 6:
            case 7:
            case 8:
                *delay = 20;
                return -32;
            }
            return NWC24_OK;
        case 4:
            switch (low) {
            case 9:
            case 10:
            case 11:
            case 12:
                *delay = 60;
                return -32;
            default:
                *delay = 20;
                return -31;
            }
        }
    }

    return NWC24_OK;
}
