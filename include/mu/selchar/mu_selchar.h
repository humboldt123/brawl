#pragma once

// Local shadow of the header of the shared headers (this directory comes first in the include paths): it names the members
// of the task that its accessors use and adds the functions that the other classes of the character select screen call.

#include <StaticAssert.h>
#include <gf/gf_task.h>
#include <mt/mt_vector.h>
#include <mu/mu_msg.h>
#include <mu/mu_object.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <nw4r/g3d/g3d_scnmdlexpand.h>
#include <nw4r/g3d/g3d_scnobjcallback.h>
#include <types.h>

class muSelCharPlayerArea;
class muSelCharCoin;
class muSelCharHand;
class muRuleTask;
class muNoticeWndTask;
#ifdef MU_SELCHAR_UNIT_DECLS
class gfArchive;
class muSelcharPanelCtrlTask;
struct gfPadStatus;
#endif
// HYPOTHESIS: the two characters of a pair high score, as getOnlinePairHighScore writes them.
struct muSelCharScorePair;

enum TeamBattleType { // custom
    Team_Battle_None = 0,
    Team_Battle_Normal = 1,
    Team_Battle_Glow  = 2,
    Team_Battle_Random = 3
};

// The callback of the scene models of the select screen: it passes each call on to the callback it was put in front of,
// and notes when the world matrices were computed.
class muSelCharCBCalcWorld : public nw4r::g3d::IScnObjCallback {
public:
    bool m_calcWorldDone : 1;                   // 0x04 HYPOTHESIS: set after the world matrices of timing C
    nw4r::g3d::IScnObjCallback* m_next;         // 0x08 the callback that is called first

    muSelCharCBCalcWorld();
    virtual ~muSelCharCBCalcWorld() { }
    virtual void ExecCallback_CALC_WORLD(nw4r::g3d::ScnObj::Timing timing, nw4r::g3d::ScnObj* obj, u32 param, void* info);
    virtual void ExecCallback_CALC_MAT(nw4r::g3d::ScnObj::Timing timing, nw4r::g3d::ScnObj* obj, u32 param, void* info);
    virtual void ExecCallback_CALC_VIEW(nw4r::g3d::ScnObj::Timing timing, nw4r::g3d::ScnObj* obj, u32 param, void* info);
    virtual void ExecCallback_DRAW_OPA(nw4r::g3d::ScnObj::Timing timing, nw4r::g3d::ScnObj* obj, u32 param, void* info);
    virtual void ExecCallback_DRAW_XLU(nw4r::g3d::ScnObj::Timing timing, nw4r::g3d::ScnObj* obj, u32 param, void* info);
};
static_assert(sizeof(muSelCharCBCalcWorld) == 0xC, "Wrong size for class!");

// What each kind of the select screen (muSelCharTask::getSelCharType) shows.
// HYPOTHESIS: the names of the fields; only the kind of the high score is known from its getter.
struct muSelCharTypeData {
    u16 m_flags;            // 0x00
    u8 m_unk2;              // 0x02
    u8 m_unk3;              // 0x03
    u8 m_unk4;              // 0x04
    u8 m_highScoreKind;     // 0x05 the high score that the screen shows
    u16 m_unk6;             // 0x06
    int m_msgNo;            // 0x08 the message of the title (-1: none)
};
static_assert(sizeof(muSelCharTypeData) == 0xC, "Wrong size for class!");

// The character select screen.
class muSelCharTask : public gfTask {
public:
    const static u8 num_player_areas = 4;

    union {
        char _0x40[4];
        int unk40;
    };
    muSelCharPlayerArea* m_selCharPlayerAreas[num_player_areas];   // 0x44
#ifdef MU_SELCHAR_UNIT_DECLS
    nw4r::g3d::G3dObj* unk54[4];                // 0x54
    nw4r::g3d::ResFile* m_texResFile;           // 0x64 HYPOTHESIS: the file of the textures of the characters
    gfTask* unk68;                              // 0x68
#else
    char _0x54[0x18];
#endif
    nw4r::g3d::ScnGroup* m_scnGroup;            // 0x6C the group of the scene that the models of the screen are drawn in
    int m_charList[50];                         // 0x70 the kinds of the characters of the screen, in their order
    int m_charListNum;                          // 0x138
    enum {
        Obj_FaceBase = 12,      // the pictures of the characters (one per line of m_charList)
        Obj_ReadyButton = 162,  // the button that starts the battle
        Obj_DiffLevel = 164,    // the level of the computers of the solo modes
        Obj_DiffDown = 165,     // HYPOTHESIS: the two arrows of the level
        Obj_DiffUp = 166,
        Obj_StockDown = 174,    // HYPOTHESIS: the two arrows of the number of stocks
        Obj_StockUp = 175,
        Obj_HomerunRule = 181,  // the rule button of the Home-Run Contest
        Obj_KumiteRule = 182,   // the rule button of the Multi-Man Brawl
        Obj_RuleNumHold = 10,   // HYPOTHESIS: the cursor of the number of a rule that a hand holds
        Obj_Num = 188,
        Expand_FaceBase = 2,
    };

