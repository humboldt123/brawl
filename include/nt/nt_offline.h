// Local BrawlHeaders shadow: adds ntFriendInfo's save-data fields and operations and ntFriendInfoSave.
#pragma once

#include <StaticAssert.h>
#include <gf/gf_task.h>
#include <sr/sr_common.h>
#include <types.h>

class gfSysRecorder2;
struct DWCstAccUserData;
typedef struct DWCstAccUserData DWCAccUserData;

// TODO: ntFriendInfoSave?
class UnkFriendInfo {
    struct UnkClass1 {
        u8 unk0;
        u8 unk1;
        u8 unk2[0x2];
    };

    // TODO: First 8 bytes likely hold the 16-digit Wii Number
    struct UnkClass2 {
        u8 unk0;
        u8 unk1;
        u8 unk2;
        u8 unk3;
        u8 unk4;
        u8 unk5;
        u8 unk6;
        u8 unk7;
        u8 unk8[0x81];
    };
public:
    static const u32 FriendRosterLimit = 64;

    // HYPOTHESIS: stands in for DWCAccFriendData (12 bytes; GameSpy dwc headers are not on the include path).
    struct FriendData {
        u8 unk0[0xC];
    };

    union {
        u8 unk0[0x350];
        struct {
            u8 _padRecords0[8];
            u32 unk8;
            u32 unkC;
        };
        struct {
            u16 unk0_0;
            u16 unk2;
            u16 unk4;
            u8 unk6;
            u8 unk7[9];
            u8 unk10[0x340];
        };
        struct {
            u8 _pad0[0x50];
            FriendData m_friendData[FriendRosterLimit];
        };
    };
    UnkClass1 unk350[FriendRosterLimit];
#ifdef MU_WIFI_INTERFACE_TASK_INTERNAL
    struct FriendStatusEntry {
        u8 unk0[0xA];
        u8 unkA[0x7F];
    };
    union {
        struct {
            u8 unk450[0x163];
            UnkClass2 unk5B3[FriendRosterLimit];
        };
        struct {
            u8 _pad450[0x131];
            FriendStatusEntry unk581[FriendRosterLimit];
        };
    };
#else
    u8 unk450[0x163];
    UnkClass2 unk5B3[FriendRosterLimit];
#endif
};
// TODO size assertion

class ntFriendInfo {

public:
    UnkFriendInfo* unk0;
    u32 unk4;
    bool unk8;
    bool unk9;

    void setSaveData(const void* data, u32 size);
    bool isValid(const DWCAccUserData* data);
    UnkFriendInfo* getFriendInfo();
    static ntFriendInfo* getInstance();
    // HYPOTHESIS: the data of the friends for the Wi-Fi part of the save (muWifiCnctWndTask copies it to the buffer of
    // gfSysRecorder2); size is the size of the buffer on entry and of the data on return.
    bool getSaveData(void** data, int* size);
    void setDirtyFlag(bool dirty);
    // HYPOTHESIS: the Wi-Fi account data inside the save data.
    DWCAccUserData* getUserData();
    static bool isSaveNeeded();
    static u32 getSaveSize();
    static bool getSaveHeader(void** data);
    static void lock();
    static void unlock();
    bool isModified();
    bool resetData();
};
// TODO size assertion

// The save of the friend data (the name comes from the text that muWifiCnctWndTask prints while it waits for it).
class ntFriendInfoSave : public gfTask {
public:
    gfSysRecorder2* unk40;
    bool unk44;

    // HYPOTHESIS: the number of saves that are running (ntFriendInfo adds one when it starts one).
    static int s_savingCount;

    bool startSave(Heaps::HeapType heap, bool exitWhenDone);
    ntFriendInfoSave();
    virtual ~ntFriendInfoSave();
    virtual void processDefault();

    static bool isNotSaving() { return s_savingCount == 0; }
};

class NtOffline {
public:
    NtOffline();
    virtual ~NtOffline();
    u32 getFriendIndexHaveWiiID(u64 id);

    u32 unk4;
};
// TODO size assertion
