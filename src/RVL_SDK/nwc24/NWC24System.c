// WiiConnect24 SDK: System.
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
 * system (shutdown hook + led)
 * ------------------------------------------------------------------------------------------- */

#define SHUTDOWN_RETRY_MAX 5
#define NWC24_IOCTL_SHUTDOWN 40

static s32 nwc24ShtFd = -1;
static s32 nwc24ShtRetryRest = 0;
static OSShutdownFunctionInfo ShutdownFuncInfo;

static inline NWC24Err EnableLedNotification(BOOL enable) {
    SCIdleModeInfo idleMode;
    u32 status;
    u32 led = 0;

    if (enable) {
        do {
            status = SCCheckStatus();

            if (status == SC_STATUS_FATAL) {
                return NWC24_ERR_FATAL;
            }
        } while (status == SC_STATUS_BUSY);

        SCGetIdleMode(&idleMode);
        led = idleMode.led;
    }

    __OSSetIdleLEDMode(led);
    return NWC24_OK;
}

NWC24Err NWC24EnableLedNotification(BOOL enable) {
    return EnableLedNotification(enable);
}

// newer than the openrvl source: scheduler ioctl 0x22, only used by the shutdown prep when we are the menu
extern NWC24Err NWC24iSetScriptMode(u32 mode);

NWC24Err NWC24iPrepareShutdown(void) {
    NWC24Err result = NWC24_OK;

    NWC24iRegister();

    ShutdownFuncInfo.func = NWC24Shutdown;
    ShutdownFuncInfo.priority = 110;
    OSRegisterShutdownFunction(&ShutdownFuncInfo);

    if (nwc24ShtFd < 0) {
        result = NWC24_OPEN_DEVICE("/dev/net/kd/request", &nwc24ShtFd, 1);
    }

    nwc24ShtRetryRest = SHUTDOWN_RETRY_MAX;
    EnableLedNotification(TRUE);

    if (OSGetAppType() == 0x40) {
        NWC24iSetScriptMode(1);
    }

    return result;
}

static u8 shtBuffer[32] __attribute__((aligned(32)));
static u8 shtResult[32] __attribute__((aligned(32)));

static inline NWC24Err RequestShutdown(u32 event, NWC24Err *resultOut) {
    *(u32 *)shtBuffer = event;
    return NWC24iIoctlResourceManagerAsync("NWC24iRequestShutdown", nwc24ShtFd, NWC24_IOCTL_SHUTDOWN, shtBuffer,
                                           sizeof(shtBuffer), shtResult, sizeof(shtResult), resultOut);
}

NWC24Err NWC24iRequestShutdown(u32 event, NWC24Err *resultOut) {
    return RequestShutdown(event, resultOut);
}

BOOL NWC24Shutdown(BOOL final, u32 event) {
    static BOOL shuttingdown = FALSE;
    static NWC24Err result = NWC24_OK;

    if (final) {
        return TRUE;
    }

    if (shuttingdown) {
        if (NWC24iIsAsyncRequestPending()) {
            return FALSE;
        }

        if (result >= 0) {
            return TRUE;
        }

        if (nwc24ShtRetryRest > 0) {
            shuttingdown = FALSE;
            nwc24ShtRetryRest--;
        } else {
            return TRUE;
        }
    } else if (RequestShutdown(event, &result) >= 0) {
        shuttingdown = TRUE;
    }

    return FALSE;
}
