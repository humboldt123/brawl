#pragma once

#include <StaticAssert.h>
#include <gm/gm_global.h>
#include <mu/menu.h>  // MuCharKind
#include <mu/mu_msg.h>
#include <mu/mu_proc_main.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <types.h>

class muProcCount;

// One line of the Statistics screen: the function that reads the value from the records, how it is written, and its two
// messages (the title of the line and the explanation in the balloon).
struct muProcCountItem {
    enum Kind {
        Kind_Number = 0,
        Kind_Time = 1,              // seconds, written h:mm
        Kind_Chara = 2,             // a MuCharKind, written by name (Char_None: "- ")
    };

    u32 (*m_get)(muProcCount* proc);
    u8 m_kind;
    u16 m_titleMsg;
    u16 m_explMsg;
};
static_assert(sizeof(muProcCountItem) == 0xC, "Class is wrong size!");

// A character and the value it is ranked by (muProcCount::makeCharRankList).
struct muProcCountRank {
    int m_kind;
    u32 m_value;
};

// The Statistics screen of the Records menu: 39 lines of counters and records, seven to a page, with the explanation of
// the line the cursor is on. Cancel goes back to the Records screen (code 0x10).
// HYPOTHESIS: the member names follow how the functions use them and the names of the map (muProcCount::makeCharStatistics,
// getNo1UseChara ...).
class muProcCount : public muProcMenu {
public:
    enum {
        Rows = 7,                   // lines on a page
        Item_Count = 39,
        Char_Count = 35,            // MuCharKind
        Char_None = 35,             // no character ranks (nobody was played, or a tie)
        Obj_Panel = 111,            // the window of the list: node + texture srt animations (comes in, goes out)
        Obj_Arrow = 117,            // the scroll arrows and the balloon: node animation, visibility, material color (the flash)
        Obj_Row = 118,              // the first of the seven rows; the material color marks the cursor row
        Mdl_Back = 10,              // the model of the screen in m_mdls
        Msg_Buf = 0x100,            // the size of a message of the list
        Msg_Count = 15,             // two messages a row (the title and the value) and the explanation
        Msg_Expl = 14,              // the message of the explanation
        Msg_Heap = 0x2B,            // the second heap of MuMsg::create (not a HeapType in the symbol)
        Code_Back = 0x10,           // the code of the Records screen
        Time_Max = 359999940,       // 99999:59:00, the longest time the screen writes
        Number_Max = 999999999,
    };

    // How far the window is (m_panelState).
    enum PanelState {
        Panel_Closed = 0,
        Panel_Opening = 1,
        Panel_Open = 2,
        Panel_Closing = 3,
        Panel_None = 4,             // before init
    };

    // Which scroll arrow flashes (m_arrowState).
    enum ArrowState {
        Arrow_None = 0,
        Arrow_Up = 1,
        Arrow_Down = 2,
        Arrow_Init = 3,
    };

    u8 m_unk66C : 1;                // 0x66C cleared by init (HYPOTHESIS: set by the menu task)
    int m_panelState;               // 0x670 PanelState
    int m_arrowState;               // 0x674 ArrowState
    int m_row;                      // 0x678 the row the cursor is on
    int m_top;                      // 0x67C the first line on the page
    MuMsg* m_msg;                   // 0x680
    void* m_msgData;                // 0x684 the message file of the lines
    // The characters of the records (MuCharKind, or Char_None), made by makeCharStatistics
    int m_no1UseChara;              // 0x688
    int m_no2UseChara;              // 0x68C
    int m_worstUseChara;            // 0x690
    int m_no1ChampChara;            // 0x694
    int m_no1KetsuChara;            // 0x698
    int m_no1GiveDamageChara;       // 0x69C
    int m_no1TakeDamageChara;       // 0x6A0
    int m_beatKing;                 // 0x6A4
    int m_fallKing;                 // 0x6A8
    int m_suicideKing;              // 0x6AC

