#include <gf/gf_application.h>
#include <gf/gf_scene.h>
#include <gm/gm_global.h>
#include <sq/sq_net_any_okiraku.h>
#include <sr/sr_common.h>
#include <types.h>

extern "C" void fn_8004D010(GameGlobal* global);

sqNetAnyOkiraku* sqNetAnyOkiraku::create() {
    return new (Heaps::GlobalMode) sqNetAnyOkiraku("sqNetAnyOkiraku");
}

void sqNetAnyOkiraku::start() {
    unk8 = 0;
    unkC = 0;
    unk10 = g_gfApplication->unk110[0] != 0;
    fn_8004D010(g_GameGlobal);
}

void sqNetAnyOkiraku::exit() {
    g_gfApplication->unk110[0] = unk10;
}
