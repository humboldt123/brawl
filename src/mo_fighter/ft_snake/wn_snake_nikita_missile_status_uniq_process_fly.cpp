// BYTES: the original calls the out-of-line Vec2f assignment (__as__5Vec2fFRC5Vec2f) for every energy-vector store.
#define MT_VEC2F_ASSIGN_NOINLINE
#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <wn/snake/wn_snake_nikita_missile_status_uniq_process_fly.h>
#include <wn/snake/so_kinetic_energy_local_normal.h>
#include <so/so_module_accesser.h>
#include <snd/snd_id.h>
#include <so/so_kinetic_energy_rot_normal.h>
#include <math.h>

// HYPOTHESIS: the turn sound's repeat delay and angle threshold; the native .rodata opens with these two values.
extern const int TURN_SE_INTERVAL = 10;
extern const float TURN_SE_MIN_ANGLE = 5.0f;

wnSnakeNikitaMissileStatusUniqProcessFly g_wnSnakeNikitaMissileStatusUniqProcessFly;


wnSnakeNikitaMissileStatusUniqProcessFly::wnSnakeNikitaMissileStatusUniqProcessFly() {}

wnSnakeNikitaMissileStatusUniqProcessFly::~wnSnakeNikitaMissileStatusUniqProcessFly() {}

void wnSnakeNikitaMissileStatusUniqProcessFly::initStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    work.setInt(0, 0x20000003);
}


void wnSnakeNikitaMissileStatusUniqProcessFly::execStatus(soModuleAccesser* moduleAccesser) {
    soKineticEnergyLocalNormal* localEnergy = dynamic_cast<soKineticEnergyLocalNormal*>(
        moduleAccesser->getKineticModule().getEnergy(2));
    soKineticEnergyRotNormal* rotationEnergy = dynamic_cast<soKineticEnergyRotNormal*>(
        moduleAccesser->getKineticModule().getEnergy(1));
    if (localEnergy == 0 || rotationEnergy == 0) {
        return;
    }

    float oldAngle = localEnergy->unk34;

    if (moduleAccesser->getWorkManageModule().getInt(0x10000006) >= 0) {
        moduleAccesser->getWorkManageModule().decInt(0x10000006);
        float angle = localEnergy->unk34;
        angle += moduleAccesser->getWorkManageModule().getFloat(0x11000001);
        moduleAccesser->getWorkManageModule().setFloat(localEnergy->unk34, 0x21000002);
        localEnergy->unk34 = angle;

        Vec3f rot(-angle, 90.0f, 0.0f);
        moduleAccesser->getPostureModule().setRot(&rot, true);
    } else if (moduleAccesser->getControllerModule().getStickX() != 0.0f ||
               moduleAccesser->getControllerModule().getStickY() != 0.0f) {
        float turnDelta = moduleAccesser->getControllerModule().getStickAngle() * 57.29578f - localEnergy->unk34;
        if (turnDelta != 0.0f) {
            if (turnDelta < -180.0f) {
                turnDelta += 360.0f;
            } else if (turnDelta > 180.0f) {
                turnDelta -= 360.0f;
            }

            float maxTurn = moduleAccesser->getWorkManageModule().getFloat(0x21000000);
            float slowRange = moduleAccesser->getWorkManageModule().getFloat(0x21000001);
            float turn = 0.0f;
            if (static_cast<float>(fabs(turnDelta)) > maxTurn) {
                turn = maxTurn * (turnDelta < 0.0f ? -1.0f : 1.0f);
            }
            if (static_cast<float>(fabs(turnDelta)) < slowRange) {
                turn *= 0.5f;
            }

            moduleAccesser->getWorkManageModule().setFloat(localEnergy->unk34, 0x21000002);
            float angle = turn + localEnergy->unk34;
            if (static_cast<float>(fabs(angle)) >= 360.0f) {
                angle += 360.0f * (angle >= 0.0f ? -1.0f : 1.0f);
            }
            localEnergy->unk34 = angle;

            Vec3f rot(-(turn + angle), 90.0f, 0.0f);
            moduleAccesser->getPostureModule().setRot(&rot, true);
        }
        processSoundEffect(moduleAccesser, turnDelta);
    }

    float lowerLimit = moduleAccesser->getWorkManageModule().getFloat(0x21000004);
    float upperLimit = moduleAccesser->getWorkManageModule().getFloat(0x21000003);
    if (oldAngle != localEnergy->unk34) {
        localEnergy->m_speedTarget = Vec2f(lowerLimit, 0.0f);

        if (!moduleAccesser->getWorkManageModule().isFlag(0x22000000)) {
            float lowSpeed = moduleAccesser->getWorkManageModule().getFloat(0x11000002);
            float highSpeed = moduleAccesser->getWorkManageModule().getFloat(0x11000003);
            float speedTarget;
            if (localEnergy->getSpeed().m_x <= lowerLimit) {
                speedTarget = highSpeed;
            } else if (localEnergy->getSpeed().m_x >= upperLimit) {
                speedTarget = lowSpeed;
            } else {
                float blend = (rotationEnergy->getRotation().m_z - lowerLimit) / (upperLimit - lowerLimit);
                speedTarget = lowSpeed * blend + highSpeed * (1.0f - blend);
            }

            Vec3f stableSpeed(0.0f, 0.0f, speedTarget);
            rotationEnergy->setStableSpeed(&stableSpeed);
        }
    } else {
        localEnergy->m_speedTarget = Vec2f(upperLimit, 0.0f);
        if (!moduleAccesser->getWorkManageModule().isFlag(0x22000000)) {
            Vec3f stableSpeed(0.0f, 0.0f, moduleAccesser->getWorkManageModule().getFloat(0x11000002));
            rotationEnergy->setStableSpeed(&stableSpeed);
        }
    }

    if (moduleAccesser->getWorkManageModule().getInt(0x20000000) == 0) {
        localEnergy->m_accel = Vec2f(0.0f, 0.0f);
        localEnergy->m_speedTarget = Vec2f(0.0f, 0.0f);
        moduleAccesser->getWorkManageModule().decInt(0x20000000);
        Vec3f stableSpeed(0.0f, 0.0f, moduleAccesser->getWorkManageModule().getFloat(0x11000002));
        rotationEnergy->setStableSpeed(&stableSpeed);
        moduleAccesser->getWorkManageModule().onFlag(0x22000000);
    }
}

void soKineticEnergyRotNormal::setStableSpeed(Vec3f* speed) {
    m_speedTarget.m_x = speed->m_x;
    m_speedTarget.m_y = speed->m_y;
    m_speedTarget.m_z = speed->m_z;
}

void wnSnakeNikitaMissileStatusUniqProcessFly::processSoundEffect(
    soModuleAccesser* moduleAccesser,
    float turnDelta
) {
    if (moduleAccesser->getWorkManageModule().getInt(0x20000003) > 0) {
        moduleAccesser->getWorkManageModule().decInt(0x20000003);
        return;
    }

    // HYPOTHESIS: the five-degree threshold gates the missile's turn sound.
    if (static_cast<float>(fabs(turnDelta)) <= TURN_SE_MIN_ANGLE) {
        return;
    }

    moduleAccesser->getSoundModule().playSE(snd_se_snake_019, false, 0, 0);
    moduleAccesser->getWorkManageModule().setInt(TURN_SE_INTERVAL, 0x20000003);
}
