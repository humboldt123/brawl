#pragma once

#include <StaticAssert.h>
#include <mu/mu_proc_main.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <types.h>

// The Options screen of the main menu: seven icons, each opens a screen of the menu task (the code of an icon is 7 plus
// its entry). Coming back from one of them saves the game if an option changed (the autosave dirty flag).
class muProcOption : public muProcMenu {
public:
    enum Entry {
        Entry_Count = 7,
        Entry_ObjBase = 29,         // the first object of the icons in m_objs
        Entry_FirstCode = 7,        // the code of the first icon
    };

    virtual ~muProcOption() { }
    virtual void init(nw4r::g3d::ScnGroup* group, int from);
    virtual int proc();
    virtual void exit();
    virtual void playSE_CursorMove();
};
static_assert(sizeof(muProcOption) == 0x66C, "Class is wrong size!");
