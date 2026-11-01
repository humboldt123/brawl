#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_kinetic_energy.h>
#include <ft/ft_util.h>
#include <so/stageobject.h>
#include <math.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <ft/fox/ft_fox_status_uniq_process.h>
#include <types.h>

void ftKineticEnergyDisableAndClear(int index, soModuleAccesser* moduleAccesser);
float vec2fAngle(Vec2f*, Vec2f*);

// Fox's up special (Fire Fox). Status 0x114 is the startup, 0x117 the flight, 0x118 the fall that follows and 0x119 the
// landing slide on the ground. The flight angle (radians, 0 = to the right) is kept in the work float 0x21000004 and is
// also shown as the rotation of the model around X.
// HYPOTHESIS: names of the statuses and of the work values.
ftFoxStatusUniqProcessFire g_ftFoxStatusUniqProcessFire;

static const float FIRE_PI_HALF = 1.57079637f;
static const float FIRE_DEGREES_FULL = 360.0f;
static const float FIRE_RAD_TO_DEG = 57.2957802f;

void ftFoxStatusUniqProcessFire::initStatus(soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x114: {
        // Startup: the carried speed is slowed down and held by the stop energy, the fall is halted.
        soKineticEnergyNormal& stop = dynamic_cast<soKineticEnergyNormal&>(*moduleAccesser->getKineticModule().getEnergy(3));
        Vec2f sumSpeed = moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1));
        sumSpeed.m_x /= soValueAccesser::getConstantFloat(moduleAccesser, 0xfaf, 0);
        stop.resetEnergy(6, &Vec2f(sumSpeed.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
        stop.enable();
        ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
        gravity.m_speedY = 0.0f;
        gravity.enable();
        ftKineticEnergyDisableAndClear(2, moduleAccesser);
        ftKineticEnergyDisableAndClear(0, moduleAccesser);
        break;
    }
    case 0x117: {
        // Flight: the direction is taken from the stick (or from the ground when the stick presses into it).
        float stickX = moduleAccesser->getControllerModule().getStickX();
        float stickY = moduleAccesser->getControllerModule().getStickY();
        float angle = FIRE_PI_HALF;
        float speed = soValueAccesser::getConstantFloat(moduleAccesser, 0xfb4, 0);
        float stickMin = soValueAccesser::getConstantFloat(moduleAccesser, 0xfb3, 0);
        float absStickX = fabs(stickX);
        float turnThreshold = soValueAccesser::getConstantFloat(moduleAccesser, 0xfb2, 0);
        if (absStickX > turnThreshold) {
            moduleAccesser->getPostureModule().setStickLR(0.0f);
            moduleAccesser->getPostureModule().updateRotYLr();
        }
        float lr = moduleAccesser->getPostureModule().getLr();
        Vec2f velocity;
        if (moduleAccesser->getSituationModule().getKind() == 0) {
            Vec2f groundNormal = moduleAccesser->getGroundModule().getTouchNormal(8, 0);
            bool leaveGround = true;
            if (!((float)fabs(stickX) + (float)fabs(stickY) < stickMin)) {
                Vec2f stick(stickX, stickY);
                if (vec2fAngle(&groundNormal, &stick) >= FIRE_PI_HALF) {
                    leaveGround = false;
                }
            }
            if (!leaveGround) {
                // The stick presses into the ground: slide along it.
                velocity.m_x = speed * lr;
                velocity.m_y = 0.0f;
                angle = atan2(-groundNormal.m_x * lr, groundNormal.m_y);
            } else {
                moduleAccesser->getSituationModule().setKind(Situation_Air, false);
                moduleAccesser->getGroundModule().setCorrect(soGroundShapeImpl::Correct_Air, 0);
            }
        }
        if (moduleAccesser->getSituationModule().getKind() == 2) {
            if ((float)fabs(stickX) + (float)fabs(stickY) >= stickMin) {
                angle = atan2(stickY, stickX * lr);
            }
            velocity.m_x = lr * (speed * (float)cos(angle));
            velocity.m_y = speed * (float)sin(angle);
            ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
            gravity.m_gravity = 0.0f;
            gravity.disable();
        }
        soKineticEnergyNormal& stop = dynamic_cast<soKineticEnergyNormal&>(*moduleAccesser->getKineticModule().getEnergy(3));
        stop.m_speed = velocity;
        stop.enable();
        ftUtil::setXAngle(moduleAccesser, FIRE_DEGREES_FULL - FIRE_RAD_TO_DEG * angle);
        moduleAccesser->getStageObject().updateNodeSRT();
        moduleAccesser->getWorkManageModule().setFloat(angle, 0x21000004);
        break;
    }
    case 0x118:
        // The fall after the flight keeps only a ground brake.
        if (moduleAccesser->getSituationModule().getKind() != 2) {
            soKineticEnergyNormal& stop = dynamic_cast<soKineticEnergyNormal&>(*moduleAccesser->getKineticModule().getEnergy(3));
            stop.m_brake = Vec2f(soValueAccesser::getConstantFloat(moduleAccesser, 0xfb6, 0), 0.0f);
        }
        break;
    case 0x119: {
        // Landing slide: the speed is kept along the ground and a spark is shown at the contact point.
        soKineticEnergyNormal& stop = dynamic_cast<soKineticEnergyNormal&>(*moduleAccesser->getKineticModule().getEnergy(3));
        Vec2f speed = stop.getSpeed();
        float lengthSq = speed.m_x * speed.m_x + speed.m_y * speed.m_y;
        float length;
        if ((float)fabs(lengthSq) <= 1.17549435e-38f) {
            length = 0.0f;
        } else {
            length = lengthSq * rsqrtf(lengthSq);
        }
        float signedLength = length * moduleAccesser->getPostureModule().getLr();
        stop.m_speed = Vec2f(signedLength * soValueAccesser::getConstantFloat(moduleAccesser, 0xfb8, 0), 0.0f);
        Vec2f contact;
        Vec2f normal;
        if (ftUtil::getAirGroundTouchInfo(moduleAccesser, &contact, &normal)) {
            float rotZ = atan2(-normal.m_x, normal.m_y);
            Vec3f rot(0.0f, 0.0f, rotZ);
            Vec3f pos(contact.m_x, contact.m_y, 0.0f);
            moduleAccesser->getEffectModule().req(static_cast<EfID>(9), &pos, &rot, 1.0f, 0, -1);
        }
        break;
    }
    }
}

