// nwc24 is one big blob in the dol (config, mail box, download list, time, etc all in one split),
// so everything lives in this one file for now. section banners mark where the original
// translation units seem to start. function order here follows my draft, not the binary.
#include "revolution.h"
#include "revolution/nwc24.h"
#include "revolution/nwc24/NWC24Internal.h"
#include "revolution/sc.h"
#include "revolution/vf.h"
#include <cstring>
#include <cstdarg>

// BYTES: the original is a bunch of separate source files, here they share one TU so mwcc would
// happily inline calls that crossed file boundaries. auto_inline is off for the whole file and the
// helpers that really were inlined inside one original file are marked inline by hand. a few global
// functions were both emitted and inlined into a sibling, those get an inline helper + thin wrapper.
#pragma auto_inline off

/* ---------------------------------------------------------------------------------------------
 * std api (little libc replacements the mail code uses instead of msl)
 * ------------------------------------------------------------------------------------------- */

char *Mail_strcpy(char *dst, const char *src) {
    char *d = dst;

    while (*src != 0) {
        *d++ = *src++;
    }

    *d = 0;
    return dst;
}

int Mail_strlen(const char *str) {
    int n = 0;

    while (*str != 0) {
        n++;
        str++;
    }

    return n;
}

int STD_strnlen(const char *str, size_t maxlen) {
    size_t i;

    for (i = 0; i < maxlen; i++, str++) {
        if (*str == 0) {
            break;
        }
    }

    return i;
}

void *Mail_memcpy(void *dst, const void *src, size_t n) {
    size_t i;

    for (i = 0; i < n; i++) {
        ((u8 *)dst)[i] = ((const u8 *)src)[i];
    }

    return dst;
}

void *Mail_memset(void *dst, int ch, size_t n) {
    size_t i;

    for (i = 0; i < n; i++) {
        ((u8 *)dst)[i] = ch;
    }

    return dst;
}

char *Mail_strcat(char *dst, const char *src) {
    int n = 0;
    const char *p = dst;
    char *d;

    while (*p != 0) {
        p++;
        n++;
    }

    d = dst + n;

    while (*src != 0) {
        *d++ = *src++;
    }

    *d = 0;
    return dst;
}

char *Mail_strncat(char *dst, const char *src, size_t maxlen) {
    int len;
    const char *p;
    char *d;
    size_t i;
    char c;

    len = 0;
    p = dst;
    while (*p != 0) {
        p++;
        len++;
    }

    d = dst + len;
    i = 0;

    while (i < maxlen && (c = *src) != 0) {
        *d++ = c;
        i++;
        src++;
    }

    dst[len + i] = 0;
    return dst;
}

// the little printf the mail code carries around. it only knows %% %c %s %d %u %o %x %X, a width
// (or *) and a '-' / '0' flag, it builds the output string in place by appending/prepending single chars
// HYPOTHESIS: the strlen / "put a char on the end" / "shift everything up and put a char at the front"
// bits were inlined helpers in the original, the unrolled shift loop is mwcc's doing

#pragma auto_inline on

static inline int StrLen(const char *str) {
    int n = 0;

    while (*str != 0) {
        n++;
        str++;
    }

    return n;
}

static inline void StrCat(char *dst, const char *src) {
    int n = 0;
    const char *p = dst;
    char *d;

    while (*p != 0) {
        p++;
        n++;
    }

    d = dst + n;

    while (*src != 0) {
        *d++ = *src++;
    }

    *d = 0;
}

// appends one char to the end of a string
static inline void PutCharBack(char *str, char ch) {
    int len = StrLen(str);
    str[len++] = ch;
    str[len] = 0;
}

// puts one char in front of a string, moving the rest (and the terminator) up by one
static inline void PutCharFront(char *str, char ch) {
    int len = StrLen(str);
    int i;

    for (i = len; i >= 0; i--) {
        str[i + 1] = str[i];
    }

    str[0] = ch;
}

static int fn_80397878(char *buf, u32 value, int base, int hexOffset, int isSigned, int width, char padCh,
                       char flag) {
    int count = 0;
    int written = 0;
    int neg;
    char digit;
    char c;

    if (isSigned && (value & 0x80000000)) {
        value = -value;
        neg = 1;
    } else {
        neg = 0;
    }

    while (value != 0) {
        digit = value % base;
        value /= base;

        PutCharFront(buf, digit > 9 ? digit + hexOffset : digit + '0');
        written++;
        count++;
    }

    if (*buf == 0) {
        PutCharFront(buf, '0');
        written++;
        count++;
    }

    if (padCh != '0') {
        padCh = ' ';
    }

    count += neg;

    while (count < width) {
        if (flag == 'L') {
            PutCharBack(buf, ' ');
            written++;
        } else {
            PutCharFront(buf, padCh);
            written++;
        }
        count++;
    }

    if (neg) {
        PutCharFront(buf, '-');
    }

    return written;
}

int Mail_vsprintf(char *buf, char *fmt, va_list args) {
    int i;
    int total;
    int isNumber;
    int hexOffset;
    int base;
    char padCh;
    int width;
    int isSigned;
    char ch;
    char flag;
    char lenMod;

    *buf = 0;
    total = 0;

    while ((ch = *fmt++) != 0) {
        while (*buf != 0) {
            buf++;
        }

        if (ch == '%') {
            ch = *fmt++;
            if (ch == '%') {
                PutCharBack(buf++, ch);
                total++;
                continue;
            }
            flag = ch;
            if (ch == '-') {
                ch = *fmt++;
            }
            padCh = ch;

            isSigned = 0;
            base = 10;
            hexOffset = '0';
            isNumber = 0;

            if (ch == '*') {
                width = va_arg(args, int);
                ch = *fmt++;
            } else {
                width = 0;
                while (ch >= '0' && ch <= '9') {
                    width = width * 10 + (ch - '0');
                    ch = *fmt++;
                }
            }

            lenMod = ch & 0xDF;
            if (lenMod == 'L') {
                ch = *fmt++;
            }

            switch (ch) {
            case 'd':
                isNumber = 1;
                isSigned = 1;
                break;
            case 'o':
                isNumber = 1;
                base = 8;
                break;
            case 'u':
                isNumber = 1;
                break;
            case 'x':
                isNumber = 1;
                base = 16;
                hexOffset = 'a' - 10;
                break;
            case 'X':
                isNumber = 1;
                base = 16;
                hexOffset = 'A' - 10;
                break;
            case 'c':
                PutCharBack(buf++, va_arg(args, int));
                total++;
                break;
            case 's': {
                char *str = va_arg(args, char *);
                int len;
                char left;

                if (str != NULL) {
                    len = StrLen(str);
                    StrCat(buf, str);
                } else {
                    len = 0;
                }

                total += len;
                left = flag;
                for (i = len; i < width; i++) {
                    total++;
                    if (left == '-') {
                        PutCharBack(buf, ' ');
                    } else {
                        PutCharFront(buf, ' ');
                    }
                }
                break;
            }
            }

            if (isNumber) {
                u32 value;

                if (lenMod == 'L') {
                    value = va_arg(args, u32);
                } else if (isSigned) {
                    value = va_arg(args, s32);
                } else {
                    value = va_arg(args, u32);
                }

                total += fn_80397878(buf, value, base, hexOffset, isSigned, width, padCh, flag);
            }
        } else {
            PutCharBack(buf++, ch);
            total++;
        }
    }

    return total;
}

int Mail_sprintf(char *buf, char *fmt, ...) {
    va_list args;
    int n;

    va_start(args, fmt);
    n = Mail_vsprintf(buf, fmt, args);
    va_end(args);
    return n;
}

#pragma auto_inline off

/* ---------------------------------------------------------------------------------------------
 * utils
 * ------------------------------------------------------------------------------------------- */

void NWC24Data_Init(NWC24Data *data) {
    data->pData = NULL;
    data->size = 0;
}

void NWC24Data_SetDataP(NWC24Data *data, const void *ptr, u32 size) {
    data->pData = ptr;
    data->size = size;
}

void NWC24Date_Init(NWC24Date *date) {
    date->year = 2000;
    date->month = 1;
    date->day = 1;
    date->hour = 12;
    date->min = 0;
    date->sec = 0;
    date->BYTE_0x7 = 0;
}

// writes addr as 16 decimal digits (despite the name), zero padded, plus a terminator
void NWC24iConvIdToStr(u64 addr, char *out) {
    u64 a = addr;
    int i;
    const int n = sizeof(u64) * 2;

    for (i = n - 1; i >= 0; i--) {
        out[i] = '0' + a % 10;
        a /= 10;
    }

    out[n] = '\0';
}

/* ---------------------------------------------------------------------------------------------
 * ipc helpers (/dev/net/kd/* resource managers)
 * ------------------------------------------------------------------------------------------- */

static BOOL NWC24iIsRequestPending = FALSE;

static s32 CallbackAsyncIpc(s32 result, void *arg);

NWC24Err NWC24iOpenResourceManager(const char *user, const char *name, s32 *fdOut, u32 mode) {
#pragma unused(user)
    NWC24Err result;

    if (fdOut == NULL) {
        return NWC24_ERR_INVALID_VALUE;
    }

    result = IOS_Open(name, mode);
    *fdOut = result;

    if (result < 0) {
        if (result == -6) {
            return NWC24_ERR_INPROGRESS;
        }

        if (result == -8) {
            return NWC24_ERR_BUSY;
        }

        return NWC24_ERR_INTERNAL_IPC;
    }

    return NWC24_OK;
}

NWC24Err NWC24iCloseResourceManager(const char *user, s32 fd) {
#pragma unused(user)
    return IOS_Close(fd) < 0 ? NWC24_ERR_INTERNAL_IPC : NWC24_OK;
}

NWC24Err NWC24iIoctlResourceManager(const char *user, s32 fd, s32 type, void *in, s32 inSize, void *out, s32 outSize) {
#pragma unused(user)
    return IOS_Ioctl(fd, type, in, inSize, out, outSize) < 0 ? NWC24_ERR_INTERNAL_IPC : NWC24_OK;
}

NWC24Err NWC24iIoctlResourceManagerAsync(const char *user, s32 fd, s32 type, void *in, s32 inSize, void *out,
                                         s32 outSize, void *callbackArg) {
#pragma unused(user)
    if (IOS_IoctlAsync(fd, type, in, inSize, out, outSize, CallbackAsyncIpc, callbackArg) < 0) {
        return NWC24_ERR_INTERNAL_IPC;
    }

    NWC24iIsRequestPending = TRUE;
    return NWC24_OK;
}

BOOL NWC24iIsAsyncRequestPending(void) {
    return NWC24iIsRequestPending;
}

static s32 CallbackAsyncIpc(s32 result, void *arg) {
    if (arg != NULL) {
        *(s32 *)arg = result;
    }

    NWC24iIsRequestPending = FALSE;
    return 0;
}

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

/* ---------------------------------------------------------------------------------------------
 * config (/shared2/wc24/nwc24msg.cfg)
 * ------------------------------------------------------------------------------------------- */

#define CONFIG_MAGIC 'WcCf'
#define CONFIG_VERSION 8
#define CONFIG_MAX_ID_COUNT 32
#define COMPANY_CODE_NINTENDO 1

// HYPOTHESIS: 0x800031C0 is a low-mem cache of the wc24 user id (8 bytes, flushed with DCStoreRange)
#define WC24_USERID_CACHE ((NWC24UserId *)0x800031C0)

static NWC24iConfig *config = NULL;
static BOOL ConfigModified = FALSE;

static const char *MBoxDir = "/shared2/wc24/mbox";
static const char *ConfigFile = "/shared2/wc24/nwc24msg.cfg";
static const char *CfgBakFile = "/shared2/wc24/nwc24msg.cbk";
// HYPOTHESIS: default group id, the sdk reads it from sdata instead of using a literal
static u16 DefaultGroupId = COMPANY_CODE_NINTENDO;

static u32 GetConfigCheckSum(void);
static NWC24Err CheckConfig(void);
static inline NWC24Err GenerateUserId(NWC24UserId *idOut);
static inline NWC24Err GenerateNewUserId_i(NWC24UserId *idOut);

// HYPOTHESIS: checks the crc on a user id (returns 0 when good). body not drafted.
extern NWC24Err NWC24iCheckUserIdCrc(NWC24UserId id);

NWC24Err NWC24GetMyUserId(NWC24UserId *idOut) {
    NWC24Err result = NWC24_OK;
    NWC24Err scdErr;

    if (NWC24IsMsgLibOpened() || NWC24IsMsgLibOpenedByTool()) {
        *idOut = config->userId;
    } else {
        // not open, so try the cached copy first
        *idOut = *WC24_USERID_CACHE;
        if (*idOut == 0) {
            result = NWC24_ERR_NULL;
        } else {
            result = NWC24iCheckUserIdCrc(*idOut);
        }

        if (result == NWC24_OK) {
            return result;
        }

        scdErr = NWC24SuspendScheduler();
        if (scdErr < 0) {
            return scdErr;
        }

        result = GenerateNewUserId_i(idOut);
        if (result == NWC24_ERR_ID_GENERATED || result == NWC24_ERR_ID_REGISTERED) {
            result = NWC24_OK;
        }

        scdErr = NWC24ResumeScheduler();
        if (scdErr < 0) {
            result = result != NWC24_OK ? result : scdErr;
        }
    }

    return result;
}

NWC24Err NWC24GenerateNewUserId(NWC24UserId *idOut) {
    return GenerateNewUserId_i(idOut);
}

NWC24Err NWC24iConfigOpen(void) {
    NWC24Err result;

    config = (NWC24iConfig *)NWC24WorkP->config;
    ConfigModified = FALSE;

    result = NWC24iConfigReload();
    if (result != NWC24_ERR_FILE_NOEXISTS) {
        if (result == NWC24_OK) {
            *WC24_USERID_CACHE = config->userId;
            DCStoreRange(WC24_USERID_CACHE, 0x20);
        }
    }

    return result;
}

NWC24Err NWC24iConfigReload(void) {
    NWC24Err result;
    NWC24Err close;
    NWC24File file;

    if (NWC24WorkP == NULL) {
        return NWC24_ERR_LIB_NOT_OPENED;
    }

    result = NWC24FOpen(&file, ConfigFile, NWC24_OPEN_NAND_R);

    if (result == NWC24_OK) {
        result = NWC24FRead(config, sizeof(NWC24iConfig), &file);
        close = NWC24FClose(&file);
        result = result != NWC24_OK ? result : close;
    }

    if (result == NWC24_OK) {
        result = CheckConfig();

        if (result == NWC24_OK) {
            ConfigModified = FALSE;
            return result;
        }
    }

    // main copy was bad, fall back to the backup
    result = NWC24FOpen(&file, CfgBakFile, NWC24_OPEN_NAND_R);

    if (result == NWC24_OK) {
        result = NWC24FRead(config, sizeof(NWC24iConfig), &file);
        close = NWC24FClose(&file);
        result = result != NWC24_OK ? result : close;
    }

    if (result == NWC24_OK) {
        result = CheckConfig();

        if (result == NWC24_OK) {
            ConfigModified = TRUE;
        }
    }

    return result;
}

NWC24Err NWC24iConfigFlush(void) {
    NWC24Err result;
    NWC24Err close;
    NWC24File file;

    if (NWC24WorkP == NULL) {
        return NWC24_ERR_LIB_NOT_OPENED;
    }

    if (!ConfigModified) {
        return NWC24_OK;
    }

    config->checksum = GetConfigCheckSum();

    result = NWC24FOpen(&file, ConfigFile, NWC24_OPEN_NAND_W);

    if (result == NWC24_OK) {
        result = NWC24FWrite(config, sizeof(NWC24iConfig), &file);
        close = NWC24FClose(&file);
        result = result != NWC24_OK ? result : close;

        ConfigModified = FALSE;
    }

    if (result != NWC24_OK) {
        return result;
    }

    result = NWC24FOpen(&file, CfgBakFile, NWC24_OPEN_NAND_W);

    if (result == NWC24_OK) {
        result = NWC24FWrite(config, sizeof(NWC24iConfig), &file);
        close = NWC24FClose(&file);
        result = result != NWC24_OK ? result : close;

        ConfigModified = FALSE;
    }

    return result;
}

const char *NWC24GetAccountDomain(void) {
    return config->acctDomain;
}

const char *NWC24GetMBoxDir(void) {
    OSIOSRev rev;
    __OSGetIOSRev(&rev);

    // HYPOTHESIS: old IOS (< 0.7.6 or so) can't do /shared2, so the sdk just gives up
    if (rev.major == 0 && ((rev.minor == 7 && rev.micro == 0) || rev.minor < 7)) {
        OSPanic("NWC24Config.c", 695, "stopped.");
    }

    return MBoxDir;
}

u32 NWC24GetAppId(void) {
    u32 appId = *(u32 *)OSGetAppGamename();

    if (appId == 0 && OSGetAppType() == 0) {
        appId = *(u32 *)DVDGetCurrentDiskID()->gameName;
    }

    return appId;
}

u16 NWC24GetGroupId(void) {
    NANDStatus stat;
    u16 groupId;
    char *dir;

    groupId = DefaultGroupId;

    switch (OSGetAppType()) {
    case 0x40:
        groupId = COMPANY_CODE_NINTENDO;
        break;
    case 0x80:
        groupId = *(u16 *)DVDGetCurrentDiskID()->company;
        break;
    case 0x81:
        dir = NWC24WorkP->pathWork;
        if (NANDGetHomeDir(dir) == NAND_RESULT_OK) {
            if (NANDGetStatus(dir, &stat) == NAND_RESULT_OK) {
                groupId = stat.groupId;
            }
        }
        break;
    }

    return groupId;
}

static inline u32 GetConfigCheckSum(void) {
    u32 i;
    u32 csum;
    u32 *ptr;

    csum = 0;
    ptr = (u32 *)config;

    // last word is the checksum itself, so skip it
    for (i = 0; i < sizeof(NWC24iConfig) / sizeof(u32) - 1; i++) {
        csum += *ptr++;
    }

    return csum;
}

static NWC24Err CheckConfig(void) {
    if (config->magic != CONFIG_MAGIC) {
        return NWC24_ERR_BROKEN;
    }

    if (config->checksum != GetConfigCheckSum()) {
        return NWC24_ERR_BROKEN;
    }

    if (config->createCount >= CONFIG_MAX_ID_COUNT) {
        return NWC24_ERR_BROKEN;
    }

    if (config->version != CONFIG_VERSION) {
        return NWC24_ERR_VER_MISMATCH;
    }

    return NWC24_OK;
}

static inline NWC24Err GenerateUserId(NWC24UserId *idOut) {
    NWC24Err result;
    u32 sp08 = 0;

    if (idOut == NULL) {
        return NWC24_ERR_INVALID_VALUE;
    }

    *idOut = 9999999999999999ULL;
    result = NWC24iRequestGenerateUserId(idOut, &sp08);

    // keep the low-mem copy in sync with whatever the scheduler handed back
    *WC24_USERID_CACHE = *idOut;
    DCStoreRange(WC24_USERID_CACHE, 0x20);
    return result;
}

static inline NWC24Err GenerateNewUserId_i(NWC24UserId *idOut) {
    NWC24Err open;
    NWC24Err close;

    open = NWC24BlockOpenMsgLib(TRUE);
    if (open < 0) {
        return open;
    }

    open = GenerateUserId(idOut);
    close = NWC24BlockOpenMsgLib(FALSE);

    return open >= 0 ? close : open;
}

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

// HYPOTHESIS: a 128 byte scratch buffer that nothing in this unit touches. it is only here
// because the .bss layout wants it (some string/time helper in the original got dropped).
static u8 nwc24TimeUnusedBuf[128];
#pragma push
#pragma force_active on
static void *nwc24TimeUnusedRef = nwc24TimeUnusedBuf;
#pragma pop

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

s32 NWC24GetErrorCode(void) {
    return GlobalErrorCode;
}

static inline void SetErrorCode_i(u32 code) {
    GlobalErrorCode = code;
}

void NWC24iSetErrorCode(u32 code) {
    SetErrorCode_i(code);
}

/* ---------------------------------------------------------------------------------------------
 * friend lists
 * ------------------------------------------------------------------------------------------- */

#define FRIEND_LIST_MAGIC 'WcFl'
#define FRIEND_LIST_VERSION 2

static const char *FLFilePath = "/shared2/wc24/nwc24fl.bin";

static NWC24Err GetCachedFLHeader(NWC24iFLHeader **header);

NWC24Err NWC24iOpenFriendList(void) {
    NWC24iFLHeader *header = (NWC24iFLHeader *)NWC24WorkP->flHeader;
    Mail_memset(header, 0, sizeof(NWC24iFLHeader));
    return GetCachedFLHeader(&header);
}

static NWC24Err GetCachedFLHeader(NWC24iFLHeader **header) {
    NWC24File file;
    NWC24Err result;
    NWC24Err read;
    NWC24Err close;

    *header = (NWC24iFLHeader *)NWC24WorkP->flHeader;

    if ((*header)->magic != FRIEND_LIST_MAGIC) {
        result = NWC24FOpen(&file, FLFilePath, NWC24_OPEN_NAND_R);
        if (result != NWC24_OK) {
            return result;
        }

        NWC24FSeek(&file, 0, NWC24_SEEK_BEG);
        read = NWC24FRead(*header, sizeof(NWC24iFLHeader), &file);
        close = NWC24FClose(&file);

        if (read != NWC24_OK) {
            result = read;
        } else {
            result = close;
        }

        if (result != NWC24_OK) {
            return result;
        }

        if ((*header)->magic != FRIEND_LIST_MAGIC) {
            return NWC24_ERR_BROKEN;
        }

        if ((*header)->version != FRIEND_LIST_VERSION) {
            return NWC24_ERR_VER_MISMATCH;
        }
    }

    return NWC24_OK;
}

#define SECRET_FRIEND_LIST_MAGIC 'WcFs'
#define SECRET_FRIEND_LIST_VERSION 2

static const char *SFLFilePath = "/shared2/wc24/nwc24fls.bin";

static NWC24Err GetCachedSecretFLHeader(NWC24iSecretFLHeader **header);

NWC24Err NWC24iOpenSecretFriendList(void) {
    NWC24iSecretFLHeader *header = (NWC24iSecretFLHeader *)NWC24WorkP->secretFlHeader;
    Mail_memset(header, 0, sizeof(NWC24iSecretFLHeader));
    return GetCachedSecretFLHeader(&header);
}

static NWC24Err GetCachedSecretFLHeader(NWC24iSecretFLHeader **header) {
    NWC24File file;
    NWC24Err result;
    NWC24Err read;
    NWC24Err close;

    *header = (NWC24iSecretFLHeader *)NWC24WorkP->secretFlHeader;

    if ((*header)->magic != SECRET_FRIEND_LIST_MAGIC) {
        result = NWC24FOpen(&file, SFLFilePath, NWC24_OPEN_NAND_R);
        if (result != NWC24_OK) {
            return result;
        }

        NWC24FSeek(&file, 0, NWC24_SEEK_BEG);
        read = NWC24FRead(*header, sizeof(NWC24iSecretFLHeader), &file);
        close = NWC24FClose(&file);

        if (read != NWC24_OK) {
            result = read;
        } else {
            result = close;
        }

        if (result != NWC24_OK) {
            return result;
        }

        if ((*header)->magic != SECRET_FRIEND_LIST_MAGIC) {
            return NWC24_ERR_BROKEN;
        }

        if ((*header)->version != SECRET_FRIEND_LIST_VERSION) {
            return NWC24_ERR_VER_MISMATCH;
        }
    }

    return NWC24_OK;
}

