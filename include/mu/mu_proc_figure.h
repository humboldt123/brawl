#pragma once

#include <StaticAssert.h>
#include <mu/mu_proc_main.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <types.h>

// The trophy screen of the Vault: five icons that each start a scene (codes 0xF ... 0x13). Cancel goes back to the
// Vault screen (code 4). Unlike the other screens, its icons go straight from the come animation to the cursor state.
// HYPOTHESIS: the icons are Trophy Gallery, Trophy Hoard, Coin Launcher, Sticker Album and Sticker Center.
class muProcFigure : public muProcMenu {
public:
    enum Entry {
        Entry_Count = 5,
        Entry_ObjBase = 175,        // the first object of the icons in m_objs
        Entry_MsgBase = 55,         // the message of the help balloon of the first icon
        Entry_BackCode = 4,         // the code of the Vault screen
    };

    virtual ~muProcFigure() { }
    virtual void init(nw4r::g3d::ScnGroup* group, int from);
    virtual int proc();
    virtual void exit();
    virtual void playSE_CursorMove();
};
static_assert(sizeof(muProcFigure) == 0x66C, "Class is wrong size!");
