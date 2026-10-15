#define WN_STATUS_PROC_VECTOR2_DEFAULT_CTOR
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define MT_VEC3F_CTOR_NOINLINE
#define MT_VEC3F_ASSIGN_NOINLINE
#define MT_VEC2F_ASSIGN_NOINLINE
#define MT_VEC2F_PLAIN_BASE
#include <wn/wn_module_accesser_builder.h>
#include <wn/donkey/wn_donkey_sound_effect.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>
#include <wn/wn_kinetic_energy_normal.h>

wnDonkeySoundEffect::wnDonkeySoundEffect(s32 articleId, const wnArticleBuildInfo& info, const wnDonkeySoundEffectData* data) :
    wnWeaponBuilder<wnDonkeySoundEffectModuleAccesserBuildConfig>(
        "wnDonkeySoundEffect", articleId, info,
        wnModuleAccesserBuildData(0x72, data, info.getHeapModule()->getInstanceHeap(), Heaps::FighterTechqniq,
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &m_paramAccesser,
                                  0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param), m_paramAccesser(data->m_param) {
    Weapon::deactivate(false);
}
