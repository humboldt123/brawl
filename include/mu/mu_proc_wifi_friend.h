#pragma once

#include <StaticAssert.h>
#include <mu/mu_proc_main.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <types.h>

// The "With Friends" screen of the Wi-Fi menu: four icons that each start an online scene (codes 0x18 ... 0x1B).
// HYPOTHESIS: the icons are Brawl, Team Brawl, Home-Run Contest and Multi-Man Brawl (the friend scenes of sora_scene).
class muProcWifiFriend : public muProcMenu {
public:
    enum Entry {
        Entry_Count = 4,
        Entry_ObjBase = 203,        // the first object of the icons in m_objs
        Entry_MsgBase = 45,         // the message of the help balloon of the first icon
        Entry_FirstCode = 0x18,     // the code of the first icon
    };

    virtual ~muProcWifiFriend() { }
    virtual void init(nw4r::g3d::ScnGroup* group, int from);
    virtual int proc();
    virtual void exit();
    virtual void playSE_CursorMove();
};
static_assert(sizeof(muProcWifiFriend) == 0x66C, "Class is wrong size!");
