#include <DWC/dwc_friend.h>
#include <stddef.h>
#include <revolution/OS/OSHardware.h>
#include <stdlib.h>
#include <string.h>

typedef struct DWCBuddyStatusView {
    s32 profile;
    u32 status;
    char statusString[256];
    char locationString[256];
    u32 unknown208;
    u32 unknown20C;
} DWCBuddyStatusView;

typedef void (*DWCFriendStatusCallback)(s32 index, u8 status, const char* location, void* parameter);
typedef void (*DWCFriendUpdateCallback)(s32 error, u8 changed, void* parameter);

/* This view describes only the verified first 0x60 bytes of the controller. */
typedef struct DWCFriendControlView {
    s32 phase;
    void* connection;
    u32 processCount;
    u32 unknownC;
    s64 lastProcessTime;
    s32 friendCount;
    DWCFriendRecord* friends;
    u8 friendIndex;
    u8 changed;
    u8 state;
    u8 completed;
    u32 statsPending;
    u32 unknown28;
    void* userData;
    DWCFriendUpdateCallback updateCallback;
    void* updateParameter;
    DWCFriendStatusCallback statusCallback;
    void* statusParameter;
    void* deleteCallback;
    void* deleteParameter;
    DWCBuddyFriendCallback buddyCallback;
    void* buddyParameter;
    u32 unknown50;
    u32 unknown54;
    u32 unknown58;
    u32 unknown5C;
} DWCFriendControlView;

typedef char DWCBuddyStatusSizeCheck[sizeof(DWCBuddyStatusView) == 0x210 ? 1 : -1];
typedef char DWCFriendRecordSizeCheck[sizeof(DWCFriendRecord) == 12 ? 1 : -1];
#ifdef __MWERKS__
typedef char DWCFriendControlStateCheck[offsetof(DWCFriendControlView, state) == 0x22 ? 1 : -1];
typedef char DWCFriendControlConnectionCheck[offsetof(DWCFriendControlView, connection) == 4 ? 1 : -1];
typedef char DWCFriendControlTimeCheck[offsetof(DWCFriendControlView, lastProcessTime) == 0x10 ? 1 : -1];
typedef char DWCFriendControlCallbackCheck[offsetof(DWCFriendControlView, buddyCallback) == 0x48 ? 1 : -1];
typedef char DWCFriendControlViewSizeCheck[sizeof(DWCFriendControlView) == 0x60 ? 1 : -1];
#endif

extern DWCFriendControlView* lbl_805A0F70;
extern const char lbl_8059F078[];
extern const char lbl_8059F07C[];
extern const char lbl_80489460[];
extern const char lbl_8048948C[];
extern BOOL fn_80337AF0(const DWCFriendRecord* friendData, DWCBuddyStatusView* status);
extern BOOL fn_803512B4(const DWCFriendRecord* friendData);
extern BOOL fn_80339410(void);
extern void* fn_8033891C(void);
extern s32 fn_803517BC(void* userData, const DWCFriendRecord* friendData);
extern BOOL fn_803685D0(void* connection, s32 profile);
extern s32 fn_80368648(void* connection, s32 profile);
extern s32 DWCi_SetGPStatus(s32 status, const char* statusString, const char* locationString);
extern s32 DWC_GetCommonValueString(const char* key, char* value, const char* text, char delimiter);
extern s32 DWC_Base64Encode(const void* source, u32 size, void* destination, u32 capacity);
extern s32 DWC_Base64Decode(const char* source, u32 size, void* destination, u32 capacity);
extern void DWC_Printf(u32 level, const char* format, ...);

u8 DWC_GetFriendStatusSC(const DWCFriendRecord* friendData, u8* maxPlayers, u8* players, char* location) {
    char value[8];
    DWCBuddyStatusView status;
    if (!fn_80337AF0(friendData, &status)) {
        if (maxPlayers != NULL) *maxPlayers = 0;
        if (players != NULL) *players = 0;
        return 0;
    }
    if (status.status == 6) {
        if (maxPlayers != NULL) {
            if (DWC_GetCommonValueString(lbl_8059F078, value, status.statusString, '/') < 1) *maxPlayers = 0;
            else *maxPlayers = strtoul(value, NULL, 10);
        }
        if (players != NULL) {
            if (DWC_GetCommonValueString(lbl_8059F07C, value, status.statusString, '/') < 1) *players = 0;
            else *players = strtoul(value, NULL, 10);
        }
    } else {
        if (maxPlayers != NULL) *maxPlayers = 0;
        if (players != NULL) *players = 0;
    }
    if (location != NULL) strcpy(location, status.locationString);
    return status.status;
}

