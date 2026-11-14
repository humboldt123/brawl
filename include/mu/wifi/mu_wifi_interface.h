// Local BrawlHeaders shadow: adds game-rule and friend-list accessors.
#pragma once
// Local BrawlHeaders shadow: adds the recovered online menu task accessors and the
// task fields they read.

#include <StaticAssert.h>
#include <gf/gf_task.h>
#include <types.h>

// HYPOTHESIS: one slot per global player (owning AID, local player index, flags).
struct muWifiPlayerArea {
#ifdef MU_WIFI_INTERFACE_TASK_INTERNAL
    union {
        struct {
            u8 m_aid;
            u8 m_localPlayer;
            union { u8 unk2[3]; struct { u8 unk2Data; u8 _pad3[2]; }; };
            u8 unk5_b7 : 1;
            u8 unk5_b0 : 7;
            u8 unk6[2];
        };
        struct {
            u32 _pad0;
            s32 unk4_b24 : 8;
            u32 _pad4_b0 : 24;
        };
    };
#else
    u8 m_aid;
    u8 m_localPlayer;
    union { u8 unk2[3]; struct { u8 unk2Data; u8 _pad3[2]; }; };
    u8 unk5_b7 : 1;
    u8 unk5_b0 : 7;
    u8 unk6[2];
#endif
};
static_assert(sizeof(muWifiPlayerArea) == 0x8, "Class is wrong size!");

