#define MT_VEC2F_LENGTH_SQ_SINGLE_EXPR
#define MT_VEC2F_MUL_ASSIGN_DIRECT // MATCH-ONLY: native compound scaling updates components directly.
#define MT_VEC2F_PLAIN_BASE // MATCH-ONLY: returned vectors copy as word pairs, assignments as floats.
#define FT_KINETIC_STOP_IMPLEMENTATION
#include <ft/ft_kinetic_energy.h>
#include <so/so_kinetic_utility.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>

// Cancel movement into the wall that faces the current horizontal velocity.
// HYPOTHESIS: original inline helper name; the two update branches share this block.
static inline void cancelSpeedIntoWall(soKineticEnergyNormal* energy, soModuleAccesser* acc) {
    int touch = acc->getGroundModule().getTouchFlag(0);
    Vec2f current = energy->getSpeed();
    if (current.m_x > 0.0f) {
        if (touch & 4) energy->m_speed = Vec2f(0.0f, 0.0f);
    } else {
        if (touch & 2) energy->m_speed = Vec2f(0.0f, 0.0f);
    }
}

ftKineticEnergyStop::ftKineticEnergyStop() :
    soKineticEnergyNormal(Vec2f(0.0f, 0.0f), Vec2f(0.0f, 0.0f),
        Vec2f(0.0f, 0.0f), Vec2f(0.0f, 0.0f), Vec2f(-1.0f, -1.0f), false, false),
    m_resetType(-1), unk38(false), unk39(false) { }

void ftKineticEnergyStop::resetEnergy(int type, Vec2f* speed, Vec3f*, soModuleAccesser* acc) {
    clearSpeed();
    m_resetType = -1;
    float initialX = speed->m_x;
    float initialY = speed->m_y;
    m_accel = Vec2f(0.0f, 0.0f);
    m_speedTarget = Vec2f(0.0f, 0.0f);
    m_brake = Vec2f(0.0f, 0.0f);
    m_speedLimit = Vec2f(-1.0f, -1.0f);
    m_speed.m_x = initialX;
    m_speed.m_y = initialY;
    m_resetType = type;
    switch (type) {
    case 0:
    case 13:
    case 14:
    case 15: {
        // Ground braking follows the surface rather than preserving world X/Y.
        Vec2f projected = soKineticUtility::projectionGroundSpeed(&getSpeed(), acc);
        m_speed = projected;
        float brake = soValueAccesser::getConstantFloat(acc, 0xBBC, 0);
        if (type == 13) brake *= soValueAccesser::getConstantFloat(acc, 0xCC4, 0);
        else if (type == 14) brake *= soValueAccesser::getConstantFloat(acc, 0xD76, 0);
        else if (type == 15) brake *= soValueAccesser::getConstantFloat(acc, 0xD74, 0);
        float magnitude = projected.length();
        if (magnitude > soValueAccesser::getConstantFloat(acc, 0xBBB, 0)) {
            brake *= soValueAccesser::getConstantFloat(acc, 0xC2F, 0);
        }
        m_brake = Vec2f(brake, 0.0f);
        m_speedLimit = Vec2f(soValueAccesser::getConstantFloat(acc, 0xBC2, 0), 0.0f);
        break;
    }
    case 17: {
        Vec2f projected = soKineticUtility::projectionGroundSpeed(&getSpeed(), acc);
        m_speed = projected;
        m_speedLimit = Vec2f(soValueAccesser::getConstantFloat(acc, 0xBC2, 0), 0.0f);
        break;
    }
    case 18: {
        Vec2f projected = soKineticUtility::projectionGroundSpeed(&getSpeed(), acc);
        m_speed = projected;
        float mul = soValueAccesser::getConstantFloat(acc, 0xC38, 0);
        float baseBrake = soValueAccesser::getConstantFloat(acc, 0xBBC, 0);
        m_brake = Vec2f(baseBrake * mul, 0.0f);
        m_speedLimit = Vec2f(soValueAccesser::getConstantFloat(acc, 0xBC2, 0), 0.0f);
        break;
    }
    case 1:
    case 11:
    case 23: {
        if (type != 23) {
            Vec2f projected = soKineticUtility::projectionGroundSpeed(&getSpeed(), acc);
            m_speed = projected;
        }
        float mul = soValueAccesser::getConstantFloat(acc, 0xCDD, 0);
        float baseBrake = soValueAccesser::getConstantFloat(acc, 0xBBC, 0);
        m_brake = Vec2f(baseBrake * mul, 0.0f);
        m_speedLimit = Vec2f(soValueAccesser::getConstantFloat(acc, 0x7DB, 0), 0.0f);
        break;
    }
    case 21: {
        Vec2f projected = soKineticUtility::projectionGroundSpeed(&getSpeed(), acc);
        m_speed = projected;
        float mul = soValueAccesser::getConstantFloat(acc, 0xCB7, 0);
        float baseBrake = soValueAccesser::getConstantFloat(acc, 0xBBC, 0);
        m_brake = Vec2f(baseBrake * mul, 0.0f);
        m_speedLimit = Vec2f(soValueAccesser::getConstantFloat(acc, 0x7DB, 0), 0.0f);
        break;
    }
    case 8:
        m_speedLimit = Vec2f(soValueAccesser::getConstantFloat(acc, 0xBD8, 0), 0.0f);
        m_brake = Vec2f(soValueAccesser::getConstantFloat(acc, 0xC4E, 0), 0.0f);
        break;
    case 6:
    case 7:
        m_speedLimit = Vec2f(soValueAccesser::getConstantFloat(acc, 0xBD8, 0), 0.0f);
        m_brake = Vec2f(soValueAccesser::getConstantFloat(acc, 0xBD6, 0), 0.0f);
        break;
    case 19: {
        m_speedLimit = Vec2f(soValueAccesser::getConstantFloat(acc, 0xBD8, 0), 0.0f);
        m_brake = Vec2f(soValueAccesser::getConstantFloat(acc, 0xBD6, 0), 0.0f);
        // Same adjustment record used by gravity mode 7; only its X multiplier is needed here.
        // HYPOTHESIS: original record name and the unused fields.
        struct StopAdjustment { u32 unk0, unk4, unk8, unkC; float speedMulX; };
        const StopAdjustment* adjustment = static_cast<const StopAdjustment*>(
            soValueAccesser::getConstantIndefinite(acc, 0xA80B, 0));
        Vec2f current = getSpeed();
        m_speed = Vec2f(current.m_x * adjustment->speedMulX, current.m_y);
        break;
    }
    case 16:
        m_speedLimit = Vec2f(soValueAccesser::getConstantFloat(acc, 0xC14, 0), 0.0f);
        m_brake = Vec2f(soValueAccesser::getConstantFloat(acc, 0xC15, 0), 0.0f);
        break;
    case 20: {
        Vec2f projected = soKineticUtility::projectionGroundSpeed(&getSpeed(), acc);
        m_speed = projected;
        float mul = soValueAccesser::getConstantFloat(acc, 0xCBA, 0);
        float baseBrake = soValueAccesser::getConstantFloat(acc, 0xBBC, 0);
        m_brake = Vec2f(baseBrake * mul, 0.0f);
        m_speedLimit = Vec2f(soValueAccesser::getConstantFloat(acc, 0xBC2, 0), 0.0f);
        break;
    }
    case 2: case 3: case 4: case 5: case 9: case 10: case 12: case 22:
        break;
    default:
        return;
    }
    unk38 = false;
    unk39 = false;
}

