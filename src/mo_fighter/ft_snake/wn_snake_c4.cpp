#define MT_VEC3F_IMPLICIT_ASSIGN // BYTES: this unit copies Vec3f as an aggregate (word moves), see mt_vector.h
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define SO_EVENT_OBSERVER_CTOR_EXTERN
// MATCH-ONLY: retain the native one-word null-input result (as in the Mario cape unit).
#define IP_INPUT_BUTTON_RAW
#define SO_COLLISION_SEARCH_OBSERVER_INLINE
#define MT_VEC3F_CTOR_NOINLINE
#define MT_VEC3F_ASSIGN_NOINLINE
#include <types.h>
#include <wn/wn_module_accesser_builder.h>
#include <wn/snake/wn_snake_c4.h>
#include <wn/wn_activate_desc.h>
#include <wn/wn_kinetic_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/link/so_link_module_impl.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/status/so_status_module_impl.h>
#include <so/visibility/so_visibility_module_simple.h>
#include <so/work/so_work_manage_module_impl.h>
#include <mt/mt_prng.h>

// The status unique processes of the charge (wn_snake_c4_status_uniq_process_establish_*.cpp): sticking to the ground and
// to a wall.
extern soStatusUniqProcess g_wnSnakeC4StatusUniqProcessEstablishGround;
extern soStatusUniqProcess g_wnSnakeC4StatusUniqProcessEstablishWall;

extern "C" {
// wnSnakeC4Utility::detachConstraint (module map); outside any configured unit.
void fn_122_1AF04(soModuleAccesser*);
} // extern "C"

wnSnakeC4::wnSnakeC4(s32 articleId, const wnArticleBuildInfo& info, const wnSnakeC4Data* data) :
    wnWeaponBuilder<wnSnakeC4ModuleAccesserBuildConfig>(
        "SnakeC4", articleId, info,
        wnModuleAccesserBuildData(0x18, data, info.getHeapModule()->getInstanceHeap(), info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &g_soParamAccesserNull,
                                  0, 0, 0, 0, 0, 0, 0)),
    soCollisionSearchEventObserver(ftGetManageId(m_moduleAccesser), -1), m_param(data->m_param),
    m_energyGravity(0.0f, -data->m_param->m_gravity, data->m_param->m_speedLimit), m_energyGroundMovement() {
    // Statuses 1 and 2 (stuck to the ground, stuck to a wall) have their own processes; the others use none.
    soStatusUniqProcess* uniqProcs[7] = {NULL};
    uniqProcs[1] = &g_wnSnakeC4StatusUniqProcessEstablishGround;
    uniqProcs[2] = &g_wnSnakeC4StatusUniqProcessEstablishWall;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(uniqProcs, 7);
    m_moduleAccesser->getKineticModule().addEnergy(&m_energyGravity, 2, soKineticEnergy::AttributeFlag(1), -1);
    m_moduleAccesser->getKineticModule().addEnergy(&m_energyGroundMovement, 3, soKineticEnergy::AttributeFlag(8), -1);
    wnKineticModuleImpl::setOutsideMovementGroundEnergyId(&m_moduleAccesser->getKineticModule(), 3);
    Weapon::deactivate(false);
}

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
    desc.pos = position;
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
    m_moduleAccesser->getWorkManageModule().setInt(m_param->m_life, 0x10000005);
    float roll = randf();
    if (m_param->m_blinkChance > 0.0f && 1.0f / m_param->m_blinkChance >= roll) {
        m_moduleAccesser->getVisibilityModule().setStatusDefault(0, 1, true);
        m_moduleAccesser->getVisibilityModule().setDefaultAll();
    }
    m_moduleAccesser->getKineticModule().unableEnergy(1);
    m_moduleAccesser->getKineticModule().unableEnergy(2);
    m_moduleAccesser->getKineticModule().enableEnergy(3);
    m_moduleAccesser->getGroundModule().setCalcLineMove(false, 0);
    m_moduleAccesser->getGroundModule().setDetachChangeStatus(false);
}

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

void wnSnakeC4::processUpdate() { }
void wnSnakeC4::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) { }
void wnSnakeC4::updateNodeSRT() { }
void wnSnakeC4::notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObj, int unk4) { }
void wnSnakeC4::notifyEventCollisionSearch(soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) { }
bool wnSnakeC4::notifyEventCollisionSearchCheck() { return false; }
