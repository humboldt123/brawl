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

// Checks the decoded user ID against the SDK CRC polynomial.
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

NWC24Err fn_80399BB8(s32 *stage) {
    *stage = config->createStage;
    return NWC24_OK;
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
