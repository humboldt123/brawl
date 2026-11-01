#pragma once

#include <StaticAssert.h>
#include <mu/mu_msg.h>
#include <mu/mu_proc_main.h>
#include <nw4r/g3d/g3d_obj.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <types.h>

// The Rumble option of the Options screen: two parts, switched with left and right. In the first, each pad that is
// plugged in is drawn by type in its slot, and pressing A on a pad switches the rumble of that pad (it rumbles when it is
// turned on). In the second, a list of the names (five lines, scrolled with up and down) and A on any pad switches the
// rumble of the chosen name.
// HYPOTHESIS: the names of the parts and of the fields follow what the functions do.
class muProcOptVib : public muProcMenu {
public:
    enum Step {
        Step_Come = 0,              // the parts come in
        Step_Select = 1,
        Step_Leave = 2,             // the parts go out
    };

    enum Entry {
        Entry_Pads = 0,             // the rumble of each pad
        Entry_Names = 1,            // the rumble of each name
        Entry_ObjBase = 0x2A,       // the objects of the two parts in m_objs; the one of the names shows the scroll arrows
        Obj_PadModel0 = 0x2C,       // four models of each type of pad (by pad % 4), one group of four per type
        Obj_PadSlot0 = 0x3C,        // the slots of the eight pads
        Obj_NameRow0 = 0x44,        // the five lines of the list
        Mdl_Pads = 0,               // the model the pad models are attached to (its nodes are pos00 ... pos07)
        Mdl_Names = 1,
        PadCount = 8,
        RowCount = 5,
    };

    int m_padType[PadCount];        // 0x66C the type of each pad (a gfPadType::PadType; -1: not plugged in)
    u16 m_nameNo;                   // 0x68C the name the cursor is on
    u16 m_scroll;                   // 0x68E the first name of the list
    u16 m_row;                      // 0x690 the line the cursor is on
    MuMsg* m_msg;                   // 0x694 the five lines of the list

    muProcOptVib(MuObject** objs, nw4r::g3d::G3dObj** mdls);
    virtual ~muProcOptVib() { delete m_msg; }
    virtual void init(nw4r::g3d::ScnGroup* group, int from);
    virtual int proc();
    virtual void exit();
    virtual bool selectProc(int dir);
    virtual void playSE_CursorMove();
    void setPadModel(int pad);
};
static_assert(sizeof(muProcOptVib) == 0x698, "Class is wrong size!");
