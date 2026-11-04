// WiiConnect24 SDK: FriendList.
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
 * friend lists
 * ------------------------------------------------------------------------------------------- */

// The on-disk header starts with the record counts and a 128-bit in-use
// bitmap. The remaining bytes before the friend-code table are unresolved.
// HYPOTHESIS: source member names; layout follows the reader and bit tests.
typedef struct NWC24FriendListHeader {
    u32 magic;
    u32 version;
    u32 capacity;
    u32 size;
    u32 inUse[4];
    u8 unk20[0x20];
    u64 friendCodes[NWC24_FRIEND_LIST_CAPACITY];
} NWC24FriendListHeader;

#define FRIEND_LIST_MAGIC 'WcFl'
#define FRIEND_LIST_VERSION 2

static const char *FLFilePath = "/shared2/wc24/nwc24fl.bin";

static NWC24Err GetCachedFLHeader(NWC24FriendListHeader **header);

#define FL_RECORD_SIZE 0x140
#define FL_RECORDS_OFFSET 0x360

NWC24Err fn_803A1CDC(u32 *info, u32 idx);

NWC24Err fn_803A19D8(u32 *info, u32 idx) {
    NWC24FriendListHeader *hdr;
    NWC24Err result;

    result = fn_803A1CDC(info, idx);
    if (result != NWC24_OK) {
        return result;
    }

    if (info[0] == 1) {
        Mail_memset(&info[0x48 / 4], 0, 0xF8);
    }

    result = GetCachedFLHeader(&hdr);
    switch (result) {
    case NWC24_OK:
        result = (hdr->inUse[idx >> 5] >> (idx & 31)) & 1;
        break;
    }

    if (result == 1) {
        info[1] = 2;
    }

    return NWC24_OK;
}

NWC24Err fn_803A1A7C(u32 *count) {
    NWC24FriendListHeader *hdr;
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

NWC24Err fn_803A1AE8(u32 *count) {
    NWC24FriendListHeader *hdr;
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
        u32 word = hdr->inUse[i];

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

NWC24Err fn_803A1C10(u32 idx) {
    NWC24FriendListHeader *hdr;
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

NWC24Err NWC24iOpenFriendList(void) {
    NWC24FriendListHeader *header = (NWC24FriendListHeader *)NWC24WorkP->flHeader;
    Mail_memset(header, 0, sizeof(NWC24FriendListHeader));
    return GetCachedFLHeader(&header);
}

NWC24Err fn_803A1CDC(u32 *info, u32 idx) {
    NWC24FriendListHeader *hdr;
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

static NWC24Err GetCachedFLHeader(NWC24FriendListHeader **header) {
    NWC24File file;
    NWC24Err result;
    NWC24Err read;
    NWC24Err close;

    *header = (NWC24FriendListHeader *)NWC24WorkP->flHeader;

    if ((*header)->magic != FRIEND_LIST_MAGIC) {
        result = NWC24FOpen(&file, FLFilePath, NWC24_OPEN_NAND_R);
        if (result != NWC24_OK) {
            return result;
        }

        NWC24FSeek(&file, 0, NWC24_SEEK_BEG);
        read = NWC24FRead(*header, sizeof(NWC24FriendListHeader), &file);
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