u8 DWC_GetFriendStatusData(const DWCFriendRecord* friendData, void* data, s32* size) {
    char location[256];
    u8 status = DWC_GetFriendStatusSC(friendData, NULL, NULL, location);
    if (status == 0) {
        *size = -1;
    } else {
        *size = DWC_Base64Decode(location, strlen(location), NULL, 0);
        if (data != NULL && *size != -1) DWC_Base64Decode(location, strlen(location), data, *size);
    }
    return status;
}

s32 DWC_GetNumFriend(const DWCFriendRecord* friends, s32 count) {
    s32 valid = 0;
    s32 i;
    if (friends == NULL) return 0;
    for (i = 0; i < count; ++i, ++friends) {
        if (fn_803512B4(friends)) ++valid;
    }
    return valid;
}

BOOL DWC_SetOwnStatusData(const void* data, u32 size) {
    char encoded[256];
    s32 length;
    if (lbl_805A0F70 == NULL || !fn_80339410()) return 0;
    length = DWC_Base64Encode(data, size, encoded, 255);
    if (length == -1) return 0;
    encoded[length] = 0;
    return DWCi_SetGPStatus(-1, NULL, encoded) == 0;
}

BOOL DWC_CanChangeFriendList(void) {
    if (lbl_805A0F70 != NULL && (u8)(lbl_805A0F70->state - 1) < 2) return 0;
    return 1;
}

void DWC_DeleteBuddyFriendData(DWCFriendRecord* friendData) {
    if (lbl_805A0F70 != NULL && fn_80339410() && fn_8033891C() != NULL) {
        s32 profile = fn_803517BC(fn_8033891C(), friendData);
        if (profile != 0 && profile != -1 && fn_803685D0(lbl_805A0F70->connection, profile)) {
            fn_80368648(lbl_805A0F70->connection, profile);
            DWC_Printf(4, lbl_80489460);
            memset(friendData, 0, 12);
            return;
        }
    }
    DWC_Printf(4, lbl_8048948C);
    memset(friendData, 0, 12);
}

extern u64 OSGetTime(void);
extern BOOL DWCi_IsError(void);
extern void DWCi_SetError(s32 error, s32 code);
extern s32 fn_80367C74(void* connection);
extern BOOL fn_80383698(void);
extern BOOL fn_803836B0(void);
extern void fn_80383470(void);
extern void fn_80337474(DWCFriendRecord* friends, s32 count);
extern s32 lbl_805A0F78;
extern s32 lbl_805A0F7C;
extern const char lbl_804894BC[];

BOOL DWC_SetBuddyFriendCallback(DWCBuddyFriendCallback callback, void* parameter) {
    if (lbl_805A0F70 == NULL) return 0;
    lbl_805A0F70->buddyCallback = callback;
    lbl_805A0F70->buddyParameter = parameter;
    return 1;
}

void DWCi_FriendInit(void* control, void* connection, void* userData,
                     DWCFriendRecord* friends, s32 count) {
    lbl_805A0F70 = control;
    lbl_805A0F70->phase = 0;
    lbl_805A0F70->connection = connection;
    lbl_805A0F70->processCount = 0;
    lbl_805A0F70->lastProcessTime = 0;
    lbl_805A0F70->friendCount = count;
    lbl_805A0F70->friends = friends;
    lbl_805A0F70->friendIndex = 0;
    lbl_805A0F70->changed = 0;
    lbl_805A0F70->state = 0;
    lbl_805A0F70->completed = 0;
    lbl_805A0F70->statsPending = 0;
    lbl_805A0F70->unknown28 = 0;
    lbl_805A0F70->userData = userData;
    lbl_805A0F70->updateCallback = NULL;
    lbl_805A0F70->updateParameter = NULL;
    lbl_805A0F70->statusCallback = NULL;
    lbl_805A0F70->statusParameter = NULL;
    lbl_805A0F70->deleteCallback = NULL;
    lbl_805A0F70->deleteParameter = NULL;
    lbl_805A0F70->buddyCallback = NULL;
    lbl_805A0F70->buddyParameter = NULL;
    lbl_805A0F70->unknown50 = 0;
    lbl_805A0F70->unknown54 = 0;
    lbl_805A0F70->unknown58 = 0;
    lbl_805A0F70->unknown5C = 0;
}

