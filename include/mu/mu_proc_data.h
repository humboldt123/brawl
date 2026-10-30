#pragma once

#include <StaticAssert.h>
#include <mu/mu_proc_main.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <types.h>

// The Data screen of the main menu: three icons, each opens a screen of the menu task (codes 0xE, 0xF, 0x10: the code is
// 0xE plus the entry).
class muProcData : public muProcMenu {
public:
    enum Entry {
        Entry_Count = 3,
        Entry_ObjBase = 36,         // the first object of the icons in m_objs
        Entry_FirstCode = 0xE,      // the code of the first icon
    };

    virtual ~muProcData() { }
    virtual void init(nw4r::g3d::ScnGroup* group, int from);
    virtual int proc();
    virtual void exit();
    virtual void playSE_CursorMove();
};
static_assert(sizeof(muProcData) == 0x66C, "Class is wrong size!");