void ftFoxStatusUniqProcessFire::exitStatus(soModuleAccesser* moduleAccesser, int) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x118:
        break;
    case 0x117:
    case 0x119:
        if (moduleAccesser->getSituationModule().getKind() == 2) {
            // Leaving the flight in the air starts the special fall.
            int frames = soValueAccesser::getConstantInt(moduleAccesser, 0x5dc8, 0);
            moduleAccesser->getWorkManageModule().setFloat((float)frames, 0x11000000);
            float fallSpeed = soValueAccesser::getConstantFloat(moduleAccesser, 0xfb9, 0);
            moduleAccesser->getWorkManageModule().setFloat(fallSpeed, 0x11000001);
            moduleAccesser->getWorkManageModule().onFlag(0x12000003);
        }
        break;
    }
}

void ftFoxStatusUniqProcessFire::execStatus(soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x114:
        if (moduleAccesser->getSituationModule().getKind() == 2) {
            if (moduleAccesser->getWorkManageModule().getInt(0x20000000) <= 0) {
                ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
                gravity.m_gravity = -soValueAccesser::getConstantFloat(moduleAccesser, 0xfb1, 0);
            } else {
                ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
                gravity.m_gravity = 0.0f;
            }
            soKineticEnergyNormal& stop = dynamic_cast<soKineticEnergyNormal&>(*moduleAccesser->getKineticModule().getEnergy(3));
            stop.m_brake = Vec2f(soValueAccesser::getConstantFloat(moduleAccesser, 0xfb0, 0), 0.0f);
        }
        break;
    case 0x117: {
        // The flight brakes after a number of frames and follows the ground slope while on it.
        int frame = moduleAccesser->getWorkManageModule().getInt(0x20000001);
        if (frame > soValueAccesser::getConstantInt(moduleAccesser, 0x5dc7, 0)) {
            soKineticEnergyNormal& stop = dynamic_cast<soKineticEnergyNormal&>(*moduleAccesser->getKineticModule().getEnergy(3));
            stop.m_brake = Vec2f(soValueAccesser::getConstantFloat(moduleAccesser, 0xfb5, 0), 0.0f);
        } else {
            soKineticEnergyNormal& stop = dynamic_cast<soKineticEnergyNormal&>(*moduleAccesser->getKineticModule().getEnergy(3));
            stop.m_brake = Vec2f(0.0f, 0.0f);
        }
        float angle = moduleAccesser->getWorkManageModule().getFloat(0x21000004);
        if (moduleAccesser->getSituationModule().getKind() != 2) {
            Vec2f groundNormal = moduleAccesser->getGroundModule().getTouchNormal(8, 0);
            float normalY = groundNormal.m_y;
            angle = atan2(-groundNormal.m_x * moduleAccesser->getPostureModule().getLr(), normalY);
        }
        ftUtil::setXAngle(moduleAccesser, FIRE_DEGREES_FULL - FIRE_RAD_TO_DEG * angle);
        moduleAccesser->getWorkManageModule().setFloat(angle, 0x21000004);
        break;
    }
    case 0x119:
        if (moduleAccesser->getSituationModule().getKind() == 2) {
            soKineticEnergy* energy = moduleAccesser->getKineticModule().getEnergy(0);
            if (!energy->isEnable()) {
                energy->resetEnergy(6, &Vec2f(0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
                energy->enable();
            }
        } else {
            ftKineticEnergyDisableAndClear(0, moduleAccesser);
        }
        break;
    }
}

void ftFoxStatusUniqProcessFire::execStop(soModuleAccesser* moduleAccesser) {
    ftUtil::setXAngle(moduleAccesser, FIRE_DEGREES_FULL - FIRE_RAD_TO_DEG * moduleAccesser->getWorkManageModule().getFloat(0x21000004));
}

void ftFoxStatusUniqProcessFire::execFixPos(soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x117:
        if (moduleAccesser->getWorkManageModule().isFlag(0x22000011) && moduleAccesser->getSituationModule().getKind() == 0) {
            // Hitting the ground at a shallow enough angle ends the flight with a slide.
            Vec2f groundNormal = moduleAccesser->getGroundModule().getTouchNormal(8, 0);
            Vec2f sumSpeed = moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1));
            float angle = vec2fAngle(&groundNormal, &sumSpeed);
            if (angle >= 0.0174532924f * (90.0f + soValueAccesser::getConstantFloat(moduleAccesser, 0xfb7, 0))) {
                moduleAccesser->getStatusModule().changeStatusRequest(0x119, moduleAccesser);
            }
        }
        break;
    }
}
