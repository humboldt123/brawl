#pragma once

#include <StaticAssert.h>
#include <mu/mu_msg.h>
#include <mu/mu_proc_main.h>
#include <mu/mu_scn_mdl_expand.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <types.h>

// The Movies screen of the Vault: the movies the player has unlocked, six to a page in two rows, and an icon that opens
// the screen of the movies of the Adventure Mode (muProcMovieAdv). A moves on to the player of the movie (code 0x14) or
// to that screen (code 0x13), B goes back (code 6).
// HYPOTHESIS: the member names follow how the functions use them and the names of the map (muProcMovie::makeMovieList,
// dispPage, moveCursorMain).
class muProcMovie : public muProcMenu {
public:
    enum {
        Slots = 6,                  // movies on a page
        Id_Count = 5,               // the movies that are listed here (GmMovieID 0 ... 4)
        Id_FirstAdv = 5,            // the first movie of the Adventure Mode (GmMovieID 5 ... 0x78)
        Id_Adv = 0x79,              // the "Adventure Mode" icon: not a movie, it opens the other screen
        Obj_Page = 125,             // the page markers: node animation (how many movies), visibility (which arrows)
        Obj_Slot = 126,             // the first of the six movie icons
        Obj_Adv = 132,              // the Adventure Mode icon
        Obj_Cursor = 133,           // the frame that is put on the icon the cursor is on
        Mdl_Slots = 11,             // the model of the screen in m_mdls: the group of the icons (pos00 ...)
        Msg_Buf = 0x100,            // the size of a message of the list
        Msg_Count = 7,              // a title for each icon, and the one of the Adventure Mode icon
        Msg_Heap = 0x2B,            // the second heap of MuMsg::create (not a HeapType in the symbol)
        Msg_AdvTitle = 5,           // the title of the Adventure Mode icon
        Msg_AdvHelp = 0xB,          // the help of the Adventure Mode icon
        Code_Back = 6,              // the code of the Vault screen
        Code_Adv = 0x13,            // the code of the Adventure Movies screen
        Code_Player = 0x14,         // the code of the screen that plays a movie
    };

    // How far the icons are (m_animState).
    enum AnimState {
        Anim_Out = 0,               // gone
        Anim_Coming = 1,
        Anim_Shown = 2,
        Anim_Going = 3,
    };

    u8 m_unk66C_0 : 1;              // 0x66C (HYPOTHESIS: cleared by init, never set)
    u8 m_helpShown : 1;             // 0x66C the help of the icon the cursor is on was written
    int m_animState;                // 0x670 AnimState
    int* m_list;                    // 0x674 the movies to show: the Adventure Mode icon first, then the unlocked movies
    u8 m_count;                     // 0x678 the entries of m_list
    u8 m_page;                      // 0x679
    u8 m_col;                       // 0x67A the icon of the row that the cursor is on
    u8 m_row;                       // 0x67B
    MuMsg* m_msg;                   // 0x67C
    int m_unk680;                   // 0x680
    void* m_msgData;                // 0x684 the message file of the screen

    virtual ~muProcMovie() { }
    virtual void init(nw4r::g3d::ScnGroup* group, int from);
    virtual int proc();
    virtual void exit();
    virtual void playSE_CursorMove();

    void makeMovieList();
    void dispPage(int page);
    int moveCursorMain(u32 pad);
};
static_assert(sizeof(muProcMovie) == 0x688, "Class is wrong size!");
