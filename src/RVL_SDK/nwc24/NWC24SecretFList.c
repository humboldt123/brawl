// WiiConnect24 SDK: SecretFList.
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