    virtual ~muProcCount() { }
    virtual void init(nw4r::g3d::ScnGroup* group, int from);
    virtual int proc();
    virtual void exit();
    virtual void playSE_CursorMove();

    void initMsg();
    void dispPage(int top);
    // Lights the row the cursor is on (the material color of the rows)
    void setCursorRow(int row) {
        for (int i = 0; i < Rows; i++) {
            MuObject* object = m_objs[Obj_Row + i];
            object->setFrameMatCol(i == row ? 1.0f : 0.0f);
            m_objs[Obj_Row + i]->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(0.0f);
        }
    }
    void setPanelState(int state);
    int moveCursor(u32 pad);
    void makeCharStatistics();
    // MATCH-ONLY: the original calls it out of line from makeCharStatistics; MWCC inlines it (and then the getters
    // through the constant pointer) without the attribute.
    int makeCharRankList(muProcCountRank* list, u32 (*get)(MuCharKind kind)) __attribute__((never_inline));
    int findRankChar(const muProcCountRank* list, int count, int rank);

    // The values of the lines (muProcCountItem::m_get)
    static u32 getStartNum(muProcCount* proc);
    static u32 getStartTime(muProcCount* proc);
    static u32 getAllPlayTime(muProcCount* proc);
    static u32 getAdventureAllPlayTime(muProcCount* proc);
    static u32 getAdventureNobePlayTime(muProcCount* proc);
    static u32 getSingleAllPlayTime(muProcCount* proc);
    static u32 getSingleNobePlayTime(muProcCount* proc);
    static u32 getVsAllPlayTime(muProcCount* proc);
    static u32 getVsNobePlayTime(muProcCount* proc);
    static u32 getNumOfVs(muProcCount* proc);
    static u32 getNumOfOfflineVs(muProcCount* proc);
    static u32 getNumOfOnlineVs(muProcCount* proc);
    static u32 getOnlineFriendPlayTime(muProcCount* proc);
    static u32 getOnlineAnyonePlayTime(muProcCount* proc);
    static u32 getNumOfWatchGame(muProcCount* proc);
    static u32 getNumOfTimeVs(muProcCount* proc);
    static u32 getNumOfStockVs(muProcCount* proc);
    static u32 getNumOfCoinVs(muProcCount* proc);
    static u32 getNumOfJoin(muProcCount* proc);
    static u32 getNumOfInterrupt(muProcCount* proc);
    static u32 getTotalDamage(muProcCount* proc);
    static u32 getTotalBeat(muProcCount* proc);
    static u32 getTotalSuicide(muProcCount* proc);
    static u32 getNumOfChara(muProcCount* proc);
    static u32 getNumOfMap(muProcCount* proc);
    static u32 getNumOfFigure(muProcCount* proc);
    static u32 getNumOfSeal(muProcCount* proc);
    static u32 getNumOfName(muProcCount* proc);
    static u32 getNumOfFriend(muProcCount* proc);
    static u32 getNo1UseChara(muProcCount* proc);
    static u32 getNo2UseChara(muProcCount* proc);
    static u32 getWorstUseChara(muProcCount* proc);
    static u32 getNo1ChampChara(muProcCount* proc);
    static u32 getNo1KetsuChara(muProcCount* proc);
    static u32 getNo1GiveDamageChara(muProcCount* proc);
    static u32 getNo1TakeDamageChara(muProcCount* proc);
    static u32 getBeatKing(muProcCount* proc);
    static u32 getFallKing(muProcCount* proc);
    static u32 getSuicideKing(muProcCount* proc);
};
static_assert(sizeof(muProcCount) == 0x6B0, "Class is wrong size!");

// The saturating add of the record counters (gm_common.cpp inlines its own copy; this is the out-of-line one of the
// main.dol unit before it).
void addMaxCheck(u32* total, u32 add, u32 max);
