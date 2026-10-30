#include <wn/snake/wn_snake_trench_mortar_bullet.h>
#include <so/collision/so_collision_log.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/model/so_model_module_impl.h>
#include <so/posture/so_posture_module_impl.h>
#include <so/reflect/so_reflect_module.h>
#include <so/team/so_team_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>

void wnSnakeTrenchMortarBullet::updateNodeSRT() {
    float scale = m_moduleAccesser->getPostureModule().getScale();
    Vec3f scaleVec(scale, scale, scale);
    Vec3f pos = m_moduleAccesser->getPostureModule().getPos();
    Vec3f rot1 = m_moduleAccesser->getPostureModule().getRot(1);
    Vec3f rot0 = m_moduleAccesser->getPostureModule().getRot(0);
    Vec3f rot2 = m_moduleAccesser->getPostureModule().getRot(2);
    m_moduleAccesser->getModelModule().setNodeRotate(2, &rot0);
    m_moduleAccesser->getModelModule().setNodeRotate(1, &rot1);
    m_moduleAccesser->getModelModule().setNodeRotate(0, &rot2);
    m_moduleAccesser->getModelModule().setNodeScale(0, &scaleVec);
    m_moduleAccesser->getModelModule().setNodeTranslate(0, &pos);
}

void wnSnakeTrenchMortarBullet::processHit() {
    StageObject::processHit();
}

void wnSnakeTrenchMortarBullet::processCollision() {
    if (m_moduleAccesser->getStatusModule().getStatusKind() == 0) {
        m_moduleAccesser->getWorkManageModule().offFlag(0x22000004);
    }
    StageObject::processCollision();
}

void wnSnakeTrenchMortarBullet::notifyEventCollisionSearch(soCollisionLog* collisionLog, soModuleAccesser*) {
    if (collisionLog == nullptr) {
        return;
    }
    if (m_moduleAccesser->getStatusModule().getStatusKind() == 0) {
        if (static_cast<u8>(collisionLog->_33) == 3) {
            if (collisionLog->m_teamNo != m_moduleAccesser->getTeamModule().getTeam()->getNo() &&
                m_moduleAccesser->getReflectModule().isReflect() == 1) {
                m_moduleAccesser->getWorkManageModule().onFlag(0x22000005);
            }
        } else {
            m_moduleAccesser->getWorkManageModule().onFlag(0x22000004);
        }
    }
}

bool wnSnakeTrenchMortarBullet::notifyEventCollisionSearchCheck() {
    if (m_moduleAccesser->getStatusModule().getStatusKind() == 0) {
        if (m_moduleAccesser->getWorkManageModule().isFlag(0x22000005)) {
            m_moduleAccesser->getWorkManageModule().offFlag(0x22000005);
            m_moduleAccesser->getPostureModule().reverseLr();
            m_moduleAccesser->getPostureModule().updateRotYLr();
            int team = m_moduleAccesser->getReflectModule().getTeam();
            m_moduleAccesser->getTeamModule().setTeam(team, true);
            m_moduleAccesser->getTeamModule().setHitTeam(team);
            m_moduleAccesser->getTeamModule().setTeamOwnerId(m_moduleAccesser->getReflectModule().getTaskId());
            m_moduleAccesser->getKineticModule().mulSpeed(&Vec3f(-m_moduleAccesser->getReflectModule().getSpeedMul(), 1.0f, 1.0f),
                                                          soKineticEnergy::AttributeFlag(1));
        }
        if (m_moduleAccesser->getWorkManageModule().isFlag(0x22000004)) {
            m_moduleAccesser->getStatusModule().changeStatusRequest(1, m_moduleAccesser);
            return true;
        }
    }
    return false;
}

bool wnSnakeTrenchMortarBullet::notifyEventCollisionAttackCheck(u32 flags) {
    if (flags & 6) {
        return false;
    }
    return Weapon::notifyEventCollisionAttackCheck(flags);
}
