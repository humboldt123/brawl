#pragma once

#include <StaticAssert.h>
#include <mu/mu_proc_main.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <types.h>

// The Records screen of the Data menu: three icons that each open a screen of the menu task (codes 0x15, 0x12, 0x11).
// Cancel goes back to the Data screen (code 6).
class muProcRecord : public muProcMenu {
public:
    enum Entry {
        Entry_Count = 3,
        Entry_ObjBase = 108,        // the first object of the icons in m_objs
        Entry_BackCode = 6,         // the code of the Data screen
    };

    virtual ~muProcRecord() { }
    virtual void init(nw4r::g3d::ScnGroup* group, int from);
    virtual int proc();
    virtual void exit();
    virtual void playSE_CursorMove();
};
static_assert(sizeof(muProcRecord) == 0x66C, "Class is wrong size!");