// HYPOTHESIS: per-AID node information, four remote nodes plus the local fallback.
struct muWifiAIDInfo {
    struct LocalPlayer {
        u8 m_teamColor;
        u8 unk1[7];
    };
    struct AssignedPlayer {
        union { u8 m_aid; u8 unk0; };
        union { u8 unk1[3]; struct { u8 _pad1; u8 unk2Data; u8 _pad3; }; };
        u8 m_teamColor;
        u8 unk5[5];
    };

#ifdef MU_WIFI_INTERFACE_TASK_INTERNAL
    union {
        struct {
            char unk0[0x3];
            u8 unk3;
            char unk4[0x4];
        };
        struct {
            u8 unk0Data[3];
            u8 _pad3;
            u8 _pad4[2];
            u16 unk6;
        };
    };
#else
    char unk0[0x3];
    u8 unk3;
    char unk4[0x4];
#endif
#ifdef MU_WIFI_INTERFACE_TASK_INTERNAL
    union {
        struct {
            s32 unk8;
            char unkC[0x24];
        };
        struct {
            s32 _pad8;
            u32 _padC;
            u64 unk10;
            u64 unk18;
            u64 unk20;
        };
    };
#else
    s32 unk8;
    char unkC[0x24];
#endif
    u64 unk30;
#ifdef MU_WIFI_INTERFACE_TASK_INTERNAL
    struct UnkPlayerSlot {
        union {
            struct {
                u8 unk0;
                u8 unk1;
                u8 unk2;
                u8 unk3;
                u8 m_teamColor;
                u8 unk5;
                u16 unk6;
            };
            struct {
                s32 unk0_b24 : 8;
                u32 _pad0 : 24;
            };
        };
    };
    union {
        struct {
            char unk38[0x4];
            LocalPlayer m_localPlayers[2];
            union {
                char unk4C[0xA];
                struct {
                    char _pad4C[9];
                    u8 unk55;
                };
            };
            union {
                struct {
                    u8 unk56_b7 : 1;
                    u8 unk56_b6 : 1;
                    u8 unk56_b3 : 3;
                    u8 unk56_b2 : 1;
                    u8 unk56_b0 : 2;
                };
                struct {
                    u8 _pad56_b5 : 3;
                    u8 unk56_b4 : 1;
                    u8 _pad56_b0 : 4;
                };
                struct {
                    u8 _bf56_b7 : 1;
                    u8 _bf56_b6 : 1;
                    u8 unk56_b5 : 1;
                    u8 _bf56_b4 : 1;
                    u8 unk56_b2_2 : 2;
                    u8 _bf56_b0 : 2;
                };
                struct {
                    u8 _bf56_b1_7 : 7;
                    u8 unk56_b0_1 : 1;
                };
                u8 unk56Data;
            };
            union {
                char unk57[0x21];
                // HYPOTHESIS: status byte and four-byte rule packet share this region.
                struct {
                    u8 unk57Data;
                    char _pad58[0xC];
                    u8 unk64[4];
                };
                struct {
                    char _pad57[2];
                    u8 unk59_b4 : 4;
                    u8 unk59_b0 : 4;
                    union {
                        char _pad5A[0xE];
                        struct { u8 unk5A; char _pad5B[0xD]; };
                    };
                    u8 unk68[4];
                };
                struct {
                    u8 unk57_b7 : 1;
                    u8 unk57_b6 : 1;
                    u8 unk57_b0 : 6;
                    u8 unk58_b4 : 4;
                    u8 unk58_b0 : 4;
                };
            };
        };
        struct {
            UnkPlayerSlot unk38Slots[3];
            u8 unk50;
            u8 unk51;
            u8 unk52;
            u8 _pad53;
            u8 unk54;
            u8 _pad55;
            u8 _pad56_ext;
            u8 _pad57_ext;
            u8 unk58Data;
            u8 _pad59_ext[3];
            s32 unk5C;
            u8 unk60[3];
            char _pad63_ext;
            u8 unk64_b2 : 6;
            u8 unk64_b0 : 2;
            u8 unk65_b1 : 7;
            u8 unk65_b0 : 1;
            u8 unk66;
            u8 unk67;
            char _pad68_ext[4];
            u8 unk6C[4][3];
        };
    };
#else
    char unk38[0x4];
    LocalPlayer m_localPlayers[2];
    union {
        char unk4C[0xA];
        struct {
            char _pad4C[9];
            u8 unk55;
        };
    };
    u8 unk56_b7 : 1;
    u8 unk56_b6 : 1;
    u8 unk56_b3 : 3;
    u8 unk56_b2 : 1;
    u8 unk56_b0 : 2;
    union {
        char unk57[0x21];
        // HYPOTHESIS: status byte and four-byte rule packet share this region.
        struct {
            u8 unk57Data;
            char _pad58[0xC];
            u8 unk64[4];
        };
        struct {
            char _pad57[2];
            u8 unk59_b4 : 4;
            u8 unk59_b0 : 4;
            union {
                char _pad5A[0xE];
                struct { u8 unk5A; char _pad5B[0xD]; };
            };
            u8 unk68[4];
        };
        struct {
            u8 unk57_b7 : 1;
            u8 unk57_b0 : 7;
            u8 unk58_b4 : 4;
            u8 unk58_b0 : 4;
        };
    };
#endif
    union {
        // HYPOTHESIS: four-byte game-rule packet begins at offset 0x7D.
        struct {
            char _pad78Rule[5];
            u8 unk7DData[4];
        };
#ifdef MU_WIFI_INTERFACE_TASK_INTERNAL
        struct {
            char _pad78Rule_bf[5];
            u8 unk7D_b2_6 : 6;
            u8 _pad7D_b0 : 2;
        };
#endif
        struct {
            u8 unk78_b7 : 1;
            u8 unk78_b3 : 4;
            u8 unk78_b2 : 1;
            u8 unk78_b0 : 2;
            union { char unk79[0x4]; struct { u8 unk79Data; char _pad7A[3]; }; };
            u8 unk7D_b4 : 4;
            u8 unk7D_b2 : 2;
            u8 unk7D_b0 : 2;
#ifdef MU_WIFI_INTERFACE_TASK_INTERNAL
            union {
                char unk7E[0x6];
                struct {
                    u8 unk7E_b1 : 7;
                    u8 unk7E_b0 : 1;
                    u8 unk7F;
                    u8 unk80;
                    u8 _pad81[3];
                };
            };
            UnkPlayerSlot unk84[4][3];
            u8 _padE4;
            AssignedPlayer m_assignedPlayers[4];
#else
            char unk7E[0x67];
            AssignedPlayer m_assignedPlayers[4];
#endif
            char unk10D[0x2B];
        };
#ifdef MU_WIFI_INTERFACE_TASK_INTERNAL
        struct {
            u8 _pad78_b7 : 1;
            u8 unk78_b6 : 2;
            u8 unk78_b4 : 1;
            u8 unk78_b3_lo : 1;
            u8 _pad78_b0 : 3;
        };
        struct {
            u8 _pad78_b7_2 : 1;
            u8 unk78_b6_hi : 1;
            u8 unk78_b5 : 1;
            u8 _pad78_b0_2 : 5;
        };
        struct {
            u32 _pad78_mode : 24;
            s32 unk7B_8 : 8;
        };
        struct {
            u32 _pad78_mode9 : 23;
            s32 unk7B_9 : 9;
        };
#endif
        struct {
            char _pad78[0x6C];
            struct {
                // HYPOTHESIS: the assigned handicap occupies the final halfword; a character/color pair sits at
                // unk3/unk4.
                union {
                    struct { u8 unk0; u8 unk1[9]; };
                    struct { u8 _pad3[3]; u8 unk3; u8 unk4; u8 _pad5[5]; };
                    struct { u8 _pad0[8]; u16 unk8; };
                    struct { u8 _pad2[2]; u8 unk2Data; };
                    struct { u8 _pad1[1]; u8 unk1Data; };
                };
            } unkE4[4];
            u8 unk10C;
            u8 unk10DData;
            // HYPOTHESIS: appearance frequency, item switches and seed follow register access widths.
            u8 _pad10E;
            s8 unk10F;
            u32 unk110;
            char _pad114[0xC];
            u64 unk120[2];
            u8 unk130[4];
            char _pad134[4];
        };
    };
#ifdef MU_WIFI_INTERFACE_TASK_INTERNAL
    union {
        struct {
            char m_shortComment[0xA0];
            // HYPOTHESIS: the AID name holds five wide characters as byte pairs.
            union {
                char unk1D8[0x39];
                struct { char _pad1D8[8]; u8 unk1E0[0xA]; };
                struct { char _pad1D8Names[0x12]; u8 unk1EA[3][10]; u8 unk208[3]; };
                struct {
                    char _pad1D8_210[0x38];
                    u8 unk210_b7 : 1;
                    u8 unk210_b6 : 1;
                    u8 unk210_b0 : 6;
                };
            };
            s8 m_friendAIDIndex;
            char unk212[0x6];
        };
        struct {
            char _pad138_ext[0xA0];
            u32 unk1D8Data[2];
        };
    };
#else
    char m_shortComment[0xA0];
    // HYPOTHESIS: the AID name holds five wide characters as byte pairs.
    union {
        char unk1D8[0x39];
        struct { char _pad1D8[8]; u8 unk1E0[0xA]; };
        struct { char _pad1D8Names[0x12]; u8 unk1EA[3][10]; u8 unk208[3]; };
    };
    s8 m_friendAIDIndex;
    char unk212[0x6];
#endif
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
#ifdef MU_WIFI_INTERFACE_TASK_INTERNAL
    union {
        char unk44[0x8];
        struct {
            s32 unk44Data;
            u8 unk48;
            char _pad49[3];
        };
    };
#else
    char unk44[0x8];
#endif
    muWifiPlayerArea m_playerAreas[4];
    // HYPOTHESIS: game mode used when comparing the server status.
    union { char unk6C[0x4]; s32 unk6CData; };
    muWifiAIDInfo m_aidInfos[5];
    // HYPOTHESIS: bit 4 records the player-assignment receive state.
    union { char unkAE8[0x4E2]; struct { char _padAE8[0x4E1]; u8 unkFC9; }; };
#ifdef MU_WIFI_INTERFACE_TASK_INTERNAL
    union {
        struct {
            u8 unkFCA_b7 : 1;
            u8 unkFCA_b6 : 1;
            u8 unkFCA_b5 : 1;
            u8 unkFCA_b4 : 1;           // cleared after the name entry of muWifiCnctWndTask has set the name
            u8 unkFCA_b3 : 1;
            u8 unkFCA_b2 : 1;
            u8 unkFCA_b1 : 1;
            u8 unkFCA_b0 : 1;
        };
        struct {
            bool unkFCA_b7_bool : 1;
            bool unkFCA_b6_bool : 1;
            bool unkFCA_b5_bool : 1;
        };
    };
#else
    u8 unkFCA_b7 : 1;
    u8 unkFCA_b6 : 1;
    u8 unkFCA_b5 : 1;
    u8 unkFCA_b4 : 1;           // cleared after the name entry of muWifiCnctWndTask has set the name
    u8 unkFCA_b0 : 4;
#endif
    u8 unkFCB_b7 : 1;
#ifdef MU_WIFI_INTERFACE_TASK_INTERNAL
    u8 unkFCB_b6 : 1;
    u8 unkFCB_b0 : 6;
#else
    u8 unkFCB_b0 : 7;
#endif
    s32 unkFCC_b24 : 8;
    u32 unkFCC_b0 : 24;
    // HYPOTHESIS: friend index used for the server game-mode comparison.
#ifdef MU_WIFI_INTERFACE_TASK_INTERNAL
    union {
        char unkFD0[0xA];
        struct { char _padFD0[8]; u8 unkFD8; u8 _padFD9; };
        struct { char _padFD0_2[2]; u8 unkFD2; char _padFD3[2]; u8 unkFD5; char _padFD6[4]; };
    };
#else
    union { char unkFD0[0xA]; struct { char _padFD0[8]; u8 unkFD8; u8 _padFD9; }; };
#endif
    u16 m_limitTime;
#ifdef MU_WIFI_INTERFACE_TASK_INTERNAL
    union {
        char unkFDC[0x13];
        struct {
            char _padFDC[7];
            u8 unkFE3[4][3];
        };
    };
#else
    char unkFDC[0x13];
#endif
    u8 m_playerLive[4];
    char unkFF3[0x1];
    u16 m_playerLiveTime[4];
    // HYPOTHESIS: paired rule packets use the byte and halfword layout of their copy.
    union {
        struct {
            union {
                char unkFFC[0x28];
                // HYPOTHESIS: cached rule packet used to detect selection changes.
                u8 unkFFCData[4];
            };
            u8 m_assignedMeleeRule[0xA];
            u8 m_assignedHomerunRule;
            u8 m_assignedKumiteType;
            char unk1030[0x18];
        };
        struct {
            char _padFFC[4];
            struct {
                u8 unk0;
                u8 unk1;
                u8 unk2;
                u8 unk3;
                u8 unk4;
                u8 unk5;
                u8 unk6;
                u8 unk7;
                u8 unk8;
                u8 unk9;
                u8 unkA;
                u8 unkB;
                u8 unkC;
                u16 unkE;
                u16 unk10;
                u16 unk12;
                u16 unk14;
                u16 unk16;
                u16 unk18;
                u16 unk1A;
                u16 unk1C;
                u16 unk1E;
                u8 unk20;
                u8 unk21;
                u16 unk22;
            } unk1000[2];
        };
    };
    s32 m_appNetworkError[2];
#ifdef MU_WIFI_INTERFACE_TASK_INTERNAL
    union {
        char unk1050[0x8];
        struct {
            u16 unk1050Data;
            u16 unk1052Data;
        };
    };

