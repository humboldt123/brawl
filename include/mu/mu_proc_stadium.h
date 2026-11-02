#pragma once

#include <mu/mu_msg.h>
#include <mu/mu_proc_menu.h>
#include <nw4r/g3d/g3d_obj.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <types.h>

// The Stadium screen: a row of three games (four once the boss battles are open), left and right move the cursor, A
// starts the game under it. The third game opens a second page with the six kinds of the multi-man brawl, where up and
// down move the cursor.
// HYPOTHESIS: the names of the fields, of the games and of the steps follow what the functions do and the menu of the game.
class muProcStadium : public muProcMenu {
public:
    enum Step {
        Step_Come = 0,              // the panel comes in
        Step_Select = 1,            // the games
        Step_KumiteCome = 2,        // the page of the multi-man brawl comes in
        Step_KumiteSelect = 3,      // the kinds of the multi-man brawl
        Step_KumiteLeave = 4,       // B: back to the games
        Step_Leave = 5,             // B: the panel goes out, then back to the main menu
    };

    // The games, in the order of the row.
    enum Game {
        Game_TargetSmash = 0,
        Game_HomeRun = 1,
        Game_Kumite = 2,            // the multi-man brawl: opens the second page
        Game_BossBattle = 3,
        GameCount = 4,
    };

    enum Entry {
        KumiteCount = 6,            // the kinds of the multi-man brawl
        Obj_Panel = 0xC3,
        Obj_Cursor = 0xC4,          // the frame of the cursor slides along the row (frame: the place of the game)
        Obj_Game0 = 0xC5,           // the icons of the four games (frame 1: under the cursor)
        Obj_KumitePanel = 0xC9,
        Obj_KumiteCursor = 0xCA,
        Mdl_Row = 0x1A,             // the models of the screen in m_mdls: the row, the icons put into it,
        Mdl_Icons = 0x1B,
        Mdl_Kumite = 0x1C,          // and the page of the multi-man brawl
        Msg_Title = 0,              // the name of the game under the cursor
        Msg_Help = 1,               // the help: Msg_HelpGame + game, or Msg_HelpKumite + kind
        Msg_Buf = 0x400,
        Msg_HelpGame = 5,
        Msg_HelpKumite = 0xA,
    };

    MuMsg* m_msg;                   // 0x66C
    u32 m_gameCount;                // 0x670 3, or 4 with the boss battles
    int m_games[GameCount];         // 0x674 the game of each place of the row

    muProcStadium(MuObject** objs, nw4r::g3d::G3dObj** mdls, void* msgData);
    virtual ~muProcStadium() { delete m_msg; }
    virtual void init(nw4r::g3d::ScnGroup* group, int from);
    virtual int proc();
    virtual void exit();
    virtual bool selectProc(int dir);
    virtual void playSE_CursorMove();
    bool selectKumite(int dir);
};
