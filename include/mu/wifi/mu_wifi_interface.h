#pragma once
// Local BrawlHeaders shadow: adds the recovered online menu task accessors and the
// task fields they read.

#include <StaticAssert.h>
#include <gf/gf_task.h>
#include <types.h>

// HYPOTHESIS: one slot per global player (owning AID, local player index, flags).
struct muWifiPlayerArea {
    u8 m_aid;
    u8 m_localPlayer;
    u8 unk2[3];
    u8 unk5_b7 : 1;
    u8 unk5_b0 : 7;
    u8 unk6[2];
};
static_assert(sizeof(muWifiPlayerArea) == 0x8, "Class is wrong size!");

// HYPOTHESIS: per-AID node information, four remote nodes plus the local fallback.
struct muWifiAIDInfo {
    struct LocalPlayer {
        u8 m_teamColor;
        u8 unk1[7];
    };
    struct AssignedPlayer {
        u8 m_aid;
        u8 unk1[3];
        u8 m_teamColor;
        u8 unk5[5];
    };

    char unk0[0x3];
    u8 unk3;
    char unk4[0x4];
    s32 unk8;
    char unkC[0x24];
    u64 unk30;
    char unk38[0x4];
    LocalPlayer m_localPlayers[2];
    char unk4C[0xA];
    u8 unk56_b7 : 1;
    u8 unk56_b6 : 1;
    u8 unk56_b3 : 3;
    u8 unk56_b2 : 1;
    u8 unk56_b0 : 2;
    char unk57[0x21];
    u8 unk78_b7 : 1;
    u8 unk78_b3 : 4;
    u8 unk78_b2 : 1;
    u8 unk78_b0 : 2;
    char unk79[0x4];
    u8 unk7D_b4 : 4;
    u8 unk7D_b2 : 2;
    u8 unk7D_b0 : 2;
    char unk7E[0x67];
    AssignedPlayer m_assignedPlayers[4];
    char unk10D[0x2B];
    char m_shortComment[0xA0];
    char unk1D8[0x39];
    s8 m_friendAIDIndex;
    char unk212[0x6];
};
static_assert(sizeof(muWifiAIDInfo) == 0x218, "Class is wrong size!");

// HYPOTHESIS: error message table entry (message id, error code display flag).
struct muWifiErrorMsg {
    u32 m_msgId;
    u8 m_haveErrorCode : 1;
    u8 unk4_b0 : 7;
    u8 unk5[3];
};
static_assert(sizeof(muWifiErrorMsg) == 0x8, "Class is wrong size!");

class muWifiInterfaceTask : public gfTask {
public:
    s32 unk40;
    char unk44[0x8];
    muWifiPlayerArea m_playerAreas[4];
    char unk6C[0x4];
    muWifiAIDInfo m_aidInfos[5];
    char unkAE8[0x4E3];
    u8 unkFCB_b7 : 1;
    u8 unkFCB_b0 : 7;
    s32 unkFCC_b24 : 8;
    u32 unkFCC_b0 : 24;
    char unkFD0[0xA];
    u16 m_limitTime;
    char unkFDC[0x13];
    u8 m_playerLive[4];
    char unkFF3[0x1];
    u16 m_playerLiveTime[4];
    char unkFFC[0x28];
    u8 m_assignedMeleeRule[0xA];
    u8 m_assignedHomerunRule;
    u8 m_assignedKumiteType;
    char unk1030[0x18];
    s32 m_appNetworkError[2];
    char unk1050[0x8];

    virtual void processDefault();
    virtual ~muWifiInterfaceTask();
    virtual void exit();

    static muWifiInterfaceTask* create();
    static void destroy();
    // HYPOTHESIS: parameter types of the accessors below come from register use only.
    static u32 getErrorMsgID(int index);
    static bool haveErrorCode(int index);
    static u32 getBadNameErrorMsgID();
    void connect(bool);
    static u64 getUserID();
    bool disconnect();
    bool isMyPlayerArea(int index);
    u8 getTeamColor(int index);
    u8 getAssignedTeamColor(int index);
    u8 getAssignedHomerunRule();
    u8 getAssignedKumiteType();
    u8* getAssignedMeleeRule();
    u8 getAssignedDragoon();
    void setAppNetworkError(int error, int detail);
    void connectMatchingDaiRantou(int arg);
    void startMatchingBattle100(int arg);
    void connectMatchingBattle100(int arg);
    void startMatchingHomerun(int arg);
    void connectMatchingHomerun(int arg);
    void startMatchingBossBattle(int arg);
    void connectMatchingBossBattle(int arg);
    void clearAppError();
    bool isEndMatchingEnd();
    void resetLimitTime(int seconds);
    static bool testSendAIDInfoDone();
    void clearPrepareLoadStage();
    int getFriendAIDIndexAID(int aid);
    void clearShortComment(int aid);
    s32 getMeleeRestTime(u8* flag);
    u8 getRuleType();
    u8 getPlayerArea(int index, u8* localPlayer);
    int getSelCharMeleeKind();
    bool getPlayerLive(int index);
    void initPlayerLive();
    void clearPlayerLive(u8 aid, u16 time);
    u64 getComposedCharaAcceptFlag();
    muWifiAIDInfo* getSafeParentAIDInfo();
};
static_assert(sizeof(muWifiInterfaceTask) == 0x1058, "Class is wrong size!");

void netReport(const char* fmt, ...);
void netReport0(const char* fmt, ...);

extern muWifiInterfaceTask* g_muWifiInterfaceTask;