    // The models of the screen.
    // HYPOTHESIS: where the array ends.
    MuObject* m_objs[Obj_Num];                  // 0x13C
    // HYPOTHESIS: from index 2, the places of the cursors on the pictures of the characters (one per line of m_charList).
#ifdef MU_SELCHAR_UNIT_DECLS
    union {
        struct {
            nw4r::g3d::ScnMdlExpand* m_expands[52];     // 0x42C
            char _0x4fc[0x4];
            nw4r::g3d::ScnMdlExpand* m_diffExpand;      // 0x500 the level of the computers and its arrows
            nw4r::g3d::ScnMdlExpand* m_stockExpand;     // 0x504 the number of stocks and its arrows
            nw4r::g3d::ScnMdlExpand* m_highScoreExpand; // 0x508 the high score and the pictures of the characters of a pair
        };
        nw4r::g3d::G3dObj* m_allExpands[56];
    };
#else
    nw4r::g3d::ScnMdlExpand* m_expands[52];     // 0x42C
    char _0x4fc[0x4];
    nw4r::g3d::ScnMdlExpand* m_diffExpand;      // 0x500 the level of the computers and its arrows
    nw4r::g3d::ScnMdlExpand* m_stockExpand;     // 0x504 the number of stocks and its arrows
    nw4r::g3d::ScnMdlExpand* m_highScoreExpand; // 0x508 the high score and the pictures of the characters of a pair
#endif
    muSelCharCBCalcWorld m_calcWorldCBs[15];    // 0x50C
    char _0x5c0[4];
    MuMsg* m_highScoreMsg;                      // 0x5C4 the text of the high scores
    bool m_isTeamBattle;                        // 0x5C8
    u8 m_teamBattleTouch;                       // 0x5C9 HYPOTHESIS: the team button was touched
    char _0x5ca[0x2];
    int m_handiMode;                            // 0x5CC HYPOTHESIS: the handicap setting of the rules of the screen
    union {
#ifdef MU_SELCHAR_UNIT_DECLS
        struct {
            bool m_unk5D0;                              // 0x5D0 HYPOTHESIS: the handicap is shown as a number
            bool unk5D1;
            char _0x5d2[0xA];
        };
#else
        struct {
            bool m_unk5D0;                              // 0x5D0 HYPOTHESIS: the handicap is shown as a number
            char _0x5d1[0xB];
        };
#endif
        struct {
            char unk5D0[4];
            int unk5D4;
            int unk5D8;
        };
    };
    int m_ruleNumStock;                         // 0x5DC
    int m_difficulty;                           // 0x5E0
    int m_maxDiff;                              // 0x5E4
    int m_readyState;                           // 0x5E8 HYPOTHESIS: 2 while the battle can start
    union {
        struct {
            u8 m_readyButtonPointed;                    // 0x5EC a hand is on the button that starts the battle
            char _0x5ed[0xF];
        };
        struct {
            char unk5EC[0xC];
            int unk5F8;
        };
    };
    u8 m_isEntried2P;                           // 0x5FC
    char _0x5fd[3];
    int m_homerunRule;                          // 0x600 HYPOTHESIS: the rule of the Home-Run Contest (solo or co-op)
    int m_kumiteType;                           // 0x604 the kind of the Multi-Man Brawl
    union {
        char _0x608[0x18];
        struct {
#ifdef MU_SELCHAR_UNIT_DECLS
            nw4r::g3d::ResFile* unk608;
            muNoticeWndTask* unk60C;
            void* unk610;
            gfTask* unk614;
            char unk618[0x8];
#else
            char unk608[4];
            muNoticeWndTask* unk60C;
            char unk610[0x10];
#endif
        };
    };
    int m_queueMeleeDropRule;                   // 0x620
#ifdef MU_SELCHAR_UNIT_DECLS
    gfArchive* m_ruleArchive;                   // 0x624
    muSelcharPanelCtrlTask* m_panelCtrl;        // 0x628
#else
    char _0x624[0x8];
#endif
    muRuleTask* m_ruleTask;                     // 0x62C the rules screen that opens over the select screen
    int m_ruleTaskState;                        // 0x630
    union {
        struct {
            u8 m_ruleControlEnable;                     // 0x634
            union {
                char _0x635[0x13];
                struct {
                    char unk635[2];
                    // HYPOTHESIS: four-byte Wi-Fi rule packet cached for change detection.
                    struct {
                        u8 unk0_6 : 2;
                        u8 unk0_4 : 2;
                        u8 unk0_2 : 2;
                        u8 unk0_0 : 2;
                        u8 unk1_5 : 3;
                        u8 unk1_4 : 1;
                        u8 unk1_0 : 4;
                        u8 unk2;
                        u8 unk3;
                    } unk637;
                    char unk63B[0xD];
                };
            };
        };
        struct {
            u8 unk634;
            u8 unk635Byte;
            u8 unk636Byte;
            char unk637Pad[0xD];
            int unk644;
        };
    };
    int m_numPlayerAreas;                       // 0x648
    union {
        char _0x64c[0x8];
        struct {
            int unk64C;
#ifdef MU_SELCHAR_UNIT_DECLS
            int unk650;
#else
            char unk650[4];
#endif
        };
    };

