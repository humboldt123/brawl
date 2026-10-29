#define FT_SNAKE_ARTICLE_POOLS
#define MT_VEC3F_CTOR_NOINLINE
#define MT_VEC3F_ASSIGN_NOINLINE
#include <types.h>
#include <ft/builder/ft_dol_array_list.h>
#include <ft/snake/ft_snake_builder.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/link/so_link_module_impl.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/status/so_status_module_impl.h>
#include <so/visibility/so_visibility_module_simple.h>
#include <so/work/so_work_manage_module_impl.h>
#include <mt/mt_prng.h>

// HYPOTHESIS: the parameter block of the C4 charge, reached through the pointer at +0x23E8.
struct wnSnakeC4Param {
    u8 unk0[0x1C];
    s32 unk1C; // life
    float unk20; // chance divisor
};

void wnSnakeC4::activate(int founderTaskId, int etcResourceId, int team, float lr) {
    Vec3f position(0.0f, 0.0f, 0.0f);
    wnActivateDesc desc;
    desc.founderTaskId = founderTaskId;
    desc.resourceId = etcResourceId;
    desc.unk8 = etcResourceId;
    desc.unkC = etcResourceId;
    desc.unk10 = -1;
    desc.unk14 = -1;
    desc.unk18 = 0;
    desc.unk1C = 0;
#ifdef MATCHING
    // MATCH-ONLY: retain the native aggregate word copy of the position.
    __memcpy(&desc.pos, &position, sizeof(Vec3f));
#else
    desc.pos = position;
#endif
    desc.lr = lr;
    desc.team = team;
    desc.life = 0;
    desc.unk38 = 2;
    desc.unk3C = 0x80;
    desc.unk40 = 0;
    desc.unk44 = 0x35F;
    desc.unk48 = 0;
    desc.flagsHighPair = 3;
    desc.flagsMidPair = 0;
    desc.flagsBit3 = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    Weapon::activate(&desc);
    const wnSnakeC4Param* const& param = *reinterpret_cast<const wnSnakeC4Param* const*>(reinterpret_cast<const u8*>(this) + 0x23E8);
    m_moduleAccesser->getWorkManageModule().setInt(param->unk1C, 0x10000005);
    float roll = randf();
    if (param->unk20 > 0.0f && 1.0f / param->unk20 >= roll) {
        m_moduleAccesser->getVisibilityModule().setStatusDefault(0, 1, true);
        m_moduleAccesser->getVisibilityModule().setDefaultAll();
    }
    m_moduleAccesser->getKineticModule().unableEnergy(1);
    m_moduleAccesser->getKineticModule().unableEnergy(2);
    m_moduleAccesser->getKineticModule().enableEnergy(3);
    m_moduleAccesser->getGroundModule().setCalcLineMove(false, 0);
    m_moduleAccesser->getGroundModule().setDetachChangeStatus(false);
}

// Placeholder translation-unit functions (names come from the module map; bodies match the target assembly).
extern "C" {
void fn_122_1AF04(soModuleAccesser*);
} // extern "C"

void wnSnakeC4::shoot() {
    switch (m_moduleAccesser->getStatusModule().getStatusKind()) {
    case 1:
        if (m_moduleAccesser->getLinkModule().isLink(3) == 1) {
            if (static_cast<u32>(m_moduleAccesser->getLinkModule().getParentTaskId(3)) == m_moduleAccesser->getWorkManageModule().getInt(0x10000000)) {
                m_moduleAccesser->getWorkManageModule().onFlag(0x22000002);
            }
        }
        break;
    case 2:
        if (m_moduleAccesser->getLinkModule().isLink(3) == 1) {
            if (static_cast<u32>(m_moduleAccesser->getLinkModule().getParentTaskId(3)) == m_moduleAccesser->getWorkManageModule().getInt(0x10000000)) {
                m_moduleAccesser->getWorkManageModule().onFlag(0x22000002);
            }
        }
        break;
    case 4:
    case 5:
        break;
    default:
        if (m_moduleAccesser->getLinkModule().isModelConstraint() == 1) {
            fn_122_1AF04(m_moduleAccesser);
        }
        m_moduleAccesser->getStatusModule().changeStatusForce(0, m_moduleAccesser);
        break;
    }
}

bool wnSnakeC4::notifyEventCollisionAttackCheck(u32 flags) {
    if (flags & 6) {
        return false;
    }
    return Weapon::notifyEventCollisionAttackCheck(flags);
}
