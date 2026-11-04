#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define WN_TRANSITION_CTOR_INLINE
#define FT_REL_LINK_EXTERN
#define WN_WEAPON_NO_COMMON_ENERGIES
#define WN_STATUS_NO_TAIL
#include <types.h>
#include <wn/wn_module_accesser_builder.h>
#include <wn/snake/wn_snake_c4_switch.h>
#include <wn/wn_activate_desc.h>

// BYTES: the constructor addresses each singleton and string separately (no pooled data/bss anchor register).
#pragma push
#pragma pool_data off
wnSnakeC4Switch::wnSnakeC4Switch(s32 articleId, const wnArticleBuildInfo& info, const wnSnakeC4SwitchData* data) :
    wnWeaponBuilder<wnSnakeC4SwitchModuleAccesserBuildConfig>(
        "SnakeC4Switch", articleId, info,
        wnModuleAccesserBuildData(0x19, data, info.getHeapModule()->getInstanceHeap(), info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &g_soParamAccesserNull,
                                  0, 0, 0, 0, 0, 0, 0)) {
    Weapon::deactivate(false);
}
#pragma pop

void wnSnakeC4Switch::activate(int founderTaskId, int etcResourceId, int team) {
    Vec3f pos(0.0f, 0.0f, 0.0f);
    wnActivateDesc desc(founderTaskId, etcResourceId, etcResourceId, etcResourceId, -1, -1, 0, 0, pos, 0.0f, team, 0, 2,
                        0x80, 0, 0x35F, 0);
    desc.flagsHighPair = 3;
    desc.flagsMidPair = 0;
    desc.flagsBit3 = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    Weapon::activate(&desc);
}
