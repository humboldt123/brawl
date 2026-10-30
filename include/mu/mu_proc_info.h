#pragma once

#include <StaticAssert.h>
#include <gm/gm_global.h>
#include <mu/mu_msg.h>
#include <mu/mu_proc_main.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <types.h>

// The Notices screen of the Records menu: the notices the player has received (the unlock messages of the Info flags),
// four to a page with the date each one arrived, the oldest first. Cancel goes back to the Records screen (code 0x10).
// HYPOTHESIS: the member names follow how the functions use them and the names of the map (muProcInfo::makeList,
// dispPage, moveCursor, setPanelState).
class muProcInfo : public muProcMenu {
public:
    enum {
        Rows = 4,                   // notices on a page
        Info_Count = 117,           // notices the game knows (muMenu::getInfoMsgID)
        Info_Max = 128,             // notices the flag block has room for (the list is built on the stack)
        Obj_Panel = 111,            // the window of the list: node + texture srt animations (comes in, goes out)
        Obj_Arrow = 112,            // the scroll arrows: node animation, visibility (which arrows), material color (the flash)
        Obj_Row = 113,              // the first of the four rows; the material color marks the cursor row
        Mdl_Back = 9,               // the model of the screen in m_mdls
        Msg_Buf = 0x182,            // the size of a message of the list
        Msg_Count = 8,              // two messages a row: the notice and its date
        Msg_Heap = 0x2B,            // the second heap of MuMsg::create (not a HeapType in the symbol)
        Code_Back = 0x10,           // the code of the Records screen
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
    int* m_list;                    // 0x678 the notices that were received, sorted by their date (gfHeapManager::calloc)
    int m_listCount;                // 0x67C
    int m_row;                      // 0x680 the row the cursor is on
    int m_top;                      // 0x684 the first notice on the page
    MuMsg* m_msg;                   // 0x688
    void* m_msgData;                // 0x68C the message file of the notices

    virtual ~muProcInfo() { }
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
    void makeList();
};
static_assert(sizeof(muProcInfo) == 0x690, "Class is wrong size!");
