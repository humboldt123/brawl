#include <gf/gf_application.h>
#include <gf/gf_scene.h>
#include <gm/gm_global.h>
#include <sq/sq_net_any_team_melee.h>
#include <sr/sr_common.h>
#include <types.h>


sqNetAnyTeamMelee* sqNetAnyTeamMelee::create() {
    return new (Heaps::GlobalMode) sqNetAnyTeamMelee("sqNetAnyTeamMelee");
}

void sqNetAnyTeamMelee::start() {
    unk8 = 0;
    unkC = 0;
    unk10 = g_gfApplication->unk110[0] != 0;
    g_GameGlobal->initSelCharData();
}

void sqNetAnyTeamMelee::exit() { }
