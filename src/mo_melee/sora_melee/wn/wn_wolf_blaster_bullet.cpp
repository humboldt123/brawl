#include <wn/wolf/wn_wolf_blaster_bullet.h>
#include <so/so_module_accesser.h>
#include <wn/wn_activate_desc.h>
#include <so/motion/so_motion_change_param.h>


#include <so/so_slow.h>
#include <so/so_value_accesser.h>
#include <math.h>

void wnWolfBlasterBullet::activate(int founderTaskId, int, int team, Vec3f* pos,
                                   int unkMode, EfID effectId, float lr,
                                   float speed, float angle) {
    wnActivateDesc desc;
    desc.founderTaskId = founderTaskId;
    desc.resourceId = -1;
    desc.unk8 = -1;
    desc.unkC = -1;
    desc.unk10 = -1;
    desc.unk14 = -1;
    desc.unk18 = 0;
    desc.unk1C = 0;
    desc.pos = *pos;
    desc.lr = -lr;
    desc.team = team;
    desc.life = *unk213C;
    desc.unk38 = 2;
    desc.unk3C = 0x80;
    desc.unk40 = 0;
    desc.unk44 = 0x35F;
    desc.unk48 = 0;
    // HYPOTHESIS: native flag byte is read from uninitialized stack before
    // setting bits 0x80 and 0x08; preserve the proven mask while leaving those
    // unrelated bits an explicit source-level uncertainty.
    desc.flags = 0x88;
    // HYPOTHESIS: native retains the low six bits of this uninitialized byte.
    desc.unk4D &= 0x3F;
    desc.unk4E[0] = 0;
    desc.unk4E[1] = 0;
    Weapon::activate(&desc);

    m_moduleAccesser->getWorkManageModule().setFloat(0.001f, 0x11000001);
    m_moduleAccesser->getStatusModule().getPrevStatusKind(0);

    Vec3f zeroPos(0.0f, 0.0f, 0.0f);
    Vec3f zeroRot(0.0f, 0.0f, 0.0f);
    m_moduleAccesser->getEffectModule().reqFollow(effectId, 0, &zeroPos, &zeroRot,
                                                  1.0f, false, 0, 0, -1);

    wnKineticEnergyNormal& energy = dynamic_cast<wnKineticEnergyNormal&>(
        *m_moduleAccesser->getKineticModule().getEnergy(0));
    float velocityY = speed * (float)sin(angle);
    float velocityX = speed * (float)cos(angle);
    velocityX *= lr;
    energy.m_speed.m_x = velocityX;
    energy.m_speed.m_y = velocityY;
    Vec2f velocity(velocityX, velocityY);
    m_moduleAccesser->getCollisionAttackModule().setSpeed(&velocity);
    m_moduleAccesser->getWorkManageModule().setFloat(0.001f, 0x11000001);

    soGroundModule& ground = m_moduleAccesser->getGroundModule();
    if (unkMode == 0) {
        if (lr == 1.0f) {
            ground.setOffsetX(-8.0f, 0);
            ground.setShapeFlag(static_cast<soGroundShapeImpl::ShapeFlagId>(4), true, 0);
        } else {
            ground.setOffsetX(8.0f, 0);
            ground.setShapeFlag(static_cast<soGroundShapeImpl::ShapeFlagId>(3), true, 0);
        }
    } else {
        ground.setShapeFlag(static_cast<soGroundShapeImpl::ShapeFlagId>(4), true, 0);
        ground.setShapeFlag(static_cast<soGroundShapeImpl::ShapeFlagId>(3), true, 0);
    }
    soMotionChangeParam motionParam(unkMode == 0 ? 0 : 1, 0.0f, 1.0f, 0, 0, 0, 0);
    m_moduleAccesser->getMotionModule().changeMotionRequest(&motionParam);

    Vec2f currentSpeed = m_moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1));
    float speedSquared = currentSpeed.m_x * currentSpeed.m_x + currentSpeed.m_y * currentSpeed.m_y;
    float speedMagnitude = speedSquared <= 1.1754944e-38f ? 0.0f : speedSquared * (float)rsqrtf(speedSquared);
    float maximum = soValueAccesser::getConstantFloat(m_moduleAccesser, 0xFA0, 0);
    float current = m_moduleAccesser->getWorkManageModule().getFloat(0x11000001);
    float next = current + speedMagnitude / soValueAccesser::getConstantFloat(m_moduleAccesser, 0xFA1, 0);
    if (next > maximum) next = maximum;
    if (next < 0.00001f) next = 0.001f;
    m_moduleAccesser->getWorkManageModule().setFloat(next, 0x11000001);
    updateNodeSRT();
}

bool wnWolfBlasterBullet::reflect() {
    m_moduleAccesser->getWorkManageModule().setFloat(0.001f, 0x11000001);
    return Weapon::reflect();
}

float wnWolfBlasterBullet::getCollisionLr(soModuleAccesser* moduleAccesser) {
    return -moduleAccesser->getPostureModule().getLr();
}

void wnWolfBlasterBullet::processUpdate() {
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

void wnWolfBlasterBullet::updateNodeSRT() {
    soPostureModule& posture = m_moduleAccesser->getPostureModule();
    Vec2f speed = m_moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1));
    float lr = posture.getLr();
    float angle = (float)atan2(speed.m_y, -speed.m_x * lr);

    Vec3f position = posture.getPos();
    Vec3f rotation = posture.getRot(0);
    rotation.m_x = 57.29578f * angle + posture.getRotYLr();

    float scaleValue = posture.getScale();
    Vec3f scale(scaleValue, scaleValue, scaleValue);
    scale.m_z *= m_moduleAccesser->getWorkManageModule().getFloat(0x11000001);
    m_moduleAccesser->getModelModule().setNodeSRT(0, &scale, &rotation, &position);
}
