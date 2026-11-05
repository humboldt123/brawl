#define SO_ARRAY_LIST_CTOR_FLAG // MATCH-ONLY: the launcher calls the sora_melee vector constructor with a zero argument
#define FT_POOL_ROOT_NONPOLYMORPHIC // the missile pool's root has no vtable (the first pool level owns it)
#define SO_EVENT_OBSERVER_CTOR_EXTERN // BYTES: the search observer base calls the sora_melee constructor and initialize
#define SO_COLLISION_SEARCH_OBSERVER_INLINE_CTOR // BYTES: the search observer's (manage id, priority) constructor is a weak copy in this unit
#define WN_WEAPON_BUILDER
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define WN_KINETIC_GENERIC_BUILDER
#define WN_KINETIC_NORMAL_EXTERN
#define FT_REL_LINK_EXTERN
#define MT_VEC2F_CTOR_NOINLINE
#define MT_VEC3F_CTOR_NOINLINE
#define MT_VEC3F_ASSIGN_NOINLINE
#include <wn/wn_module_accesser_builder.h>
#include <wn/snake/wn_snake_nikita.h>
#include <wn/snake/wn_snake_nikita_missile_status_uniq_process_fly.h>

#include <so/kinetic/so_kinetic_module_impl.h>
#include <wn/snake/so_kinetic_energy_local_normal.h>

#include <gf/gf_task_scheduler.h>
#include <so/article/so_article.h>
#include <so/article/so_generate_article_manage_module.h>
#include <so/link/so_link_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <so/so_external_value_accesser.h>


namespace {
struct SnakeNikitaEndEvent : soLinkEventArgs {
    explicit SnakeNikitaEndEvent(int eventKind) : soLinkEventArgs(eventKind) { }
};
}

void wnSnakeNikitaMissile::setAutonomy() {
    m_moduleAccesser->getWorkManageModule().onFlag(0x12000003);

    if (m_moduleAccesser->getLinkModule().isLink(3) == true) {
        // HYPOTHESIS: the enum names attribute 4 as unknown; its meaning is unverified.
        m_moduleAccesser->getLinkModule().setAttribute(
            3, soLinkConnection::Attribute_Reference_Parent_Unknown_4, false);
    }
}

inline soKineticEnergyLocalNormal::soKineticEnergyLocalNormal(float angle, const Vec2f& speed, const Vec2f& accel,
                                                              const Vec2f& target, const Vec2f& brake,
                                                              const Vec2f& limit, bool unk30) :
    soKineticEnergyNormal(speed, accel, target, brake, limit, unk30, false, 0), unk34(angle) { }

// HYPOTHESIS: stands in for soParamAccesserNull::getNullInstance() (fn_122_23F48, map name), which the shared
// soParamAccesserNull header does not declare.
inline soParamAccesser* getNullParamAccesser() {
    return &g_soParamAccesserNull;
}
// HYPOTHESIS: stand in for soKineticEnergyRotNormal::setAccel/setBrake (fn_122_245FC/24604, map names), which the
// shared soKineticEnergyRotNormal header does not declare.
inline void setRotAccel(soKineticEnergyRotNormal* energy, const Vec3f& accel) {
    energy->m_accel = accel;
}
inline void setRotBrake(soKineticEnergyRotNormal* energy, const Vec3f& brake) {
    energy->m_brake = brake;
}

// The constructor's weak copy (fn_122_245D0, map wnSnakeNikitaMissileStatusUniqProcessFly::getInstance).
inline wnSnakeNikitaMissileStatusUniqProcessFly* wnSnakeNikitaMissileStatusUniqProcessFly::getInstance() {
    return &g_wnSnakeNikitaMissileStatusUniqProcessFly;
}