/* ---------------------------------------------------------------------------------------------
 * file api (nand + vf wrappers, buffered read/write)
 * ------------------------------------------------------------------------------------------- */

#define WC24_DRIVE "@24"
#define NAND_RETRY_COUNT 3
#define NWC24_WR_BUFFER_SIZE 2048

// MSB gets set after a buffered read so the next read needs a fresh seek first
#define INVALIDATE_ALIGN 0x80000000

#define SLEEP_MSEC(x) OSSleepTicks(OSMillisecondsToTicks((s64)(x)))

extern s32 NANDSeek(NANDFileInfo *info, s32 offset, s32 whence);

static u32 RdBufferMutex = 0;
static u32 WrBufferMutex = 0;
static u32 InstanceIdGen = 0;

static inline NWC24Err CloseVf(NWC24File *file);
static NWC24Err BufferedWrite(const void *src, s32 size, NWC24File *file);
static NWC24Err BufferedWriteFlush(NWC24File *file);
static NWC24Err BufferedRead(void *dst, s32 size, NWC24File *file);
static NWC24Err AlignedSeek(NWC24File *file, s32 offset, NWC24SeekMode whence);
static NWC24Err ConvertError(s32 nanderr, NWC24Err wc24err);
static NWC24Err ConvertVfError(s32 vferr, NWC24Err wc24err);

NWC24Err NWC24FOpen(NWC24File *file, const char *path, u32 mode) {
    InstanceIdGen++;
    InstanceIdGen |= 0x80000000;

    file->id = InstanceIdGen;
    file->align = 0;
    file->mode = mode;

    if (mode == NWC24_OPEN_NAND_WBUFF || mode == NWC24_OPEN_NAND_RBUFF || mode == NWC24_OPEN_VF_WBUFF ||
        mode == NWC24_OPEN_VF_RBUFF) {
        if (!NWC24IsMsgLibOpened() && !NWC24IsMsgLibOpenedByTool()) {
            return NWC24_ERR_LIB_NOT_OPENED;
        }
    }

    if (mode == NWC24_OPEN_NAND_WBUFF || mode == NWC24_OPEN_VF_WBUFF) {
        if (WrBufferMutex != 0) {
            return NWC24_ERR_MUTEX;
        }

        WrBufferMutex = file->id;
    }

    if (mode & NWC24_OPEN_VF) {
        return NWC24iFOpenVF(file, path, mode);
    }

    return NWC24iFOpenNand(file, path, mode);
}

NWC24Err NWC24iFOpenNand(NWC24File *file, const char *path, u32 mode) {
    s32 result;
    NANDAccessType access;
    u32 i;

    switch (mode) {
    case NWC24_OPEN_NAND_W:
    case NWC24_OPEN_NAND_WBUFF:
        result = NANDPrivateCreate(path, NAND_PERM_RWALL, 0);
        if (result != NAND_RESULT_OK && result != NAND_RESULT_EXISTS) {
            return NWC24_ERR_FILE_OTHER;
        }
        access = NAND_ACCESS_WRITE;
        break;
    case NWC24_OPEN_NAND_R:
    case NWC24_OPEN_NAND_RBUFF:
        access = NAND_ACCESS_READ;
        break;
    case NWC24_OPEN_NAND_RW:
        access = NAND_ACCESS_RW;
        break;
    default:
        return NWC24_ERR_INVALID_VALUE;
    }

    for (i = 0; i < NAND_RETRY_COUNT; i++) {
        result = NANDPrivateOpen(path, &file->nandf, access);
        if (!(result == NAND_RESULT_BUSY || result == NAND_RESULT_ALLOC_FAILED)) {
            break;
        }

        SLEEP_MSEC(1000);
    }

    if (result != NAND_RESULT_OK) {
        if (mode == NWC24_OPEN_NAND_WBUFF) {
            WrBufferMutex = 0;
        }

        if (result == NAND_RESULT_NOEXISTS) {
            return NWC24_ERR_FILE_NOEXISTS;
        }

        return NWC24_ERR_FILE_OPEN;
    }

    return NWC24_OK;
}

NWC24Err NWC24iFOpenVF(NWC24File *file, const char *path, u32 mode) {
    s32 result;
    const char *access;

    switch (mode) {
    case NWC24_OPEN_VF_W:
    case NWC24_OPEN_VF_WBUFF:
        access = "w";
        break;
    case NWC24_OPEN_VF_R:
    case NWC24_OPEN_VF_RBUFF:
        access = "r";
        break;
    default:
        return NWC24_ERR_INVALID_VALUE;
    }

    file->vff = VFOpenFile(path, access, 0);

    if (file->vff == NULL) {
        result = VFGetLastError();

        if (mode == NWC24_OPEN_VF_WBUFF) {
            WrBufferMutex = 0;
        }

        if (*access == 'r' && result == VF_ERROR_0002) {
            return NWC24_ERR_FILE_NOEXISTS;
        }

        return ConvertVfError(result, NWC24_ERR_FILE_OPEN);
    }

    return NWC24_OK;
}

NWC24Err NWC24FClose(NWC24File *file) {
    NWC24Err close = NWC24_OK;
    NWC24Err read = NWC24_OK;

    if (file->mode == NWC24_OPEN_NAND_WBUFF || file->mode == NWC24_OPEN_VF_WBUFF) {
        read = BufferedWriteFlush(file);
        WrBufferMutex = 0;
    }

    if (file->mode & NWC24_OPEN_VF) {
        close = CloseVf(file);
    } else {
        close = NWC24iFCloseNand(file);
    }

    return read != NWC24_OK ? read : close;
}

NWC24Err NWC24iFCloseNand(NWC24File *file) {
    s32 result;
    NWC24Err err;
    u32 i;

    err = NWC24_OK;

    for (i = 0; i < NAND_RETRY_COUNT; i++) {
        result = NANDClose(&file->nandf);
        if (!(result == NAND_RESULT_BUSY || result == NAND_RESULT_ALLOC_FAILED)) {
            break;
        }

        SLEEP_MSEC(1000);
    }

    if (result == NAND_RESULT_CORRUPT) {
        err = NWC24_ERR_NAND_CORRUPT;
    } else if (result != NAND_RESULT_OK) {
        err = NWC24_ERR_FILE_CLOSE;
    }

    return err;
}

// the body is also inlined into NWC24FClose (same original file), so it lives in an inline helper
static inline NWC24Err CloseVf(NWC24File *file) {
    s32 result;

    result = VFCloseFile(file->vff);
    if (result != VF_OK) {
        return ConvertVfError(result, NWC24_ERR_FILE_CLOSE);
    }

    return NWC24_OK;
}

NWC24Err NWC24iFCloseVF(NWC24File *file) {
    return CloseVf(file);
}

NWC24Err NWC24FSeek(NWC24File *file, s32 offset, NWC24SeekMode whence) {
    s32 result;
    u32 i;

    if (file->mode == NWC24_OPEN_NAND_RBUFF || file->mode == NWC24_OPEN_VF_RBUFF) {
        return AlignedSeek(file, offset, whence);
    }

    if (file->mode & NWC24_OPEN_VF) {
        result = VFSeekFile(file->vff, offset, whence);
        if (result != VF_OK) {
            return ConvertVfError(result, NWC24_ERR_FILE_OTHER);
        }

        return NWC24_OK;
    }

    for (i = 0; i < NAND_RETRY_COUNT; i++) {
        result = NANDSeek(&file->nandf, offset, whence);
        if (!(result == NAND_RESULT_BUSY || result == NAND_RESULT_ALLOC_FAILED)) {
            break;
        }

        SLEEP_MSEC(1000);
    }

    if (result < 0) {
        return NWC24_ERR_FILE_OTHER;
    }

    return NWC24_OK;
}

NWC24Err NWC24FRead(void *dst, s32 size, NWC24File *file) {
    s32 result;
    u32 i;

    switch (file->mode) {
    case NWC24_OPEN_NAND_W:
    case NWC24_OPEN_NAND_WBUFF:
    case NWC24_OPEN_VF_W:
        return NWC24_ERR_PROTECTED;
    case NWC24_OPEN_NAND_RBUFF:
    case NWC24_OPEN_VF_RBUFF:
        return BufferedRead(dst, size, file);
    }

    if (file->mode & NWC24_OPEN_VF) {
        result = VFReadFile(file->vff, dst, size, NULL);
        if (result != VF_OK) {
            return ConvertVfError(result, NWC24_ERR_FILE_READ);
        }

        return NWC24_OK;
    }

    for (i = 0; i < NAND_RETRY_COUNT; i++) {
        result = NANDRead(&file->nandf, dst, size);
        if (!(result == NAND_RESULT_BUSY || result == NAND_RESULT_ALLOC_FAILED)) {
            break;
        }

        SLEEP_MSEC(1000);
    }

    if (result == NAND_RESULT_CORRUPT) {
        return NWC24_ERR_NAND_CORRUPT;
    }

    if (result < 0) {
        return ConvertError(result, NWC24_ERR_FILE_READ);
    }

    return NWC24_OK;
}

NWC24Err NWC24FWrite(const void *src, s32 size, NWC24File *file) {
    s32 result;
    u32 i;

    switch (file->mode) {
    case NWC24_OPEN_NAND_WBUFF:
    case NWC24_OPEN_VF_WBUFF:
        return BufferedWrite(src, size, file);
    // HYPOTHESIS: NWC24_OPEN_VF_RBUFF is missing from this list on purpose-ish (looks like an sdk oversight)
    case NWC24_OPEN_NAND_R:
    case NWC24_OPEN_NAND_RBUFF:
    case NWC24_OPEN_VF_R:
        return NWC24_ERR_PROTECTED;
    }

    if (file->mode & NWC24_OPEN_VF) {
        result = VFWriteFile(file->vff, (void *)src, size);
        if (result != VF_OK) {
            return ConvertVfError(result, NWC24_ERR_FILE_WRITE);
        }

        return NWC24_OK;
    }

    for (i = 0; i < NAND_RETRY_COUNT; i++) {
        result = NANDWrite(&file->nandf, src, size);
        if (!(result == NAND_RESULT_BUSY || result == NAND_RESULT_ALLOC_FAILED)) {
            break;
        }

        SLEEP_MSEC(1000);
    }

    if (result == NAND_RESULT_CORRUPT) {
        return NWC24_ERR_NAND_CORRUPT;
    }

    if (result < 0) {
        return ConvertError(result, NWC24_ERR_FILE_WRITE);
    }

    if (result != size) {
        return NWC24_ERR_FILE_WRITE;
    }

    return NWC24_OK;
}

NWC24Err NWC24FGetLength(NWC24File *file, u32 *lengthOut) {
    s32 error;

    if (file->mode & NWC24_OPEN_VF) {
        error = VFGetFileSizeByFd(file->vff);
        if (error < 0) {
            return ConvertVfError(error, NWC24_ERR_FILE_OTHER);
        }

        *lengthOut = error;
        return NWC24_OK;
    }

    error = NANDGetLength(&file->nandf, lengthOut);
    if (error != NAND_RESULT_OK) {
        return NWC24_ERR_FILE_OTHER;
    }

    return NWC24_OK;
}

NWC24Err NWC24FDeleteVF(const char *path) {
    s32 result;

    result = VFDeleteFile(path);
    if (result != VF_OK) {
        return ConvertVfError(result, NWC24_ERR_FILE_OTHER);
    }

    return NWC24_OK;
}

NWC24Err NWC24MountVF(const char *drive, const char *filename) {
    s32 result;

    result = VFMountDriveNANDFlashEx(drive, filename);
    if (result == VF_ERROR_B001) {
        return NWC24_ERR_FILE_NOEXISTS;
    }

    if (result != VF_OK) {
        return ConvertVfError(result, NWC24_ERR_INTERNAL_VF);
    }

    result = VFSetSyncMode(drive, 1);
    if (result != VF_OK) {
        return ConvertVfError(result, NWC24_ERR_INTERNAL_VF);
    }

    return NWC24_OK;
}

NWC24Err NWC24UnmountVF(const char *drive) {
    s32 result;

    result = VFUnmountDrive(drive);
    if (result != VF_OK) {
        return ConvertVfError(result, NWC24_ERR_INTERNAL_VF);
    }

    return NWC24_OK;
}

NWC24Err NWC24CheckSizeVF(const char *drive, u32 *sizeOut) {
    s32 size;

    size = VFGetDriveFreeSize(drive);
    *sizeOut = size;

    if (size < 0) {
        return ConvertVfError(VFGetLastError(), NWC24_ERR_INTERNAL_VF);
    }

    return NWC24_OK;
}

static NWC24Err BufferedWrite(const void *src, s32 size, NWC24File *file) {
    u32 now;
    u32 left;
    u32 pos;
    u8 *buf;
    const u8 *bsrc;
    s32 result;
    NWC24Err err;
    u32 i;

    left = size;
    pos = file->align;
    buf = NWC24WorkP->writeBuffer;
    bsrc = (u8 *)src;
    err = NWC24_OK;

    while (left != 0) {
        now = NWC24_WR_BUFFER_SIZE - pos;

        if (left < now) {
            now = left;
        }

        memcpy(buf + pos, bsrc, now);

        pos += now;
        left -= now;
        bsrc += now;

        if (pos < NWC24_WR_BUFFER_SIZE) {
            continue;
        }

        pos = 0;

        if (file->mode & NWC24_OPEN_VF) {
            result = VFWriteFile(file->vff, buf, NWC24_WR_BUFFER_SIZE);
            if (result == VF_OK) {
                continue;
            }

            err = ConvertVfError(result, NWC24_ERR_FILE_WRITE);
            break;
        }

        for (i = 0; i < NAND_RETRY_COUNT; i++) {
            result = NANDWrite(&file->nandf, buf, NWC24_WR_BUFFER_SIZE);
            if (!(result == NAND_RESULT_BUSY || result == NAND_RESULT_ALLOC_FAILED)) {
                break;
            }

            SLEEP_MSEC(1000);
        }

        if (result == NAND_RESULT_CORRUPT) {
            return NWC24_ERR_NAND_CORRUPT;
        }

        if (result != NWC24_WR_BUFFER_SIZE) {
            err = NWC24_ERR_FILE_WRITE;
            break;
        }
    }

    file->align = pos;
    return err;
}

static NWC24Err BufferedWriteFlush(NWC24File *file) {
    s32 result;
    u32 pos;
    u8 *buf;
    NWC24Err err;
    u32 i;

    pos = file->align;
    buf = NWC24WorkP->writeBuffer;
    err = NWC24_OK;

    if (pos == 0) {
        return err;
    }

    while (pos % 32 != 0) {
        buf[pos++] = 0x00;
    }

    if (file->mode & NWC24_OPEN_VF) {
        result = VFWriteFile(file->vff, buf, pos);
        if (result != VF_OK) {
            err = ConvertVfError(result, NWC24_ERR_FILE_WRITE);
        }

        return err;
    }

    for (i = 0; i < NAND_RETRY_COUNT; i++) {
        result = NANDWrite(&file->nandf, buf, pos);
        if (!(result == NAND_RESULT_BUSY || result == NAND_RESULT_ALLOC_FAILED)) {
            break;
        }

        SLEEP_MSEC(1000);
    }

    if (result == NAND_RESULT_CORRUPT) {
        return NWC24_ERR_NAND_CORRUPT;
    }

    if (result != pos) {
        err = NWC24_ERR_FILE_WRITE;
    }

    return err;
}

static NWC24Err BufferedRead(void *dst, s32 size, NWC24File *file) {
    u32 total;
    u32 now;
    u32 left;
    s32 result;
    u32 pos;
    u8 *buf;
    u8 *bdst;
    NWC24Err err;
    u32 i;
    u32 bytesread;

    bdst = (u8 *)dst;
    total = size;
    buf = NWC24WorkP->readBuffer;
    err = NWC24_OK;

    if (RdBufferMutex != 0) {
        return NWC24_ERR_MUTEX;
    }

    RdBufferMutex = file->id;

    if (file->align & INVALIDATE_ALIGN) {
        return NWC24_ERR_INVALID_OPERATION;
    }

    pos = file->align;

    while (total > 0) {
        now = NWC24i_IO_BUFFER_SIZE;
        left = now - pos;

        if (total < left) {
            left = total;
            now = (total + pos + 31) & ~31;
        }

        if (file->mode & NWC24_OPEN_VF) {
            result = VFReadFile(file->vff, buf, now, &bytesread);
            if (result != VF_OK) {
                err = ConvertVfError(result, NWC24_ERR_FILE_READ);
                break;
            }

            result = bytesread;
        } else {
            for (i = 0; i < NAND_RETRY_COUNT; i++) {
                result = NANDRead(&file->nandf, buf, now);
                if (!(result == NAND_RESULT_BUSY || result == NAND_RESULT_ALLOC_FAILED)) {
                    break;
                }

                SLEEP_MSEC(1000);
            }

            if (result == NAND_RESULT_CORRUPT) {
                return NWC24_ERR_NAND_CORRUPT;
            }
        }

        if (result != now) {
            err = NWC24_ERR_FILE_READ;
            break;
        } else {
            memcpy(bdst, buf + pos, left);
            bdst += left;
            total -= left;
            pos = 0;
        }
    }

    file->align = (file->align + size) % 32;
    file->align |= INVALIDATE_ALIGN;
    RdBufferMutex = 0;
    return err;
}

static inline NWC24Err AlignedSeek(NWC24File *file, s32 offset, NWC24SeekMode whence) {
    s32 result;
    s32 alignofs;

    file->align = offset % 32;
    alignofs = offset - file->align;

    if (file->mode & NWC24_OPEN_VF) {
        result = VFSeekFile(file->vff, alignofs, whence);
        if (result != VF_OK) {
            return ConvertVfError(result, NWC24_ERR_FILE_OTHER);
        }

        return NWC24_OK;
    }

    result = NANDSeek(&file->nandf, alignofs, whence);
    if (result < 0) {
        return NWC24_ERR_FILE_OTHER;
    }

    return NWC24_OK;
}

static inline NWC24Err ConvertError(s32 nanderr, NWC24Err wc24err) {
    switch (nanderr) {
    case NAND_RESULT_ECC_CRIT:
    case NAND_RESULT_AUTHENTICATION:
        return NWC24_ERR_FILE_BROKEN;
    case NAND_RESULT_CORRUPT:
        return NWC24_ERR_NAND_CORRUPT;
    default:
        return wc24err;
    }
}

static inline NWC24Err ConvertVfError(s32 vferr, NWC24Err wc24err) {
    if (vferr == VF_ERROR_0005) {
        return ConvertError(VFGetLastDeviceError(WC24_DRIVE), wc24err);
    }

    return wc24err;
}

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
static u8 nwc24ScdStatBuf[256] __attribute__((aligned(32)));

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

// HYPOTHESIS: plain "no parameter" scheduler commands 6..9. only the socket library (so.c) calls
// them, around its startup/cleanup, so they are probably "tell kd the network is going up/down".
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

/* ---------------------------------------------------------------------------------------------
 * mime / charset / encoding lookup tables
 * ------------------------------------------------------------------------------------------- */

const char *MIMETypeStr[] = {"application/octet-stream", "application/x-wii-minidata", "application/x-wii-msgboard",
                             "image/jpeg",               "image/x-wii-picture",        "multipart/alternative",
                             "multipart/mixed",          "multipart/related",          "text/html",
                             "text/plain"};
NWC24MIMEType MIMETypeConv[] = {NWC24_APP_OCTET_STREAM, NWC24_APP_WII_MINIDATA, NWC24_APP_WII_MSGBOARD,
                                NWC24_IMG_JPEG,         NWC24_IMG_WII_PICTURE,  NWC24_MUL_ALTERNATIVE,
                                NWC24_MUL_MIXED,        NWC24_MUL_RELATED,      NWC24_TXT_HTML,
                                NWC24_TXT_PLAIN};
const char *MIMETypeSuffix[] = {"dat", "wii", "wii", "jpg", "wii", "???", "???", "???", "htm", "txt"};

const char *CharsetStr[] = {"euc-jp",      "iso-2022-jp", "iso-8859-1", "iso-8859-10", "iso-8859-15", "iso-8859-2",
                            "iso-8859-3",  "iso-8859-5",  "iso-8859-7", "iso-8859-9",  "shift_jis",   "us-ascii",
                            "utf-16be",    "utf-32be",    "utf-8",      "windows-1252"};
NWC24Charset CharsetConv[] = {NWC24_EUC_JP,      NWC24_ISO_2022_JP, NWC24_ISO_8859_1, NWC24_ISO_8859_10,
                              NWC24_ISO_8859_15, NWC24_ISO_8859_2,  NWC24_ISO_8859_3, NWC24_ISO_8859_5,
                              NWC24_ISO_8859_7,  NWC24_ISO_8859_9,  NWC24_SHIFT_JIS,  NWC24_US_ASCII,
                              NWC24_UTF_16BE,    NWC24_UTF_32BE,    NWC24_UTF_8,      NWC24_WINDOWS_1252};

const char *EncodingStr[] = {"7bit", "8bit", "base64", "quoted-printable"};
NWC24Encoding EncodingConv[] = {NWC24_ENC_7BIT, NWC24_ENC_8BIT, NWC24_ENC_BASE64, NWC24_ENC_QUOTED_PRINTABLE};

#define LENGTHOF(x) (sizeof(x) / sizeof((x)[0]))

const char *NWC24GetMIMETypeStr(NWC24MIMEType type) {
    int i;

    for (i = 0; i < LENGTHOF(MIMETypeConv); i++) {
        if (type == MIMETypeConv[i]) {
            return MIMETypeStr[i];
        }
    }

    return NULL;
}

const char *NWC24iGetMIMETypeSuffix(NWC24MIMEType type) {
    int i;

    for (i = 0; i < LENGTHOF(MIMETypeConv); i++) {
        if (type == MIMETypeConv[i]) {
            return MIMETypeSuffix[i];
        }
    }

    return NULL;
}

const char *NWC24GetCharsetStr(NWC24Charset set) {
    int i;

    for (i = 0; i < LENGTHOF(CharsetConv); i++) {
        if (set == CharsetConv[i]) {
            return CharsetStr[i];
        }
    }

    return NULL;
}

const char *NWC24GetEncodingStr(NWC24Encoding enc) {
    int i;

    for (i = 0; i < LENGTHOF(EncodingConv); i++) {
        if (enc == EncodingConv[i]) {
            return EncodingStr[i];
        }
    }

    return NULL;
}

/* ---------------------------------------------------------------------------------------------
 * message objects
 * ------------------------------------------------------------------------------------------- */

#define YEAR_MIN 2000
#define YEAR_MAX 2035
#define ALT_NAME_MAX 35
#define SMTP_LINE_MAX 1000