// Air modes reduce the magnitude along the current velocity. Ground modes leave
// the integration to normal energy; wall contact can cancel the entire vector.
void ftKineticEnergyStop::updateEnergy(soModuleAccesser* acc) {
    switch (m_resetType) {
    case 1: case 2: case 3: case 9: case 10: case 21: {
        float magnitude = getSpeed().length();
        if (magnitude <= 0.00001f) {
            m_accel = Vec2f(0.0f, 0.0f);
        } else if (magnitude > 0.00001f) {
            bool useNormalBrake = false;
            float brake = 0.0f;
            switch (m_resetType) {
            case 1:
            case 2:
                if (unk39 == true) {
                    Vec2f current = getSpeed();
                    current *= soValueAccesser::getConstantFloat(acc, 0x7DC, 0);
                    unk39 = false;
                    m_speed = current;
                }
                if (unk38 == true) {
                    unk39 = true;
                    unk38 = false;
                }
                if (acc->getSituationModule().getKind() == 0) {
                    float mul = soValueAccesser::getConstantFloat(acc, 0xCDD, 0);
                    float baseBrake = soValueAccesser::getConstantFloat(acc, 0xBBC, 0);
                    m_brake = Vec2f(baseBrake * mul, 0.0f);
                    m_accel = Vec2f(0.0f, 0.0f);
                    m_speedLimit = Vec2f(soValueAccesser::getConstantFloat(acc, 0x7DB, 0), 0.0f);
                    cancelSpeedIntoWall(this, acc);
                    useNormalBrake = true;
                } else {
                    brake = soValueAccesser::getConstantFloat(acc, 0xCDE, 0);
                    m_brake = Vec2f(0.0f, 0.0f);
                    m_speedLimit = Vec2f(-1.0f, -1.0f);
                }
                break;
            case 3:
                if (acc->getSituationModule().getKind() == 0) {
                    float mul = soValueAccesser::getConstantFloat(acc, 0xCDD, 0);
                    float baseBrake = soValueAccesser::getConstantFloat(acc, 0xBBC, 0);
                    m_brake = Vec2f(baseBrake * mul, 0.0f);
                    m_accel = Vec2f(0.0f, 0.0f);
                    m_speedLimit = Vec2f(soValueAccesser::getConstantFloat(acc, 0x7DB, 0), 0.0f);
                    useNormalBrake = true;
                } else {
                    float airBrake = soValueAccesser::getConstantFloat(acc, 0xD0E, 0);
                    m_brake = Vec2f(0.0f, 0.0f);
                    m_speedLimit = Vec2f(-1.0f, -1.0f);
                    brake = airBrake;
                }
                break;
            case 21:
                if (acc->getSituationModule().getKind() == 0) {
                    float mul = soValueAccesser::getConstantFloat(acc, 0xCB7, 0);
                    float baseBrake = soValueAccesser::getConstantFloat(acc, 0xBBC, 0);
                    m_brake = Vec2f(baseBrake * mul, 0.0f);
                    m_accel = Vec2f(0.0f, 0.0f);
                    m_speedLimit = Vec2f(soValueAccesser::getConstantFloat(acc, 0x7DB, 0), 0.0f);
                    cancelSpeedIntoWall(this, acc);
                    useNormalBrake = true;
                } else {
                    float airBrake = soValueAccesser::getConstantFloat(acc, 0xCB6, 0);
                    m_brake = Vec2f(0.0f, 0.0f);
                    m_speedLimit = Vec2f(-1.0f, -1.0f);
                    brake = airBrake;
                }
                break;
            default:
                brake = m_brake.length();
                break;
            }
            if (useNormalBrake != true) {
                float remainingSpeed = magnitude - brake;
                if (remainingSpeed < 0.0f) {
                    if (m_resetType == 10) {
                        Vec2f accel = getSpeed();
                        Vec2f target = m_speedTarget;
                        accel *= -1.0f;
                        // Independent tests are intentional: crossing zero changes the second test.
                        if (accel.m_x > 0.0f) accel.m_x -= target.m_x;
                        if (accel.m_x < 0.0f) accel.m_x += target.m_x;
                        if (accel.m_y > 0.0f) accel.m_y -= target.m_y;
                        if (accel.m_y < 0.0f) accel.m_y += target.m_y;
                        m_accel = accel;
                    } else {
                        m_accel = Vec2f(0.0f, 0.0f);
                        m_speed = Vec2f(0.0f, 0.0f);
                    }
                } else {
                    Vec2f accel = getSpeed() * (remainingSpeed / magnitude - 1.0f);
                    m_accel = accel;
                }
            }
        }
        break;
    }
    case 7: {
        double absoluteSpeedX = __fabs(getSpeed().m_x);
        float speedX = absoluteSpeedX;
        if (speedX > soValueAccesser::getConstantFloat(acc, 0xBD5, 0)) {
            m_brake = Vec2f(soValueAccesser::getConstantFloat(acc, 0xC47, 0), 0.0f);
        } else {
            m_brake = Vec2f(soValueAccesser::getConstantFloat(acc, 0xBD6, 0), 0.0f);
        }
        break;
    }
    case 14: {
        // Animation-local forward motion is Z; turn it into world X using facing.
        Vec3f motionSpeed(0.0f, 0.0f, 0.0f);
        if (acc->getMotionModule().isTransMove() == true) {
            acc->getMotionModule().updateTransMoveSpeed();
            motionSpeed = acc->getMotionModule().getTransMoveSpeed();
            motionSpeed.m_x = motionSpeed.m_z * acc->getPostureModule().getLr();
            m_accel = Vec2f(motionSpeed.m_x, motionSpeed.m_y) - getSpeed();
            m_brake = Vec2f(0.0f, 0.0f);
        }
        break;
    }
    case 15: {
        Vec3f motionSpeed(0.0f, 0.0f, 0.0f);
        if (acc->getMotionModule().isTransMove() == true) {
            acc->getMotionModule().updateTransMoveSpeed();
            motionSpeed = acc->getMotionModule().getTransMoveSpeed();
            motionSpeed.m_x = motionSpeed.m_z * acc->getPostureModule().getLr();
            m_accel = Vec2f(motionSpeed.m_x, motionSpeed.m_y) - getSpeed();
            m_brake = Vec2f(0.0f, 0.0f);
        } else {
            int frame = soValueAccesser::getConstantInt(acc, 0x5A5D, 0);
            if (acc->getMotionModule().getFrame() > frame) {
                m_brake = Vec2f(soValueAccesser::getConstantFloat(acc, 0xD75, 0) *
                    (soValueAccesser::getConstantFloat(acc, 0xBBC, 0) *
                     soValueAccesser::getConstantFloat(acc, 0xD74, 0)), 0.0f);
            }
        }
        break;
    }
    }
    soKineticEnergyNormal::updateEnergy(acc);
}

ftKineticEnergyStop::~ftKineticEnergyStop() { }
