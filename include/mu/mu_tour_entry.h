#pragma once

#include <StaticAssert.h>
#include <gf/gf_file_io_handle.h>
#include <gf/gf_task.h>
#include <memory.h>
#include <mu/menu.h>
#include <mu/mu_menu_controller.h>
#include <mu/mu_menuroot.h>
#include <mu/mu_msg.h>
#include <mu/mu_object.h>
#include <mu/mu_tour_table.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <types.h>

// One fighter of a match as the scene gives it to the screen of the match.
// HYPOTHESIS: the names follow how the screen reads the fields.
struct muTourFighter {
    int m_kind;                 // 0x00 0: a player's, 1: a CPU, 2: nobody (the fighter has a bye)
    char m_name[0x15];          // 0x04
    u8 m_nameTag;               // 0x19 the number of the saved name (0x78: none)
    u8 m_unk1A[2];
    int m_character;            // 0x1C the kind of the fighter in the menu
    u8 m_color;                 // 0x20 the costume
    u8 m_unk21[3];
    int m_slot;                 // 0x24 the place of the fighter in the table
    u8 m_cpuLevel;              // 0x28 the level of the CPU
    u8 m_unk29;
    u16 m_handicap;             // 0x2A
};
static_assert(sizeof(muTourFighter) == 0x2C, "Class is wrong size!");

// The screen of a match of the tournament: the table moves to the match, the fighters walk to the node, the players pick the
// pads that play which fighter, and the match starts. It also shows the winner of the tournament.
class muTourEntryTask : public gfTask {
public:
    enum State {
        State_Load = 0,
        State_Appear = 1,           // the title appears
        State_Title = 2,            // waits for A
        State_TitleOut = 3,         // the title goes away
        State_Wait = 4,             // the screen waits for a while
        State_Table = 5,            // the winner of a match walks in the table, the next fighters are prepared
        State_ShowMatch = 6,        // the table is moved to the match, the fighters walk to it
        State_PickPad = 7,          // a player picks a pad (the next slot gets its pad)
        State_PickPadAnim = 8,
        State_WaitPad = 9,          // waits for the players to press A
        State_PadTaken = 10,
        State_PadChosen = 11,
        State_PadFrame = 12,        // a pad is on a slot
        State_PadAsk = 13,          // the player is asked to take the pad off a slot
        State_Confirm = 14,         // start the match or pick the fighters by chance
        State_FinalWinner = 15,     // the winner of the tournament walks to the end of the table
        State_FinalWait = 16,
        State_FinalEnd = 17,
        State_FinalAsk = 18,        // the end of the tournament: go on, or end the tournament
        State_Bye = 19,
        State_Leave = 20,
    };

    // How the screen ends (m_result).
    enum Result {
        Result_Start = 1,           // the match starts
        Result_Next = 2,            // the tournament goes on
        Result_End = 3,             // the tournament ends
        Result_Auto = 4,            // the match is decided by chance
        Result_AutoFinal = 5,       // the final is decided by chance
    };

    MenuRoot* m_root;                           // 0x040
    int m_state;                                // 0x044
    int m_result;                               // 0x048
    int m_timer;                                // 0x04C
    bool m_dialogPending;                       // 0x050 the balloon of the pads is shown until its animation is over
    muMenuController m_ctrl;                    // 0x054
    MuMsg* m_msg;                               // 0x0FC the texts of the screen
    MuMsg* m_msgNames;                          // 0x100 the names of the fighters
    muTourFighter m_fighters[4];                // 0x104 the fighters of the match
    muTourFighter m_nextFighters[4];            // 0x1B4 the fighters of the next match (the winners of this one)
    bool m_hasNext;                             // 0x264
    nw4r::g3d::ResFile m_resGame;               // 0x268 the models of the screen
    nw4r::g3d::ResFile m_resFace[8];            // 0x26C the pictures of the fighters (the ones of the next match come last)
    MuObject* m_objs[0x13];                     // 0x28C
    nw4r::g3d::ScnGroup* m_mdls[7];             // 0x2D8
    muObjProcessTask* m_objProc;                // 0x2F4
    muTourTable* m_table;                       // 0x2F8
    int m_cursor;                               // 0x2FC 0: the first button
    int m_count;                                // 0x300 how many fighters take part in the match
    int m_nextCount;                            // 0x304
    int m_sizeIndex;                            // 0x308
    int m_round;                                // 0x30C
    int m_match;                                // 0x310
    u32 m_roundKind;                            // 0x314 10: ..., 11: the final
    u32 m_taken;                                // 0x318 which fighters have a pad (or are not played by a player)
    u32 m_skipped;                              // 0x31C which fighters are a bye
    int m_pad[4];                               // 0x320 the pad of each fighter (-1: none)
    int m_curSlot;                              // 0x330 the fighter that the pad is picked for
    int m_prevSlot;                             // 0x334
    int m_winner;                               // 0x338
    int m_finalWinner;                          // 0x33C
    bool m_loaded[12];                          // 0x340 which of the files are loaded (and some flags)
    gfFileIOHandle m_files[10];                 // 0x34C
    int m_mode;                                 // 0x374 0, 1: a match; 2: the end of the tournament
    int m_voiceState;                           // 0x378
    int m_voiceTimer;                           // 0x37C

    muTourEntryTask(int mode, int round, int match, int roundKind, int count, int nextCount, muTourTable* table, const muTourFighter* fighters,
                    const muTourFighter* nextFighters);
    virtual void processAnim();
    virtual ~muTourEntryTask();

    void loadResource();
    void setName();
    void setCtrlFrame(MuObject* object, const gfPadStatus* pad, u32 padNo, u32 msgIndex);
    void setNextGame(int round, int match, int roundKind, int nextCount, const muTourFighter* nextFighters);
    void setDrawBye();
    void setFinalWinner(u32 winner);
    void initMdlData();
    void setVsState();
};
static_assert(sizeof(muTourEntryTask) == 0x380, "Class is wrong size!");
