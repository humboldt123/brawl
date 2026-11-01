#pragma once

#include <StaticAssert.h>
#include <mu/mu_msg.h>
#include <mu/mu_proc_main.h>
#include <nw4r/g3d/g3d_obj.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <types.h>

// The Delete Data option of the Options screen: five entries (four kinds of data and, last, all the data). Choosing one
// opens a window that asks twice (three times for all the data) whether to delete it, with Yes and No switched by left
// and right; then muDeleteNandTask deletes the data and the window waits for a button.
// HYPOTHESIS: the names of the steps follow the texts and the sounds of each step.
class muProcOptDel : public muProcMenu {
public:
    enum Step {
        Step_Come = 0,              // the entries come in
        Step_Select = 1,            // the cursor is on an entry
        Step_Confirm1 = 2,          // the window asks the first time
        Step_Confirm2 = 3,          // the second time
        Step_Confirm3 = 4,          // the third time (all the data)
        Step_Close = 5,             // the window goes out
        Step_Delete = 6,            // the deleting starts
        Step_Deleting = 7,          // muDeleteNandTask is deleting
        Step_Deleted = 8,           // the window says it is done and waits for a button
        Step_Leave = 9,             // the entries go out
    };

    enum Entry {
        Entry_All = 4,              // all the data
        Entry_Count = 5,
        Entry_ObjBase = 99,         // the first object of the entries in m_objs
        Entry_ObjWindow = 104,      // the window of the questions; its texts are the three lines of m_msg
    };

    MuMsg* m_msg;                   // 0x66C the texts of the window
    u8 m_no;                        // 0x670 the window is on No (0: Yes)
    s32 m_seHandle;                 // 0x674 the sound of the question that is asked

    muProcOptDel(MuObject** objs, nw4r::g3d::G3dObj** mdls, void* msgData);
    virtual ~muProcOptDel();
    virtual void init(nw4r::g3d::ScnGroup* group, int from);
    virtual int proc();
    virtual void exit();
    virtual void playSE_CursorMove();
};
static_assert(sizeof(muProcOptDel) == 0x678, "Class is wrong size!");