    muWifiInterfaceTask();
#else
    char unk1050[0x8];
#endif

    virtual void processDefault();
    virtual ~muWifiInterfaceTask();
    virtual void exit();

    static muWifiInterfaceTask* create();
    static void destroy();
    // HYPOTHESIS: parameter types of the accessors below come from register use only.
    static u32 getErrorMsgID(int index);
    static bool haveErrorCode(int index);
    static u32 getBadNameErrorMsgID();
    // HYPOTHESIS: opaque four-byte rule packet and transmission flag.
    void setSelCharGameRule(const void* rule, bool send);
    void connect(bool);
    // HYPOTHESIS: 1 while the connection runs, then 2 connected, 3 failed, 4 the name was refused; the outputs are
    // the error (an index for getErrorMsgID), the error code and whether a new Wi-Fi user was made.
    int getConnectResult(int* errorIndex, int* errorCode, bool* isNewUser);
    // HYPOTHESIS: 1 while the disconnection runs, then 2 done.
    int getDisconnectResult(int* errorIndex);
    void putMyFriendStatusName(const wchar_t* name);
    static u64 getUserID();
    bool disconnect();
    bool isPlayerAssignRecieved();
    bool isEqualServerGameMode();
    bool isMyPlayerArea(int index);
    void noticeEndOfCharSelect();
    bool isCancelApprovalCalled(u8* aid, int* player);
    void answerRequestedEntry(bool accepted, u8 aid, int player);
    u8 getTeamColor(int index);
    u8 getAssignedTeamColor(int index);
    u8 getAssignedCpuLevel(int index);
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
    void sendAIDInfo(bool send, u8 aid);
    int getFriendAIDIndexAID(int aid);
    void clearShortComment(int aid);
    void putShortComment(int aid, int index, const wchar_t* comment);
    void closeAIDWifi(int aid);
    s32 getMeleeRestTime(u8* flag);
    u8 getRuleType();
    u8 getPlayerArea(int index, u8* localPlayer);
    int getSelCharMeleeKind();
    bool getPlayerLive(int index);
    void initPlayerLive();
    void clearPlayerLive(u8 aid, u16 time);
    u64 getComposedCharaAcceptFlag();
    muWifiAIDInfo* getSafeParentAIDInfo();
    void requestExchangeFriendKey(int playerArea);
    // HYPOTHESIS: parameter types follow the player index, name buffer and byte-sized name number.
    void setLocalPlayersOnamae(int player, const wchar_t* name, u8 nameNo);
    bool requestApproval(int playerArea);
    u8 getMyFriendStatusIcon();
    u8 getFriendStatusIcon(int index);
    u8 getFriendStatusWiFiStatus(int index);
    u8 getFriendStatusRoundTripTime(int index);
    u64 getMyRFLID();
    void endMatching();
    void setMyRFLID(u64 id);
    u8 getFriendStatusEntryPlayers(int index);
    int getNetcond();
    // HYPOTHESIS: parameter types follow the bit index and flag register use.
    void setFriendFlag(int index, bool flag);
    bool testRequestApproval(int areaIdx);
    int getNumNode();
    // HYPOTHESIS: outputs are wide strings stored as byte pairs in the save data.
    wchar_t* getMyFriendStatusName(wchar_t* name);
    wchar_t* getMyFriendStatusComment(wchar_t* comment);
    wchar_t* getFriendStatusName(int index, wchar_t* name);
    wchar_t* getFriendStatusComment(int index, wchar_t* comment);
    wchar_t* getMyShortComment(int index, wchar_t* comment);
    // HYPOTHESIS: argument and result types follow the original register accesses.
    int getNetworkErrorLevel();
    int _getNetworkErrorLevel();
    int selectRandomNetCharKind(u64 accepted);
    bool isCompleteCloseMatchingAllNode();
    bool isPreparedLoadAllStage();
    bool needKeepAlive();
    u8 getAssignedStageKind(u8* substage, u32* music);
    void clearGlobalAIDStatusLive();
    u8 getAssignedPlayerKind(int index);
    // HYPOTHESIS: output is a four-byte rule packet; force bypasses comparison.
    bool getSelCharGameRule(void* rule, bool force);
    // HYPOTHESIS: argument indexes the global player-area table.
    bool isEnoughRoomForFriendList(int player);
    // HYPOTHESIS: argument and return types follow the original register accesses.
    bool isExchangingFriendKey(int index);
    u8 getLocalPlayerContNum(int index);
    bool isWifiPreloadCharacter();
    wchar_t* getGlobalPlayersOnamae(int index, wchar_t* name, u8* nameNo);
    wchar_t* getLocalPlayersOnamae(int index, wchar_t* name, u8* nameNo);
    // HYPOTHESIS: the mask parameter is word-sized in the original comparisons.
    bool isStageSyncFlagSet(u32 flags);
    // HYPOTHESIS: argument and field types follow access widths in the original functions.
    u16 getAssignedPlayerHandicap(int index);
    u64* takeItemSwitch(s8* appearance);
    wchar_t* getAIDName(u32 aid, wchar_t* name);
    u32 setGlobalRandSeed();
    // HYPOTHESIS: accessor types follow byte results and word-sized input registers.
    u8 getGlobalLiveAIDBitmap();
    u8 getGlobalConnectAIDBitmap();
    u8 getRuleMasterAID();
    u8 getFreeCharColor(int character, int count);
    int _getNetworkError(int* code);
    void startMatchingDaiRantou(int arg);
    bool startMatchingAnybody(int arg);
    // HYPOTHESIS: argument and result types follow register use and string copies.
    int getFriendAIDIndex(int index);
    wchar_t* getShortComment(unsigned int aid, int index, wchar_t* comment);
    u8 getAssignedPlayerContNum(int index);
    int isPermittedEntry(int playerArea);
    void exitNodePlayer(int playerArea);
    // HYPOTHESIS: the second argument is the computer flag the select screen passes as 1.
    void setCpuPlayerArea(int playerArea, int cpu);
    // HYPOTHESIS: parameter types follow the original register accesses.
    bool isRequestedExchangeFriendKey(int index);
    bool getWifiPlayerInfo(int index, int* character, u8* color, bool* connected);
    static s32 exchangeDWCErrorCode(u32 code);
};
static_assert(sizeof(muWifiInterfaceTask) == 0x1058, "Class is wrong size!");

extern muWifiInterfaceTask* g_muWifiInterfaceTask;
