#include <DWC/dwc_friend.h>
#include <stddef.h>
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

typedef struct DWCFriendControlView {
    u32 unknown0;
    void* connection;
    u8 unknown8[0x1A];
    u8 state;
    u8 unknown23;
} DWCFriendControlView;

typedef char DWCBuddyStatusSizeCheck[sizeof(DWCBuddyStatusView) == 0x210 ? 1 : -1];
typedef char DWCFriendRecordSizeCheck[sizeof(DWCFriendRecord) == 12 ? 1 : -1];
#ifdef __MWERKS__
typedef char DWCFriendControlStateCheck[offsetof(DWCFriendControlView, state) == 0x22 ? 1 : -1];
typedef char DWCFriendControlConnectionCheck[offsetof(DWCFriendControlView, connection) == 4 ? 1 : -1];
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
extern s32 fn_80337350(s32 status, const char* statusString, const char* locationString);
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
    return fn_80337350(-1, NULL, encoded) == 0;
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
