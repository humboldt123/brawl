#include <gf/gf_application.h>
#include <gf/gf_scene.h>
#include <gm/gm_global.h>
#include <sq/sq_qu_melee.h>
#include <sr/sr_common.h>
#include <types.h>

extern "C" void fn_8004D010(GameGlobal* global);

static u8 s_handicap;

sqQuMelee* sqQuMelee::create() {
    return new (Heaps::GlobalMode) sqQuMelee("sqQuMelee");
}

void sqQuMelee::start() {
    unk8 = 0;
    unk18 = g_gfApplication->unk110[0] != 0;
    fn_8004D010(g_GameGlobal);
    s_handicap = g_GameGlobal->m_setRule->m_handicap;
    g_GameGlobal->m_setRule->m_handicap = 0;
}

void sqQuMelee::exit() {
    g_gfApplication->unk110[0] = unk18;
}
