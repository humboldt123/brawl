#include <types.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <revolution/OS/OSHardware.h>

typedef void (*DWCLoginCallback)(s32 error, s32 profile, void* parameter);
/* Verified layout; the rest of the login controller remains opaque. */
typedef struct DWCLoginControlView {
    void* connection;
    s32 state;
    u32 product;
    u32 gameCode;
    char* playerName;
    DWCLoginCallback callback;
    void* parameter;
    void* userData;
    u8 unknown20[0x10];
    u32 timed;
    u32 unknown34;
    s64 startTime;
    u8 unknown40[0x228];
} DWCLoginControlView;
#ifdef __MWERKS__
typedef char DWCLoginSizeCheck[sizeof(DWCLoginControlView) == 0x268 ? 1 : -1];
typedef char DWCLoginTimerCheck[offsetof(DWCLoginControlView, startTime) == 0x38 ? 1 : -1];
typedef char DWCLoginCallbackCheck[offsetof(DWCLoginControlView, callback) == 0x14 ? 1 : -1];
#endif
extern DWCLoginControlView* lbl_805A0F80;
extern const char lbl_80489A00[];
extern void DWC_Printf(u32 level, const char* format, ...);
extern u64 fn_80350C0C(const void* loginId);
extern u32 fn_80350C1C(const void* loginId);
extern BOOL fn_80338D90(void);
extern void fn_80338F5C(void);
extern BOOL DWCi_IsError(void);
extern void DWCi_SetError(s32 error, s32 code);
extern s32 fn_80367C74(void* connection);
extern u64 OSGetTime(void);

void DWCi_LoginInit(void* control, void* userData, void* connection, u32 product, u32 gameCode,
                    char* playerName, DWCLoginCallback callback, void* parameter) {
    DWC_Printf(0x20, lbl_80489A00);
    lbl_805A0F80 = control;
    memset(control, 0, 0x268);
    lbl_805A0F80->connection = connection;
    lbl_805A0F80->state = 0;
    lbl_805A0F80->product = product;
    lbl_805A0F80->gameCode = gameCode;
    lbl_805A0F80->playerName = playerName;
    lbl_805A0F80->callback = callback;
    lbl_805A0F80->parameter = parameter;
    lbl_805A0F80->userData = userData;
    DWC_Printf(0x20, lbl_80489A00 + 0xC);
    DWC_Printf(0x20, lbl_80489A00 + 0x38, fn_80350C0C((u8*)userData + 4));
    DWC_Printf(0x20, lbl_80489A00 + 0x58, fn_80350C1C((u8*)userData + 4));
    DWC_Printf(0x20, lbl_80489A00 + 0x78, fn_80350C0C((u8*)userData + 0x10));
    DWC_Printf(0x20, lbl_80489A00 + 0x98, fn_80350C1C((u8*)userData + 0x10));
    DWC_Printf(0x20, lbl_80489A00 + 0xC);
}

BOOL DWCi_LoginAsync(void) {
    if (fn_80338D90()) {
        lbl_805A0F80->state = 1;
        lbl_805A0F80->timed = 0;
        return 1;
    }
    return 0;
}

void DWCi_LoginProcess(void) {
    if (lbl_805A0F80 == NULL || DWCi_IsError()) return;
    switch (lbl_805A0F80->state) {
    case 1:
        fn_80338F5C();
        break;
    case 2: case 3: case 4:
        if (lbl_805A0F80->connection != NULL && *(u32*)lbl_805A0F80->connection != 0)
            fn_80367C74(lbl_805A0F80->connection);
        if (lbl_805A0F80->timed != 0 &&
            (s64)(OSGetTime() - lbl_805A0F80->startTime) / (OS_BUS_CLOCK_SPEED / 4 / 1000) > 60000) {
            if (lbl_805A0F80 != NULL) {
                DWCi_SetError(6, -61070);
                if (lbl_805A0F80->callback != NULL)
                    lbl_805A0F80->callback(6, 0, lbl_805A0F80->parameter);
                if (lbl_805A0F80 != NULL) {
                    lbl_805A0F80->state = 0;
                    lbl_805A0F80->timed = 0;
                }
            }
            lbl_805A0F80->timed = 0;
        }
        break;
    default:
        DWC_Printf(4, lbl_80489A00 + 0xB8);
        break;
    }
}

void* DWCi_GetUserData(void) {
    if (lbl_805A0F80 != NULL) return lbl_805A0F80->userData;
    return NULL;
}

void DWCi_StopLogin(s32 error, s32 code) {
    if (lbl_805A0F80 == NULL || error == 0) return;
    DWCi_SetError(error, code);
    if (lbl_805A0F80->callback != NULL)
        lbl_805A0F80->callback(error, 0, lbl_805A0F80->parameter);
    if (lbl_805A0F80 != NULL) {
        lbl_805A0F80->state = 0;
        lbl_805A0F80->timed = 0;
    }
}

