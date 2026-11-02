#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define FT_REL_LINK_EXTERN
#define MT_VEC3F_CTOR_NOINLINE
#include <wn/wn_module_accesser_builder.h>
#include <wn/falco/wn_falco_illusion.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>

#include <wn/fox/wn_fox_illusion_util.h>

wnFalcoIllusion::wnFalcoIllusion(s32 articleId, const wnArticleBuildInfo& info, const wnFalcoIllusionData* data) :
    wnWeaponBuilder<wnFalcoIllusionModuleAccesserBuildConfig>(
        "FalcoIllusion", articleId, info,
        wnModuleAccesserBuildData(37, data,
            info.getHeapModule()->getInstanceHeap(),
            info.getHeapModule()->getNWModelInstanceHeap(),
            info.getHeapModule()->getNWMotionInstanceHeap(),
            static_cast<u8>(data->m_resGroupNo), -1, &m_paramAccesser,
            0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param), m_paramAccesser(data->m_param) {
    Weapon::deactivate(false);
}

void wnFalcoIllusion::activate(s32 founderTaskId, s32 unk1, s32 team, float lr, s32 unk2, s32 unk3) {
    Vec3f pos(0.0f, 0.0f, 0.0f);
    wnActivateDesc desc(founderTaskId, 0xFFFF, 0xFFFF, 0xFFFF, -1, -1, 0, 0, pos, lr, team, 0, 2, 0x80, 0, 0x35F, 0);
    desc.flagsHigh = 1;
    desc.flagsMidHigh = 0;
    desc.flagsMidLow = 1;
    desc.flagsLow = 0;
    desc.unk4D = (desc.unk4D & ~0xC0) | 0x80;

    Weapon::activate(&desc);
    wnFoxIllusionUtil::activate(m_moduleAccesser, static_cast<u8>(unk2), unk3);
}

void wnFalcoIllusion::processUpdate() {
    wnFoxIllusionUtil::processUpdate(m_moduleAccesser);
    Weapon::processUpdate();
}

bool wnFalcoIllusion::notifyEventCollisionAttackCheck(u32) {
    return false;
}
