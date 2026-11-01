#pragma once

#include <StaticAssert.h>
#include <mu/mu_proc_main.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <types.h>

// The Sound option of the Options screen: the balance between the music and the sound effects (a slider from 0 to 100,
// held in the middle for a moment at 50) and an OK icon below it. Moving the slider applies the balance at once; leaving
// the screen saves it in the menu record (m_volBalance).
class muProcOptSound : public muProcMenu {
public:
    enum Entry {
        Entry_Slider = 0,
        Entry_Ok = 1,
        Entry_Count = 2,
        Entry_ObjBase = 86,         // the first object of the entries in m_objs
        Entry_ObjCursor = 85,       // the cursor object; its node animation frame is the entry it is on
        Entry_MdlSlider = 4,        // the model of the slider in m_mdls
    };

    u16 m_volume;                   // 0x66C the balance, 0 ... 100
    u16 m_holdCount;                // 0x66E how long the slider has rested at 50

    virtual ~muProcOptSound() { }
    virtual void init(nw4r::g3d::ScnGroup* group, int from);
    virtual int proc();
    virtual void exit();
    virtual bool selectProc(int dir);
    virtual void playSE_CursorMove();
};
static_assert(sizeof(muProcOptSound) == 0x670, "Class is wrong size!");