extern BOOL fn_80352AB8(void);
extern void fn_803522DC(void);
void DWCi_ShutdownLogin(void) {
    if (!fn_80352AB8()) fn_803522DC();
    lbl_805A0F80 = NULL;
}

s32 DWCi_LoginHandleGPError(s32 result) {
    s32 error;
    s32 code;
    if (result == 0) return 0;
    DWC_Printf(2, lbl_80489A00 + 0xD8, result);
    /* Other values leave these locals unset in the original. */
    switch (result) {
    case 1: error = 9; code = -1; break;
    case 2: error = 9; code = -2; break;
    case 3: error = 6; code = -10; break;
    case 4: error = 6; code = -20; break;
    }
    if (lbl_805A0F80 != NULL && error != 0) {
        DWCi_SetError(error, code - 61000);
        if (lbl_805A0F80->callback != NULL)
            lbl_805A0F80->callback(error, 0, lbl_805A0F80->parameter);
        if (lbl_805A0F80 != NULL) {
            lbl_805A0F80->state = 0;
            lbl_805A0F80->timed = 0;
        }
    }
    return result;
}

typedef struct DWCLoginUserProfileView { u8 unknown0[0x1C]; s32 profile; } DWCLoginUserProfileView;
typedef struct DWCLoginConnectEvent { s32 result; s32 profile; } DWCLoginConnectEvent;
typedef struct DWCLoginMatchTimeView {
    u8 unknown0[0x4B8];
    s64 startTime;
    s64 elapsedTime;
} DWCLoginMatchTimeView;
typedef char DWCLoginMatchTimeOffsetCheck[offsetof(DWCLoginMatchTimeView, elapsedTime) == 0x4C0 ? 1 : -1];
extern const char* fn_8034F7A4(void);
extern u64 fn_8034F684(void);
extern const char* fn_8034F79C(void);
extern s32 fn_80368058(void* connection, s32 field, const char* text);
extern DWCLoginMatchTimeView* fn_8034E158(void);
extern u32 fn_80342554(void);
extern s32 fn_80342328(s32 status);
extern s32 fn_8033AAF0(void);
extern s32 fn_8033CD90(s32 profile);
extern void fn_803391A8(void* connection, void* event, void* parameter);
extern s32 fn_80367F78(void* connection, s32 profile, s32 cache, s32 blocking,
                      void (*callback)(void*, void*, void*), void* parameter);

void DWCi_GPConnectCallback(void* connection, void* argument, void* parameter) {
    DWCLoginConnectEvent* event = argument;
    char identity[31];
    const char* gameName;
    const char* consoleName;
    u64 consoleId;
    u64 now;
    s64 start;
    DWCLoginMatchTimeView* match;
    DWC_Printf(0x20, lbl_80489A00 + 0xEC, event->result);
    lbl_805A0F80->timed = 0;
    gameName = fn_8034F7A4();
    consoleId = fn_8034F684();
    consoleName = fn_8034F79C();
    snprintf(identity, 31, lbl_80489A00 + 0x11C, consoleName, consoleId, gameName);
    if (DWCi_LoginHandleGPError(fn_80368058(connection, 0x704, identity)) != 0) return;
    if (event->result != 0) {
        DWCi_LoginHandleGPError(event->result);
        return;
    }
    if (lbl_805A0F80->state == 2) {
        if (((DWCLoginUserProfileView*)lbl_805A0F80->userData)->profile == event->profile) {
            DWC_Printf(0x20, lbl_80489A00 + 0x12C);
            lbl_805A0F80->state = 5;
            now = OSGetTime();
            start = fn_8034E158()->startTime;
            match = fn_8034E158();
            match->elapsedTime = (s64)(now - (u64)start);
            DWC_Printf(0x200000, lbl_80489A00 + 0x148,
                       (s32)(fn_8034E158()->elapsedTime / (OS_BUS_CLOCK_SPEED / 4 / 1000)));
            DWC_Printf(0x200000, lbl_80489A00 + 0x164, fn_80342554());
            if (DWCi_LoginHandleGPError(fn_80342328(1)) == 0) {
                lbl_805A0F80->callback(0, event->profile, lbl_805A0F80->parameter);
                if (fn_8033AAF0() == 0) fn_8033CD90(event->profile);
            }
        } else {
            DWC_Printf(0x20, lbl_80489A00 + 0x184);
            if (lbl_805A0F80 != NULL) {
                DWCi_SetError(6, -60000);
                if (lbl_805A0F80->callback != NULL)
                    lbl_805A0F80->callback(6, 0, lbl_805A0F80->parameter);
                if (lbl_805A0F80 != NULL) {
                    lbl_805A0F80->state = 0;
                    lbl_805A0F80->timed = 0;
                }
            }
        }
    } else if (lbl_805A0F80->state == 3) {
        DWCi_LoginHandleGPError(fn_80367F78(connection, event->profile, 0, 0, fn_803391A8, NULL));
    }
}
