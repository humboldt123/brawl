#pragma once

#include <StaticAssert.h>
#include <mu/mu_proc_main.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <types.h>

// The Deflicker option of the Options screen: one switch icon (its texture frame shows off/on). A toggles the video
// deflicker at once; B goes back and saves the setting in the menu record (m_deflicker).
class muProcOptDeflick : public muProcMenu {
public:
    enum Entry {
        Entry_Obj = 41,             // the switch icon in m_objs
    };

    virtual ~muProcOptDeflick() { }
    virtual void init(nw4r::g3d::ScnGroup* group, int from);
    virtual int proc();
    virtual void exit();
    virtual void playSE_CursorMove();
};
static_assert(sizeof(muProcOptDeflick) == 0x66C, "Class is wrong size!");
