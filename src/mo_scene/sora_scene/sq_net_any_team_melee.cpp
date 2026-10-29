#include <gf/gf_application.h>
#include <gf/gf_scene.h>
#include <gm/gm_global.h>
#include <sq/sq_net_any_team_melee.h>
#include <sr/sr_common.h>
#include <types.h>

extern "C" void fn_8004D010(GameGlobal* global);

sqNetAnyTeamMelee* sqNetAnyTeamMelee::create() {
    return new (Heaps::GlobalMode) sqNetAnyTeamMelee("sqNetAnyTeamMelee");
}

void sqNetAnyTeamMelee::start() {
    unk8 = 0;
    unkC = 0;
    unk10 = g_gfApplication->unk110[0] != 0;
    fn_8004D010(g_GameGlobal);
}

void sqNetAnyTeamMelee::exit() { }
