#pragma once

// Local shadow of the header of the shared headers (this directory comes first in the include paths): it names the members
// of the task that its accessors use and adds the functions that the other classes of the character select screen call.

#include <StaticAssert.h>
#include <gf/gf_task.h>
#include <mt/mt_vector.h>
#include <mu/mu_msg.h>
#include <mu/mu_object.h>
#include <nw4r/g3d/g3d_scngroup.h>
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

// The character select screen.
class muSelCharTask : public gfTask {
public:
    const static u8 num_player_areas = 4;

    char _0x40[4];
    muSelCharPlayerArea* m_selCharPlayerAreas[num_player_areas];   // 0x44
    char _0x54[0x18];
    nw4r::g3d::ScnGroup* m_scnGroup;            // 0x6C the group of the scene that the models of the screen are drawn in
    char _0x70[0xCC];
    // The models of the screen.
    // HYPOTHESIS: where the array ends; the buttons below have names from their functions.
    MuObject* m_objs[162];                      // 0x13C
    MuObject* m_readyButton;                    // 0x3C4 the button that starts the battle
    char _0x3c8[0x48];
    MuObject* m_homerunRuleObj;                 // 0x410 the rule button of the Home-Run Contest
    MuObject* m_kumiteRuleObj;                  // 0x414 the rule button of the Multi-Man Brawl
    char _0x418[0x1AC];
    MuMsg* m_highScoreMsg;                      // 0x5C4 the text of the high scores
    u8 m_teamBattleType;                        // 0x5C8 a TeamBattleType
    u8 m_teamBattleTouch;                       // 0x5C9 HYPOTHESIS: the team button was touched
    char _0x5ca[0x12];
    int m_ruleNumStock;                         // 0x5DC
    int m_difficulty;                           // 0x5E0
    int m_maxDiff;                              // 0x5E4
    char _0x5e8[0x14];
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
    u8 isTeamBattle();
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
    void eraseHighScore(u32 msgIndex);
    void addObj(int obj);
    void removeObj(int obj);
    void setRuleControlEnable(u8 enable);
    int findRuleNumHoldPlayer();
    void dispHighScore(int, int, int);

    // Which part of the screen a point is in: 0 above the area of the characters, 1 the area of the characters, 2 below it
    // (the areas of the players).
    // HYPOTHESIS: the meaning of the three values, from the two heights that the point is compared with.
    static int getScreenArea(Vec2f pos);
};

static_assert(sizeof(muSelCharTask) == 0x654, "Wrong size for class!");
