// MATCH-ONLY: the original copies returned speed vectors with the aggregate word copy.
#define MT_VEC2F_PLAIN_BASE
#include <wn/falco/wn_falco_blaster_bullet.h>
#include <types.h>
#include <wn/wn_kinetic_energy_normal.h>
#include <so/so_module_accesser.h>
#include <wn/wn_activate_desc.h>
#include <so/motion/so_motion_change_param.h>
#include <so/so_slow.h>
#include <so/so_value_accesser.h>
#include <math.h>
#include <wn/wn_class_info_impl.h>

// Registers the Falco Blaster Bullet article class (kind 36) in the class info table during static initialization.
wnClassInfoImpl<36, wnFalcoBlasterBullet, wnFalcoBlasterBulletData> g_wnFalcoBlasterBulletClassInfo;

void wnFalcoBlasterBullet::activate(int founderTaskId, int, int team, Vec3f* pos,
                                    int unkMode, EfID effectId, float lr,
                                    float speed, float angle) {
    wnActivateDesc desc;
    desc.founderTaskId = founderTaskId;
    desc.resourceId = 0xFFFF;
    desc.unk8 = 0xFFFF;
    desc.unkC = 0xFFFF;
    desc.unk10 = -1;
    desc.unk14 = -1;
    desc.unk18 = 0;
    desc.unk1C = 0;
#ifdef MATCHING
    // MATCH-ONLY: the original copies the position as an aggregate (word copies).
    __memcpy(&desc.pos, pos, sizeof(Vec3f));
#else
    desc.pos = *pos;
#endif
    desc.lr = -lr;
    desc.team = team;
    desc.life = m_param->unk0;
    desc.unk38 = 2;
    desc.unk3C = 0x80;
    desc.unk40 = 0;
    desc.unk44 = 0x35F;
    desc.unk48 = 0;
    desc.flagsHigh = 1;
    desc.flagsMidHigh = 0;
    desc.flagsMidLow = 1;
    desc.flagsLow = 0;
    // HYPOTHESIS: the low six bits retain their prior stack value; their meaning is unknown.
    desc.unk4DHigh = 0;
    Weapon::activate(&desc);

    m_moduleAccesser->getWorkManageModule().setFloat(0.001f, 0x11000001);
    m_moduleAccesser->getStatusModule().changeStatusForce(0, m_moduleAccesser);

    Vec3f zeroPos(0.0f, 0.0f, 0.0f);
    Vec3f zeroRot(0.0f, 0.0f, 0.0f);
    u32 effect = m_moduleAccesser->getEffectModule().reqFollow(effectId, 0, &zeroPos, &zeroRot,
                                                               1.0f, false, 0, 0, -1);
    m_moduleAccesser->getWorkManageModule().setInt(effect, 0x10000005);

    wnKineticEnergyNormal& energy = dynamic_cast<wnKineticEnergyNormal&>(
        *m_moduleAccesser->getKineticModule().getEnergy(0));
    float velocityY = speed * (float)sin(angle);
    float velocityX = lr * (speed * (float)cos(angle));
    Vec2f velocity(velocityX, velocityY);
    energy.m_speed.m_x = velocity.m_x;
    energy.m_speed.m_y = velocity.m_y;
    Vec2f energySpeed = energy.getSpeed();
    m_moduleAccesser->getCollisionAttackModule().setSpeed(&energySpeed);
    m_moduleAccesser->getWorkManageModule().setFloat(0.0f, 0x11000001);

    if (unkMode == 0) {
        if (lr == 1.0f) {
            m_moduleAccesser->getGroundModule().setOffsetX(-8.0f, 0);
            m_moduleAccesser->getGroundModule().setShapeFlag(static_cast<soGroundShapeImpl::ShapeFlagId>(4), true, 0);
        } else {
            m_moduleAccesser->getGroundModule().setOffsetX(8.0f, 0);
            m_moduleAccesser->getGroundModule().setShapeFlag(static_cast<soGroundShapeImpl::ShapeFlagId>(3), true, 0);
        }
        soMotionChangeParam motionParam;
        motionParam.m_kind = 0;
        motionParam.m_frame = 0.0f;
        motionParam.m_rate = 1.0f;
        motionParam._12 = 0;
        motionParam._13 = 0;
        motionParam._14 = 0;
        motionParam._15 = 0;
        m_moduleAccesser->getMotionModule().changeMotionRequest(&motionParam);
    } else {
        m_moduleAccesser->getGroundModule().setShapeFlag(static_cast<soGroundShapeImpl::ShapeFlagId>(4), true, 0);
        m_moduleAccesser->getGroundModule().setShapeFlag(static_cast<soGroundShapeImpl::ShapeFlagId>(3), true, 0);
        soMotionChangeParam motionParam;
        motionParam.m_kind = 1;
        motionParam.m_frame = 0.0f;
        motionParam.m_rate = 1.0f;
        motionParam._12 = 0;
        motionParam._13 = 0;
        motionParam._14 = 0;
        motionParam._15 = 0;
        m_moduleAccesser->getMotionModule().changeMotionRequest(&motionParam);
    }

    Vec2f currentSpeed = m_moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1));
    float speedMagnitude = currentSpeed.length();
    float maximum = soValueAccesser::getConstantFloat(m_moduleAccesser, 0xFA0, 0);
    float current = m_moduleAccesser->getWorkManageModule().getFloat(0x11000001);
    float next = current + speedMagnitude / soValueAccesser::getConstantFloat(m_moduleAccesser, 0xFA1, 0);
    if (next > maximum) next = maximum;
    if (next < 0.00001f) next = 0.001f;
    m_moduleAccesser->getWorkManageModule().setFloat(next, 0x11000001);
    updateNodeSRT();
}

