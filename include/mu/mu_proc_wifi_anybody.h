#pragma once

#include <StaticAssert.h>
#include <mu/mu_proc_main.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <types.h>

// The "With Anyone" screen of the Wi-Fi menu: three icons that each start an online scene (codes 0x1D ... 0x1F). The
// first icon is only shown when the network condition allows it (nteGlobal::isEnableButton(0x10)); without it the two
// others move left and the cursor graph skips it.
// HYPOTHESIS: the icons are Basic Brawl, Team Brawl and Spectator (the anybody scenes of sora_scene).
class muProcWifiAnybody : public muProcMenu {
public:
    enum Entry {
        Entry_Count = 3,
        Entry_ObjBase = 207,        // the first object of the icons in m_objs
        Entry_MsgBase = 50,         // the message of the help balloon of the first icon
        Entry_FirstCode = 0x1D,     // the code of the first icon
    };

    virtual ~muProcWifiAnybody() { }
    virtual void init(nw4r::g3d::ScnGroup* group, int from);
    virtual int proc();
    virtual void exit();
    virtual void playSE_CursorMove();
};
static_assert(sizeof(muProcWifiAnybody) == 0x66C, "Class is wrong size!");
