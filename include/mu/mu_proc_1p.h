#pragma once

#include <StaticAssert.h>
#include <mu/mu_proc_main.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <types.h>

// The Solo screen of the main menu: six icons the cursor moves between. Five of them start a scene (the code of the icon
// becomes the start code of the scene that opens), the Stadium icon opens the Stadium screen of the same menu task.
// The base class lives in sora_menu_rule (mu_proc_menu.cpp): this module only adds the screens.
// HYPOTHESIS: the names of the entries follow the codes the screen sends (4 Classic ... 0xC Training) and the icon that
// All-Star (entry 2) hides when GameGlobal::checkHideMenuAllstar says so.
class muProc1p : public muProcMenu {
public:
    enum Entry {
        Entry_Classic = 0,
        Entry_Adventure = 1,
        Entry_AllStar = 2,
        Entry_Event = 3,
        Entry_Stadium = 4,
        Entry_Training = 5,
        Entry_Count = 6,
        Entry_ObjBase = 12,         // the first object of the icons in m_objs
    };

    virtual ~muProc1p() { }
    virtual void init(nw4r::g3d::ScnGroup* group, int from);
    virtual int proc();
    virtual void exit();
    virtual void playSE_CursorMove();
};
static_assert(sizeof(muProc1p) == 0x66C, "Class is wrong size!");
