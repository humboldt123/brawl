#define IP_INPUT_BUTTON_RAW // BYTES: IpNull::getButton returns the raw button mask (as in wn_pit_ikarus.cpp)
#define WN_SHARED_ARRAYS_1 // the one-element attack part vector is the sora_melee copy
#define WN_STATUS_NO_TAIL // the null kinetic builder follows the status builder directly
#define WN_WEAPON_NO_COMMON_ENERGIES // no movement energies
#define WN_ARTICLE_ANIMCMD_INT
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define FT_REL_LINK_EXTERN
#include <wn/wn_module_accesser_builder.h>
#include <wn/fox/wn_fox_illusion.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>

#include <wn/fox/wn_fox_illusion_util.h>

wnFoxIllusion::wnFoxIllusion(s32 articleId, const wnArticleBuildInfo& info, const wnFoxIllusionData* data) :
    wnWeaponBuilder<wnFoxIllusionModuleAccesserBuildConfig>(
        "FoxIllusion", articleId, info,
        wnModuleAccesserBuildData(32, data,
            info.getHeapModule()->getInstanceHeap(),
            info.getHeapModule()->getNWModelInstanceHeap(),
            info.getHeapModule()->getNWMotionInstanceHeap(),
            static_cast<u8>(data->m_resGroupNo), -1, &m_paramAccesser,
            0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param), m_paramAccesser(data->m_param) {
    Weapon::deactivate(false);
}

void wnFoxIllusion::activate(s32 founderTaskId, s32 unk1, s32 team, u8 situationKind, float lr, bool followUp) {
    Vec3f pos(0.0f, 0.0f, 0.0f);
    wnActivateDesc desc(founderTaskId, 0xFFFF, 0xFFFF, 0xFFFF, -1, -1, 0, 0, pos, lr, team, 0, 2, 0x80, 0, 0x35F, 0);
    desc.flagsHigh = 1;
    desc.flagsMidHigh = 0;
    desc.flagsMidLow = 1;
    desc.flagsLow = 0;
    desc.unk4D = (desc.unk4D & ~0xC0) | 0x80;

    Weapon::activate(&desc);
    wnFoxIllusionUtil::activate(m_moduleAccesser, situationKind, followUp);
}

void wnFoxIllusion::processUpdate() {
    wnFoxIllusionUtil::processUpdate(m_moduleAccesser);
    Weapon::processUpdate();
}

bool wnFoxIllusion::notifyEventCollisionAttackCheck(u32) {
    return false;
}
