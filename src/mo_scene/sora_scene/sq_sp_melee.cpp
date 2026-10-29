#include <gf/gf_scene.h>
#include <gm/gm_global.h>
#include <sq/sq_sp_melee.h>
#include <sr/sr_common.h>
#include <types.h>

extern "C" void fn_8004D010(GameGlobal* global);
extern "C" void fn_80050160(gmGlobalModeMelee* modeMelee);

static gfScene* s_selStage;

sqSpMelee* sqSpMelee::create() {
    return new (Heaps::GlobalMode) sqSpMelee("sqSpMelee");
}

void sqSpMelee::start() {
    if (gfSceneManager::getInstance()->unk31C == 0xff) {
        unk8 = 0xf;
    } else {
        unk8 = 0;
        unk11 = 0;
        s_selStage = gfSceneManager::getInstance()->searchScene("scSelStage");
        fn_8004D010(g_GameGlobal);
        fn_80050160(g_GameGlobal->m_modeMelee);
    }
}

void sqSpMelee::exit() { }
