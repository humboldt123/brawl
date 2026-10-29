#define WN_WEAPON_NO_COMMON_ENERGIES
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define WN_KINETIC_GENERIC_BUILDER
#define WN_KINETIC_NORMAL_HOLDER_DEFAULT
#define WN_KINETIC_NORMAL_EXTERN
#define MT_VEC2F_ASSIGN_NOINLINE
#define MT_VEC3F_CTOR_NOINLINE
#include <wn/wn_module_accesser_builder.h>
#include <wn/popo/wn_popo_ice_berg.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>
#include <so/collision/so_collision_hit_module_impl.h>
#include <so/status/so_status_module_impl.h>
#include <wn/wn_kinetic_energy_normal.h>

// MATCH-ONLY: the unit's rodata starts with this word (HYPOTHESIS: the remains of a constant of an unreconstructed function).
#pragma force_active on
extern const int sIceBergRodataLead = 3;
#pragma force_active reset

wnPopoIceBerg::wnPopoIceBerg(s32 articleId, const wnArticleBuildInfo& info, const wnPopoIceBergData* data) :
    wnWeaponBuilder<wnPopoIceBergModuleAccesserBuildConfig>(
        "PopoIceBerg", articleId, info,
        wnModuleAccesserBuildData(0x82, data, info.getHeapModule()->getInstanceHeap(), Heaps::FighterTechqniq,
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &m_paramAccesser,
                                  0, 0, data->unk40, 0, 0, 0, 0)),
    soDamageEventObserver(ftGetManageId(m_moduleAccesser), -1), m_param(data->m_param), m_paramAccesser(data->m_param) {
    m_moduleAccesser->getCollisionHitModule().add(reinterpret_cast<soSet<soCollisionHitData>*>(const_cast<u8*>(data->unk38)), 0);
    Weapon::deactivate(false);
}

void wnPopoIceBerg::onDeactivate() {
    wnPopoIceBergLinkEvent event(0x45c);
    m_moduleAccesser->getLinkModule().sendEventParents(3, event);
}

bool wnPopoIceBerg::notifyEventCollisionAttackCheck(u32 flags) {
    if (flags & 6) {
        return false;
    }
    return Weapon::notifyEventCollisionAttackCheck(flags);
}

void wnPopoIceBerg::syncCapsuleSRT(u32 index) {
    if (m_moduleAccesser->getCollisionAttackModule().isAttack(index, false) &&
        m_moduleAccesser->getCollisionAttackModule().getData(index, false) != 0) {
        m_moduleAccesser->getWorkManageModule().getFloat(0x11000001);
        m_moduleAccesser->getWorkManageModule().getFloat(0x11000002);
    }
}
