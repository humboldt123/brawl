#pragma once

#include <StaticAssert.h>
#include <gf/gf_pad_status.h>
#include <mu/mu_menu_controller.h>
#include <mu/mu_menu_main.h>
#include <mu/mu_object.h>
#include <mu/mu_panel_ctrl.h>
#include <nw4r/g3d/g3d_obj.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <types.h>

// The menu that sets the rules of a match is made of one task and several "procs", one for each screen of the menu. A proc
// knows how to draw its screen, how to move the cursor and how to take the input; the task decides which proc runs.
// HYPOTHESIS: the names follow how the menu uses the fields and the names of the functions in the map of the module.

// A node of the graph that the cursor moves in: where it goes when the player presses a direction, and whether holding the
// direction repeats the move.
struct muProcMenuNode {
    struct Link {
        u16 m_next;         // the node the cursor goes to
        u8 m_repeat;        // the direction repeats while it is held
        u8 m_pad;
    };
    Link m_up;              // 0x00
    Link m_down;            // 0x04
    Link m_left;            // 0x08
    Link m_right;           // 0x0C
};
static_assert(sizeof(muProcMenuNode) == 0x10, "Class is wrong size!");

// The base of the screens of the menu of the rules: the pads, the cursor, and the objects and the models that the screen
// draws with.
class muProcMenu {
public:
    // What padUpdate returns: the buttons of the menu that were pressed in the frame.
    enum PadFlag {
        Pad_Up = 0x1,
        Pad_Down = 0x2,
        Pad_Left = 0x4,
        Pad_Right = 0x8,
        Pad_Ok = 0x10,          // A or Start; m_padNo is the pad that pressed it
        Pad_Cancel = 0x20,      // B
        Pad_L = 0x40,
        Pad_R = 0x80,
        Pad_Start = 0x100,
        Pad_A = 0x200,
        Pad_X = 0x400,
        Pad_Y = 0x800,
        Pad_All = 0x1000,       // A, Start, L and R together
    };

    gfPadStatus m_pad;                  // 0x00 the pad of the menu: all the pads that m_padMask lets in, put together
    s16 m_padNo;                        // 0x40 the pad that pressed A or Start the last time
    u16 m_cursor;                      // 0x42 the node the cursor is on
    u8 m_padMask;                       // 0x44 the pads that can use the menu (bit n: pad n; 0xFF: the pad of the system menu)
    u8 m_unk45[3];                      // 0x45
    muMenuController m_ctrl[9];         // 0x48 the controllers of the pads 0 ... 7, then the one of the system menu
    int m_unk630;                       // 0x630
    int m_result;                       // 0x634 the proc/entry that the player chose (0x1E + the number of the proc)
    int m_code;                         // 0x638 HYPOTHESIS: the code that the cursor entry of this proc stands for
    muProcMenuNode* m_nodes;            // 0x63C
    u16 m_itemCount;                    // 0x640 how many items the cursor goes through
    int m_objBase;                      // 0x644 the first object of the items in m_objs
    MuAnimIdxData* m_anims;             // 0x648 the animations of the items (come, wait, select, leave)
    MuObject** m_objs;                  // 0x64C
    nw4r::g3d::G3dObj** m_mdls;         // 0x650
    nw4r::g3d::ScnGroup* m_group;       // 0x654 the group of the scene the objects are put in
    muPanelCtrlTask* m_panel;           // 0x658
    int m_unk65C;                       // 0x65C
    int m_unk660;                       // 0x660
    int m_unk664;                       // 0x664

    muProcMenu(MuObject** objs, nw4r::g3d::G3dObj** mdls);
    virtual ~muProcMenu() { }
    virtual void init(nw4r::g3d::ScnGroup* group, int from) { m_group = group; }
    virtual void setPanelTask(muPanelCtrlTask* panel) { m_panel = panel; }
    virtual int proc() { return 1; }
    virtual void exit() { }
    virtual void setCtrlFlg(u8 mask);
    virtual bool selectProc(int dir);
    virtual void playSE_CursorMove() = 0;

    u32 padUpdate();
    void padRepeatClear(int buttons);
    void setAnimFrame(int obj, float frame, u32 flags);
    void setAnimUpdateRate(int obj, float rate, u32 flags);
    static int getFixedControllerID();
    int getSceneStartCode();            // not static: the screens of the main menu pass this to it
    void exitScene(int code, bool force);
    void setComeAnim(bool keepFrame);
    void setSelectAnim(int item, bool selected);
    void setOutAnim(bool selected);
};
static_assert(sizeof(muProcMenu) == 0x66C, "Class is wrong size!");