#pragma dont_inline on // BYTES: the native missile constructor calls every helper (accessors included) out of line
wnSnakeNikitaMissile::wnSnakeNikitaMissile(s32 articleId, const wnArticleBuildInfo& info,
                                           const wnSnakeNikitaMissileData* data) :
    wnWeaponBuilder<wnSnakeNikitaMissileModuleAccesserBuildConfig>(
        "SnakeNikitaMissile", articleId, info,
        wnModuleAccesserBuildData(0x1D, data, info.getHeapModule()->getInstanceHeap(), info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1,
                                  getNullParamAccesser(), 0, 0, 0, 0, 0, 0, 0)),
    soDamageEventObserver(static_cast<soEventManager&>(getModuleAccesser()->getEventManageModule()).getManageId(), -1),
    soCollisionSearchEventObserver(static_cast<soEventManager&>(getModuleAccesser()->getEventManageModule()).getManageId(), -1),
    m_param(data->m_param),
    m_energyLocal(0.0f, Vec2f(0.0f, 0.0f), Vec2f(0.0f, 0.0f), Vec2f(-1.0f, -1.0f), Vec2f(0.0f, 0.0f), Vec2f(-1.0f, -1.0f), false),
    m_energyGravity(0.0f, -m_param->m_gravity, m_param->m_speedLimit) {
    getModuleAccesser()->getCollisionHitModule().add(reinterpret_cast<soSet<soCollisionHitData>*>(const_cast<u8*>(data->unk38)), 0);

    soStatusUniqProcess* uniqProcs[3] = { 0 };
    uniqProcs[0] = wnSnakeNikitaMissileStatusUniqProcessFly::getInstance();
    getModuleAccesser()->getStatusModule().addRangeUniqProc(uniqProcs, 3);

    getModuleAccesser()->getKineticModule().addEnergy(&m_energyLocal, wnSnakeNikitaMissile_Energy_LocalNormal,
                                                      soKineticEnergy::AttributeFlag(1), -1);
    getModuleAccesser()->getKineticModule().addEnergy(&m_energyGravity, wnSnakeNikitaMissile_Energy_Gravity,
                                                      soKineticEnergy::AttributeFlag(1), -1);
    soKineticEnergyRotNormal* rot = dynamic_cast<soKineticEnergyRotNormal*>(
        getModuleAccesser()->getKineticModule().getEnergy(wnSnakeNikitaMissile_Energy_Rot));
    if (rot != NULL) {
        setRotAccel(rot, Vec3f(0.0f, 0.0f, m_param->m_rotAccel));
        setRotBrake(rot, Vec3f(0.0f, 0.0f, m_param->m_rotBrake));
    }
    Weapon::deactivate(false);
}
#pragma dont_inline reset

wnWeaponBuilder<wnSnakeNikitaMissileModuleAccesserBuildConfig>::wnWeaponBuilder(
    const char* name, s32 articleId, const wnArticleBuildInfo& info, const wnModuleAccesserBuildData& bd) :
    Weapon(name, bd.m_kind, bd.m_data, articleId, &info, m_modules.getModuleAccesser(), true), m_modules(this, bd),
    m_energyNormal(), m_energyRotNormal() {
    Weapon::initialize();
    m_moduleAccesser->getKineticModule().addEnergy(&m_energyNormal, 0, soKineticEnergy::AttributeFlag(1), -1);
    m_moduleAccesser->getKineticModule().addEnergy(&m_energyRotNormal, 1, soKineticEnergy::AttributeFlag(1), -1);
}

void wnSnakeNikitaMissile::updateNodeSRT() {
    float scale = m_moduleAccesser->getPostureModule().getScale();
    Vec3f scaling;
    scaling.m_z = scale;
    scaling.m_y = scale;
    scaling.m_x = scale;
    Vec3f position = m_moduleAccesser->getPostureModule().getPos();
    Vec3f rotation = m_moduleAccesser->getPostureModule().getRot(1);
    m_moduleAccesser->getModelModule().setNodeSRT(0, &scaling, &rotation, &position);
    // Rotation 0 is the steering roll; keep it below a full turn.
    Vec3f roll = m_moduleAccesser->getPostureModule().getRot(0);
    if (roll.m_z >= 360.0f) {
        roll.m_z -= 360.0f;
        m_moduleAccesser->getPostureModule().setRot(&roll, 0);
    }
    m_moduleAccesser->getModelModule().setNodeRotate(1, &roll);
}

void wnSnakeNikitaMissile::notifyEventOnDamage(soDamage* damage, bool, soModuleAccesser* moduleAccesser) {
}

void wnSnakeNikitaMissile::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData,
                                                   soModuleAccesser* moduleAccesser) {
    Weapon::notifyEventChangeStatus(statusKind, prevStatusKind, statusData, moduleAccesser);
    if (statusKind == 2) {
        if (prevStatusKind == 0) {
            if (m_param->m_life - m_moduleAccesser->getWorkManageModule().getInt(0x20000000) > m_param->m_armFrame) {
                m_moduleAccesser->getWorkManageModule().onFlag(0x22000000);
                return;
            }
        }
        m_moduleAccesser->getWorkManageModule().offFlag(0x22000000);
    }
}

void wnSnakeNikitaMissile::processCollision() {
    if (m_moduleAccesser->getStatusModule().getStatusKind() == 0) {
        m_moduleAccesser->getWorkManageModule().offFlag(0x22000001);
    } else if (m_moduleAccesser->getStatusModule().getStatusKind() == 1) {
        m_moduleAccesser->getWorkManageModule().offFlag(0x22000000);
    }
    StageObject::processCollision();
}

