#include "revolution.h"
#include "revolution/nwc24.h"
#include "revolution/nwc24/NWC24Internal.h"
#include "revolution/sc.h"
#include "revolution/vf.h"
#include <cstring>
#include <cstdarg>

// BYTES: only the explicitly inline SDK helpers are inlined in this unit.
#pragma auto_inline off

#define MBC_FIRST_ENTRY 0x80
#define MBC_BLOCK_SIZE 0x80
BOOL fn_8039B4C8(NWC24iMBCEntry *entry);

typedef struct MsgListState MsgListState;
NWC24Err fn_8039FD68(const NWC24MsgObj *pMsg, char *buf, u32 size);
NWC24Err fn_8039FEB0(const NWC24MsgObj *pMsg, u32 idx, u8 *buf, u32 size);
NWC24Err fn_803A001C(NWC24File *file, const NWC24Data *src, void *buffer, u32 outSize, u32 *written);
NWC24Err fn_803A0158(void);
NWC24Err fn_803A01D0(NWC24MsgBoxId box);
NWC24Err fn_803A0240(u32 appId);
NWC24Err fn_803A02A4(NWC24iMsgObj *out, u32 max, u32 *count, u32 *remaining);
NWC24Err fn_803A03F8(MsgListState *state, u32 *ids);
static BOOL fn_803A0594(MsgListState *state, NWC24iMBCEntry *entry);

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

// BYTES: the static file-read buffer starts on a 32-byte boundary.
static NWC24iMBCEntry ListEntry ATTRIBUTE_ALIGN(32);

// which box a committed message lives in
static inline NWC24Err MsgBoxOf_i(const NWC24iMsgObj *msg, NWC24MsgBoxId *id) {
    if (msg->flags & NWC24_MSGOBJ_TO_SEND) {
        *id = NWC24_MSGBOX_SEND;
    } else if (msg->flags & NWC24_MSGOBJ_TO_RECV) {
        *id = NWC24_MSGBOX_RECV;
    } else {
        return NWC24_ERR_INVALID_VALUE;
    }

    return NWC24_OK;
}

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
    u32 messageTag;
    u32 filterTag;
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
        filterTag = state->tagFilter & 0xFFFF;
        messageTag = entry->tag & 0xFFFF;

        switch (mode) {
        case 0:
            ok = messageTag != filterTag;
            break;
        case 1:
            ok = messageTag == filterTag;
            break;
        case 2:
            ok = filterTag >= messageTag;
            break;
        case 3:
            ok = filterTag <= messageTag;
            break;
        case 4:
            ok = (messageTag & filterTag) != 0;
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

// reads the subject of a committed message back out of its file
// NWC24ReadMsgSubject: truncate the committed subject and terminate a successful read.
NWC24Err fn_8039FD68(const NWC24MsgObj *pMsg, char *buf, u32 size) {
    NWC24iMsgObj *msg = (NWC24iMsgObj *)pMsg;
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
    if (result == NWC24_OK) { result = close; }
    return result;
}

// reads (and decodes) attachment number idx of a committed message
// NWC24ReadMsgAttached: require the decoded length to match the stored attachment size.
NWC24Err fn_8039FEB0(const NWC24MsgObj *pMsg, u32 idx, u8 *buf, u32 size) {
    NWC24iMsgObj *msg = (NWC24iMsgObj *)pMsg;
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
    // BYTES: refresh the const private view for the post-decode size check.
    if (result == NWC24_OK && got != ((const NWC24iMsgObj *)pMsg)->attachedSize[idx]) {
        result = NWC24_ERR_FORMAT;
    }

    close = NWC24iMBoxCloseMsg(&file);
    if (result == NWC24_OK) { result = close; }
    return result;
}

// decodes a base64 block out of the message file (up to the line that ends in "=" padding)
NWC24Err fn_803A001C(NWC24File *file, const NWC24Data *src, void *buffer, u32 outSize, u32 *written) {
    int chunk;
    int remaining = src->size;
    u32 off = src->offset;
    u8 *readBuf = (u8 *)NWC24WorkP->WORK_0x400;
    u8 *out = (u8 *)buffer;
    u32 total = 0;
    int cut;
    NWC24Err result = NWC24_OK;
    BOOL done = FALSE;
    u32 n;
    u8 *p;
    int i;

    while (remaining > 0 && !done) {

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
        for (i = 0; i < chunk; p++, i++) {
            if ((char)*p == '\r') {
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
