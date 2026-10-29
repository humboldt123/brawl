#include <gf/gf_scene.h>
#include <gm/gm_global.h>
#include <sq/sq_vs_melee.h>
#include <sr/sr_common.h>
#include <types.h>

extern "C" void fn_8004D010(GameGlobal* global);
extern "C" void fn_80050160(gmGlobalModeMelee* modeMelee);

static gfScene* s_selStage;

sqVsMelee* sqVsMelee::create() {
    return new (Heaps::GlobalMode) sqVsMelee("sqVsMelee");
}

void sqVsMelee::start() {
    s32 kind = gfSceneManager::getInstance()->unk31C;
    s_selStage = gfSceneManager::getInstance()->searchScene("scSelStage");
    switch (kind) {
    case 0:
        unk8 = 0;
        unkC = 0;
        fn_8004D010(g_GameGlobal);
        fn_80050160(g_GameGlobal->m_modeMelee);
        break;
    case 0xff:
        unk8 = 0xf;
        unkC = 0xf;
        break;
    }
}

void sqVsMelee::exit() { }
