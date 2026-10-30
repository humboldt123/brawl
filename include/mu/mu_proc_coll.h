#pragma once

#include <StaticAssert.h>
#include <mu/mu_proc_main.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <types.h>

// The Vault screen of the main menu: seven icons. Six of them start a scene; the first opens the trophy screen
// (muProcFigure) of the same menu task.
// HYPOTHESIS: the name of the first entry comes from the code of the screen it opens (0x17, the one muProcFigure
// returns with); the other icons are only known by the codes they send.
class muProcColl : public muProcMenu {
public:
    enum Entry {
        Entry_Figure = 0,
        Entry_Count = 7,
        Entry_ObjBase = 22,         // the first object of the icons in m_objs
    };

    virtual ~muProcColl() { }
    virtual void init(nw4r::g3d::ScnGroup* group, int from);
    virtual int proc();
    virtual void exit();
    virtual void playSE_CursorMove();
};
static_assert(sizeof(muProcColl) == 0x66C, "Class is wrong size!");