NWC24Err NWC24InitMsgObj(NWC24MsgObj *pMsg, NWC24MsgType type) {
    u32 i;
    NWC24iMsgObj *msg = (NWC24iMsgObj *)pMsg;

    Mail_memset(msg, 0, sizeof(NWC24iMsgObj));

    msg->id = 0;
    msg->flags = NWC24_MSGOBJ_INITIALIZED;
    msg->appId = NWC24GetAppId();
    msg->command = 0;
    msg->tag = 0;
    msg->groupId = NWC24GetGroupId();

    switch (type) {
    case NWC24_MSGTYPE_RVL_MENU_SHARED:
        msg->flags |= NWC24_MSGOBJ_FOR_RECIPIENT | NWC24_MSGOBJ_FOR_APP | NWC24_MSGOBJ_FOR_MENU;
        break;
    case NWC24_MSGTYPE_RVL_APP:
        msg->flags |= NWC24_MSGOBJ_FOR_RECIPIENT | NWC24_MSGOBJ_FOR_APP;
        break;
    case NWC24_MSGTYPE_RVL_MENU:
        msg->flags |= NWC24_MSGOBJ_FOR_RECIPIENT | NWC24_MSGOBJ_FOR_MENU;
        break;
    case NWC24_MSGTYPE_RVL_APP_HIDDEN:
        msg->flags |= NWC24_MSGOBJ_FOR_RECIPIENT;
        break;
    case NWC24_MSGTYPE_PUBLIC:
        msg->flags |= NWC24_MSGOBJ_FOR_PUBLIC;
        break;
    default:
        return NWC24_ERR_INVALID_VALUE;
    }

    msg->createTime = 0;
    msg->flags |= 0x200000; // HYPOTHESIS: newer-sdk flag bit (1 << 21), not in the openrvl source or sibling enum
    msg->unk2C = 0;
    NWC24GetMyUserId(&msg->from.id);
    msg->numTo = 0;

    for (i = 0; i < NWC24i_MSG_RECIPIENT_MAX; i++) {
        if (type == NWC24_MSGTYPE_PUBLIC) {
            NWC24Data_Init(&msg->to[i].data);
        } else {
            msg->to[i].id = 0;
        }
    }

    NWC24Data_Init(&msg->subject);
    NWC24Data_Init(&msg->text);
    msg->charset = NWC24_US_ASCII;
    msg->encoding = NWC24_ENC_7BIT;
    msg->numAttached = 0;

    for (i = 0; i < NWC24i_MSG_ATTACHMENT_MAX; i++) {
        NWC24Data_Init(&msg->attached[i]);
        msg->attachedSize[i] = 0;
        msg->attachedType[i] = NWC24_TXT_PLAIN;
    }

    NWC24Data_Init(&msg->fromField);
    NWC24Data_Init(&msg->toField);
    NWC24Data_Init(&msg->contentType);
    NWC24Data_Init(&msg->txEncoding);
    NWC24Data_Init(&msg->user);
    NWC24Data_Init(&msg->face);
    NWC24Data_Init(&msg->alt);
    msg->msgBoard = 0;
    msg->dwcId.size = 0;
    msg->iconNew = 0x80000000;

    return NWC24_OK;
}

NWC24Err NWC24SetMsgToId(NWC24MsgObj *pMsg, NWC24UserId id) {
    NWC24iMsgObj *msg = (NWC24iMsgObj *)pMsg;

    if (!(msg->flags & NWC24_MSGOBJ_INITIALIZED) || (msg->flags & NWC24_MSGOBJ_DELIVERING)) {
        return NWC24_ERR_PROTECTED;
    }

    if (!(msg->flags & NWC24_MSGOBJ_FOR_RECIPIENT)) {
        return NWC24_ERR_NOT_SUPPORTED;
    }

    if (msg->numTo >= NWC24i_MSG_RECIPIENT_MAX) {
        return NWC24_ERR_FULL;
    }

    msg->to[msg->numTo].id = id;
    msg->numTo++;

    return NWC24_OK;
}

NWC24Err NWC24SetMsgText(NWC24MsgObj *pMsg, const char *text, u32 len, NWC24Charset charset, NWC24Encoding encoding) {
    NWC24iMsgObj *msg = (NWC24iMsgObj *)pMsg;

    if (!(msg->flags & NWC24_MSGOBJ_INITIALIZED) || (msg->flags & NWC24_MSGOBJ_DELIVERING)) {
        return NWC24_ERR_PROTECTED;
    }

    if (NWC24GetCharsetStr(charset) == NULL) {
        return NWC24_ERR_INVALID_VALUE;
    }

    if (NWC24GetEncodingStr(encoding) == NULL) {
        return NWC24_ERR_INVALID_VALUE;
    }

    if (text == NULL) {
        NWC24Data_Init(&msg->text);
        return NWC24_OK;
    }

    // HYPOTHESIS: this sdk build checks 8bit-vs-public instead of requiring a terminator after len
    if (encoding == NWC24_ENC_8BIT && (msg->flags & NWC24_MSGOBJ_FOR_PUBLIC)) {
        return NWC24_ERR_NOT_SUPPORTED;
    }

    // smtp lines max out at 1000 chars incl the CRLF
    if (encoding == NWC24_ENC_7BIT) {
        u32 lineLength;
        const char *it;
        const char *end;

        lineLength = 0;
        it = text;
        end = text + len;

        for (; it < end; it++) {
            if (it[0] == '\r' && it[1] == '\n') {
                lineLength = 0;
                it++;
            } else if (++lineLength > SMTP_LINE_MAX - 2) {
                return NWC24_ERR_FORMAT;
            }
        }
    }

    NWC24Data_SetDataP(&msg->text, text, len);
    msg->charset = charset;
    msg->encoding = encoding;
    return NWC24_OK;
}

// HYPOTHESIS: name for the subject setter (the field is called subject, setter shape is the same as
// SetMsgText). not in the sibling header, the asm only shows it filling msg->subject.
NWC24Err NWC24SetMsgSubject(NWC24MsgObj *pMsg, const char *text, u32 len) {
    NWC24iMsgObj *msg = (NWC24iMsgObj *)pMsg;

    if (!(msg->flags & NWC24_MSGOBJ_INITIALIZED) || (msg->flags & NWC24_MSGOBJ_DELIVERING)) {
        return NWC24_ERR_PROTECTED;
    }

    if (text == NULL || *text == 0) {
        return NWC24_ERR_NULL;
    }

    if (text[len] != 0) {
        return NWC24_ERR_STRING_END;
    }

    NWC24Data_SetDataP(&msg->subject, text, len);
    return NWC24_OK;
}

NWC24Err NWC24SetMsgTag(NWC24MsgObj *pMsg, u16 tag) {
    NWC24iMsgObj *msg = (NWC24iMsgObj *)pMsg;

    if (!(msg->flags & NWC24_MSGOBJ_INITIALIZED) || (msg->flags & NWC24_MSGOBJ_DELIVERING)) {
        return NWC24_ERR_PROTECTED;
    }

    if (!(msg->flags & NWC24_MSGOBJ_FOR_RECIPIENT)) {
        return NWC24_ERR_NOT_SUPPORTED;
    }

    msg->tag = (msg->tag & 0xFFFF0000) | tag;
    return NWC24_OK;
}

NWC24Err NWC24SetMsgLedPattern(NWC24MsgObj *pMsg, u16 pattern) {
    NWC24iMsgObj *msg = (NWC24iMsgObj *)pMsg;

    if (!(msg->flags & NWC24_MSGOBJ_INITIALIZED) || (msg->flags & NWC24_MSGOBJ_DELIVERING)) {
        return NWC24_ERR_PROTECTED;
    }

    if (pattern == 0) {
        return NWC24_ERR_INVALID_VALUE;
    }

    if (!(msg->flags & NWC24_MSGOBJ_FOR_RECIPIENT) || !(msg->flags & NWC24_MSGOBJ_FOR_MENU)) {
        return NWC24_ERR_NOT_SUPPORTED;
    }

    if (msg->command != 0 && !(msg->command & 0x40000)) {
        return NWC24_ERR_NOT_SUPPORTED;
    }

    // only the system menu (app id HAEA) may use the big pattern values
    if (!NWC24IsMsgLibOpenedByTool() && NWC24GetAppId() != NWC24i_APP_ID_IPL && pattern >= 0x4000) {
        return NWC24_ERR_INVALID_VALUE;
    }

    msg->command = pattern | 0x40000;
    return NWC24_OK;
}

NWC24Err NWC24GetMsgSize(const NWC24MsgObj *pMsg, u32 *size) {
    const NWC24iMsgObj *msg = (const NWC24iMsgObj *)pMsg;

    if (!(msg->flags & NWC24_MSGOBJ_DELIVERING)) {
        return NWC24_ERR_PROTECTED;
    }

    *size = msg->length;
    return NWC24_OK;
}

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
NWC24Err fn_80399BB8(s32 *stage) {
    *stage = config->createStage;
    return NWC24_OK;
}

// checks a string: not null and not empty, no longer than maxlen, at least minlen
NWC24Err fn_80399EE0(const char *str, u32 minLen, u32 maxLen) {
    u32 len;

    if (str == NULL) {
        return NWC24_ERR_NULL;
    }

    len = STD_strnlen(str, maxLen + 1);
    if (len == 0) {
        return NWC24_ERR_NULL;
    }

    if (len > maxLen) {
        return NWC24_ERR_FULL;
    }

    if (len < minLen) {
        return NWC24_ERR_FORMAT;
    }

    return NWC24_OK;
}