void DWCi_FriendProcess(void) {
    s32 result;
    if (lbl_805A0F70 == NULL || DWCi_IsError()) return;
    if (lbl_805A0F70->friends == NULL) {
        if (lbl_805A0F70->connection != NULL && *(u32*)lbl_805A0F70->connection != 0) {
            if ((s64)(OSGetTime() - lbl_805A0F70->lastProcessTime) / (OS_BUS_CLOCK_SPEED / 4 / 1000) >= 300) {
                ++lbl_805A0F70->processCount;
                fn_80367C74(lbl_805A0F70->connection);
                lbl_805A0F70->lastProcessTime = OSGetTime();
            }
        }
        return;
    }
    if (lbl_805A0F70->statsPending || fn_80383698()) {
        lbl_805A0F78 = 1;
        lbl_805A0F7C = 0;
        if (!fn_803836B0()) {
            lbl_805A0F78 = 0;
            DWC_Printf(8, lbl_804894BC);
        }
        lbl_805A0F78 = 0;
        if (lbl_805A0F7C == 1) {
            lbl_805A0F7C = 0;
            fn_80383470();
        }
    }
    if (lbl_805A0F70->connection != NULL && *(u32*)lbl_805A0F70->connection != 0) {
        result = 0;
        if ((s64)(OSGetTime() - lbl_805A0F70->lastProcessTime) / (OS_BUS_CLOCK_SPEED / 4 / 1000) >= 300) {
            ++lbl_805A0F70->processCount;
            result = fn_80367C74(lbl_805A0F70->connection);
            lbl_805A0F70->lastProcessTime = OSGetTime();
        }
        if (result != 0 || lbl_805A0F70->phase == 0) return;
        if (lbl_805A0F70->friends != NULL && lbl_805A0F70->state != 3 && lbl_805A0F70->processCount > 7) {
            if (lbl_805A0F70->state < 2) fn_80337474(lbl_805A0F70->friends, lbl_805A0F70->friendCount);
            if (lbl_805A0F70->friendIndex >= lbl_805A0F70->friendCount) {
                lbl_805A0F70->state = 3;
                ++lbl_805A0F70->completed;
            }
        }
    }
    if (lbl_805A0F70->completed >= 2) {
        lbl_805A0F70->completed = 0;
        lbl_805A0F70->updateCallback(0, lbl_805A0F70->changed, lbl_805A0F70->updateParameter);
        lbl_805A0F70->phase = 2;
    }
}

void DWCi_UpdateServersAsync(void* unused0, void* unused1,
                             DWCFriendUpdateCallback callback, void* parameter,
                             DWCFriendStatusCallback statusCallback, void* statusParameter,
                             void* deleteCallback, void* deleteParameter) {
    lbl_805A0F70->updateCallback = callback;
    lbl_805A0F70->updateParameter = parameter;
    lbl_805A0F70->statusCallback = statusCallback;
    lbl_805A0F70->statusParameter = statusParameter;
    lbl_805A0F70->deleteCallback = deleteCallback;
    lbl_805A0F70->deleteParameter = deleteParameter;
    lbl_805A0F70->changed = 0;
    lbl_805A0F70->state = 0;
    lbl_805A0F70->completed = 0;
    lbl_805A0F70->friendIndex = 0;
    lbl_805A0F70->phase = 1;
    if (lbl_805A0F70->friends == NULL) ++lbl_805A0F70->completed;
    ++lbl_805A0F70->completed;
}

void DWCi_StopFriendProcess(s32 error, s32 code) {
    if (lbl_805A0F70 == NULL || error == 0) return;
    DWCi_SetError(error, code);
    if (lbl_805A0F70->phase != 0 && lbl_805A0F70->phase != 2) {
        lbl_805A0F70->updateCallback(error, lbl_805A0F70->changed, lbl_805A0F70->updateParameter);
    }
    if (lbl_805A0F70 != NULL) {
        lbl_805A0F70->phase = 0;
        lbl_805A0F70->state = 0;
        lbl_805A0F70->completed = 0;
    }
}

typedef struct DWCGPBuddyEventView {
    s32 profile;
    u32 unknown4;
    s32 buddyIndex;
} DWCGPBuddyEventView;
typedef void (*DWCGPInfoCallback)(void* connection, void* event, void* parameter);
extern s32 fn_80367F78(void* connection, s32 profile, s32 checkCache, s32 blocking,
                      DWCGPInfoCallback callback, void* parameter);
extern void fn_80338128(void* connection, void* event, void* parameter);
extern void fn_80338358(void* connection, void* event, void* parameter);
extern s32 fn_803683E8(void* connection, s32 index, DWCBuddyStatusView* status);
extern const char lbl_804894E8[];
extern const char lbl_80489508[];
extern const char lbl_80489524[];
extern const char lbl_80489554[];

