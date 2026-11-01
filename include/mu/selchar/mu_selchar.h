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

    char _0x40[4];
    muSelCharPlayerArea* m_selCharPlayerAreas[num_player_areas];   // 0x44
    char _0x54[0x18];
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
        Obj_Num = 188,
        Expand_FaceBase = 2,
    };

    // The models of the screen.
    // HYPOTHESIS: where the array ends.
    MuObject* m_objs[Obj_Num];                  // 0x13C
    // HYPOTHESIS: from index 2, the places of the cursors on the pictures of the characters (one per line of m_charList).
    nw4r::g3d::ScnMdlExpand* m_expands[52];     // 0x42C
    char _0x4fc[0x4];
    nw4r::g3d::ScnMdlExpand* m_diffExpand;      // 0x500 the level of the computers and its arrows
    nw4r::g3d::ScnMdlExpand* m_stockExpand;     // 0x504 the number of stocks and its arrows
    nw4r::g3d::ScnMdlExpand* m_highScoreExpand; // 0x508 the high score and the pictures of the characters of a pair
    char _0x50c[0xB8];
    MuMsg* m_highScoreMsg;                      // 0x5C4 the text of the high scores
    bool m_isTeamBattle;                        // 0x5C8
    u8 m_teamBattleTouch;                       // 0x5C9 HYPOTHESIS: the team button was touched
    char _0x5ca[0x2];
    int m_handiMode;                            // 0x5CC HYPOTHESIS: the handicap setting of the rules of the screen
    bool m_unk5D0;                              // 0x5D0 HYPOTHESIS: the handicap is shown as a number
    char _0x5d1[0xB];
    int m_ruleNumStock;                         // 0x5DC
    int m_difficulty;                           // 0x5E0
    int m_maxDiff;                              // 0x5E4
    int m_readyState;                           // 0x5E8 HYPOTHESIS: 2 while the battle can start
    u8 m_readyButtonPointed;                    // 0x5EC a hand is on the button that starts the battle
    char _0x5ed[0xF];
    u8 m_isEntried2P;                           // 0x5FC
    char _0x5fd[3];
    int m_homerunRule;                          // 0x600 HYPOTHESIS: the rule of the Home-Run Contest (solo or co-op)
    int m_kumiteType;                           // 0x604 the kind of the Multi-Man Brawl
    char _0x608[0x18];
    int m_queueMeleeDropRule;                   // 0x620
    char _0x624[0x8];
    muRuleTask* m_ruleTask;                     // 0x62C the rules screen that opens over the select screen
    int m_ruleTaskState;                        // 0x630
    u8 m_ruleControlEnable;                     // 0x634
    char _0x635[0x13];
    int m_numPlayerAreas;                       // 0x648
    char _0x64c[0x8];

    virtual void processDefault();
    virtual ~muSelCharTask();

    void setHomerunRule(int rule);
    void setKumiteType(int type);
    void setQueueMeleeDropRule(int rule);
    bool isTeamBattle();
    muSelCharPlayerArea* getPlayerArea(int areaIdx);
    int getRuleNumStock();
    int getHomerunRule();
    int getKumiteType();
    int getQueueMeleeDropRule();
    int getDifficulty();
    u8 isEntried2P();
    muSelCharCoin* getCoin(int areaIdx);
    muSelCharHand* getHand(int areaIdx);
    int getRuleTaskState();
    void setDifficulty(int difficulty);
    u8 isTeamBattleTouch();
    int getMaxDiff();
    void setReadyButtonDecided();
    void clickHomerunRule();
    void clickKumiteRule();
    void eraseAllHighScore();
    void eraseHighScore(u32 msgIndex);
    void eraseHighScorePairChar(int pairIdx);
    void addObj(int obj);
    void removeObj(int obj);
    void setRuleControlEnable(u8 enable);
    int findRuleNumHoldPlayer();
    int getCharListIdx(int charKind);
    bool getFacePos(Vec2f* pos, int charKind);
    bool getFaceRect(Rect2D* rect, int charKind);
    void setCursor(int charKind, MuObject* cursor);
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
    u8 getSelCharTypeHighScoreKind(int selCharType);
    static u8 getGlobalHandiType();
    int getRuleTimeMax();
    int getRuleTimeMin();
    void dispHighScore(int, int, int);

    // Which part of the screen a point is in: 0 above the area of the characters, 1 the area of the characters, 2 below it
    // (the areas of the players).
    // HYPOTHESIS: the meaning of the three values, from the two heights that the point is compared with.
    static int getScreenArea(Vec2f pos);
};

static_assert(sizeof(muSelCharTask) == 0x654, "Wrong size for class!");
