#pragma once

#include <StaticAssert.h>
#include <mu/mu_proc_main.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <types.h>

// The Group screen of the main menu: six icons. Three start a scene (Brawl, Special Brawl, Rotation), three open a
// screen of the same menu task (Rules, Tourney, Names).
// HYPOTHESIS: the names of the entries follow the codes the screen sends and the screens it returns from (0x18 Rules,
// 0x1E Tourney, 0x16 Names, which also saves the game).
class muProcVs : public muProcMenu {
public:
    enum Entry {
        Entry_Brawl = 0,
        Entry_Rule = 1,
        Entry_Special = 2,
        Entry_Rotation = 3,
        Entry_Tourney = 4,
        Entry_Name = 5,
        Entry_Count = 6,
        Entry_ObjBase = 6,          // the first object of the icons in m_objs
    };

    virtual ~muProcVs() { }
    virtual void init(nw4r::g3d::ScnGroup* group, int from);
    virtual int proc();
    virtual void exit();
    virtual void playSE_CursorMove();
};
static_assert(sizeof(muProcVs) == 0x66C, "Class is wrong size!");