void DWCi_GPRecvBuddyRequestCallback(void* connection, const DWCGPBuddyEventView* event, void* parameter) {
    DWC_Printf(0x20000, lbl_804894E8, event->profile);
    if (lbl_805A0F70->friends != NULL) {
        DWC_Printf(0x20000, lbl_80489508);
        fn_80367F78(connection, event->profile, 0, 0, fn_80338128, NULL);
    }
}

BOOL DWCi_GPRecvBuddyAuthCallback(void* connection, const DWCGPBuddyEventView* event, void* parameter) {
    DWC_Printf(0x20000, lbl_80489524, event->profile);
    DWC_Printf(0x20000, lbl_80489508);
    fn_80367F78(connection, event->profile, 0, 0, fn_80338358, NULL);
    return 1;
}

void DWCi_GPRecvBuddyStatusCallback(void* connection, const DWCGPBuddyEventView* event, void* parameter) {
    DWCBuddyStatusView status;
    s32 index;
    s32 profile;
    DWC_Printf(0x20000, lbl_80489554, event->profile);
    if (lbl_805A0F70->statusCallback != NULL) {
        if (lbl_805A0F70 == NULL || event->profile == 0) {
            index = -1;
        } else {
            for (index = 0; index < lbl_805A0F70->friendCount; ++index) {
                DWCFriendRecord* friends = lbl_805A0F70->friends;
                if (friends == NULL) profile = 0;
                else {
                    profile = fn_803517BC(fn_8033891C(), friends + index);
                    if (profile == 0 || profile == -1) profile = 0;
                }
                if (event->profile == profile) break;
            }
            if (index >= lbl_805A0F70->friendCount) index = -1;
        }
        if (index != -1) {
            fn_803683E8(connection, event->buddyIndex, &status);
            lbl_805A0F70->statusCallback(index, status.status, status.locationString, lbl_805A0F70->statusParameter);
        }
    }
}

s32 DWCi_GetProfileIDFromList(s32 index) {
    DWCFriendRecord* friends = lbl_805A0F70->friends;
    s32 profile;
    if (friends == NULL) return 0;
    profile = fn_803517BC(fn_8033891C(), friends + index);
    if (profile == 0 || profile == -1) return 0;
    return profile;
}

s32 DWCi_GetFriendListIndex(s32 profile) {
    s32 index;
    if (lbl_805A0F70 == NULL || profile == 0) return -1;
    for (index = 0; index < lbl_805A0F70->friendCount; ++index) {
        DWCFriendRecord* friends = lbl_805A0F70->friends;
        s32 candidate;
        if (friends == NULL) candidate = 0;
        else {
            candidate = fn_803517BC(fn_8033891C(), friends + index);
            if (candidate == 0 || candidate == -1) candidate = 0;
        }
        if (profile == candidate) return index;
    }
    return -1;
}

/* Verified fields in the GP connection; the rest remains opaque. */
typedef struct DWCGPConnectionStatusView {
    u8 unknown0[0x230];
    s32 status;
    char statusString[256];
    char locationString[256];
} DWCGPConnectionStatusView;
#ifdef __MWERKS__
typedef char DWCGPStatusOffsetCheck[offsetof(DWCGPConnectionStatusView, status) == 0x230 ? 1 : -1];
typedef char DWCGPLocationOffsetCheck[offsetof(DWCGPConnectionStatusView, locationString) == 0x334 ? 1 : -1];
#endif
extern s32 fn_803686D0(void* connection, s32 status, const char* statusString, const char* locationString);

void DWCi_InitGPProcessCount(void) {
    if (lbl_805A0F70 != NULL) {
        lbl_805A0F70->processCount = 0;
        lbl_805A0F70->lastProcessTime = OSGetTime();
    }
}

s32 DWCi_SetGPStatus(s32 status, const char* statusString, const char* locationString) {
    if (lbl_805A0F70 == NULL || !fn_80339410()) return 0;
    if (status == -1) status = (*(DWCGPConnectionStatusView**)lbl_805A0F70->connection)->status;
    else DWC_Printf(4, lbl_80489460 + 0x118, status);
    if (statusString == NULL) statusString = (*(DWCGPConnectionStatusView**)lbl_805A0F70->connection)->statusString;
    else DWC_Printf(4, lbl_80489460 + 0x138, statusString);
    if (locationString == NULL) locationString = (*(DWCGPConnectionStatusView**)lbl_805A0F70->connection)->locationString;
    else DWC_Printf(4, lbl_80489460 + 0x15C, locationString);
    return fn_803686D0(lbl_805A0F70->connection, status, statusString, locationString);
}

void DWCi_ShutdownFriend(void) { lbl_805A0F70 = NULL; }
