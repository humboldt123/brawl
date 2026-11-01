#pragma once

#include <StaticAssert.h>
#include <mu/mu_proc_main.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <types.h>

// The Screen option of the Options screen: two icons, standard (4:3) and widescreen (16:9). Moving the cursor switches the
// camera to the ratio at once; B goes back and saves the choice in the menu record (m_isWidescreen).
class muProcOptScreen : public muProcMenu {
public:
    enum Entry {
        Entry_Standard = 0,
        Entry_Wide = 1,
        Entry_Count = 2,
        Entry_ObjBase = 39,         // the first object of the icons in m_objs
    };

    virtual ~muProcOptScreen() { }
    virtual void init(nw4r::g3d::ScnGroup* group, int from);
    virtual int proc();
    virtual void exit();
    virtual void playSE_CursorMove();
};
static_assert(sizeof(muProcOptScreen) == 0x66C, "Class is wrong size!");