    virtual void processDefault();
    virtual ~muSelCharTask();

    muSelCharTask();
    void appearHomerunRule();
    int bButtonHoldProc();
    void setReadyState(int state);
    // HYPOTHESIS: initProc receives a pointer to the screen initialization data.
    static muSelCharTask* create(void* data);
    void initProc(void* data);
    void wifiMain();
    void mainRule();
    void faceIconMain() __attribute__((never_inline));
    void selectingMain();
    void setToGlobal();
    void setState(int state, int argument, int extra);
    // Closes the Wi-Fi dialog when one is open. Returns whether a dialog was closed.
    bool closeWifiDlg() __attribute__((never_inline));
    // HYPOTHESIS: classification shared by the menu-mode checks.
    static inline int getSelCharTypeGroup(int type) {
        if (isSelCharTypeIn(type, 0, 4)) return 0;
        if (isSelCharTypeIn(type, 4, 12)) return 1;
        bool inGroup = isSelCharTypeIn(type, 12, 16);
        int group = 3;
        if (inGroup) group = 2;
        return group;
    }
    // HYPOTHESIS: online modes exclude the first two mode groups.
    static inline bool isSelCharTypeOnline(int type) {
        bool online;
        if (getSelCharTypeGroup(type) == 0 || getSelCharTypeGroup(type) == 1) online = false;
        else online = true;
        return online;
    }
    void createWifiModel(nw4r::g3d::ResFile* resFile);
    // HYPOTHESIS: the model table contains eight-byte records; the last flag varies priority.
    void createObjResFile(const void* models, int count, nw4r::g3d::ResFile* resFile,
                          nw4r::g3d::ResFile* textureFile, bool varyPriority);
    void setHighScoreStyle(int style);
    void dispNumStock(int count, bool enabled);
    int strcatHighScoreUnit(char* text, int capacity, int kind);
    void sendWifiGameRule(bool force, bool send);
    void dispDiffLevel(int level, bool enabled);
    void ruleCall();
    void sendSystemAllCharKind();
    void setHomerunRule(int rule);
    void setKumiteType(int type);
    void setQueueMeleeDropRule(int rule);
    bool isTeamBattle();
    muSelCharPlayerArea* getPlayerArea(int areaIdx);
    int getRuleNumStock();
    void setRuleNumStock(int count);
    int getHomerunRule();
    int getKumiteType();
    int getQueueMeleeDropRule();
    int getDifficulty();
    u8 isEntried2P();
#ifdef MU_SELCHAR_UNIT_DECLS
    muSelCharCoin* getCoin(int areaIdx) __attribute__((never_inline));
    muSelCharHand* getHand(int areaIdx) __attribute__((never_inline));
#else
    muSelCharCoin* getCoin(int areaIdx);
    muSelCharHand* getHand(int areaIdx);
#endif
    int getRuleTaskState();
    void setDifficulty(int difficulty);
    u8 isTeamBattleTouch();
    int getMaxDiff();
    void setReadyButtonDecided()
#ifdef MU_SELCHAR_UNIT_DECLS
        __attribute__((never_inline))
#endif
        ;
    void clickHomerunRule();
    void clickKumiteRule();
    void eraseAllHighScore();
    void eraseHighScore(u32 msgIndex);
    void eraseHighScorePairChar(int pairIdx);
    void addObj(int obj);
    void removeObj(int obj);
    void removeCursor(MuObject* cursor);
    void setRuleControlEnable(u8 enable) __attribute__((never_inline));
    int findRuleNumHoldPlayer();
    int getCharListIdx(int charKind);
    bool getFacePos(Vec2f* pos, int charKind);
    bool getFaceRect(Rect2D* rect, int charKind);
    void setCursor(int charKind, MuObject* cursor)
#ifdef MU_SELCHAR_UNIT_DECLS
        __attribute__((never_inline))
#endif
        ;
    void dispCanceled();
    void setReadyButtonPointed(u8 pointed);
    void setVisibleDiff(bool visible);
    void setVisibleStock(bool visible);
    void initDiffObj();
    void initStockObj();
    void arrangeCoinPrio();
    int getCharApparState(int selchKind, bool refer);
    // The kind of the select screen (versus, a solo mode ...), from the global select data.
    static int getSelCharType();
    static const muSelCharTypeData* getSelCharTypeData(int selCharType);
#ifdef MU_SELCHAR_UNIT_DECLS
    // HYPOTHESIS: the original returns an int here (the callers do not extend it).
    int getSelCharTypeHighScoreKind(int selCharType) __attribute__((never_inline));
#else
    u8 getSelCharTypeHighScoreKind(int selCharType);
#endif
    static u8 getGlobalHandiType();
    int getRuleTimeMax();
    int getRuleTimeMin();
    void dispHighScore(int, int, int);
    void dispHighScorePairChar(int pairIdx, int charKind, int color);
    int getOnlinePairHighScore(int* style, int* level, muSelCharScorePair* pair);
    int isCharHighScoreCleared(int charKind);
#ifdef MU_SELCHAR_UNIT_DECLS
    int getCharHighScore(int charKind, int difficulty, int* style, int* level);
#else
    int getCharHighScore(int charKind, int difficulty, int a, int b);
#endif
    int dispTargetTotalHighScore();
    int getPairHighScore(int difficulty, int* style, int* level, muSelCharScorePair* pair);
    void dispOnlinePairHighScore();
    void dispPairHighScore();
    int getRuleDispType();
    // An integer: a bool return normalizes the 0/1 result a second time.
    int isWifiGameMaster() __attribute__((never_inline));
#ifdef MU_SELCHAR_UNIT_DECLS
    void setRuleNumToGlobal() __attribute__((never_inline));
#else
    void setRuleNumToGlobal();
#endif
    void setReferFlag(int selchKind)
#ifdef MU_SELCHAR_UNIT_DECLS
        __attribute__((never_inline))
#endif
        ;
    void bindCalcWorldFlag();
    int getCollKindPlayerArea(Vec2f* pos, int* areaIdx, int* arg, float* rect);
    void setRuleNumHoldCursorVisible(bool visible);
    void setFaceIconMotion(int faceIdx, int motion)
#ifdef MU_SELCHAR_UNIT_DECLS
        __attribute__((never_inline))
#endif
        ;
    int findFreeWiiControllerNo();
    void dispWifiPrompt(u32 message);
    int getCollCharKind(Vec2f* pos)
#ifdef MU_SELCHAR_UNIT_DECLS
        __attribute__((never_inline))
#endif
        ;
    // Out of line on the pair high-score screens. The body still inlines getSelCharType.
    void dispHighScoreDiffLevel(int level) __attribute__((never_inline));
    int isPushedUnusedControllerStartButton();
    bool canWifiEntry();
    void wifiEntryPlayer();
    void dispKumiteRule(int type);
    void assignTeam();
    void dispHomerunRule(int rule);
    bool isWifiMode();
    void readyStateMain();
#ifdef MU_SELCHAR_UNIT_DECLS
    // Declarations that only mu_selchar.cpp defines.
    void setVisibleAllPlayersHandi(bool visible);
    void dispVersusHighScore();
    void decideCharKind(int areaIdx, int charKind);
    int getFaceSize();
    void updateCharHighScore(int line, int selchKind, int difficulty);
    void setStockCharKind(int charKind, int color);
    void appearKumiteRule();
    bool canTeamBattle();
    void changeDuplicateCharColor();
    bool isSameChar(int charKind1, int charKind2);
    void changeToRule(int mode);
    void dispRuleNum(int num);
    void buttonProcInFaceArea(int areaIdx, gfPadStatus* pad);
    void openWifiEntryConfirmDlg(const wchar_t* name);
    void setPlayerKind(int areaIdx, int playerKind);
    int readyProc();
    void wifiProc(int areaIdx);
    void incRuleNum();
    void decRuleNum();
    void getDefaultRuleFromGlobal();
    void moveCoin(int areaIdx);
#endif
    // HYPOTHESIS: range classification shared by the character select menus.
    static inline bool isSelCharTypeIn(int type, int first, int end) {
        bool in;
        if (type >= first && type < end) {
            in = true;
        } else {
            in = false;
        }
        return in;
    }

    // Which part of the screen a point is in: 0 above the area of the characters, 1 the area of the characters, 2 below it
    // (the areas of the players).
    // HYPOTHESIS: the meaning of the three values, from the two heights that the point is compared with.
    static int getScreenArea(Vec2f pos);
};

static_assert(sizeof(muSelCharTask) == 0x654, "Wrong size for class!");