void wnSnakeNikitaMissile::notifyEventCollisionSearch(soCollisionLog* collisionLog, soModuleAccesser*) {
    if (collisionLog != NULL) {
        if (m_moduleAccesser->getStatusModule().getStatusKind() == 0) {
            if (collisionLog->m_kind == 3) {
                if (collisionLog->m_teamNo != m_moduleAccesser->getTeamModule().getTeam()->getNo() &&
                    m_moduleAccesser->getReflectModule().isReflect() == 1) {
                    m_moduleAccesser->getWorkManageModule().onFlag(0x22000002);
                }
            } else {
                m_moduleAccesser->getWorkManageModule().onFlag(0x22000001);
            }
        } else if (m_moduleAccesser->getStatusModule().getStatusKind() == 1) {
            if (collisionLog->m_kind == 3) {
                if (collisionLog->m_teamNo != m_moduleAccesser->getTeamModule().getTeam()->getNo()) {
                    m_moduleAccesser->getWorkManageModule().onFlag(0x22000001);
                }
            } else {
                m_moduleAccesser->getWorkManageModule().onFlag(0x22000000);
            }
        }
    }
}

bool wnSnakeNikitaMissile::notifyEventCollisionSearchCheck() {
    soWorkManageModule& work = m_moduleAccesser->getWorkManageModule();
    const int statusKind = m_moduleAccesser->getStatusModule().getStatusKind();
    bool shouldReflect = false;
    if (statusKind == 0) {
        shouldReflect = work.isFlag(0x22000002);
        if (shouldReflect) {
            work.offFlag(0x22000002);
        }
    } else if (statusKind == 1) {
        shouldReflect = work.isFlag(0x22000001);
        if (shouldReflect) {
            work.offFlag(0x22000001);
        }
    }

    if (shouldReflect) {
        setAutonomy();
        SnakeNikitaEndEvent event(0x839);
        m_moduleAccesser->getLinkModule().sendEventParents(3, event);

        const int team = m_moduleAccesser->getReflectModule().getTeam();
        m_moduleAccesser->getTeamModule().setTeam(team, true);
        m_moduleAccesser->getTeamModule().setHitTeam(team);

        soKineticEnergyLocalNormal* energy = dynamic_cast<soKineticEnergyLocalNormal*>(
            m_moduleAccesser->getKineticModule().getEnergy(2));
        if (energy != NULL) {
            energy->unk34 -= 0.3846154f;
            if (energy->unk34 < 0.0f) {
                energy->unk34 += 360.0f;
            }
            Vec3f rotation(-energy->unk34, 90.0f, 0.0f);
            m_moduleAccesser->getPostureModule().setRot(&rotation, 1);
        }
        return false;
    }

    // HYPOTHESIS: these two work flags represent the native status-specific
    // collision transition gates; the flags themselves remain unnamed.
    if (statusKind == 0 && work.isFlag(0x22000001)) {
        m_moduleAccesser->getStatusModule().changeStatusRequest(2, m_moduleAccesser);
        return true;
    }
    if (statusKind == 1 && work.isFlag(0x22000000)) {
        m_moduleAccesser->getStatusModule().changeStatusRequest(2, m_moduleAccesser);
        return true;
    }
    return false;
}

bool wnSnakeNikitaMissile::notifyEventCollisionAttackCheck(u32 flags) {
    if (flags & 0x6) {
        return false;
    }
    Weapon::notifyEventCollisionAttackCheck(flags);
    return false;
}

void wnSnakeNikitaMissile::onDeactivate() {
    if (!m_moduleAccesser->getWorkManageModule().isFlag(0x12000003)) {
        u32 category = 0;
        category |= 3;
        if (m_moduleAccesser->getStatusModule().isCollisionAttackOccer() == 1 &&
            m_moduleAccesser->getStatusModule().isCollisionAttackTarget(&category)) {
            SnakeNikitaEndEvent event(0x838);
            m_moduleAccesser->getLinkModule().sendEventParents(3, event);
        } else {
            SnakeNikitaEndEvent event(0x839);
            m_moduleAccesser->getLinkModule().sendEventParents(3, event);
        }
    }
}

// Leaf functions shared with other fighter modules (same module-map names and bodies).
extern "C" {
int fn_122_24410(u8* p) { return *(int*)(p + 0x60); }
void* fn_122_245DC(u8* p) { return *(void**)(*(u8**)(p + 0xd8) + 0x70); }
} // extern "C"
