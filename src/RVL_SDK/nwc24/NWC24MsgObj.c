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

// HYPOTHESIS: name for the subject setter (the field is called subject, setter shape is the same as
// SetMsgText). not in the sibling header, the asm only shows it filling msg->subject.

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

NWC24Err fn_8039B160(const NWC24iMsgObj *msg, u32 *count) {
    NWC24Err result = NWC24_OK;
    u32 n = msg->numAttached;

    *count = n;
    if (n > NWC24i_MSG_ATTACHMENT_MAX) {
        result = NWC24_ERR_BROKEN;
    }

    return result;
}

NWC24Err fn_8039B17C(const NWC24iMsgObj *msg, u32 idx, u32 *size) {
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
