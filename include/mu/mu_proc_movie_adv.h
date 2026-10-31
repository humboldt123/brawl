#pragma once

#include <StaticAssert.h>
#include <mu/mu_msg.h>
#include <mu/mu_proc_main.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <types.h>

// The Adventure Movies screen of the Vault: the cutscenes of the Adventure Mode that the player has unlocked, nine lines
// at a time with a cursor bar that scrolls the list at its ends. A plays the movie (code 0x14), B goes back to the Movies
// screen (code 0xE).
// HYPOTHESIS: the member names follow how the functions use them and the names of the map (muProcMovieAdv::makeMovieList,
// dispPage, moveCursorMain).
class muProcMovieAdv : public muProcMenu {
public:
    enum {
        Lines = 9,                  // lines that the screen shows
        Id_First = 5,               // the first GmMovieID of the Adventure Mode
        Id_Count = 0x74,            // the movies of the Adventure Mode (GmMovieID 5 ... 0x78)
        Obj_Intro = 132,            // plays when the screen comes in
        Obj_List = 134,             // the list: its texts, the picture of the movie and the arrows
        Obj_Bar = 135,              // the cursor bar
        Mdl_List = 12,              // the model of the screen in m_mdls
        Msg_Buf = 0x100,            // the size of a message of the list
        Msg_Heap = 0x2B,            // the second heap of MuMsg::create (not a HeapType in the symbol)
        Msg_Help = 0xB,             // the help of the screen
        Code_Back = 0xE,            // the code of the Movies screen
        Code_Player = 0x14,         // the code of the screen that plays a movie
    };

    // How far the list is (m_animState).
    enum AnimState {
        Anim_Out = 0,               // gone
        Anim_Coming = 1,
        Anim_Shown = 2,
        Anim_Going = 3,
    };

    u8 m_introDone : 1;             // 0x66C the intro animation has ended
    int m_animState;                // 0x670 AnimState
    int* m_list;                    // 0x674 the unlocked movies (GmMovieID)
    u8 m_count;                     // 0x678 the entries of m_list
    u8 m_cursor;                    // 0x679 the line of the bar
    u8 m_top;                       // 0x67A the first movie of the screen
    MuMsg* m_msg;                   // 0x67C
    void* m_msgData;                // 0x680 the message file of the screen

    virtual ~muProcMovieAdv() { }
    virtual void init(nw4r::g3d::ScnGroup* group, int from);
    virtual int proc();
    virtual void exit();
    virtual void playSE_CursorMove();

    void makeMovieList() __attribute__((never_inline));
    void dispPage(u8 top);
    int moveCursorMain(u32 pad);
};
static_assert(sizeof(muProcMovieAdv) == 0x684, "Class is wrong size!");
