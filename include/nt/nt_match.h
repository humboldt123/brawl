#pragma once

#include <StaticAssert.h>
#include <types.h>
#include <revolution/OS/OSThread.h>

// Local shadow of BrawlHeaders' nt/nt_match.h: adds NtMatching (watchResetNodeMissMatch and thSaveUserDataNow; main.dol 0x80147434 ..., names from the sora map).

class DWCAllocType {
public:
    static void* ntAlloc(int, u32, u32);
    static void ntFree(int, void*);
    static void* ntDwcAlloc(int, u32, u32);
    static void ntDwcFree(int, void*, u32);
};
// TODO size assertion

class NtMatching {
public:
    // HYPOTHESIS: matching arguments are word-sized integers in the menu callers.
    bool retryMatching(int, int);
    // HYPOTHESIS: the matching manager NtTr resets (only the methods its callers use are declared).
    void watchResetNodeMissMatch();
    // HYPOTHESIS: starts the save of the user data (the name of the Wi-Fi user) in a thread, when the network state
    // allows it.
    static void thSaveUserDataNow();
    bool deleteFriend(int index);
    void freeDisplayOrder();
    static void updateServerCallback(int error, int isChanged, void* param);
    static void deleteFriendListCallback(int deletedIndex, int srcIndex, void* param);
    static int getAvailableFriendListIndex(u64 friendKey);
    static void udpRecvCallback(u8 aid, u8* buffer, int size);
    static void newClientCallback(int index, void* param);
    static void changetFriendStatusCallback(int index, u8 status, const char* statusString, void* param);
    // HYPOTHESIS: the final argument selects updates to the local display order.
    bool deleteDisplayOrder(int index, bool updateLocal);

    void requestDisconnectAIDBitmap(u8 bitmap);
    bool isSaveFriendInfoDone();
    u8 getCloseAIDBitmap();
    void setSequenceWatch(int sequence, int state);
    u64 getRegisterFriendCode(int index);
    void shutdown();
    bool isNetShutdownErrorOccured();
    void resetWork();
    static void setErrorStatus(s32 error);
    bool requestSuspendMatching(bool suspend, bool wait);
#ifdef MU_WIFI_INTERFACE_TASK_INTERNAL
    int calcWinningPercentage();
#else
    static int calcWinningPercentage();
#endif
    bool startMatching(int mode, int rule, int players);
    // HYPOTHESIS: small accessors on the matching manager.
    u8 isShutdowned();
    void resetWatchDogTimer();
    void putPacketDelay(int, u16 delay);
    void putPacketLost(int, u8 lost);
    // HYPOTHESIS: finds the friend roster index of the player on the AID.
    int searchFriendID(u32 aid);

    union {
        struct {
            u8 _padState0[0x33F4];
            u8 unk33F4;
            u8 _padState33F5[0x33];
            u32 unk3428;
            u32 unk342C;
            u8 _padState3430[0x68];
            int unk3498;
            u8 _padState349C[8];
            int unk34A4;
            u8 unk34A8;
            u8 unk34A9;
        };
        struct {
            u8 unk0[0x12];
            u8 unk12[0x40];
            union {
                u8 unk52[0x33BF];
                struct {
                    u8 _pad52[0x337A];
                    u8 unk33CC;
                };
            };
            bool unk3411;
            bool unk3412;
            u8 unk3413[0xE];
            s8 unk3421;
            u8 unk3422[0x11A];
            void* unk353C;
        };
        struct {
            u8 _padW0[0x3422];
            u16 unk3422w;
            u8 _padW1[0x11C];
            u16 unk3540;
            u8 _padW2[0xA];
            u8 unk354C;
        };
        // HYPOTHESIS: typed state and thread fields occupy the existing padding.
        struct {
            u8 _pad0[0x11];
            u8 unk11;
            u8 _pad12[0x8E];
            OSThread unkA0;
            u8 _pad3B8[0x3015];
            u8 unk33CD;
            u8 unk33CE;
            u8 _pad33CF[0x26];
            u8 unk33F5;
            u8 unk33F6;
            u8 _pad33F7[0xD];
            u32 unk3404;
#ifdef MU_WIFI_INTERFACE_TASK_INTERNAL
            union {
                u32 unk3408;
                struct {
                    u32 _pad3408_b7 : 25;
                    u32 unk3408_b6 : 1;
                    u32 _pad3408_b0 : 6;
                };
            };
            u8 _pad3408[0x24];
#else
            u8 _pad3408[0x28];
#endif
            int unk3430;
            int unk3434;
            u32 unk3438;
            u8 _pad343C[0x6E];
            u8 unk34AA;
            u8 unk34AB;
        };
    };
};