// strlcpy, hands back the number of chars copied
int fn_80399F5C(char *dst, const char *src, int size) {
    char *d;
    int i;

    if (dst == NULL || src == NULL) {
        return 0;
    }

    d = dst;
    i = 0;
    while (i + 1 < size) {
        *d = *src;
        if (*src == 0) {
            break;
        }

        src++;
        i++;
        d++;
    }

    if (i + 1 >= size && size != 0) {
        dst[i] = 0;
    }

    return i;
}

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
    result = fn_8039A53C((s32 *)&sp08, mode);
    if (result < 0) {
        GlobalErrorCode = sp08;
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
    s32 n;
    s8 d0, d1, d2, d3, d4, d5;
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

/* ---------------------------------------------------------------------------------------------
 * mail box control files (wc24send.ctl / wc24recv.ctl) and the message files next to them
 * ------------------------------------------------------------------------------------------- */

#define MBC_MAGIC 0x57635466 // 'WcTf'
#define MBC_MAGIC_BROKEN 0x42524B4E // 'BRKN'
#define MBC_VERSION 4
#define MBC_BLOCK_SIZE 0x80
#define MBC_FIRST_ENTRY 0x80

// HYPOTHESIS: message ids count up to a million and wrap, so older/newer has to be decided with a window
#define MSG_ID_WRAP_HIGH 900000
#define MSG_ID_WRAP_LOW 100000

// how many people have the mbox filesystem mounted, and which box it is mounted for
static struct {
    s32 count;
    NWC24MsgBoxId box;
} MBoxMount;

extern NWC24Err fn_8039D0EC(NWC24iMBCEntry *entry, NWC24iMsgObj *msg);
extern NWC24Err fn_8039CF40(NWC24iMsgObj *msg, NWC24iMBCEntry *entry);

static NWC24Err fn_8039C984(NWC24MsgBoxId id, NWC24iMBCHeader **hdrOut);
static NWC24Err fn_8039CDB4(NWC24MsgBoxId id);

static inline NWC24Err MBoxCtrlPath_i(NWC24MsgBoxId id, char *dst, s32 maxlen) {
    char *dir = (char *)NWC24GetMBoxDir();

    if (maxlen < STD_strnlen(dir, NWC24i_MBOX_DIR_LENGTH) + 14) {
        return NWC24_ERR_NOMEM;
    }

    switch (id) {
    case NWC24_MSGBOX_SEND:
        Mail_sprintf(dst, "%s%s", dir, "/wc24send.ctl");
        break;
    case NWC24_MSGBOX_RECV:
        Mail_sprintf(dst, "%s%s", dir, "/wc24recv.ctl");
        break;
    default:
        return NWC24_ERR_INVALID_VALUE;
    }

    return NWC24_OK;
}

static inline NWC24Err MBoxMbxPath_i(NWC24MsgBoxId id, char *dst, s32 maxlen) {
    char *dir = (char *)NWC24GetMBoxDir();

    if (maxlen < STD_strnlen(dir, NWC24i_MBOX_DIR_LENGTH) + 14) {
        return NWC24_ERR_NOMEM;
    }

    switch (id) {
    case NWC24_MSGBOX_SEND:
        Mail_sprintf(dst, "%s%s", dir, "/wc24send.mbx");
        break;
    case NWC24_MSGBOX_RECV:
        Mail_sprintf(dst, "%s%s", dir, "/wc24recv.mbx");
        break;
    default:
        return NWC24_ERR_INVALID_VALUE;
    }

    return NWC24_OK;
}

// the files for single messages live on the vf drive, they are named after the message id
static inline NWC24Err MBoxMsgPath_i(NWC24MsgBoxId id, char *dst, u32 msgId) {
    switch (id) {
    case NWC24_MSGBOX_SEND:
        Mail_sprintf(dst, "@24:/mb/s%07d.msg", msgId);
        break;
    case NWC24_MSGBOX_RECV:
        Mail_sprintf(dst, "@24:/mb/r%07d.msg", msgId);
        break;
    default:
        return NWC24_ERR_INVALID_VALUE;
    }

    return NWC24_OK;
}

static inline NWC24Err MBoxUnmount_i(void) {
    u32 size;
    NWC24iMBCHeader *hdr;
    NWC24Err result;

    if (MBoxMount.count == 0) {
        return NWC24_OK;
    }

    if (--MBoxMount.count > 0) {
        return NWC24_OK;
    }

    result = fn_8039C984(MBoxMount.box, &hdr);
    if (result != NWC24_OK) {
        return result;
    }

    result = NWC24CheckSizeVF("@24", &size);
    if (result != NWC24_OK) {
        return result;
    }

    hdr->freeSpace = size;
    return NWC24UnmountVF("@24");
}

// MATCH-ONLY: same again for the copy that sits inside MBoxRemoveMsgFile_i
static inline NWC24Err MBoxUnmountRm_i(void) {
    NWC24iMBCHeader *hdr;
    u32 size;
    NWC24Err result;

    if (MBoxMount.count == 0) {
        return NWC24_OK;
    }

    if (--MBoxMount.count > 0) {
        return NWC24_OK;
    }

    result = fn_8039C984(MBoxMount.box, &hdr);
    if (result != NWC24_OK) {
        return result;
    }

    result = NWC24CheckSizeVF("@24", &size);
    if (result != NWC24_OK) {
        return result;
    }

    hdr->freeSpace = size;
    return NWC24UnmountVF("@24");
}

// MATCH-ONLY: CloseMsg has its own copy of the unmount code, the locals are laid out the other way round
static inline NWC24Err MBoxUnmountClose_i(void) {
    NWC24iMBCHeader *hdr;
    u32 size;
    NWC24Err result;

    if (MBoxMount.count == 0) {
        return NWC24_OK;
    }

    if (--MBoxMount.count > 0) {
        return NWC24_OK;
    }

    result = fn_8039C984(MBoxMount.box, &hdr);
    if (result != NWC24_OK) {
        return result;
    }

    result = NWC24CheckSizeVF("@24", &size);
    if (result != NWC24_OK) {
        return result;
    }

    hdr->freeSpace = size;
    return NWC24UnmountVF("@24");
}

// deletes the single message file and gives the mount back
static inline NWC24Err MBoxRemoveMsgFile_i(NWC24MsgBoxId id, u32 msgId) {
    NWC24Err result;
    NWC24Err del;
    NWC24Err unmount;
    char *path;

    result = fn_8039CDB4(id);
    if (result != NWC24_OK) {
        return result;
    }

    path = NWC24WorkP->pathWork;
    result = MBoxMsgPath_i(id, path, msgId);
    if (result != NWC24_OK) {
        return result;
    }

    del = NWC24FDeleteVF(path);
    unmount = MBoxUnmountRm_i();
    if (del != NWC24_OK) {
        return del;
    }

    return unmount;
}

// writes the first block (the header) of a control file
static inline NWC24Err MBoxWriteHeader_i(NWC24iMBCHeader *hdr, NWC24File *file) {
    NWC24Err result = NWC24FSeek(file, 0, NWC24_SEEK_BEG);

    if (result != NWC24_OK) {
        return result;
    }

    return NWC24FWrite(hdr, MBC_BLOCK_SIZE, file);
}

// hands out the next message id of a box
static inline NWC24Err MBoxGetNextMsgId_i(NWC24iMBCHeader *hdr, u32 *idOut) {
    if (hdr->magic != MBC_MAGIC) {
        return NWC24_ERR_INVALID_VALUE;
    }

    if (hdr->nextMsgId > 1000000) {
        hdr->nextMsgId = 1;
    }

    *idOut = hdr->nextMsgId;
    hdr->nextMsgId++;
    return NWC24_OK;
}

static inline int CompareMsgId(u32 a, u32 b) {
    if (a == b) {
        return 0;
    }

    if (a > MSG_ID_WRAP_HIGH && b < MSG_ID_WRAP_LOW) {
        return -1;
    }

    if (a < MSG_ID_WRAP_LOW && b > MSG_ID_WRAP_HIGH) {
        return 1;
    }

    return a > b ? 1 : -1;
}

NWC24Err NWC24iMBoxGetCtrlFilePath(NWC24MsgBoxId id, char *dst, s32 maxlen) {
    return MBoxCtrlPath_i(id, dst, maxlen);
}

// how many messages are in a box
NWC24Err fn_8039B1C0(NWC24MsgBoxId id, u32 *numMsgs) {
    NWC24iMBCHeader *hdr;
    NWC24Err result;

    if (!NWC24IsMsgLibOpened() && !NWC24IsMsgLibOpenedByTool()) {
        return NWC24_ERR_LIB_NOT_OPENED;
    }

    result = fn_8039C984(id, &hdr);
    if (result != NWC24_OK) {
        *numMsgs = 0;
        return result;
    }

    *numMsgs = hdr->numMsgs;
    return NWC24_OK;
}

// can the current app see this mail box entry?
static BOOL fn_8039B4C8(NWC24iMBCEntry *entry) {
    u32 flags = entry->flags;

    if (!(flags & 0x200000)) {
        return FALSE;
    }

    if (flags & 0xFE000000) {
        return FALSE;
    }

    if (entry->command & 0xFFF80000) {
        return FALSE;
    }

    if (flags & NWC24_MSGOBJ_FOR_PUBLIC) {
        if (entry->appId == 0) {
            return TRUE;
        }

        return (NWC24GetAppId() & ~0xFF) == (entry->appId & ~0xFF);
    }

    if (NWC24GetAppId() == NWC24i_APP_ID_IPL) {
        return TRUE;
    }

    if (entry->flags & NWC24_MSGOBJ_FOR_APP) {
        return TRUE;
    }

    return (NWC24GetAppId() & ~0xFF) == (entry->appId & ~0xFF);
}

NWC24Err fn_8039C16C(NWC24MsgBoxId id, u32 msgId, BOOL checkAccess);

NWC24Err fn_8039B5AC(NWC24MsgBoxId id, u32 msgId) {
    BOOL checkAccess = TRUE;

    if (NWC24IsMsgLibOpenedByTool()) {
        checkAccess = FALSE;
    }

    return fn_8039C16C(id, msgId, checkAccess);
}

NWC24Err fn_8039B244(NWC24iMsgObj *out, NWC24MsgBoxId id, u32 msgId) {
    NWC24iMBCHeader *hdr;
    NWC24File file;
    NWC24Err result;
    NWC24Err err;
    NWC24Err closeErr;
    NWC24iMBCEntry *entry;
    char *path;
    u32 count = 0;
    u32 off;

    if (!NWC24IsMsgLibOpened() && !NWC24IsMsgLibOpenedByTool()) {
        return NWC24_ERR_LIB_NOT_OPENED;
    }

    entry = &NWC24WorkP->mbcEntry;
    memset(entry, 0, MBC_BLOCK_SIZE);

    if (msgId == 0) {
        return NWC24_ERR_HIDDEN;
    }

    result = fn_8039C984(id, &hdr);
    if (result != NWC24_OK) {
        return result;
    }

    path = NWC24WorkP->pathWork;
    result = MBoxCtrlPath_i(id, path, NWC24i_PATH_WORK_SIZE);
    if (result != NWC24_OK) {
        return result;
    }

    result = NWC24FOpen(&file, path, NWC24_OPEN_RW);
    if (result != NWC24_OK) {
        return result;
    }

    result = NWC24_ERR_NOT_FOUND;
    for (off = MBC_FIRST_ENTRY; off < hdr->fileSize; off += MBC_BLOCK_SIZE) {
        err = NWC24FSeek(&file, off, NWC24_SEEK_BEG);
        if (err != NWC24_OK) {
            result = err;
            break;
        }

        err = NWC24FRead(entry, MBC_BLOCK_SIZE, &file);
        if (err != NWC24_OK) {
            result = err;
            break;
        }

        if (entry->id != 0) {
            count++;
            if (entry->id == msgId) {
                result = NWC24_ERR_HIDDEN;
                if (NWC24IsMsgLibOpenedByTool()) {
                    result = NWC24_OK;
                    break;
                }

                if (fn_8039B4C8(entry)) {
                    result = NWC24_OK;
                }

                break;
            }
        }
    }

    // the cached message count was wrong, fix it up while the file is open
    if (result == NWC24_ERR_NOT_FOUND && hdr->numMsgs != count) {
        hdr->numMsgs = count;
        MBoxWriteHeader_i(hdr, &file);
    }

    closeErr = NWC24FClose(&file);
    if (result != NWC24_OK) {
        if (msgId == 0) {
            result = NWC24_ERR_INVALID_VALUE;
        }

        return result;
    }

    if (closeErr != NWC24_OK) {
        return closeErr;
    }

    fn_8039D0EC(entry, out);
    return result;
}

NWC24Err fn_8039B9B0(NWC24MsgBoxId id, u32 msgId, NWC24File *file);

NWC24Err NWC24iMBoxOpenNewMsg(NWC24MsgBoxId id, NWC24File *file, u32 *pMsgID) {
    NWC24iMBCHeader *hdr;
    NWC24Err result;
    char *path;

    result = fn_8039C984(id, &hdr);
    if (result != NWC24_OK) {
        return result;
    }

    result = MBoxGetNextMsgId_i(hdr, pMsgID);
    if (result != NWC24_OK) {
        return result;
    }

    result = fn_8039CDB4(id);
    if (result != NWC24_OK) {
        return result;
    }

    path = NWC24WorkP->pathWork;
    result = MBoxMsgPath_i(id, path, *pMsgID);
    if (result != NWC24_OK) {
        return result;
    }

    return NWC24FOpen(file, path, NWC24_OPEN_VF_WBUFF);
}

NWC24Err fn_8039B9B0(NWC24MsgBoxId id, u32 msgId, NWC24File *file) {
    NWC24Err result;
    char *path;

    result = fn_8039CDB4(id);
    if (result != NWC24_OK) {
        return result;
    }

    path = NWC24WorkP->pathWork;
    result = MBoxMsgPath_i(id, path, msgId);
    if (result != NWC24_OK) {
        return result;
    }

    return NWC24FOpen(file, path, NWC24_OPEN_VF_RBUFF);
}

NWC24Err NWC24iMBoxCloseMsg(NWC24File *file) {
    NWC24Err close;
    NWC24Err unmount;

    close = NWC24FClose(file);
    unmount = MBoxUnmountClose_i();
    if (close != NWC24_OK && unmount != NWC24_OK) {
        return unmount;
    }

    return close;
}

NWC24Err NWC24iMBoxCancelMsg(NWC24File *file, NWC24MsgBoxId id, u32 msgId) {
    NWC24Err result;
    NWC24Err err;
    char *path;
    NWC24Err close;
    NWC24Err del;

    close = NWC24FClose(file);

    err = fn_8039CDB4(id);
    if (err != NWC24_OK) {
        result = err;
        goto forceUnmount;
    }

    path = NWC24WorkP->pathWork;
    err = MBoxMsgPath_i(id, path, msgId);
    if (err != NWC24_OK) {
        result = err;
        goto forceUnmount;
    }

    del = NWC24FDeleteVF(path);
    err = MBoxUnmount_i();
    if (del != NWC24_OK) {
        err = del;
    }
    result = err;

forceUnmount:
    MBoxMount.count = 1;
    err = MBoxUnmount_i();

    if (close != NWC24_OK) {
        return close;
    }

    if (result != NWC24_OK) {
        return result;
    }

    return err != NWC24_OK ? err : NWC24_OK;
}

// finds (and loads, if needed) the control file header of a box
static NWC24Err fn_8039C984(NWC24MsgBoxId id, NWC24iMBCHeader **hdrOut) {
    NWC24Err result = NWC24_OK;
    NWC24File file;
    NWC24Err closeErr;
    NWC24Err err;
    NWC24iMBCHeader *hdr;
    char *path;

    if (id == NWC24_MSGBOX_SEND) {
        *hdrOut = &NWC24WorkP->sendCtrl;
    } else if (id == NWC24_MSGBOX_RECV) {
        *hdrOut = &NWC24WorkP->recvCtrl;
    } else {
        *hdrOut = NULL;
        return NWC24_ERR_INVALID_VALUE;
    }

    if ((*hdrOut)->magic != MBC_MAGIC) {
        path = NWC24WorkP->pathWork;
        result = MBoxCtrlPath_i(id, path, NWC24i_PATH_WORK_SIZE);
        if (result != NWC24_OK) {
            return result;
        }

        result = NWC24FOpen(&file, path, NWC24_OPEN_READ);
        if (result != NWC24_OK) {
            return result;
        }

        hdr = *hdrOut;
        err = NWC24FSeek(&file, 0, NWC24_SEEK_BEG);
        if (err != NWC24_OK) {
            result = err;
        } else {
            err = NWC24FRead(hdr, MBC_BLOCK_SIZE, &file);
            if (err != NWC24_OK) {
                result = err;
            } else {
                result = NWC24_OK;
                if (hdr->magic != MBC_MAGIC) {
                    result = NWC24_ERR_BROKEN;
                }
            }
        }

        closeErr = NWC24FClose(&file);
        if (result == NWC24_OK && closeErr != NWC24_OK) {
            result = closeErr;
        }
    }

    if ((*hdrOut)->version != MBC_VERSION) {
        return NWC24_ERR_VER_MISMATCH;
    }

    if ((*hdrOut)->freeChain & 0x1F) {
        result = NWC24_ERR_BROKEN;
    }

    return result;
}

// takes a free block off the free chain and puts the message in it
static NWC24Err fn_8039CB7C(NWC24iMBCHeader *hdr, NWC24iMsgObj *msg, NWC24File *file) {
    u32 block = hdr->freeChain;
    NWC24iMBCEntry *entry = &NWC24WorkP->mbcEntry;
    NWC24Err result;
    u32 next;

    if (block == 0) {
        return NWC24_ERR_FULL;
    }

    if (block >= hdr->fileSize || (block - MBC_FIRST_ENTRY) % MBC_BLOCK_SIZE != 0) {
        return NWC24_ERR_BROKEN;
    }

    result = NWC24FSeek(file, block, NWC24_SEEK_BEG);
    if (result != NWC24_OK) {
        return result;
    }

    result = NWC24FRead(entry, MBC_BLOCK_SIZE, file);
    if (result != NWC24_OK) {
        return result;
    }

    next = entry->appId;
    if (next >= hdr->fileSize || (next - MBC_FIRST_ENTRY) % MBC_BLOCK_SIZE != 0) {
        return NWC24_ERR_BROKEN;
    }

    hdr->freeChain = next;
    fn_8039CF40(msg, entry);

    result = NWC24FSeek(file, block, NWC24_SEEK_BEG);
    if (result != NWC24_OK) {
        return result;
    }

    result = NWC24FWrite(entry, MBC_BLOCK_SIZE, file);
    if (result != NWC24_OK) {
        return result;
    }

    if (hdr->numMsgs == 0) {
        hdr->oldestMsgId = msg->id;
    }

    hdr->totalMsgSize += msg->length;
    hdr->numMsgs++;
    return NWC24_OK;
}

// gives a block back to the free chain
static NWC24Err fn_8039CCC8(NWC24iMBCHeader *hdr, NWC24File *file, u32 block) {
    NWC24iMBCEntry *entry = &NWC24WorkP->mbcEntry;
    NWC24Err result;

    result = NWC24FSeek(file, block, NWC24_SEEK_BEG);
    if (result != NWC24_OK) {
        return result;
    }

    result = NWC24FRead(entry, MBC_BLOCK_SIZE, file);
    if (result != NWC24_OK) {
        return result;
    }

    hdr->numMsgs--;
    hdr->totalMsgSize -= entry->length;
    memset(entry, 0, MBC_BLOCK_SIZE);
    entry->appId = hdr->freeChain;
    hdr->freeChain = block;

    result = NWC24FSeek(file, block, NWC24_SEEK_BEG);
    if (result != NWC24_OK) {
        return result;
    }

    return NWC24FWrite(entry, MBC_BLOCK_SIZE, file);
}

// mounts the vf drive that holds the message files of one box
static NWC24Err fn_8039CDB4(NWC24MsgBoxId id) {
    NWC24Err result;
    char *path;

    if (MBoxMount.count != 0 && MBoxMount.box != id) {
        MBoxMount.count = 1;
        MBoxUnmount_i();
        return NWC24_ERR_FATAL;
    }

    if (++MBoxMount.count > 1) {
        return NWC24_OK;
    }

    path = NWC24WorkP->pathWork;
    result = MBoxMbxPath_i(id, path, NWC24i_PATH_WORK_SIZE);
    if (result != NWC24_OK) {
        return result;
    }

    result = NWC24MountVF("@24", path);
    if (result != NWC24_OK) {
        return result;
    }

    MBoxMount.box = id;
    return NWC24_OK;
}

static NWC24Err fn_8039C60C(NWC24iMBCHeader *hdr, NWC24iMsgObj *msg, NWC24File *file, NWC24MsgBoxId id);

NWC24Err NWC24iMBoxAddMsgObj(NWC24MsgBoxId id, NWC24iMsgObj *msg) {
    NWC24iMBCHeader *hdr;
    NWC24File file;
    NWC24Err result;
    char *path;

    result = fn_8039C984(id, &hdr);
    if (result != NWC24_OK) {
        return result;
    }

    if (hdr->numMsgs == hdr->capacity) {
        return NWC24_ERR_FULL;
    }

    path = NWC24WorkP->pathWork;
    result = MBoxCtrlPath_i(id, path, NWC24i_PATH_WORK_SIZE);
    if (result != NWC24_OK) {
        return result;
    }

    result = NWC24FOpen(&file, path, NWC24_OPEN_RW);
    if (result != NWC24_OK) {
        return result;
    }

    result = fn_8039CB7C(hdr, msg, &file);
    if (result == NWC24_OK) {
        result = fn_8039C60C(hdr, msg, &file, id);
        if (result != NWC24_OK) {
            goto close;
        }
    } else if (result == NWC24_ERR_BROKEN) {
        hdr->magic = MBC_MAGIC_BROKEN;
    } else {
        goto close;
    }

    result = MBoxWriteHeader_i(hdr, &file);

close: {
    NWC24Err closeErr = NWC24FClose(&file);

    return result != NWC24_OK ? result : closeErr;
}
}

NWC24Err NWC24iMBoxFlushHeader(NWC24MsgBoxId id) {
    NWC24iMBCHeader *hdr;
    NWC24File file;
    NWC24Err result;
    NWC24Err closeErr;
    char *path;

    result = fn_8039C984(id, &hdr);
    if (result != NWC24_OK) {
        return result;
    }

    path = NWC24WorkP->pathWork;
    result = MBoxCtrlPath_i(id, path, NWC24i_PATH_WORK_SIZE);
    if (result != NWC24_OK) {
        return result;
    }

    result = NWC24FOpen(&file, path, NWC24_OPEN_RW);
    if (result != NWC24_OK) {
        return result;
    }

    result = MBoxWriteHeader_i(hdr, &file);

    closeErr = NWC24FClose(&file);
    return result != NWC24_OK ? result : closeErr;
}

// makes room for a message of the given size: the send box just refuses when full, the receive box throws
// the oldest message out until there is space
NWC24Err NWC24iMBoxCheck(NWC24MsgBoxId id, u32 size) {
    NWC24iMBCHeader *hdr;
    NWC24Err result;
    u32 limit;
    volatile u32 oldest;

    if (size >= NWC24i_MBOX_MAX_SIZE) {
        return NWC24_ERR_OVERFLOW;
    }

    limit = size + 0x4000;
    result = fn_8039C984(id, &hdr);
    if (result != NWC24_OK) {
        return result;
    }

    if (id == NWC24_MSGBOX_SEND) {
        if (hdr->numMsgs >= hdr->capacity) {
            return NWC24_ERR_FULL;
        }

        if (hdr->freeSpace <= limit) {
            return NWC24_ERR_FULL;
        }
    } else if (id == NWC24_MSGBOX_RECV) {
        while (hdr->numMsgs >= hdr->capacity || hdr->freeSpace <= limit) {
            oldest = hdr->oldestMsgId;
            result = fn_8039C16C(id, oldest, FALSE);
            if (result != NWC24_OK) {
                return result;
            }

            result = fn_8039C984(id, &hdr);
            if (result != NWC24_OK) {
                return result;
            }
        }
    } else {
        return NWC24_ERR_INVALID_VALUE;
    }

    return NWC24_OK;
}

// deletes one message from a box (checking if the current app may, unless told not to)
NWC24Err fn_8039C16C(NWC24MsgBoxId id, u32 msgId, BOOL checkAccess) {
    NWC24Err err;
    char *path;
    u32 off;
    NWC24File file;
    u32 appId;
    u32 count = 0;
    u32 foundOff = 0;
    NWC24Err removeErr;
    NWC24Err writeErr;
    u32 oldest = 0;
    NWC24Err result;
    NWC24iMBCHeader *hdr;
    NWC24iMBCEntry *entry;

    if (!NWC24IsMsgLibOpened() && !NWC24IsMsgLibOpenedByTool()) {
        return NWC24_ERR_LIB_NOT_OPENED;
    }

    entry = &NWC24WorkP->mbcEntry;
    result = fn_8039C984(id, &hdr);
    if (result != NWC24_OK) {
        return result;
    }

    path = NWC24WorkP->pathWork;
    result = MBoxCtrlPath_i(id, path, NWC24i_PATH_WORK_SIZE);
    if (result != NWC24_OK) {
        return result;
    }

    result = NWC24FOpen(&file, path, NWC24_OPEN_RW);
    if (result != NWC24_OK) {
        return result;
    }

    result = NWC24_ERR_NOT_FOUND;
    for (off = MBC_FIRST_ENTRY; off < hdr->fileSize; off += MBC_BLOCK_SIZE) {
        writeErr = NWC24FSeek(&file, off, NWC24_SEEK_BEG);
        if (writeErr != NWC24_OK) {
            result = writeErr;
            break;
        }

        writeErr = NWC24FRead(entry, MBC_BLOCK_SIZE, &file);
        if (writeErr != NWC24_OK) {
            result = writeErr;
            break;
        }

        if (entry->id == 0) {
            continue;
        }

        count++;
        if (entry->id == msgId) {
            if (checkAccess) {
                if ((entry->flags & NWC24_MSGOBJ_FOR_PUBLIC) && entry->appId == 0) {
                    if (NWC24GetAppId() != NWC24i_APP_ID_IPL) {
                        result = NWC24_ERR_PROTECTED;
                        break;
                    }
                } else if ((NWC24GetAppId() & ~0xFF) != (entry->appId & ~0xFF)) {
                    if (!(entry->flags & NWC24_MSGOBJ_FOR_MENU) || NWC24GetAppId() != NWC24i_APP_ID_IPL) {
                        result = NWC24_ERR_PROTECTED;
                        break;
                    }
                }
            }

            foundOff = off;
            result = NWC24_OK;
        } else if (oldest == 0 || CompareMsgId(oldest, entry->id) > 0) {
            oldest = entry->id;
        }
    }

    if (result == NWC24_ERR_NOT_FOUND && hdr->numMsgs != count) {
        hdr->numMsgs = count;
        MBoxWriteHeader_i(hdr, &file);
    }

    if (result != NWC24_OK) {
        NWC24FClose(&file);
        if (msgId == 0) {
            result = NWC24_ERR_INVALID_VALUE;
        }

        return result;
    }

    hdr->oldestMsgId = oldest;
    removeErr = fn_8039CCC8(hdr, &file, foundOff);
    err = MBoxRemoveMsgFile_i(id, msgId);
    if (removeErr != NWC24_OK) {
        err = removeErr;
    }

    writeErr = MBoxWriteHeader_i(hdr, &file);

    if (err != NWC24_OK) {
        writeErr = err;
    }

    result = NWC24FClose(&file);
    return writeErr != NWC24_OK ? writeErr : result;
}

// looks for an older copy of the message that is about to be stored (same tag or same contents) and
// throws it out, also keeps the oldest message id of the box up to date
static NWC24Err fn_8039C60C(NWC24iMBCHeader *hdr, NWC24iMsgObj *msg, NWC24File *file, NWC24MsgBoxId id) {
    NWC24Err result = NWC24_OK;
    u32 off;
    u32 oldest = 0;
    u32 differs;
    u32 foundId = 0;
    u32 count = 0;
    u32 foundOff = 0;
    NWC24Err removeErr;
    NWC24iMBCEntry *entry;

    if (id == NWC24_MSGBOX_SEND) {
        return NWC24_OK;
    }

    entry = &NWC24WorkP->mbcEntry;
    for (off = MBC_FIRST_ENTRY; off < hdr->fileSize; off += MBC_BLOCK_SIZE) {
        if (count >= hdr->numMsgs) {
            break;
        }

        result = NWC24FSeek(file, off, NWC24_SEEK_BEG);
        if (result != NWC24_OK) {
            break;
        }

        result = NWC24FRead(entry, MBC_BLOCK_SIZE, file);
        if (result != NWC24_OK) {
            break;
        }

        if (entry->id == 0) {
            continue;
        }

        count++;
        differs = 1;
        if (entry->id == msg->id) {
            differs = 0;
        }

        if ((entry->flags & msg->flags) & 1) {
            if (entry->appId != msg->appId) {
                differs = 0;
            }

            if (entry->from.id != msg->from.id) {
                differs = 0;
            }
        }

        if (differs) {
            if ((msg->tag & 0xFFFF) != 0 && (entry->tag & 0xFFFF) == (msg->tag & 0xFFFF)) {
                foundId = entry->id;
                foundOff = off;
                continue;
            }

            if (msg->unk1C != 0 && entry->unk1C == msg->unk1C && entry->flags == msg->flags &&
                entry->length == msg->length && entry->unk10 == msg->unk10 && entry->createTime == msg->createTime) {
                foundId = entry->id;
                foundOff = off;
                continue;
            }
        }

        if (oldest == 0 || CompareMsgId(oldest, entry->id) > 0) {
            oldest = entry->id;
        }
    }

    if (result != NWC24_OK) {
        return result;
    }

    hdr->oldestMsgId = oldest;
    if (foundId != 0) {
        removeErr = fn_8039CCC8(hdr, file, foundOff);
        result = MBoxRemoveMsgFile_i(id, foundId);
        if (removeErr != NWC24_OK) {
            result = removeErr;
        }
    }

    return result;
}

// the message object keeps strings/blobs as {offset, size} pairs, in the control file they are packed into
// one word (size in the top 12 bits, offset in the low 20)
static inline u32 PackData_i(const NWC24Data *data) {
    return (data->size << 20) | (data->offset & 0xFFFFF);
}

static inline void UnpackData_i(NWC24Data *data, u32 packed) {
    data->offset = packed & 0xFFFFF;
    data->size = packed >> 20;
}

// message object -> control file entry
NWC24Err fn_8039CF40(NWC24iMsgObj *msg, NWC24iMBCEntry *entry) {
    entry->id = msg->id;
    entry->flags = msg->flags;
    entry->length = msg->length;
    entry->appId = msg->appId;
    entry->unk10 = msg->unk10;
    entry->tag = msg->tag;
    entry->command = msg->command;
    entry->unk1C = msg->unk1C;
    entry->from = msg->from;
    entry->createTime = msg->createTime;
    entry->unk2C = msg->unk2C;
    entry->numTo = msg->numTo;
    entry->numAttached = msg->numAttached;
    entry->groupId = msg->groupId;
    entry->fromField = PackData_i(&msg->fromField);
    entry->toField = PackData_i(&msg->toField);
    entry->subject = PackData_i(&msg->subject);
    entry->contentType = PackData_i(&msg->contentType);
    entry->txEncoding = PackData_i(&msg->txEncoding);
    entry->text = msg->text;
    entry->attached[0] = msg->attached[0];
    entry->attached[1] = msg->attached[1];
    entry->attachedSize[0] = msg->attachedSize[0];
    entry->attachedSize[1] = msg->attachedSize[1];
    entry->attachedType[0] = msg->attachedType[0];
    entry->attachedType[1] = msg->attachedType[1];
    entry->dwcId = msg->dwcId;
    entry->unk78 = msg->iconNew;
    return NWC24_OK;
}

// control file entry -> message object
NWC24Err fn_8039D0EC(NWC24iMBCEntry *entry, NWC24iMsgObj *msg) {
    int i;

    msg->id = entry->id;
    msg->flags = entry->flags;
    msg->length = entry->length;
    msg->appId = entry->appId;
    msg->unk10 = entry->unk10;
    msg->tag = entry->tag;
    msg->command = entry->command;
    msg->unk1C = entry->unk1C;
    msg->from = entry->from;
    msg->createTime = entry->createTime;
    msg->unk2C = entry->unk2C;
    msg->numTo = entry->numTo;
    msg->numAttached = entry->numAttached;
    msg->groupId = entry->groupId;
    UnpackData_i(&msg->fromField, entry->fromField);
    UnpackData_i(&msg->toField, entry->toField);
    UnpackData_i(&msg->subject, entry->subject);
    UnpackData_i(&msg->contentType, entry->contentType);
    UnpackData_i(&msg->txEncoding, entry->txEncoding);
    msg->text = entry->text;
    msg->attached[0] = entry->attached[0];
    msg->attached[1] = entry->attached[1];
    msg->attachedSize[0] = entry->attachedSize[0];
    msg->attachedSize[1] = entry->attachedSize[1];
    msg->attachedType[0] = entry->attachedType[0];
    msg->attachedType[1] = entry->attachedType[1];
    msg->dwcId = entry->dwcId;
    msg->iconNew = entry->unk78;

    for (i = 0; i < NWC24i_MSG_RECIPIENT_MAX; i++) {
        msg->to[i].id = 0;
    }

    return NWC24_OK;
}

/* ---------------------------------------------------------------------------------------------
 * base64 / quoted printable
 * ------------------------------------------------------------------------------------------- */

// the alphabet, with the pad char on the end
static char *Base64Chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";

// makes a lookup table that maps ascii to the 6 bit value (-1 for everything that is not part of the alphabet)
void NWC24InitBase64Table(char *table) {
    int i;

    for (i = 0; i < 256; i++) {
        table[i] = -1;
    }

    for (i = 0; i < 26; i++) {
        table['A' + i] = i;
    }

    for (i = 0; i < 26; i++) {
        table['a' + i] = 26 + i;
    }

    for (i = 0; i < 10; i++) {
        table['0' + i] = 52 + i;
    }

    table['+'] = 62;
    table['/'] = 63;
}

static NWC24Err fn_8039D30C(const u8 *data, int dataSize, int *dataRead, char *text, int textSize, int *textWritten) {
    int out = 0;
    int in = 0;
    NWC24Err result = NWC24_OK;

    for (; in < dataSize; in += 3) {
        u32 value;
        BOOL has1;
        BOOL has2;

        if (out + 3 >= textSize) {
            result = NWC24_ERR_OVERFLOW;
            break;
        }

        value = data[in];
        has1 = FALSE;
        has2 = FALSE;
        value <<= 8;
        if (in + 1 < dataSize) {
            value |= data[in + 1];
            has1 = TRUE;
        }

        value <<= 8;
        if (in + 2 < dataSize) {
            value |= data[in + 2];
            has2 = TRUE;
        }

        text[out] = Base64Chars[(value >> 18) & 0x3F];
        text[out + 1] = Base64Chars[(value >> 12) & 0x3F];
        text[out + 2] = Base64Chars[has1 ? (value >> 6) & 0x3F : 64];
        text[out + 3] = Base64Chars[has2 ? value & 0x3F : 64];
        out += 4;
    }

    *textWritten = out;
    if (dataRead != NULL) {
        *dataRead = in < dataSize ? in : dataSize;
    }

    return result;
}

NWC24Err NWC24Base64Encode(const u8 *data, s32 dataSize, char *text, s32 textSize, u32 *textWritten) {
    return fn_8039D30C(data, dataSize, NULL, text, textSize, (int *)textWritten);
}

// goes the other way, skips everything that is not in the alphabet
NWC24Err fn_8039D45C(const u8 *data, int dataSize, u8 *out, u32 outSize, u32 *outWritten) {
    char *table = NWC24WorkP->base64Work;
    int bits = 0;
    u32 n = 0;
    int acc = 0;
    int i;

    for (i = 0; i < dataSize; i++) {
        s8 value = table[*data];

        if (value >= 0 && value < 64) {
            bits += 6;
            acc = (acc << 6) | value;
            if (bits >= 8) {
                if (n >= outSize) {
                    *outWritten = outSize;
                    return NWC24_ERR_OVERFLOW;
                }

                bits -= 8;
                n++;
                *out++ = acc >> bits;
            }
        }

        data++;
    }

    *outWritten = n;
    return NWC24_OK;
}

// HYPOTHESIS: this is the inlined memcpy the compiler unrolled, the loop counter is a signed int
static inline void CopyBytes_i(char *dst, const char *src, int n) {
    int i;

    for (i = 0; i < n; i++) {
        *dst++ = *src++;
    }
}

// a hex digit as ascii, the compiler turned the modulo 10 into a multiply so it must be written like that
static inline int HexDigit_i(int digit) {
    if (digit >= 10) {
        return 'A' + digit % 10;
    }

    return '0' + digit % 10;
}

static NWC24Err fn_8039D7C4(char *text, u32 textSize, u32 *textWritten, const char *data, u32 dataSize,
                            u32 *dataRead, BOOL noSoftBreaks) {
    NWC24Err result = NWC24_OK;
    int lineLen;
    u32 written;
    u32 consumedTotal;
    char buf[4];

    if (text == NULL) {
        return NWC24_ERR_INVALID_VALUE;
    }

    if (data == NULL) {
        return NWC24_ERR_INVALID_VALUE;
    }

    if (dataSize == 0) {
        return NWC24_ERR_OVERFLOW;
    }

    if (dataRead != NULL) {
        *dataRead = 0;
    }

    if (textWritten != NULL) {
        *textWritten = 0;
    }

    lineLen = 0;
    written = 0;
    consumedTotal = 0;
    while (consumedTotal < dataSize) {
        int consumed;
        int out;
        BOOL softBreak = FALSE;
        char ch = *data;

        if (ch >= 0x21 && ch <= 0x7E && ch != '=') {
            buf[0] = ch;
            consumed = 1;
            out = 1;
        } else if (consumedTotal + 1 < dataSize && ch == '\r' && data[1] == '\n') {
            buf[0] = ch;
            consumed = 2;
            out = 1;
            buf[1] = data[1];
        } else {
            buf[0] = '=';
            buf[1] = HexDigit_i((u8)*data >> 4);
            buf[2] = HexDigit_i(*data & 0xF);
            consumed = 1;
            out = 3;
        }

        if (!noSoftBreaks && lineLen + out >= 75) {
            softBreak = TRUE;
            out += 3;
        }

        if (written + out >= textSize) {
            result = NWC24_ERR_OVERFLOW;
            break;
        }

        written += out;
        if (softBreak) {
            *text = '=';
            lineLen = 0;
            out -= 3;
            text[1] = '\r';
            text[2] = '\n';
            text += 3;
        }

        lineLen += out;
        CopyBytes_i(text, buf, out);
        text += out;
        consumedTotal += consumed;
        data += consumed;
    }

    if (dataRead != NULL) {
        *dataRead = consumedTotal;
    }

    if (textWritten != NULL) {
        *textWritten = written;
    }

    return result;
}

NWC24Err NWC24EncodeQuotedPrintable(char *text, u32 textSize, u32 *textWritten, const u8 *data, u32 dataSize,
                                    u32 *dataRead) {
    return fn_8039D7C4(text, textSize, textWritten, (const char *)data, dataSize, dataRead, FALSE);
}

/* ---------------------------------------------------------------------------------------------
 * message object helpers + message list
 * ------------------------------------------------------------------------------------------- */

// how big is a base64 part: 4 chars per 3 bytes, a CRLF every 76 chars and the closing line
static inline u32 Base64PartSize_i(u32 n) {
    return (n * 4 + 2) / 3 + (n / 57) * 2 + 4;
}

NWC24Err NWC24SetMsgAttached(NWC24MsgObj *pMsg, const char *data, u32 size, NWC24MIMEType type) {
    NWC24iMsgObj *msg = (NWC24iMsgObj *)pMsg;
    u32 total = 0;
    u32 i;

    if (!(msg->flags & NWC24_MSGOBJ_INITIALIZED) || (msg->flags & NWC24_MSGOBJ_DELIVERING)) {
        return NWC24_ERR_PROTECTED;
    }

    if (data == NULL || size == 0) {
        return NWC24_ERR_NULL;
    }

    if (msg->numAttached >= NWC24i_MSG_ATTACHMENT_MAX) {
        return NWC24_ERR_FULL;
    }

    if (NWC24GetMIMETypeStr(type) == NULL) {
        return NWC24_ERR_INVALID_VALUE;
    }

    // messages to the public can only carry the plain types
    if (msg->flags & NWC24_MSGOBJ_FOR_PUBLIC) {
        if (type != NWC24_TXT_PLAIN && type != NWC24_TXT_HTML && type != NWC24_IMG_JPEG && type != NWC24_APP_OCTET_STREAM) {
            return NWC24_ERR_NOT_SUPPORTED;
        }
    }

    if (type == NWC24_APP_WII_MINIDATA && size > 0x80) {
        return NWC24_ERR_INVALID_VALUE;
    }

    for (i = 0; i < msg->numAttached; i++) {
        total += msg->attachedSize[i];
    }

    total += size;
    if (total >= 0x245B0) {
        return NWC24_ERR_OVERFLOW;
    }

    NWC24Data_SetDataP(&msg->attached[msg->numAttached], data, size);
    msg->attachedSize[msg->numAttached] = size;
    msg->attachedType[msg->numAttached] = type;
    msg->numAttached++;
    msg->flags |= NWC24_MSGOBJ_MULTIPART;
    return NWC24_OK;
}

NWC24Err fn_8039B160(NWC24iMsgObj *msg, u32 *count) {
    NWC24Err result = NWC24_OK;
    u32 n = msg->numAttached;

    *count = n;
    if (n > NWC24i_MSG_ATTACHMENT_MAX) {
        result = NWC24_ERR_BROKEN;
    }

    return result;
}

NWC24Err fn_8039B17C(NWC24iMsgObj *msg, u32 idx, u32 *size) {
    if (idx >= NWC24i_MSG_ATTACHMENT_MAX || idx >= msg->numAttached) {
        return NWC24_ERR_INVALID_VALUE;
    }

    *size = msg->attachedSize[idx];
    return NWC24_OK;
}

NWC24Err fn_8039B1B0(u32 *src, u32 *dst) {
    *dst = *src;
    return NWC24_OK;
}

// checks the header fields of a message
NWC24Err fn_8039E9CC(NWC24iMsgObj *msg) {
    char *it;

    if (msg->numTo == 0) {
        return NWC24_ERR_NULL;
    }

    if (msg->numTo > NWC24i_MSG_RECIPIENT_MAX) {
        return NWC24_ERR_INVALID_VALUE;
    }

    // folded header lines have to start with a blank
    it = (char *)msg->subject.pData;
    if (it != NULL) {
        for (; *it != 0; it++) {
            if (*it == '\r' && it[1] != '\n') {
                return NWC24_ERR_FORMAT;
            }

            if (*it == '\n' && it[1] != ' ') {
                return NWC24_ERR_FORMAT;
            }
        }
    }

    if (msg->numAttached > NWC24i_MSG_ATTACHMENT_MAX) {
        return NWC24_ERR_INVALID_VALUE;
    }

    return NWC24_OK;
}

// works out how big the finished message gets and makes sure the box has room for it
NWC24Err fn_8039EA6C(NWC24iMsgObj *msg, NWC24MsgBoxId id) {
    u32 total = 0;
    u32 extra;
    int i;
    NWC24Err result;

    for (i = 0; i < msg->numAttached; i++) {
        total = total + (msg->attachedSize[i] * 4 + 2) / 3 + msg->attachedSize[i] / 57 * 2 + 4;
    }

    switch (msg->encoding) {
    case NWC24_ENC_7BIT:
    case NWC24_ENC_8BIT:
        extra = msg->text.size;
        break;
    case NWC24_ENC_BASE64:
        extra = Base64PartSize_i(msg->text.size);
        break;
    case NWC24_ENC_QUOTED_PRINTABLE:
        extra = msg->text.size * 4 / 3;
        break;
    default:
        extra = 0;
        break;
    }

    total += extra;
    if (total >= NWC24i_MBOX_MAX_SIZE) {
        return NWC24_ERR_OVERFLOW;
    }

    result = NWC24iMBoxCheck(id, total + 0x400);
    return result != NWC24_OK ? result : NWC24_OK;
}

// the state of the "list my messages" calls
typedef struct MsgListState {
    u32 valid;
    u32 pos;
    u32 count;
    u32 total;
    u32 flags;
    NWC24MsgBoxId box;
    NWC24UserId from;
    u32 appId;
    u32 tagFilter;
} MsgListState;

static MsgListState MsgList;

static NWC24iMBCEntry ListEntry;

NWC24Err fn_803A0158(void) {
    if (!NWC24IsMsgLibOpened() && !NWC24IsMsgLibOpenedByTool()) {
        return NWC24_ERR_LIB_NOT_OPENED;
    }

    MsgList.valid = 0;
    MsgList.pos = 0;
    MsgList.count = 0;
    MsgList.total = 0;
    MsgList.flags = 0;
    MsgList.box = NWC24_MSGBOX_RECV;
    MsgList.from = 0;
    MsgList.appId = 0;
    MsgList.tagFilter = 0;
    return NWC24_OK;
}

NWC24Err fn_803A01D0(NWC24MsgBoxId box) {
    if (!NWC24IsMsgLibOpened() && !NWC24IsMsgLibOpenedByTool()) {
        return NWC24_ERR_LIB_NOT_OPENED;
    }

    if (box != NWC24_MSGBOX_SEND && box != NWC24_MSGBOX_RECV) {
        return NWC24_ERR_INVALID_VALUE;
    }

    MsgList.box = box;
    return NWC24_OK;
}

NWC24Err fn_803A0240(u32 appId) {
    if (!NWC24IsMsgLibOpened() && !NWC24IsMsgLibOpenedByTool()) {
        return NWC24_ERR_LIB_NOT_OPENED;
    }

    MsgList.flags |= 0x20;
    MsgList.appId = appId;
    return NWC24_OK;
}

// does a mail box entry pass the filters that were set up for the list?
static BOOL fn_803A0594(MsgListState *state, NWC24iMBCEntry *entry) {
    u32 flags;
    u32 mode;
    u32 y;
    u32 x;
    BOOL ok;

    if (!fn_8039B4C8(entry)) {
        return FALSE;
    }

    flags = state->flags;
    if ((flags & 1) && !(entry->flags & NWC24_MSGOBJ_FOR_MENU)) {
        return FALSE;
    }

    if ((flags & 0x20) && state->appId != entry->appId) {
        return FALSE;
    }

    if (flags & 0x10) {
        if (!(entry->flags & NWC24_MSGOBJ_FOR_RECIPIENT)) {
            return FALSE;
        }

        if (state->from != entry->from.id) {
            return FALSE;
        }
    }

    if (flags & 0x100) {
        mode = state->tagFilter >> 16;
        x = state->tagFilter & 0xFFFF;
        y = entry->tag & 0xFFFF;

        switch (mode) {
        case 0:
            ok = x != y;
            break;
        case 1:
            ok = x == y;
            break;
        case 2:
            ok = x >= y;
            break;
        case 3:
            ok = x <= y;
            break;
        case 4:
            ok = (x & y) != 0;
            break;
        default:
            ok = FALSE;
            break;
        }

        if (!ok) {
            return FALSE;
        }
    }

    return TRUE;
}

// reads the control file of the chosen box and writes the ids of everything that passes the filters
NWC24Err fn_803A03F8(MsgListState *state, u32 *ids) {
    u32 matched = 0;
    u32 numMsgs;
    NWC24File file;
    NWC24iMBCHeader *hdr;
    NWC24Err result;
    NWC24Err err;
    char *path;
    u32 off;

    state->count = 0;
    state->total = 0;

    err = fn_8039B1C0(state->box, &numMsgs);
    if (err != NWC24_OK) {
        return err;
    }

    if (numMsgs == 0) {
        return NWC24_OK;
    }

    if (state->box == NWC24_MSGBOX_SEND) {
        hdr = &NWC24WorkP->sendCtrl;
    } else if (state->box == NWC24_MSGBOX_RECV) {
        hdr = &NWC24WorkP->recvCtrl;
    }

    path = NWC24WorkP->pathWork;
    err = NWC24iMBoxGetCtrlFilePath(state->box, path, NWC24i_PATH_WORK_SIZE);
    if (err != NWC24_OK) {
        return err;
    }

    err = NWC24FOpen(&file, path, NWC24_OPEN_READ);
    if (err != NWC24_OK) {
        return err;
    }

    result = NWC24_ERR_BROKEN;
    for (off = MBC_FIRST_ENTRY; off < hdr->fileSize; off += MBC_BLOCK_SIZE) {
        NWC24FSeek(&file, off, NWC24_SEEK_BEG);
        err = NWC24FRead(&ListEntry, MBC_BLOCK_SIZE, &file);
        if (err != NWC24_OK) {
            result = err;
            break;
        }

        if (ListEntry.id != 0 && ListEntry.id <= 1000000) {
            if (fn_803A0594(state, &ListEntry)) {
                ids[state->count++] = ListEntry.id;
            }

            matched++;
            if (matched == numMsgs) {
                result = NWC24_OK;
                break;
            }
        }
    }

    NWC24FClose(&file);
    if (result != NWC24_OK) {
    }

    state->total = state->count;
    return NWC24_OK;
}

// hands out the next batch of messages from the list
NWC24Err fn_803A02A4(NWC24iMsgObj *out, u32 max, u32 *count, u32 *remaining) {
    MsgListState *state = &MsgList;
    NWC24Err result;
    u32 i;
    u32 *ids;

    if (!NWC24IsMsgLibOpened() && !NWC24IsMsgLibOpenedByTool()) {
        return NWC24_ERR_LIB_NOT_OPENED;
    }

    *count = 0;
    *remaining = 0;
    ids = (u32 *)NWC24WorkP->WORK_0x1400;
    if (out == NULL && max != 0) {
        return NWC24_ERR_NULL;
    }

    if (state->valid == 0) {
        Mail_memset(ids, 0, 0x1000);
        result = fn_803A03F8(state, ids);
        if (result != NWC24_OK) {
            return result;
        }

        state->valid = 1;
    }

    if (state->total == 0) {
        return NWC24_OK;
    }

    for (i = 0; i < state->total; i++) {
        if (i >= max) {
            break;
        }

        result = fn_8039B244(out, state->box, ids[state->pos++]);
        if (result != NWC24_OK) {
            return result;
        }

        (*count)++;
        out++;
    }

    state->total -= *count;
    *remaining = state->total;
    return NWC24_OK;
}

/* ---------------------------------------------------------------------------------------------
 * committing a message: builds the mail text (smtp envelope + headers + body parts) in the mailbox file
 * ------------------------------------------------------------------------------------------- */

// the file the message text is currently being written into
static NWC24File *OutFile;

// the pieces of the multipart headers, kept as pointers because that is what the sdk did
static char *PartTypeFmt = "Content-Type: %s;\r\n name=%c%07d.%s\r\n";
static char *PartEncodingFmt = "Content-Transfer-Encoding: base64\r\n";
static char *PartDispositionFmt = "Content-Disposition: attachment;\r\n filename=%c%07d.%s\r\n\r\n";
static char *TextTypeFmt = "Content-Type: text/plain; charset=%s\r\n";
static char *TextEncodingFmt = "Content-Transfer-Encoding: %s\r\n\r\n";

// HYPOTHESIS: a switch somewhere, a message sent to your own id lands in the receive box
static BOOL SendToSelfIsReceive = TRUE;

// writes a mail address, either "w<id>@domain" or a plain string
NWC24Err fn_8039EDE8(NWC24iAddr *addr, u32 flags, char *dst, int maxlen, int *written) {
    NWC24Err result = NWC24_OK;
    char *domain;
    char idStr[24];
    int n;

    if (flags & 1) {
        domain = (char *)NWC24GetAccountDomain();
        n = Mail_strlen(domain);
        if (n <= 0) {
            result = NWC24_ERR_CONFIG;
        } else if (n + 0x12 >= maxlen) {
            result = NWC24_ERR_NOMEM;
        } else {
            NWC24iConvIdToStr(addr->id, idStr);
            n = Mail_sprintf(dst, "%c%s%s", 'w', idStr, domain);
            if (n == 0) {
                result = NWC24_ERR_FATAL;
            } else {
                *written = n;
            }
        }
    } else if (flags & 2) {
        if (addr->data.size + 3 >= (u32)maxlen) {
            result = NWC24_ERR_NOMEM;
        } else {
            n = Mail_sprintf(dst, "%s", addr->data.pData);
            if (n == 0) {
                result = NWC24_ERR_FATAL;
            } else {
                *written = n;
            }
        }
    } else {
        result = NWC24_ERR_INVALID_VALUE;
    }

    return result;
}

// MAIL FROM: <sender>
NWC24Err fn_8039EEF8(NWC24iMsgObj *msg) {
    char *buf = NWC24WorkP->stringWork;
    char *p;
    int n;
    int len;
    int mode;
    NWC24Err result;

    Mail_memset(buf, 0, NWC24i_STRING_WORK_SIZE);
    Mail_strncat(buf, "MAIL FROM: ", 0x3FE);
    p = buf + 11;

    mode = 1;
    if (msg->flags & NWC24_MSGOBJ_FLAG_20) {
        mode = 2;
    }

    result = fn_8039EDE8(&msg->from, mode, p, 0x3FE, &n);
    if (result == NWC24_OK) {
        p += n;
        p[0] = '\r';
        p[1] = '\n';
        len = n + 13;
        if (0x3FE - len <= 0) {
            return NWC24_ERR_NOMEM;
        }
    }

    if (result == NWC24_OK) {
        result = NWC24FWrite(NWC24WorkP, len, OutFile);
        if (result == NWC24_OK) {
            msg->length += len;
        }
    }

    return result;
}

// RCPT TO: <rcpt> for everybody in the to list
NWC24Err fn_8039EFE0(NWC24iMsgObj *msg) {
    char *buf = NWC24WorkP->stringWork;
    int n;
    NWC24Err result = NWC24_OK;
    char *p = buf;
    u32 i;
    int avail = 0x3FE;
    u32 total = 0;
    NWC24iAddr *addr = msg->to;

    Mail_memset(buf, 0, NWC24i_STRING_WORK_SIZE);
    for (i = 0; i < msg->numTo; i++, addr++) {
        Mail_strncat(p, "RCPT TO: ", avail);
        p += 9;

        result = fn_8039EDE8(addr, msg->flags, p, avail, &n);
        if (result != NWC24_OK) {
            break;
        }

        p += n;
        *p = '\r';
        p[1] = '\n';
        p += 2;

        avail -= n + 11;
        total += n + 11;
        if (avail <= 0) {
            result = NWC24_ERR_NOMEM;
            break;
        }
    }

    if (result == NWC24_OK) {
        result = NWC24FWrite(NWC24WorkP, total, OutFile);
        if (result == NWC24_OK) {
            msg->length += total;
        }
    }

    return result;
}

// From:
NWC24Err fn_8039F0F4(NWC24iMsgObj *msg) {
    char *buf = NWC24WorkP->stringWork;
    char *p;
    int n;
    int len;
    int mode;
    NWC24Err result;

    Mail_memset(buf, 0, NWC24i_STRING_WORK_SIZE);
    Mail_strncat(buf, "From: ", 0x3FE);
    p = buf + 6;
    msg->fromField.offset = msg->length + 6;

    mode = 1;
    if (msg->flags & NWC24_MSGOBJ_FLAG_20) {
        mode = 2;
    }

    result = fn_8039EDE8(&msg->from, mode, p, 0x3FE, &n);
    if (result == NWC24_OK) {
        p += n;
        p[0] = '\r';
        p[1] = '\n';
        len = n + 8;
        if (0x3FE - len <= 0) {
            return NWC24_ERR_NOMEM;
        }
    }

    if (result == NWC24_OK) {
        result = NWC24FWrite(NWC24WorkP, len, OutFile);
        if (result == NWC24_OK) {
            u32 end = msg->length + len;

            msg->length = end;
            msg->fromField.size = end - msg->fromField.offset - 2;
        }
    }

    return result;
}

// To: (comma separated, folded)
NWC24Err fn_8039F1F4(NWC24iMsgObj *msg) {
    int i;
    int n;
    char *p;
    char *buf = NWC24WorkP->stringWork;
    int total;
    int avail;
    NWC24iAddr *addr;
    NWC24Err result = NWC24_OK;

    Mail_memset(buf, 0, NWC24i_STRING_WORK_SIZE);
    Mail_strncat(buf, "To: ", NWC24i_STRING_WORK_SIZE);
    msg->toField.offset = msg->length + 4;

    p = buf + 4;
    addr = msg->to;
    total = 4;
    avail = 0x3FC;
    for (i = 0; i < msg->numTo; i++, addr++) {
        result = fn_8039EDE8(addr, msg->flags, p, avail, &n);
        if (result != NWC24_OK) {
            break;
        }

        avail -= n;
        p += n;
        total += n;
        if (avail <= 4) {
            result = NWC24_ERR_NOMEM;
            break;
        }

        if (i < msg->numTo - 1) {
            p[0] = ',';
            p[1] = '\r';
            p[2] = '\n';
            p[3] = ' ';
            total += 4;
            avail -= 4;
            p += 4;
        }
    }

    p[0] = '\r';
    p[1] = '\n';

    if (result == NWC24_OK) {
        result = NWC24FWrite(NWC24WorkP, total + 2, OutFile);
        if (result == NWC24_OK) {
            u32 end = msg->length + total + 2;

            msg->length = end;
            msg->toField.size = end - msg->toField.offset - 2;
        }
    }

    return result;
}

static char *MonthNames[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

extern void fn_8038EC6C(OSCalendarTime *ct);
NWC24Err fn_803A1344(NWC24Date *date, s32 minutes);

// Date:
NWC24Err fn_8039F35C(NWC24iMsgObj *msg) {
    NWC24Date date;
    OSCalendarTime ct;
    char *buf;
    int len;
    NWC24Err result;

    NWC24Date_Init(&date);
    fn_8038EC6C(&ct);

    date.year = ct.year;
    date.month = ct.mon + 1;
    date.day = ct.mday;
    date.hour = ct.hour;
    date.min = ct.min;
    date.sec = ct.sec;

    if (date.month > 12) {
        return NWC24_ERR_FORMAT;
    }

    NWC24iDateToMinutes(&msg->createTime, &date);
    if (msg->flags & 0x1000000) {
        fn_803A1344(&date, msg->unk2C);
    }

    buf = NWC24WorkP->stringWork;
    Mail_memset(buf, 0, NWC24i_STRING_WORK_SIZE);
    Mail_sprintf(buf, "Date: %02d %s %d %02d:%02d:%02d -0000\r\n", date.day, MonthNames[date.month - 1], date.year, date.hour,
                 date.min, date.sec);

    len = Mail_strlen(buf);
    result = NWC24FWrite(buf, len, OutFile);
    if (result == NWC24_OK) {
        msg->length += len;
    }

    return result;
}

// X-Wii-AppId:
NWC24Err fn_8039F494(NWC24iMsgObj *msg) {
    char *buf = NWC24WorkP->stringWork;
    u32 flags = 0;
    char tmp[16];
    int len;
    NWC24Err result;

    Mail_memset(buf, 0, NWC24i_STRING_WORK_SIZE);
    Mail_strcpy(buf, "X-Wii-AppId: ");

    if (!NWC24IsMsgLibOpenedByTool() && NWC24GetAppId() != NWC24i_APP_ID_IPL && msg->appId != FOURCC('H', 'A', 'C', 'A')) {
        msg->appId = NWC24GetAppId();
    }

    if (msg->flags & NWC24_MSGOBJ_FOR_APP) {
        flags |= 1;
    }

    if (msg->flags & NWC24_MSGOBJ_FOR_MENU) {
        flags |= 2;
    }

    Mail_memset(tmp, 0, sizeof(tmp));
    Mail_sprintf(tmp, "%d-%08X-%04X\r\n", flags, msg->appId, msg->groupId);
    Mail_strcat(buf, tmp);

    len = Mail_strlen(buf);
    result = NWC24FWrite(buf, len, OutFile);
    if (result == NWC24_OK) {
        msg->length += len;
    }

    return result;
}

// X-WiiFace: (base64, folded)
NWC24Err fn_8039F5B0(NWC24iMsgObj *msg) {
    int remaining;
    u32 n;
    int pos;
    const u8 *data;
    NWC24Err result;
    int chunk;
    char *buf;

    if (msg->face.size == 0) {
        return NWC24_OK;
    }

    buf = NWC24WorkP->stringWork;
    Mail_memset(buf, 0, NWC24i_STRING_WORK_SIZE);
    Mail_strcpy(buf, "X-WiiFace:");

    data = (const u8 *)msg->face.pData;
    remaining = msg->face.size;
    pos = 10;
    while (remaining > 0) {
        buf[pos++] = ' ';
        chunk = remaining;
        if (chunk > 42) {
            chunk = 42;
        }

        NWC24Base64Encode(data, chunk, buf + pos, 0x4C, &n);
        data += chunk;
        remaining -= chunk;
        pos += n;
        buf[pos++] = '\r';
        buf[pos++] = '\n';
    }

    result = NWC24FWrite(buf, pos, OutFile);
    if (result == NWC24_OK) {
        msg->length += pos;
    }

    return result;
}

// X-Wii-AltName: (base64, folded)
NWC24Err fn_8039F6B0(NWC24iMsgObj *msg) {
    char *buf;
    const u8 *data;
    int pos;
    int remaining;
    int chunk;
    u32 n;
    NWC24Err result;

    if (msg->alt.size == 0) {
        return NWC24_OK;
    }

    buf = NWC24WorkP->stringWork;
    Mail_memset(buf, 0, NWC24i_STRING_WORK_SIZE);
    Mail_strcpy(buf, "X-Wii-AltName:");

    data = (const u8 *)msg->alt.pData;
    remaining = msg->alt.size;
    pos = 14;
    while (remaining > 0) {
        buf[pos++] = ' ';
        chunk = remaining;
        if (chunk > 42) {
            chunk = 42;
        }

        NWC24Base64Encode(data, chunk, buf + pos, 0x4C, &n);
        data += chunk;
        remaining -= chunk;
        pos += n;
        buf[pos++] = '\r';
        buf[pos++] = '\n';
    }

    result = NWC24FWrite(buf, pos, OutFile);
    if (result == NWC24_OK) {
        msg->length += pos;
    }

    return result;
}

// header of one attachment part
NWC24Err fn_8039F7B0(NWC24iMsgObj *msg, int idx) {
    char *buf;
    char *mimeStr;
    char *suffix;
    int n1;
    int n2;
    int n3;
    int total;
    NWC24Err result;

    Mail_memset(NWC24WorkP->stringWork, 0, NWC24i_STRING_WORK_SIZE);
    buf = NWC24WorkP->stringWork;

    mimeStr = (char *)NWC24GetMIMETypeStr(msg->attachedType[idx]);
    suffix = (char *)NWC24iGetMIMETypeSuffix(msg->attachedType[idx]);

    if (Mail_strlen(PartTypeFmt) + Mail_strlen(PartEncodingFmt) + Mail_strlen(PartDispositionFmt) + Mail_strlen(mimeStr) + 4 >=
        NWC24i_STRING_WORK_SIZE) {
        return NWC24_ERR_NOMEM;
    }

    n1 = Mail_sprintf(buf, PartTypeFmt, mimeStr, (char)('a' + idx), msg->id, suffix);
    if (n1 <= 0) {
        return NWC24_ERR_FAILED;
    }

    buf += n1;
    n2 = Mail_sprintf(buf, PartEncodingFmt);
    if (n2 <= 0) {
        return NWC24_ERR_FAILED;
    }

    total = n1 + n2;
    n3 = Mail_sprintf(buf + n2, PartDispositionFmt, (char)('a' + idx), msg->id, suffix);
    if (n3 <= 0) {
        return NWC24_ERR_FAILED;
    }

    total += n3;
    result = NWC24FWrite(NWC24WorkP, total, OutFile);
    if (result == NWC24_OK) {
        msg->length += total;
    }

    return result;
}

// headers for the text part (charset + transfer encoding)
NWC24Err fn_8039F920(NWC24iMsgObj *msg) {
    char *buf = NWC24WorkP->stringWork;
    int len = 0;
    char *name;
    NWC24Err result;

    Mail_memset(buf, 0, NWC24i_STRING_WORK_SIZE);

    name = (char *)NWC24GetCharsetStr(msg->charset);
    if (name != NULL) {
        Mail_sprintf(buf, TextTypeFmt, name);
        msg->contentType.offset = msg->length + 0x22;
        msg->contentType.size = Mail_strlen(name) + 1;
        len = Mail_strlen(buf);
        buf += len;
    }

    name = (char *)NWC24GetEncodingStr(msg->encoding);
    if (name != NULL) {
        Mail_sprintf(buf, TextEncodingFmt, name);
        msg->txEncoding.offset = len + msg->length + 0x1B;
        msg->txEncoding.size = Mail_strlen(name) + 1;
        len += Mail_strlen(buf);
    }

    if (len == 0) {
        return NWC24_OK;
    }

    result = NWC24FWrite(NWC24WorkP, len, OutFile);
    if (result == NWC24_OK) {
        msg->length += len;
    }

    return result;
}

NWC24Err fn_8039FB14(const u8 *data, int size, u32 *written);
NWC24Err fn_8039FC78(const char *data, int size, u32 *written);

// the body text
NWC24Err fn_8039FA54(NWC24iMsgObj *msg) {
    NWC24Err result;
    u32 written;

    if (msg->text.size == 0) {
        return NWC24_ERR_NULL;
    }

    switch (msg->encoding) {
    case NWC24_ENC_7BIT:
    case NWC24_ENC_8BIT:
        result = NWC24FWrite(msg->text.pData, msg->text.size, OutFile);
        written = msg->text.size;
        break;
    case NWC24_ENC_BASE64:
        result = fn_8039FB14((const u8 *)msg->text.pData, msg->text.size, &written);
        break;
    case NWC24_ENC_QUOTED_PRINTABLE:
        result = fn_8039FC78((const char *)msg->text.pData, msg->text.size, &written);
        break;
    default:
        result = NWC24_ERR_INVALID_VALUE;
        break;
    }

    if (result == NWC24_OK) {
        msg->dwcId.offset = msg->text.size;
        msg->length += written;
    }

    return result;
}

// base64 text, 57 bytes (76 chars) per line
NWC24Err fn_8039FB14(const u8 *data, int size, u32 *written) {
    char *buf = NWC24WorkP->stringWork;
    u32 total = 0;
    NWC24Err result = NWC24_OK;
    u32 n;

    while (size >= 57) {
        Mail_memset(buf, 0, NWC24i_STRING_WORK_SIZE);
        result = NWC24Base64Encode(data, 57, buf, 0x4C, &n);
        if (result != NWC24_OK) {
            break;
        }

        buf[n++] = '\r';
        buf[n++] = '\n';
        buf[n] = 0;
        result = NWC24FWrite(buf, n, OutFile);
        if (result != NWC24_OK) {
            break;
        }

        data += 57;
        size -= 57;
        total += n;
    }

    if (result == NWC24_OK) {
        Mail_memset(buf, 0, NWC24i_STRING_WORK_SIZE);
        result = NWC24Base64Encode(data, size, buf, 0x4C, &n);
        if (result == NWC24_OK) {
            buf[n++] = '\r';
            buf[n++] = '\n';
            buf[n++] = '\r';
            buf[n++] = '\n';
            buf[n] = 0;
            result = NWC24FWrite(buf, n, OutFile);
            if (result == NWC24_OK) {
                total += n;
            }
        }
    }

    *written = total;
    return result;
}

// quoted printable text, the soft line break has to go out between the chunks
NWC24Err fn_8039FC78(const char *data, int size, u32 *written) {
    char *buf = NWC24WorkP->stringWork;
    NWC24Err result = NWC24_OK;
    u32 total = 0;
    u32 inN;
    u32 outN;

    while (size > 0) {
        if (total > 0) {
            Mail_memcpy(buf, "=\r\n", 3);
            result = NWC24FWrite(buf, 3, OutFile);
            if (result != NWC24_OK) {
                break;
            }

            total += 3;
        }

        Mail_memset(buf, 0, NWC24i_STRING_WORK_SIZE);
        result = NWC24EncodeQuotedPrintable(buf, 0x400, &outN, (const u8 *)data, size, &inN);
        if (result != NWC24_OK && result != NWC24_ERR_OVERFLOW) {
            break;
        }

        result = NWC24FWrite(buf, outN, OutFile);
        if (result != NWC24_OK) {
            break;
        }

        data += inN;
        size -= inN;
        total += outN;
    }

    *written = total;
    return result;
}

// writes a buffer into the message file and counts it
static inline NWC24Err WriteBuf_i(NWC24iMsgObj *msg, char *buf, int len) {
    NWC24Err result = NWC24FWrite(buf, len, OutFile);

    if (result == NWC24_OK) {
        msg->length += len;
    }

    return result;
}

// also used to separate parts of the message, "boundary" goes in here
static char BoundaryStr[40];

NWC24Err NWC24CommitMsgInternal(NWC24iMsgObj *msg, NWC24MsgBoxId id) {
    NWC24File file;
    NWC24Data subject;
    NWC24Data text;
    NWC24Data attached[NWC24i_MSG_ATTACHMENT_MAX];
    NWC24UserId myId;
    u32 msgId;
    char tmp[32];
    char *work;
    char *domain;
    NWC24Err result;
    NWC24Err err;
    NWC24Err addErr;
    int len;
    int i;
    u32 mb;

    NWC24Data_Init(&subject);
    NWC24Data_Init(&text);
    for (i = 0; i < NWC24i_MSG_ATTACHMENT_MAX; i++) {
        NWC24Data_Init(&attached[i]);
    }

    NWC24iSetErrorCode(0);

    result = fn_8039E9CC(msg);
    if (result != NWC24_OK) {
        goto done;
    }

    result = fn_8039EA6C(msg, id);
    if (result != NWC24_OK) {
        goto done;
    }

    err = NWC24iMBoxOpenNewMsg(id, &file, &msgId);
    if (err != NWC24_OK) {
        result = err;
        goto finish;
    }

    msg->id = msgId;
    msg->length = 0;
    OutFile = &file;

    if (id == NWC24_MSGBOX_SEND) {
        err = fn_8039EEF8(msg);
        if (err != NWC24_OK) {
            NWC24FClose(&file);
            result = err;
            goto finish;
        }

        err = fn_8039EFE0(msg);
        if (err != NWC24_OK) {
            NWC24FClose(&file);
            result = err;
            goto finish;
        }

        work = NWC24WorkP->stringWork;
        Mail_strcpy(work, "DATA\r\n");
        err = WriteBuf_i(msg, work, Mail_strlen(work));
        if (err != NWC24_OK) {
            NWC24FClose(&file);
            result = err;
            goto finish;
        }
    }

    err = fn_8039F35C(msg);
    if (err != NWC24_OK) {
        NWC24FClose(&file);
        result = err;
        goto finish;
    }

    err = fn_8039F0F4(msg);
    if (err != NWC24_OK) {
        NWC24FClose(&file);
        result = err;
        goto finish;
    }

    err = fn_8039F1F4(msg);
    if (err != NWC24_OK) {
        NWC24FClose(&file);
        result = err;
        goto finish;
    }

    // Message-Id
    work = NWC24WorkP->stringWork;
    domain = (char *)NWC24GetAccountDomain();
    NWC24GetMyUserId(&myId);
    Mail_memset(work, 0, NWC24i_STRING_WORK_SIZE);
    Mail_strcpy(work, "Message-Id: <");
    Mail_memset(tmp, 0, 0x20);
    Mail_sprintf(tmp, "%05X%08X%08X%08X", msg->id, (u32)(myId >> 32), (u32)myId, msg->createTime);
    Mail_strcat(work, tmp);
    Mail_strcat(work, domain);
    Mail_strcat(work, ">\r\n");
    len = Mail_strlen(work);
    err = WriteBuf_i(msg, work, len);
    if (err != NWC24_OK) {
        NWC24FClose(&file);
        result = err;
        goto finish;
    }

    // Subject
    subject.offset = msg->length + 9;
    if (msg->subject.size == 0) {
        err = NWC24_ERR_NULL;
    } else {
        work = NWC24WorkP->stringWork;
        Mail_memset(work, 0, NWC24i_STRING_WORK_SIZE);
        Mail_strcpy(work, "Subject: ");
        Mail_strncat(work, (const char *)msg->subject.pData, 0x3FD);
        Mail_strncat(work, "\r\n", NWC24i_STRING_WORK_SIZE);
        len = STD_strnlen(work, NWC24i_STRING_WORK_SIZE);
        err = WriteBuf_i(msg, work, len);
    }

    if (err == NWC24_OK) {
        subject.size = msg->length - subject.offset - 2;
    } else if (err == NWC24_ERR_NULL) {
        err = NWC24_OK;
        subject.offset = 0;
    }

    if (err != NWC24_OK) {
        NWC24FClose(&file);
        result = err;
        goto finish;
    }

    if (msg->flags & NWC24_MSGOBJ_FOR_RECIPIENT) {
        err = fn_8039F494(msg);
        if (err != NWC24_OK) {
            NWC24FClose(&file);
            result = err;
            goto finish;
        }

        if (msg->command == 0) {
            err = NWC24_OK;
        } else {
            work = NWC24WorkP->stringWork;
            Mail_memset(work, 0, NWC24i_STRING_WORK_SIZE);
            Mail_sprintf(work, "X-Wii-Cmd: %08X\r\n", msg->command);
            len = Mail_strlen(work);
            err = WriteBuf_i(msg, work, len);
        }

        if (err != NWC24_OK) {
            NWC24FClose(&file);
            result = err;
            goto finish;
        }

        if (msg->tag == 0) {
            err = NWC24_OK;
        } else {
            work = NWC24WorkP->stringWork;
            Mail_memset(work, 0, NWC24i_STRING_WORK_SIZE);
            Mail_sprintf(work, "X-Wii-Tag: %08X\r\n", msg->tag);
            len = Mail_strlen(work);
            err = WriteBuf_i(msg, work, len);
        }

        if (err != NWC24_OK) {
            NWC24FClose(&file);
            result = err;
            goto finish;
        }

        if (!(msg->flags & NWC24_MSGOBJ_DWC)) {
            err = NWC24_OK;
        } else {
            work = NWC24WorkP->stringWork;
            Mail_memset(work, 0, NWC24i_STRING_WORK_SIZE);
            Mail_sprintf(work, "X-Wii-DWCId: %08X\r\n", msg->dwcId.size);
            len = Mail_strlen(work);
            err = WriteBuf_i(msg, work, len);
        }

        if (err != NWC24_OK) {
            NWC24FClose(&file);
            result = err;
            goto finish;
        }

        err = fn_8039F6B0(msg);
        if (err != NWC24_OK) {
            NWC24FClose(&file);
            result = err;
            goto finish;
        }

        err = fn_8039F5B0(msg);
        if (err != NWC24_OK) {
            NWC24FClose(&file);
            result = err;
            goto finish;
        }

        if (msg->iconNew == 0 || msg->iconNew == 0x80000000) {
            err = NWC24_OK;
        } else {
            work = NWC24WorkP->stringWork;
            Mail_memset(work, 0, NWC24i_STRING_WORK_SIZE);
            Mail_sprintf(work, "X-Wii-IconNew: %08X\r\n", msg->iconNew);
            len = Mail_strlen(work);
            err = WriteBuf_i(msg, work, len);
        }

        if (err != NWC24_OK) {
            NWC24FClose(&file);
            result = err;
            goto finish;
        }

        if (msg->msgBoard != 0) {
            if (!(msg->msgBoard & 0x80000000)) {
                err = NWC24_OK;
            } else {
                work = NWC24WorkP->stringWork;
                Mail_memset(work, 0, NWC24i_STRING_WORK_SIZE);
                Mail_strcpy(work, "X-Wii-MB-NoReply: -\r\n");
                err = NWC24FWrite(work, 0x15, OutFile);
                if (err == NWC24_OK) {
                    msg->length += 0x15;
                }
            }

            if (err != NWC24_OK) {
                NWC24FClose(&file);
                result = err;
                goto finish;
            }

            mb = msg->msgBoard & 0xFFFF;
            if (mb == 0) {
                err = NWC24_OK;
            } else {
                work = NWC24WorkP->stringWork;
                Mail_memset(work, 0, NWC24i_STRING_WORK_SIZE);
                Mail_sprintf(work, "X-Wii-MB-RegDate: %04X\r\n", mb);
                len = STD_strnlen(work, NWC24i_STRING_WORK_SIZE);
                err = WriteBuf_i(msg, work, len);
            }

            if (err != NWC24_OK) {
                NWC24FClose(&file);
                result = err;
                goto finish;
            }

            mb = msg->msgBoard & 0xFF0000;
            if (mb == 0) {
                err = NWC24_OK;
            } else {
                work = NWC24WorkP->stringWork;
                Mail_memset(work, 0, NWC24i_STRING_WORK_SIZE);
                Mail_sprintf(work, "X-Wii-MB-Delay: %02X\r\n", mb >> 16);
                len = STD_strnlen(work, NWC24i_STRING_WORK_SIZE);
                err = WriteBuf_i(msg, work, len);
            }

            if (err != NWC24_OK) {
                NWC24FClose(&file);
                result = err;
                goto finish;
            }
        }
    }

    // raw extra headers from the app
    if (msg->user.size == 0) {
        err = NWC24_OK;
    } else {
        err = NWC24FWrite(msg->user.pData, msg->user.size, OutFile);
        if (err == NWC24_OK) {
            msg->length += msg->user.size;
        }
    }

    if (err != NWC24_OK) {
        NWC24FClose(&file);
        result = err;
        goto finish;
    }

    Mail_memset(BoundaryStr, 0, 0x28);
    Mail_sprintf(BoundaryStr, "Boundary-NWC24-%08X%05X", msg->createTime, msg->id);

    work = NWC24WorkP->stringWork;
    Mail_memset(work, 0, NWC24i_STRING_WORK_SIZE);
    Mail_strcpy(work, "MIME-Version: 1.0\r\n");
    if (msg->flags & NWC24_MSGOBJ_MULTIPART) {
        Mail_strcat(work, "Content-Type: multipart/mixed;\r\n boundary=\"");
        Mail_strcat(work, BoundaryStr);
        Mail_strcat(work, "\"\r\n");
    }

    len = Mail_strlen(work);
    result = WriteBuf_i(msg, work, len);
    if (result != NWC24_OK) {
        NWC24FClose(&file);
        goto finish;
    }

    if (msg->flags & NWC24_MSGOBJ_MULTIPART) {
        msg->unk10 = msg->length;
        work = NWC24WorkP->stringWork;
        Mail_memset(work, 0, NWC24i_STRING_WORK_SIZE);
        Mail_sprintf(work, "\r\n--%s", BoundaryStr);
        Mail_strcat(work, "\r\n");
        len = Mail_strlen(work);
        result = WriteBuf_i(msg, work, len);
        if (result != NWC24_OK) {
            NWC24FClose(&file);
            goto finish;
        }
    }

    err = fn_8039F920(msg);
    if (err != NWC24_OK) {
        NWC24FClose(&file);
        result = err;
        goto finish;
    }

    if (!(msg->flags & NWC24_MSGOBJ_MULTIPART)) {
        msg->unk10 = msg->length;
    }

    text.offset = msg->length;
    err = fn_8039FA54(msg);
    if (err == NWC24_ERR_NULL) {
        err = NWC24_OK;
    }

    if (err != NWC24_OK) {
        NWC24FClose(&file);
        result = err;
        goto finish;
    }

    text.size = msg->length - text.offset;

    for (i = 0; i < msg->numAttached; i++) {
        work = NWC24WorkP->stringWork;
        Mail_memset(work, 0, NWC24i_STRING_WORK_SIZE);
        Mail_sprintf(work, "\r\n--%s", BoundaryStr);
        Mail_strcat(work, "\r\n");
        len = Mail_strlen(work);
        err = WriteBuf_i(msg, work, len);
        if (err != NWC24_OK) {
            NWC24FClose(&file);
            result = err;
            goto finish;
        }

        err = fn_8039F7B0(msg, i);
        if (err != NWC24_OK) {
            NWC24FClose(&file);
            result = err;
            goto finish;
        }

        attached[i].offset = msg->length;
        {
            u32 written;

            err = fn_8039FB14((const u8 *)msg->attached[i].pData, msg->attached[i].size, &written);
            if (err == NWC24_OK) {
                msg->length += written;
            }
        }

        if (err != NWC24_OK) {
            NWC24FClose(&file);
            result = err;
            goto finish;
        }

        attached[i].size = msg->length - attached[i].offset;

        if (i == msg->numAttached - 1) {
            work = NWC24WorkP->stringWork;
            Mail_memset(work, 0, NWC24i_STRING_WORK_SIZE);
            Mail_sprintf(work, "\r\n--%s", BoundaryStr);
            Mail_strcat(work, "--\r\n");
            len = Mail_strlen(work);
            err = WriteBuf_i(msg, work, len);
            if (err != NWC24_OK) {
                NWC24FClose(&file);
                result = err;
                goto finish;
            }
        }
    }

    result = NWC24_OK;

finish:
    if (result == NWC24_OK) {
        err = NWC24iMBoxCloseMsg(&file);
    } else {
        NWC24iMBoxCancelMsg(&file, id, msgId);
        NWC24iMBoxFlushHeader(id);
        goto done;
    }

    msg->subject = subject;
    msg->flags |= NWC24_MSGOBJ_DELIVERING;
    msg->text = text;
    msg->attached[0] = attached[0];
    msg->attached[1] = attached[1];

    if (id == NWC24_MSGBOX_RECV) {
        msg->flags |= NWC24_MSGOBJ_TO_RECV;
    } else if (id == NWC24_MSGBOX_SEND) {
        msg->flags |= NWC24_MSGOBJ_TO_SEND;
    }

    addErr = NWC24iMBoxAddMsgObj(id, msg);
    result = err != NWC24_OK ? err : addErr;

    if (id == NWC24_MSGBOX_RECV) {
        u32 arrived = NWC24_MSG_ARRIVED;

        if (msg->flags & NWC24_MSGOBJ_FOR_MENU) {
            arrived |= NWC24_MSG_ARRIVED_FOR_MENU;
        }

        NWC24iSetNewMsgArrived(arrived);
    }

done:
    // a few of these errors are worth showing to the user
    if (result == NWC24_ERR_FULL || (result <= -16 && result >= -21) || result == -38 || result == -41 || result == -43 ||
        result == -46) {
        NWC24iSetErrorCode(result - NWC24i_MSG_ERROR_CODE_BASE);
    }

    return result;
}

NWC24Err NWC24CommitMsg(NWC24MsgObj *pMsg) {
    NWC24iMsgObj *msg = (NWC24iMsgObj *)pMsg;
    NWC24MsgBoxId id = NWC24_MSGBOX_SEND;
    NWC24UserId myId;

    if (!NWC24IsMsgLibOpened() && !NWC24IsMsgLibOpenedByTool()) {
        return NWC24_ERR_LIB_NOT_OPENED;
    }

    if (!(msg->flags & NWC24_MSGOBJ_INITIALIZED) || (msg->flags & NWC24_MSGOBJ_DELIVERING)) {
        return NWC24_ERR_PROTECTED;
    }

    // a message to yourself skips the send box and lands in the receive box directly
    if (SendToSelfIsReceive && (msg->flags & NWC24_MSGOBJ_FOR_RECIPIENT)) {
        NWC24GetMyUserId(&myId);
        if (msg->numTo == 1 && msg->to[0].id == myId) {
            id = NWC24_MSGBOX_RECV;
        }
    }

    return NWC24CommitMsgInternal(msg, id);
}

/* ---------------------------------------------------------------------------------------------
 * dates (days / minutes since 1900, calendar conversions)
 * ------------------------------------------------------------------------------------------- */

static const u8 DaysInMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
static const u16 DaysBeforeMonth[12] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};

#define NWC24i_EPOCH_1900_TO_1970 2208988800LL

static inline BOOL IsLeapYear_i(int year) {
    return (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
}

// days since 1 jan 1900, -1 when the date does not exist
s32 fn_803A1678(u16 year, u8 month, u8 day) {
    s32 days;
    int y;

    if (year < NWC24i_YEAR_MIN || month < NWC24i_MONTH_MIN || month > NWC24i_MONTH_MAX) {
        return -1;
    }

    if (month == 2 && IsLeapYear_i(year)) {
        if (day < NWC24i_DAY_MIN || day > 29) {
            return -1;
        }
    } else if (day < NWC24i_DAY_MIN || DaysInMonth[month - 1] < day) {
        return -1;
    }

    days = DaysBeforeMonth[month - 1] + (day - 1);
    if (month >= 3 && IsLeapYear_i(year)) {
        days++;
    }

    y = year - NWC24i_YEAR_MIN;
    return days + y * 365 + (y + 299) / 400 + ((y - 1) / 4 - (y - 1) / 100);
}

NWC24Err NWC24iDateToMinutes(s32 *minutes, const NWC24Date *date) {
    s32 days;
    s32 minOfDay;

    days = fn_803A1678(date->year, date->month, date->day);
    if (days == -1) {
        return NWC24_ERR_FAILED;
    }

    if (date->hour > 23 || date->min > 59) {
        minOfDay = -1;
    } else {
        minOfDay = date->min + date->hour * 60;
    }

    if (minOfDay == -1 || date->sec > 60) {
        return NWC24_ERR_FAILED;
    }

    *minutes = minOfDay + days * 1440;
    return NWC24_OK;
}

// splits a day count back into year / month / day
void fn_803A183C(u16 *year, u8 *month, u8 *day, int days);

NWC24Err fn_803A1344(NWC24Date *date, s32 minutes) {
    int clamped = minutes;
    int days;
    int rem;

    if (minutes < 0) {
        clamped = 0;
    }

    days = clamped / 1440;
    rem = clamped % 1440;
    date->hour = rem / 60;
    date->min = rem % 60;
    fn_803A183C(&date->year, &date->month, &date->day, days);
    return NWC24_OK;
}

NWC24Err NWC24iEpochSecondsToDate(NWC24Date *date, s64 timestamp) {
    int minutes;

    if (timestamp + NWC24i_EPOCH_1900_TO_1970 < 0) {
        timestamp = -NWC24i_EPOCH_1900_TO_1970;
    }

    date->sec = (timestamp + NWC24i_EPOCH_1900_TO_1970) % 60;
    minutes = (timestamp + NWC24i_EPOCH_1900_TO_1970) / 60;
    if (minutes < 0) {
        minutes = 0;
    }

    date->hour = minutes % 1440 / 60;
    date->min = minutes % 1440 % 60;
    fn_803A183C(&date->year, &date->month, &date->day, minutes / 1440);
    return NWC24_OK;
}

NWC24Err NWC24iDateToOSCalendarTime(OSCalendarTime *ct, const NWC24Date *date) {
    int month = date->month - 1;
    int yday;

    ct->year = date->year;
    ct->mon = month;
    ct->mday = date->day;
    ct->hour = date->hour;
    ct->min = date->min;
    ct->sec = date->sec;
    ct->msec = 0;
    ct->usec = 0;

    yday = DaysBeforeMonth[month] + date->day - 1;
    ct->yday = yday;
    if (IsLeapYear_i(date->year) && date->month > 2) {
        ct->yday++;
    }

    ct->wday = (fn_803A1678(date->year, date->month, date->day) + 1) % 7;
    return NWC24_OK;
}

void fn_803A183C(u16 *year, u8 *month, u8 *day, int days) {
    int saved;
    int ylen;

    *year = NWC24i_YEAR_MIN;
    *month = 1;
    *day = 1;
    if (days < 0) {
        return;
    }

    while (TRUE) {
        saved = days;
        ylen = IsLeapYear_i(*year) ? 366 : 365;
        days -= ylen;
        if (days < 0) {
            days = saved;
            break;
        }

        (*year)++;
    }

    while (TRUE) {
        saved = days;
        if (*month == 2 && IsLeapYear_i(*year)) {
            days -= 29;
        } else {
            days -= DaysInMonth[*month - 1];
        }

        if (days < 0) {
            break;
        }

        (*month)++;
    }

    *day += saved;
}

/* ---------------------------------------------------------------------------------------------
 * friend list accessors (the list file has a 128 bit "slot is used" map at 0x10 and 0x140 byte records)
 * ------------------------------------------------------------------------------------------- */

#define FL_RECORD_SIZE 0x140
#define FL_RECORDS_OFFSET 0x360

NWC24Err fn_803A1CDC(u32 *info, u32 idx);

// reads one record and fills in some of its state from the map
NWC24Err fn_803A19D8(u32 *info, u32 idx) {
    NWC24iFLHeader *hdr;
    NWC24Err result;

    result = fn_803A1CDC(info, idx);
    if (result != NWC24_OK) {
        return result;
    }

    if (info[0] == 1) {
        Mail_memset(&info[0x48 / 4], 0, 0xF8);
    }

    result = GetCachedFLHeader(&hdr);
    if (result == NWC24_OK) {
        result = (((u32 *)hdr->unk10)[idx >> 5] >> (idx & 31)) & 1;
    }

    if (result == 1) {
        info[1] = 2;
    }

    return NWC24_OK;
}

// capacity of the list
NWC24Err fn_803A1A7C(u32 *count) {
    NWC24iFLHeader *hdr;
    NWC24Err result;

    if (!NWC24IsMsgLibOpened() && !NWC24IsMsgLibOpenedByTool()) {
        return NWC24_ERR_LIB_NOT_OPENED;
    }

    result = GetCachedFLHeader(&hdr);
    if (result != NWC24_OK) {
        return result;
    }

    *count = hdr->capacity;
    return NWC24_OK;
}

// how many slots are in use
NWC24Err fn_803A1AE8(u32 *count) {
    NWC24iFLHeader *hdr;
    NWC24Err result;
    u32 used = 0;
    u32 i;
    u32 j;

    if (!NWC24IsMsgLibOpened() && !NWC24IsMsgLibOpenedByTool()) {
        return NWC24_ERR_LIB_NOT_OPENED;
    }

    *count = 0;
    result = GetCachedFLHeader(&hdr);
    if (result != NWC24_OK) {
        return result;
    }

    for (i = 0; i < 4; i++) {
        u32 word = ((u32 *)hdr->unk10)[i];

        for (j = 0; j < 32; j++) {
            if (word & 1) {
                used++;
            }

            word >>= 1;
        }
    }

    *count = used;
    return NWC24_OK;
}

// does this slot hold a friend code?
NWC24Err fn_803A1C10(u32 idx) {
    NWC24iFLHeader *hdr;
    NWC24Err result;

    if (!NWC24IsMsgLibOpened() && !NWC24IsMsgLibOpenedByTool()) {
        return NWC24_ERR_LIB_NOT_OPENED;
    }

    result = GetCachedFLHeader(&hdr);
    if (result != NWC24_OK) {
        return result;
    }

    if (idx >= hdr->capacity) {
        return NWC24_ERR_INVALID_VALUE;
    }

    return hdr->friendCodes[idx] != 0;
}

// reads one record from the list file
NWC24Err fn_803A1CDC(u32 *info, u32 idx) {
    NWC24iFLHeader *hdr;
    NWC24File file;
    NWC24Err result;
    NWC24Err read;

    if (!NWC24IsMsgLibOpened() && !NWC24IsMsgLibOpenedByTool()) {
        return NWC24_ERR_LIB_NOT_OPENED;
    }

    if ((u32)info & 0x1F) {
        return NWC24_ERR_ALIGNMENT;
    }

    result = GetCachedFLHeader(&hdr);
    if (result != NWC24_OK) {
        return result;
    }

    if (idx >= hdr->capacity) {
        return NWC24_ERR_INVALID_VALUE;
    }

    if (hdr->friendCodes[idx] == 0) {
        return NWC24_ERR_NULL;
    }

    result = NWC24FOpen(&file, FLFilePath, NWC24_OPEN_NAND_RBUFF);
    if (result != NWC24_OK) {
        return result;
    }

    NWC24FSeek(&file, idx * FL_RECORD_SIZE + FL_RECORDS_OFFSET, NWC24_SEEK_BEG);
    read = NWC24FRead(info, FL_RECORD_SIZE, &file);
    result = NWC24FClose(&file);
    if (read != NWC24_OK) {
        result = read;
    }

    if (result != NWC24_OK) {
        return result;
    }

    return NWC24_OK;
}

/* ---------------------------------------------------------------------------------------------
 * download task list (/shared2/wc24/nwc24dl.bin)
 * ------------------------------------------------------------------------------------------- */

#define DL_HEADER_SIZE 0x800
#define DL_TASK_SIZE 0x200
#define DL_TASK_FLAG_ANYONE 0x40
#define DL_TASK_FLAG_NO_LIMITS 0x40000000

static const char *DLFilePath = "/shared2/wc24/nwc24dl.bin";

// HYPOTHESIS: when set the interval / count limits are not enforced
static BOOL DlIgnoreLimits;

extern int snprintf(char *, size_t, const char *, ...);
extern unsigned long strtoul(const char *, char **, int);

static inline NWC24iDlEntry *DlEntry_i(u16 i);

static inline BOOL SameAppId_i(u32 a, u32 b) {
    return (a & ~0xFF) == (b & ~0xFF);
}

static inline NWC24iDlHeader *DlHeader_i(void) {
    return NWC24WorkP != NULL ? (NWC24iDlHeader *)NWC24WorkP->dlHeader : NULL;
}

static inline NWC24iDlEntry *DlEntry_i(u16 i) {
    return &DlHeader_i()->entries[i];
}

// the app that owns a task, or one with the same group, may change it
static inline NWC24Err CheckDlTask_i(const NWC24iDlTask *task, BOOL wantWrite) {
    NWC24iDlHeader *hdr = DlHeader_i();

    if (task == NULL) {
        return NWC24_ERR_INVALID_VALUE;
    }

    if (hdr == NULL) {
        return NWC24_ERR_LIB_NOT_OPENED;
    }

    if (wantWrite && !NWC24IsMsgLibOpenedByTool()) {
        u32 appId = task->appId;
        BOOL same = (NWC24GetAppId() & ~0xFF) == (appId & ~0xFF);

        if (!same) {
            BOOL ok = FALSE;
            u16 group = task->groupId;

            if ((task->flags & DL_TASK_FLAG_ANYONE) && group == NWC24GetGroupId()) {
                ok = TRUE;
            }

            if (!ok) {
                return NWC24_ERR_PROTECTED;
            }
        }
    }

    if (task->id != 0xFFFF && task->id >= hdr->maxTasks) {
        return NWC24_ERR_INVALID_VALUE;
    }

    return NWC24_OK;
}

NWC24Err NWC24CheckDlTask(const NWC24DlTask *pTask, BOOL wantWrite) {
    return CheckDlTask_i((const NWC24iDlTask *)pTask, wantWrite);
}

// stamps "now + minutes" into the next-update time of the task's list entry
static inline void SetNextTime_i(NWC24iDlTask *task, u32 minutes) {
    s64 now = 0;
    NWC24Err result;

    result = NWC24iGetUniversalTime(&now);
    if (result >= 0) {
        s64 when = now + task->interval * 60;

        result = CheckDlTask_i(task, FALSE);
        if (result == NWC24_OK) {
            u16 id = task->id;

            if (id != 0xFFFF) {
                DlHeader_i()->entries[id].nextTime = when / 60;
            }
        }
    }
}

// sets up a fresh task of the given type
NWC24Err fn_803A2A18(NWC24iDlTask *task, int type) {
    char dir[64] = {0};
    u32 titleHi;
    u32 titleLo;
    NWC24iDlHeader *hdr;
    NWC24Err result;
    BOOL isOctet;

    NANDGetHomeDir(dir);
    dir[15] = 0;
    titleHi = strtoul(&dir[7], NULL, 16);
    dir[24] = 0;
    titleLo = strtoul(&dir[16], NULL, 16);

    hdr = DlHeader_i();
    if (hdr == NULL) {
        return NWC24_ERR_LIB_NOT_OPENED;
    }

    if (task == NULL) {
        return NWC24_ERR_INVALID_VALUE;
    }

    if (type >= 4) {
        return NWC24_ERR_INVALID_VALUE;
    }

    memset(task, 0, DL_TASK_SIZE);
    task->type = type;
    task->priority = 0x7F;
    task->appId = NWC24GetAppId();
    task->groupId = NWC24GetGroupId();
    task->titleIdHi = titleHi;
    task->titleIdLo = titleLo;
    task->id = 0xFFFF;
    task->count = 1;
    task->interval = 0xB40;
    task->margin = 0x5A0;

    isOctet = type == NWC24_DLTYPE_OCTETSTREAM_V1 || type == NWC24_DLTYPE_OCTETSTREAM_V2;
    if (isOctet) {
        strcpy(task->fileName, "content.bin");
    }

    result = CheckDlTask_i(task, TRUE);
    return result >> 31;
}

// how often (in minutes) the task should run
NWC24Err fn_803A2C58(NWC24iDlTask *task, u32 interval) {
    NWC24Err result;

    result = CheckDlTask_i(task, TRUE);
    if (result != NWC24_OK) {
        return result;
    }

    if (!DlIgnoreLimits && task->id >= DlHeader_i()->privateTasks && !(task->flags & DL_TASK_FLAG_NO_LIMITS)) {
        if (interval < 180 || interval > 10080) {
            return NWC24_ERR_INVALID_VALUE;
        }
    }

    if (task->interval != interval) {
        task->interval = interval;
        if (task->id != 0xFFFF) {
            SetNextTime_i(task, interval);
        }
    }

    return NWC24_OK;
}

// the url to download from, http or https
NWC24Err fn_803A2EA4(NWC24iDlTask *task, const char *url) {
    NWC24Err result;

    result = CheckDlTask_i(task, TRUE);
    if (result != NWC24_OK) {
        return result;
    }

    result = fn_80399EE0(url, 7, 0x100);
    if (result >= 0) {
        if (strncmp(url, "http://", 7) != 0 && strncmp(url, "https://", 8) != 0) {
            result = NWC24_ERR_FORMAT;
        } else {
            result = NWC24_OK;
        }
    }

    if (result < 0) {
        return result;
    }

    if (!DlIgnoreLimits && (task->flags & 4) && strncmp(url, "http://", 7) == 0) {
        return NWC24_ERR_INVALID_VALUE;
    }

    fn_80399F5C(task->url, url, 0xEC);
    return NWC24_OK;
}

// task flags
NWC24Err fn_803A3064(NWC24iDlTask *task, u32 flags) {
    NWC24Err result;

    result = CheckDlTask_i(task, TRUE);
    if (result != NWC24_OK) {
        return result;
    }

    if (!DlIgnoreLimits && (flags & 4) && strncmp(task->url, "http://", 7) == 0) {
        return NWC24_ERR_INVALID_VALUE;
    }

    switch (task->type) {
    case NWC24_DLTYPE_MULTIPART_V1:
    case NWC24_DLTYPE_OCTETSTREAM_V1:
        // the old types know nothing about the newer flag bits
        if ((flags & 0x80000000) | (flags & 0x38)) {
            return -40;
        }
        break;
    default:
        break;
    }

    task->flags = flags;
    return NWC24_OK;
}

// how many times the task may run
NWC24Err fn_803A31D0(NWC24iDlTask *task, int count) {
    NWC24Err result;

    result = CheckDlTask_i(task, TRUE);
    if (result != NWC24_OK) {
        return result;
    }

    if (count < 1 || (task->id >= DlHeader_i()->privateTasks && !(task->flags & DL_TASK_FLAG_NO_LIMITS) && count > 100)) {
        return NWC24_ERR_INVALID_VALUE;
    }

    task->count = count;
    return NWC24_OK;
}

// ---- list entry accessors, they get handed to the sorter below ----

u32 fn_803A5C38(u16 idx) {
    return DlHeader_i()->entries[idx].lastAccess;
}

u32 fn_803A5C60(u16 idx) {
    return DlHeader_i()->entries[idx].nextTime;
}

u8 fn_803A5C88(u16 idx) {
    return DlHeader_i()->entries[idx].flags;
}

// ---- raw file helpers ----

// writes some data (and then some zero padding) at an offset into a file, creating it if needed
NWC24Err fn_803A5A8C(const char *path, u32 offset, const void *data, u32 len, u32 pad) {
    NWC24File file;
    NWC24Err result;
    NWC24Err close;
    u32 i;
    u8 zero;

    result = NWC24FOpen(&file, path, NWC24_OPEN_NAND_WBUFF);
    if (result < 0) {
        return result;
    }

    result = NWC24FSeek(&file, offset, NWC24_SEEK_BEG);
    if (result < 0) {
        return result;
    }

    if (data != NULL && len != 0) {
        result = NWC24FWrite(data, len, &file);
    }

    for (i = 0; i < pad; i++) {
        zero = 0;
        result = NWC24FWrite(&zero, 1, &file);
        if (result < 0) {
            break;
        }
    }

    close = NWC24FClose(&file);
    return result != NWC24_OK ? result : close;
}

// reads some data from an offset in a file
NWC24Err fn_803A5B84(const char *path, u32 offset, void *buf, u32 len) {
    NWC24File file;
    NWC24Err result;
    NWC24Err close;

    result = NWC24FOpen(&file, path, NWC24_OPEN_NAND_RBUFF);
    if (result < 0) {
        return result;
    }

    result = NWC24FSeek(&file, offset, NWC24_SEEK_BEG);
    if (result < 0) {
        return result;
    }

    if (buf != NULL && len != 0) {
        result = NWC24FRead(buf, len, &file);
    }

    close = NWC24FClose(&file);
    return result != NWC24_OK ? result : close;
}

// ---- writing tasks / header back to the list file ----

static inline NWC24Err WriteDlTaskEntry_i(const NWC24iDlTask *task, NWC24File *file) {
    NWC24Err result;
    u16 id = task->id;
    u16 max = DlHeader_i()->maxTasks;

    if (max > NWC24i_DL_TASK_MAX || id >= max) {
        result = NWC24_ERR_INVALID_VALUE;
    } else {
        result = NWC24FSeek(file, DL_HEADER_SIZE + id * DL_TASK_SIZE, NWC24_SEEK_BEG);
    }

    if (result < 0) {
        return result;
    }

    memcpy(NWC24WorkP->dlTask, task, DL_TASK_SIZE);
    result = NWC24FWrite(NWC24WorkP->dlTask, DL_TASK_SIZE, file);
    if (result < 0) {
        return result;
    }

    return NWC24_OK;
}

static inline NWC24Err WriteDlHeader_i(NWC24File *file) {
    NWC24Err result = NWC24FSeek(file, 0, NWC24_SEEK_BEG);

    if (result < 0) {
        return result;
    }

    result = NWC24FWrite(DlHeader_i(), DL_HEADER_SIZE, file);
    if (result < 0) {
        return result;
    }

    return NWC24_OK;
}

// writes one task (and its list entry) out
NWC24Err fn_803A50EC(NWC24iDlTask *task) {
    NWC24File file;
    NWC24Err result;
    NWC24Err close;

    result = NWC24FOpen(&file, DLFilePath, NWC24_OPEN_NAND_RW);
    if (result < 0) {
        return result;
    }

    result = WriteDlTaskEntry_i(task, &file);
    if (result >= 0) {
        NWC24iDlEntry *entry = DlEntry_i(task->id);

        entry->app = task->appId;
        result = NWC24_OK;
        entry->flags = task->priority;
        if (result >= 0) {
            result = WriteDlHeader_i(&file);
        }
    }

    close = NWC24FClose(&file);
    return result != NWC24_OK ? result : close;
}

// clears a task's slot in the file and in the list header
NWC24Err fn_803A58C8(NWC24iDlTask *task) {
    NWC24File file;
    NWC24Err result;
    NWC24Err close;
    u16 id;
    NWC24iDlTask *empty;

    result = NWC24FOpen(&file, DLFilePath, NWC24_OPEN_NAND_RW);
    if (result < 0) {
        return result;
    }

    empty = (NWC24iDlTask *)NWC24WorkP->dlTask;
    id = task->id;
    memset(empty, 0, DL_TASK_SIZE);
    empty->type = 0xFF;
    empty->id = id;
    memset(&NWC24WorkP->dlHeader[sizeof(NWC24iDlEntry) * id + 0x80], 0, sizeof(NWC24iDlEntry));

    result = WriteDlTaskEntry_i(empty, &file);
    if (result >= 0) {
        memset(&NWC24WorkP->dlHeader[sizeof(NWC24iDlEntry) * task->id + 0x80], 0, sizeof(NWC24iDlEntry));
        result = WriteDlHeader_i(&file);
    }

    close = NWC24FClose(&file);
    return result != NWC24_OK ? result : close;
}

// ---- looking at tasks ----

// is there a task in the slot?
static inline NWC24Err CheckDlEntryAvailable_i(u16 i) {
    if (i >= DlHeader_i()->maxTasks || i == 0xFFFF) {
        return NWC24_ERR_INVALID_VALUE;
    }

    if (DlEntry_i(i)->app == 0) {
        return NWC24_ERR_NOT_FOUND;
    }

    return NWC24_OK;
}

// steps *idx to the next used slot (or the first one)
NWC24Err fn_803A3540(u16 *idx, BOOL first) {
    u16 i;

    if (DlHeader_i() == NULL) {
        return NWC24_ERR_LIB_NOT_OPENED;
    }

    if (idx == NULL) {
        return NWC24_ERR_INVALID_VALUE;
    }

    if (first) {
        *idx = 0;
    } else {
        (*idx)++;
    }

    if (*idx >= DlHeader_i()->maxTasks) {
        return NWC24_ERR_DONE;
    }

    for (i = *idx; i < DlHeader_i()->maxTasks; i++) {
        if (CheckDlEntryAvailable_i(i) >= 0) {
            *idx = i;
            return NWC24_OK;
        }
    }

    return NWC24_ERR_DONE;
}

static inline NWC24Err DeleteDlTaskForced_i(NWC24iDlTask *task) {
    NWC24Err result;

    result = CheckDlTask_i(task, FALSE);
    if (result != NWC24_OK) {
        return result;
    }

    result = fn_803A58C8(task);
    if (result >= 0) {
        task->id = 0xFFFF;
    }

    return result;
}

NWC24Err NWC24DeleteDlTaskForced(NWC24DlTask *pTask) {
    return DeleteDlTaskForced_i((NWC24iDlTask *)pTask);
}

static inline NWC24Err GetDlTask_i(NWC24iDlTask *pTask, u16 id) {
    NWC24File file;
    NWC24Err result;
    NWC24Err close;
    u16 max;

    if (DlHeader_i() == NULL) {
        return NWC24_ERR_LIB_NOT_OPENED;
    }

    result = CheckDlEntryAvailable_i(id);
    if (result < 0) {
        return result;
    }

    result = NWC24FOpen(&file, DLFilePath, NWC24_OPEN_NAND_RBUFF);
    if (result < 0) {
        return result;
    }

    max = DlHeader_i()->maxTasks;
    if (max > NWC24i_DL_TASK_MAX || id >= max) {
        result = NWC24_ERR_INVALID_VALUE;
    } else {
        result = NWC24FSeek(&file, DL_HEADER_SIZE + id * DL_TASK_SIZE, NWC24_SEEK_BEG);
    }

    if (result < 0) {
        close = result;
    } else {
        result = NWC24FRead(pTask, DL_TASK_SIZE, &file);
        close = NWC24_OK;
        if (result < 0) {
            close = result;
        }
    }

    result = NWC24FClose(&file);
    return close != NWC24_OK ? close : result;
}

NWC24Err NWC24GetDlTask(NWC24DlTask *pTask, u16 id) {
    return GetDlTask_i((NWC24iDlTask *)pTask, id);
}

// the sorted walk over the task list. the state says what to sort by (last access / next time / flags),
// which direction and where the last call ended
typedef struct DlWalkState {
    u32 order;
    s32 best;
    s32 last;
    s32 lastIdx;
    u32 started;
    u32 valid;
} DlWalkState;

NWC24Err fn_803A367C(DlWalkState *state, u16 *out) {
    typedef u32 (*KeyFn)(u16);
    KeyFn getKey;
    NWC24Err result;
    BOOL desc;
    BOOL found = FALSE;
    u16 idx;
    s32 key;

    if (state->valid == 0) {
        return NWC24_ERR_INVALID_VALUE;
    }

    desc = state->order >> 31;
    switch (state->order & 0xFFFF) {
    case 0:
        getKey = fn_803A5C38;
        break;
    case 1:
        getKey = fn_803A5C60;
        break;
    case 2:
        getKey = (KeyFn)fn_803A5C88;
        break;
    default:
        return NWC24_ERR_INVALID_VALUE;
    }

    if (state->started == 0) {
        result = fn_803A3540(&idx, TRUE);
        while (result >= 0) {
            key = getKey(idx);
            if (key == state->best && state->lastIdx < idx) {
                *out = idx;
                state->best = key;
                state->last = key;
                state->lastIdx = idx;
                return NWC24_OK;
            }

            result = fn_803A3540(&idx, FALSE);
        }
    }

    if (!(state->order & 0x80000000)) {
        state->best = 0x7FFFFFFF;
    } else {
        state->best = 0x80000001;
    }

    result = fn_803A3540(&idx, TRUE);
    while (result >= 0) {
        key = getKey(idx);
        if (desc ? (state->last < key) : (key < state->last)) {
            if (desc ? (key < state->best) : (state->best < key)) {
                found = TRUE;
                *out = idx;
                state->best = key;
                state->lastIdx = idx;
            }
        }

        result = fn_803A3540(&idx, FALSE);
    }

    if (found) {
        state->last = state->best;
        return NWC24_OK;
    }

    state->valid = 0;
    return NWC24_ERR_DONE;
}

// throws the least recently used task out of the list to make space
NWC24Err fn_803A4110(void) {
    DlWalkState state;
    NWC24iDlTask task;
    u16 idx;
    NWC24Err result;
    NWC24Err close;
    NWC24File file;
    u16 max;

    memset(&state, 0, sizeof(state));
    state.best = 0x7FFFFFFF;
    state.last = 0x80000001;
    state.order = 0;
    state.lastIdx = -1;
    state.started = 1;
    state.valid = 1;

    do {
        result = fn_803A367C(&state, &idx);
    } while (result == NWC24_OK && idx < DlHeader_i()->privateTasks);

    if (result >= 0) {
        u16 id = idx;

        if (DlHeader_i() == NULL) {
            result = NWC24_ERR_LIB_NOT_OPENED;
        } else {
            result = CheckDlEntryAvailable_i(id);
            if (result >= 0) {
                result = NWC24FOpen(&file, DLFilePath, NWC24_OPEN_NAND_RBUFF);
                if (result >= 0) {
                    max = DlHeader_i()->maxTasks;
                    if (max > NWC24i_DL_TASK_MAX || id >= max) {
                        result = NWC24_ERR_INVALID_VALUE;
                    } else {
                        result = NWC24FSeek(&file, DL_HEADER_SIZE + id * DL_TASK_SIZE, NWC24_SEEK_BEG);
                    }

                    if (result < 0) {
                        close = result;
                    } else {
                        result = NWC24FRead(&task, DL_TASK_SIZE, &file);
                        close = NWC24_OK;
                        if (result < 0) {
                            close = result;
                        }
                    }

                    result = NWC24FClose(&file);
                    if (close != NWC24_OK) {
                        result = close;
                    }
                }
            }
        }

        if (result >= 0) {
            result = CheckDlTask_i(&task, FALSE);
            if (result == NWC24_OK) {
                result = fn_803A58C8(&task);
                if (result >= 0) {
                    task.id = 0xFFFF;
                }
            }
        }

        return result;
    }

    if (result == NWC24_ERR_DONE) {
        return NWC24_ERR_FAILED;
    }

    return result;
}

// is the sub task counter pointing at a sub task that is switched on? (-39 says "no, try the next one")
static inline NWC24Err SubTaskStatus_i(NWC24iDlTask *task, u8 counter) {
    NWC24Err check = CheckDlTask_i(task, FALSE);

    if (check == NWC24_OK) {
        if (task->subTaskType == 0) {
            check = -40;
        } else if (task->subTaskMask == 0) {
            check = NWC24_ERR_FATAL;
        } else if (counter > 31) {
            check = NWC24_ERR_INVALID_VALUE;
        } else if (!((1 << counter) & task->subTaskMask)) {
            check = -39;
        } else {
            check = NWC24_OK;
        }
    }

    return check;
}

// marks a task as just used: stamps the time, clears the errors, moves on to the next sub task and saves
static inline NWC24Err UpdateDlTask_i(NWC24iDlTask *task) {
    NWC24Err result;
    NWC24Err check;
    s64 now;

    result = CheckDlTask_i(task, TRUE);
    if (result != NWC24_OK) {
        return result;
    }

    if (task->id == 0xFFFF || task->id >= DlHeader_i()->maxTasks) {
        return NWC24_ERR_INVALID_VALUE;
    }

    result = NWC24iGetUniversalTime(&now);
    if (result >= 0) {
        s64 minutes = now;
        u16 id;

        check = CheckDlTask_i(task, FALSE);
        if (check == NWC24_OK) {
            id = task->id;
            if (id == 0xFFFF) {
                check = NWC24_ERR_FAILED;
            } else {
                DlEntry_i(id)->lastAccess = minutes / 60;
                check = NWC24_OK;
            }
        }

        result = check;
    }

    if (result < 0) {
        return result;
    }

    if (CheckDlTask_i(task, TRUE) == NWC24_OK) {
        task->lastError = 0;
        task->errorCount = 0;
    }

    if (task->subTaskType == 1) {
        while ((check = SubTaskStatus_i(task, task->subTaskCounter)) == -39) {
            task->subTaskCounter = (task->subTaskCounter + 1) & 0x1F;
        }

        if (check < 0) {
            return check;
        }
    }

    return fn_803A50EC(task);
}

NWC24Err fn_803A38C0(NWC24iDlTask *task) {
    return UpdateDlTask_i(task);
}

// takes a task for the list: gets a slot for it when it has none yet (kicking the oldest one out when the
// list is full), then saves it
NWC24Err fn_803A5284(NWC24iDlTask *task, u16 first, u16 last) {
    NWC24Err result;
    int url;

    result = CheckDlTask_i(task, TRUE);
    if (result != NWC24_OK) {
        return result;
    }

    if (task->id >= DlHeader_i()->maxTasks && task->id != 0xFFFF) {
        result = NWC24_ERR_INVALID_VALUE;
    } else {
        result = fn_80399EE0(task->url, 7, 0x100);
        if (result >= 0) {
            if (strncmp(task->url, "http://", 7) != 0 && strncmp(task->url, "https://", 8) != 0) {
                result = NWC24_ERR_FORMAT;
            } else {
                result = NWC24_OK;
            }
        }
    }

    if (result < 0) {
        return result;
    }

    for (;;) {
        if (task->id != 0xFFFF) {
            return UpdateDlTask_i(task);
        }

        if (task == NULL || first > last || first >= DlHeader_i()->maxTasks || last > DlHeader_i()->maxTasks) {
            result = NWC24_ERR_INVALID_VALUE;
        } else {
            u16 i;

            result = NWC24_ERR_FULL;
            for (i = first; i < last; i++) {
                if (DlEntry_i(i)->app == 0) {
                    task->id = i;
                    result = NWC24_OK;
                    break;
                }
            }
        }

        if (result == NWC24_ERR_FULL) {
            result = fn_803A4110();
            if (result < 0) {
                return result;
            }
        } else if (result < 0) {
            return result;
        }
    }
}

// older list files only have a count for the sub tasks, this fills in the task limits of the header
static inline void FixDlHeader_i(void) {
    NWC24iDlHeader *hdr = DlHeader_i();

    if (hdr->maxTasks == 0 && hdr->maxSubTasks != 0) {
        NWC24File file;

        hdr->maxTasks = hdr->maxSubTasks;
        if (hdr->maxSubTasks > NWC24i_DL_SUBTASK_MAX) {
            hdr->maxSubTasks = NWC24i_DL_SUBTASK_MAX;
        }

        if (NWC24FOpen(&file, DLFilePath, NWC24_OPEN_NAND_RW) >= 0) {
            if (NWC24FSeek(&file, 0, NWC24_SEEK_BEG) >= 0) {
                NWC24FWrite(DlHeader_i(), DL_HEADER_SIZE, &file);
            }

            NWC24FClose(&file);
        }
    }
}

static inline NWC24Err CheckDlHeaderLimits_i(NWC24iDlHeader *hdr) {
    if (hdr->maxTasks < 1 || hdr->privateTasks < 1 || hdr->maxTasks < hdr->privateTasks) {
        return NWC24_ERR_BROKEN;
    }

    return NWC24_OK;
}

// puts a task into the list, filling in the next update time
NWC24Err fn_803A3D64(NWC24iDlTask *task) {
    NWC24Err result;

    FixDlHeader_i();
    result = CheckDlHeaderLimits_i(DlHeader_i());
    if (result < 0) {
        return result;
    }

    result = fn_803A5284(task, DlHeader_i()->privateTasks, DlHeader_i()->maxTasks);
    if (result < 0) {
        return result;
    }

    {
        s64 now = 0;

        result = NWC24iGetUniversalTime(&now);
        if (result >= 0) {
            s64 when = now + task->interval * 60;

            result = CheckDlTask_i(task, FALSE);
            if (result == NWC24_OK) {
                u16 id = task->id;

                if (id == 0xFFFF) {
                    result = NWC24_ERR_FAILED;
                } else {
                    DlEntry_i(id)->nextTime = when / 60;
                    result = NWC24_OK;
                }
            }
        }
    }

    return result;
}

// reads the list header from the file
NWC24Err fn_803A4D00(void) {
    NWC24File file;
    NWC24Err result;
    NWC24Err read = NWC24_OK;
    u32 length = 0;

    result = NWC24FOpen(&file, DLFilePath, NWC24_OPEN_READ);
    if (result < 0) {
        return result;
    }

    result = NWC24FSeek(&file, 0, NWC24_SEEK_BEG);
    if (result >= 0) {
        result = NWC24FRead(NWC24WorkP->dlHeader, DL_HEADER_SIZE, &file);
        if (result < 0) {
            read = result;
        }

        result = read;
    }

    if (result < 0) {
        return result;
    }

    read = NWC24FGetLength(&file, &length);
    if (read >= 0) {
        FixDlHeader_i();
        read = CheckDlHeaderLimits_i(DlHeader_i());
    }

    result = NWC24FClose(&file);
    return read != NWC24_OK ? read : result;
}

// walks the list and drops tasks that cannot be read back or that have run out of runs
NWC24Err fn_803A49B0(NWC24iDlHeader *hdr, BOOL clear) {
    NWC24iDlTask task;
    u16 i;

    for (i = 0; i < hdr->maxTasks; i++) {
        if (CheckDlEntryAvailable_i(i) == NWC24_OK && clear) {
            if (GetDlTask_i(&task, i) < 0) {
                DeleteDlTaskForced_i(&task);
            } else if (i >= DlHeader_i()->privateTasks && task.count == 0) {
                DeleteDlTaskForced_i(&task);
            }
        }
    }

    return NWC24_OK;
}

NWC24Err NWC24iOpenDlTaskList(void) {
    NWC24Err result;

    result = fn_803A4D00();
    if (result >= 0) {
        NWC24iSynchronizeRtcCounter(FALSE);
        result = fn_803A49B0(DlHeader_i(), FALSE);
        if (result >= 0) {
            return NWC24_OK;
        }
    }

    return result;
}

NWC24Err NWC24iCloseDlTaskList(void) {
    return NWC24_OK;
}

// copies the url out, optionally with a ".nn" counter on the end
NWC24Err fn_803A4E90(NWC24iDlTask *task, char *buf, u32 size, BOOL withNumber, int number) {
    NWC24Err result;
    int len;

    result = CheckDlTask_i(task, FALSE);
    if (result != NWC24_OK) {
        return result;
    }

    len = strlen(task->url);
    if (len == 0) {
        return NWC24_ERR_NOT_FOUND;
    }

    if ((u32)len < 8) {
        return NWC24_ERR_FAILED;
    }

    if (len + (withNumber ? 3 : 0) + 1 > size) {
        return NWC24_ERR_NOMEM;
    }

    strncpy(buf, task->url, len);
    buf[len] = 0;
    if (withNumber) {
        snprintf(buf + len, size - len, ".%02d", number);
    }

    return NWC24_OK;
}

NWC24Err fn_803A3058(NWC24iDlTask *task, char *buf, u32 size) {
    return fn_803A4E90(task, buf, size, FALSE, 0xFF);
}

// bits 0 and 1 of the optional flags exclude each other
NWC24Err fn_803A4FCC(NWC24iDlTask *task, u8 flags) {
    NWC24Err result;

    result = CheckDlTask_i(task, TRUE);
    if (result != NWC24_OK) {
        return result;
    }

    if ((flags & 1) && (flags & 2)) {
        return NWC24_ERR_INVALID_VALUE;
    }

    task->optFlags = flags;
    return NWC24_OK;
}

// finds the first task of the running app and loads it
NWC24Err fn_803A3324(NWC24iDlTask *out) {
    NWC24Err result;
    u16 idx;
    u32 appId = NWC24GetAppId();

    if (DlHeader_i() == NULL) {
        result = NWC24_ERR_LIB_NOT_OPENED;
    } else if (appId == 0) {
        result = NWC24_ERR_INVALID_VALUE;
    } else {
        result = fn_803A3540(&idx, TRUE);
        while (result >= 0) {
            if (DlEntry_i(idx)->app == appId) {
                result = NWC24_OK;
                goto found;
            }

            result = fn_803A3540(&idx, FALSE);
        }

        result = NWC24_ERR_NOT_FOUND;
    }

found:
    if (result < 0) {
        return result;
    }

    return GetDlTask_i(out, idx);
}

static const char *WC24PubKFile = "wc24pubk.mod";

// <title data dir>/<file> for the title the task belongs to
static inline NWC24Err DlTaskCheckApp_i(const NWC24iDlTask *task) {
    NWC24Err result = CheckDlTask_i(task, FALSE);

    if (result != NWC24_OK) {
        return result;
    }

    if (task->appId == 0) {
        return NWC24_ERR_INVALID_VALUE;
    }

    return NWC24_OK;
}

static inline NWC24Err DlTaskDataPath_i(const NWC24iDlTask *task, char *path, const char *file) {
    NWC24Err result = DlTaskCheckApp_i(task);

    if (result != NWC24_OK) {
        return result;
    }

    result = DlTaskCheckApp_i(task);
    if (result != NWC24_OK) {
        return result;
    }

    snprintf(path, 0x40, "/title/%08x/%08x/data", task->titleIdHi, task->titleIdLo);
    return NWC24_OK;
}

// stores the 256 byte public key of the task's server
NWC24Err fn_803A43D0(NWC24iDlTask *task, const void *key) {
    char path[64];
    NWC24Err result;
    const char *file = WC24PubKFile;
    int len;

    result = CheckDlTask_i(task, TRUE);
    if (result != NWC24_OK) {
        return result;
    }

    if (key == NULL) {
        return NWC24_ERR_INVALID_VALUE;
    }

    result = DlTaskDataPath_i(task, path, file);
    if (result >= 0) {
        len = strlen(path);
        snprintf(path + len, 0x40 - len, "/%s", file);
        result = NWC24_OK;
    }

    if (result < 0) {
        return result;
    }

    result = NANDCreate(path, 0x3D, 0);
    if (result != 0 && result != NAND_RESULT_EXISTS) {
        return -21;
    }

    return fn_803A5A8C(path, 0, key, 0x100, 0);
}

// stores the 16 byte hash that belongs to the public key
NWC24Err fn_803A4684(NWC24iDlTask *task, const void *hash) {
    char path[64];
    NWC24Err result;
    const char *file = WC24PubKFile;
    int len;

    result = CheckDlTask_i(task, TRUE);
    if (result != NWC24_OK) {
        return result;
    }

    if (hash == NULL) {
        return NWC24_ERR_INVALID_VALUE;
    }

    result = DlTaskDataPath_i(task, path, file);
    if (result >= 0) {
        len = strlen(path);
        snprintf(path + len, 0x40 - len, "/%s", file);
        result = NWC24_OK;
    }

    if (result < 0) {
        return result;
    }

    if (fn_803A5B84(path, 0, NULL, 0) < 0) {
        return -40;
    }

    fn_803A5A8C(path, 0x100, NULL, 0, 0x100);
    return fn_803A5A8C(path, 0x200, hash, 0x10, 0);
}

/* ---------------------------------------------------------------------------------------------
 * more mail box bits: opening the box files, reading text back out of committed messages
 * ------------------------------------------------------------------------------------------- */

NWC24Err NWC24iOpenMBox(void) {
    NWC24iMBCHeader *hdr;
    NWC24File file;
    NWC24Err result;
    char *path = NWC24WorkP->pathWork;

    Mail_memset(&NWC24WorkP->sendCtrl, 0, MBC_BLOCK_SIZE);
    result = fn_8039C984(NWC24_MSGBOX_SEND, &hdr);
    if (result != NWC24_OK) {
        return result;
    }

    Mail_memset(&NWC24WorkP->recvCtrl, 0, MBC_BLOCK_SIZE);
    result = fn_8039C984(NWC24_MSGBOX_RECV, &hdr);
    if (result != NWC24_OK) {
        return result;
    }

    result = MBoxMbxPath_i(NWC24_MSGBOX_RECV, path, NWC24i_PATH_WORK_SIZE);
    if (result != NWC24_OK) {
        return result;
    }

    result = NWC24FOpen(&file, path, NWC24_OPEN_READ);
    if (result == NWC24_OK) {
        result = NWC24FClose(&file);
    }

    if (result != NWC24_OK) {
        return result;
    }

    result = MBoxMbxPath_i(NWC24_MSGBOX_SEND, path, NWC24i_PATH_WORK_SIZE);
    if (result != NWC24_OK) {
        return result;
    }

    result = NWC24FOpen(&file, path, NWC24_OPEN_READ);
    if (result == NWC24_OK) {
        result = NWC24FClose(&file);
    }

    if (result != NWC24_OK) {
        return result;
    }

    MBoxMount.count = 0;
    MBoxMount.box = NWC24_MSGBOX_SEND;
    return NWC24_OK;
}

// which box a committed message lives in
static inline NWC24Err MsgBoxOf_i(NWC24iMsgObj *msg, NWC24MsgBoxId *id) {
    if (msg->flags & NWC24_MSGOBJ_TO_SEND) {
        *id = NWC24_MSGBOX_SEND;
    } else if (msg->flags & NWC24_MSGOBJ_TO_RECV) {
        *id = NWC24_MSGBOX_RECV;
    } else {
        return NWC24_ERR_INVALID_VALUE;
    }

    return NWC24_OK;
}

// reads the subject of a committed message back out of its file
NWC24Err fn_8039FD68(NWC24iMsgObj *msg, char *buf, u32 size) {
    NWC24File file;
    NWC24MsgBoxId id;
    NWC24Err result;
    NWC24Err close;
    u32 len;

    if (!NWC24IsMsgLibOpened() && !NWC24IsMsgLibOpenedByTool()) {
        return NWC24_ERR_LIB_NOT_OPENED;
    }

    if (!(msg->flags & NWC24_MSGOBJ_DELIVERING)) {
        return NWC24_ERR_PROTECTED;
    }

    result = MsgBoxOf_i(msg, &id);
    if (result != NWC24_OK) {
        return result;
    }

    len = msg->subject.size;
    if (len == 0) {
        return NWC24_ERR_NULL;
    }

    result = fn_8039B9B0(id, msg->id, &file);
    if (result != NWC24_OK) {
        return result;
    }

    if (len > size - 1) {
        len = size - 1;
    }

    NWC24FSeek(&file, msg->subject.offset, NWC24_SEEK_BEG);
    result = NWC24FRead(buf, len, &file);
    if (result == NWC24_OK) {
        buf[len] = 0;
        if (msg->subject.size > size - 1) {
            result = NWC24_ERR_OVERFLOW;
        }
    }

    close = NWC24iMBoxCloseMsg(&file);
    return result != NWC24_OK ? result : close;
}

NWC24Err fn_803A001C(NWC24File *file, NWC24Data *src, u8 *out, u32 outSize, u32 *written);

// reads (and decodes) attachment number idx of a committed message
NWC24Err fn_8039FEB0(NWC24iMsgObj *msg, u32 idx, u8 *buf, u32 size) {
    NWC24File file;
    NWC24MsgBoxId id;
    NWC24Err result;
    NWC24Err close;
    u32 got;

    if (!NWC24IsMsgLibOpened() && !NWC24IsMsgLibOpenedByTool()) {
        return NWC24_ERR_LIB_NOT_OPENED;
    }

    if (!(msg->flags & NWC24_MSGOBJ_DELIVERING)) {
        return NWC24_ERR_PROTECTED;
    }

    if (idx >= msg->numAttached) {
        return NWC24_ERR_NOT_FOUND;
    }

    result = MsgBoxOf_i(msg, &id);
    if (result != NWC24_OK) {
        return result;
    }

    if (msg->attached[idx].size == 0 || msg->attachedSize[idx] == 0) {
        return NWC24_ERR_NULL;
    }

    result = fn_8039B9B0(id, msg->id, &file);
    if (result != NWC24_OK) {
        return result;
    }

    got = 0;
    result = fn_803A001C(&file, &msg->attached[idx], buf, size, &got);
    if (result == NWC24_OK && got != msg->attachedSize[idx]) {
        result = NWC24_ERR_FORMAT;
    }

    close = NWC24iMBoxCloseMsg(&file);
    return result != NWC24_OK ? result : close;
}

// decodes a base64 block out of the message file (up to the line that ends in "=" padding)
NWC24Err fn_803A001C(NWC24File *file, NWC24Data *src, u8 *out, u32 outSize, u32 *written) {
    u8 *readBuf = (u8 *)NWC24WorkP->WORK_0x400;
    u32 off = src->offset;
    int remaining = src->size;
    u32 total = 0;
    NWC24Err result = NWC24_OK;
    BOOL done = FALSE;
    u32 n;

    while (remaining > 0 && !done) {
        int chunk;
        int cut;
        int i;
        u8 *p;

        chunk = 0x400;
        if (remaining <= 0x400) {
            chunk = remaining;
        }

        NWC24FSeek(file, off, NWC24_SEEK_BEG);
        result = NWC24FRead(readBuf, chunk, file);
        if (result != NWC24_OK) {
            break;
        }

        cut = 0;
        p = readBuf;
        for (i = 0; i < chunk; i++, p++) {
            if (*p == '\r') {
                cut = i;
            }

            if ((char)*p == '=') {
                cut = i + 1;
                done = TRUE;
                result = NWC24_OK;
                break;
            }
        }

        if (cut == 0) {
            break;
        }

        result = fn_8039D45C(readBuf, cut, out, outSize - total, &n);
        if (result != NWC24_OK) {
            break;
        }

        off += cut;
        remaining -= cut;
        total += n;
        out += n;
    }

    if (result == NWC24_OK) {
        *written = total;
    }

    return result;
}

extern u64 fn_803A2088(u64 id);

// the user id has a checksum in its low bits: after the decode step the whole thing has to be a multiple
// of the generator polynomial 0x635
NWC24Err NWC24iCheckUserIdCrc(NWC24UserId id) {
    u64 v = fn_803A2088(id);
    int i;

    for (i = 0; i < 43; i++) {
        if ((v >> (53 - (i + 1))) & 1) {
            v ^= (u64)0x635 << (42 - i);
        }
    }

    if (v != 0) {
        return NWC24_ERR_ID_CRC;
    }

    return NWC24_OK;
}

// HYPOTHESIS: untyped placeholders for the zero-initialized .bss of this unit, sizes and order from the original layout.
unsigned char lbl_805372C0[0x40] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_80537300[0x40] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_80537340[0x80] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_805373C0[0x180] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_80537540[0xe0] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_80537620[0x20] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_80537640[0x20] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_80537660[0x20] __attribute__((aligned(8))) = { 0 };