bool wnFalcoBlasterBullet::reflect() {
    m_moduleAccesser->getWorkManageModule().setFloat(0.001f, 0x11000001);
    return Weapon::reflect();
}

float wnFalcoBlasterBullet::getCollisionLr(soModuleAccesser* moduleAccesser) {
    return -moduleAccesser->getPostureModule().getLr();
}

void wnFalcoBlasterBullet::processUpdate() {
    Weapon::processUpdate();

    soSlow* slow = soSlow::getInstance();
    if (slow->isEstimate() != 1) {
        return;
    }
    if (m_moduleAccesser->getStopModule().isStop()) {
        return;
    }

    Vec2f speed = m_moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1));
    float speedMagnitude = speed.length();
    float maximum = soValueAccesser::getConstantFloat(m_moduleAccesser, 0xFA0, 0);
    float current = m_moduleAccesser->getWorkManageModule().getFloat(0x11000001);
    float next = current + speedMagnitude / soValueAccesser::getConstantFloat(m_moduleAccesser, 0xFA1, 0);
    if (next > maximum) {
        next = maximum;
    }
    if (next < 0.00001f) {
        next = 0.001f;
    }
    m_moduleAccesser->getWorkManageModule().setFloat(next, 0x11000001);
}

void wnFalcoBlasterBullet::updateNodeSRT() {
    Vec2f speed;
    Vec2f::copy(speed, m_moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)));
    float negativeLr = -m_moduleAccesser->getPostureModule().getLr();
    float angle = (float)atan2(speed.m_y, speed.m_x * negativeLr);
    Vec3f position = m_moduleAccesser->getPostureModule().getPos();
    Vec3f rotation = m_moduleAccesser->getPostureModule().getRot(0);
    rotation.m_x = 57.29578f * angle;
    rotation.m_y += m_moduleAccesser->getPostureModule().getRotYLr();
    float scaleValue = m_moduleAccesser->getPostureModule().getScale();
    Vec3f scale;
    scale.m_x = scale.m_y = scale.m_z = scaleValue;
    scale.m_z *= m_moduleAccesser->getWorkManageModule().getFloat(0x11000001);
    m_moduleAccesser->getModelModule().setNodeSRT(0, &scale, &rotation, &position);
}
