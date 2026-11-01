#define MT_VEC3F_IMPLICIT_ASSIGN // BYTES: this unit copies Vec3f as an aggregate (word moves), see mt_vector.h
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
#include <wn/snake/wn_snake_cypher.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>
#include <so/collision/so_collision_hit_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/link/so_link_module_impl.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <wn/wn_kinetic_energy_normal.h>

wnSnakeCypher::wnSnakeCypher(s32 articleId, const wnArticleBuildInfo& info, const wnSnakeCypherData* data) :
    wnWeaponBuilder<wnSnakeCypherModuleAccesserBuildConfig>(
        "wnSnakeCypher", articleId, info,
        wnModuleAccesserBuildData(0x17, data, info.getHeapModule()->getInstanceHeap(), info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &g_soParamAccesserNull,
                                  0, 0, 0, 0, 0, 0, 0), false),
    soDamageEventObserver(ftGetManageId(m_moduleAccesser), -1), m_param(data->m_param) {
    m_moduleAccesser->getCollisionHitModule().add(reinterpret_cast<soSet<soCollisionHitData>*>(const_cast<u8*>(data->unk38)), 0);
    Weapon::deactivate(false);
}

void wnSnakeCypher::activate(int resourceId, int team, int founderTaskId) {
    Vec3f position(0.0f, 0.0f, 0.0f);
    wnActivateDesc desc;
    desc.founderTaskId = founderTaskId;
    desc.resourceId = resourceId;
    desc.unk8 = resourceId;
    desc.unkC = resourceId;
    desc.unk10 = -1;
    desc.unk14 = -1;
    desc.unk18 = 0;
    desc.unk1C = 0;
    desc.pos = position;
    desc.lr = 0.0f;
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
    m_moduleAccesser->getWorkManageModule().setInt(static_cast<int>(m_param->unk1C + m_param->unk20 / 1.5f), 0x10000006);
    m_moduleAccesser->getStatusModule().changeStatusForce(0, m_moduleAccesser);
    m_moduleAccesser->getWorkManageModule().onFlag(0x12000000);
    m_moduleAccesser->getLinkModule().setAttribute(3, static_cast<soLinkConnection::Attribute>(0xC), true);
    m_moduleAccesser->getPostureModule().setSyncConstraintNode(static_cast<const wnSnakeCypherData*>(m_data)->m_constraintNode);
}

void wnSnakeCypher::shoot(const float* speedX) {
    if (m_moduleAccesser->getStatusModule().getStatusKind() == 2 || m_moduleAccesser->getStatusModule().getStatusKind() == 3) {
        return;
    }
    {
        removeModelConstraint();
        m_moduleAccesser->getWorkManageModule().setInt(m_param->unk30, 0x10000005);
        m_moduleAccesser->getStatusModule().changeStatusForce(2, m_moduleAccesser);
        wnKineticEnergyNormal& energy = dynamic_cast<wnKineticEnergyNormal&>(*m_moduleAccesser->getKineticModule().getEnergy(0));
        energy.resetEnergy(0, &Vec2f(*speedX, m_param->unk24), &Vec3f(0.0f, 0.0f, 0.0f), m_moduleAccesser);
        energy.m_accel = Vec2f(0.0f, m_param->unk28);
        energy.m_speedTarget = Vec2f(0.0f, m_param->unk2C);
        m_moduleAccesser->getWorkManageModule().offFlag(0x12000000);
        m_moduleAccesser->getLinkModule().setAttribute(3, static_cast<soLinkConnection::Attribute>(0xC), false);
    }
}

void wnSnakeCypher::notifyEventOnDamage(soDamage* damage, bool, soModuleAccesser*) {
    m_moduleAccesser->getWorkManageModule().subInt(static_cast<int>(damage->m_damageAdd), 0x10000006);
    if (m_moduleAccesser->getWorkManageModule().getInt(0x10000006) <= 0 && m_moduleAccesser->getLinkModule().isModelConstraint() == true) {
        removeModelConstraint();
    }
}

void wnSnakeCypher::removeModelConstraint() {
    if (m_moduleAccesser->getLinkModule().isModelConstraint()) {
        m_moduleAccesser->getPostureModule().setSyncConstraintNode(-1);
        Matrix mtx(true);
        m_moduleAccesser->getModelModule().getNodeGlobalMtx(3, &mtx, false);
        m_moduleAccesser->getLinkModule().removeModelConstraint(true);
        m_moduleAccesser->getLinkModule().setAttribute(3, static_cast<soLinkConnection::Attribute>(0xC), false);
        Vec3f pos;
        float z = mtx.m[2][3];
        float y = mtx.m[1][3];
        float x = mtx.m[0][3];
        pos.m_x = x;
        pos.m_y = y;
        pos.m_z = z;
        pos.m_z = m_moduleAccesser->getGroundModule().getZ();
        m_moduleAccesser->getPostureModule().setPos(&pos);
        Vec3f rot;
        mtx.getRotate(&rot);
        Vec3f degrees(57.29578f * rot.m_x, 57.29578f * rot.m_y, 57.29578f * rot.m_z);
        rot.m_x = degrees.m_x;
        rot.m_y = degrees.m_y;
        rot.m_z = degrees.m_z;
        m_moduleAccesser->getPostureModule().setRot(&rot, 0);
        updateNodeSRT();
    }
}

void wnSnakeCypher::updateNodeSRT() {
    float scaleValue = m_moduleAccesser->getPostureModule().getScale();
    Vec3f scale;
    scale.m_x = scale.m_y = scale.m_z = scaleValue;
    Vec3f pos = m_moduleAccesser->getPostureModule().getPos();
    Vec3f rot = m_moduleAccesser->getPostureModule().getRot(0);
    m_moduleAccesser->getModelModule().setNodeSRT(0, &scale, &rot, &pos);
}

void wnSnakeCypher::adjustParentGroundCollision(int, float* x) {
    *x += m_param->unk34;
}
