#define FT_KINETIC_MOTION_IMPLEMENTATION
#define MT_VEC2F_MUL_ASSIGN_DIRECT // MATCH-ONLY: scale planar vectors in place.
#define MT_VEC3F_MUL_INLINE
#define MT_VEC3F_SCALE_SPLIT_RESULT // MATCH-ONLY: retain the native paired-single input/result lifetimes.
#include <ft/ft_kinetic_energy.h>
#include <math.h>
#include <so/so_kinetic_utility.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>

// HYPOTHESIS: original inline helper name. Both rotating modes use this transform.
static inline void rotateMotionSpeed(Vec2f& speed, const float& angle) {
    // MATCH-ONLY: retain the native X-before-Y snapshot in the shared transform.
    float y;
    float x = speed.m_x;
    y = speed.m_y;
    float cosine = cosf(angle);
    float sine = sinf(angle);
    speed.m_x = x * cosine - y * sine;
    speed.m_y = x * sine + y * cosine;
}

// HYPOTHESIS: original inline helper name. Cancels the remaining velocity when translation ends.
static inline Vec2f negateMotionSpeed(const Vec2f& speed) {
    Vec2f result;
    result.m_x = -speed.m_x;
    result.m_y = -speed.m_y;
    return result;
}

ftKineticEnergyMotion::ftKineticEnergyMotion() :
    m_motionMode(-1), m_facing(1.0f), m_prevMotionSpeed(0.0f, 0.0f),
    m_hasPrevMotionSpeed(false) { }

void ftKineticEnergyMotion::resetEnergy(int mode, Vec2f* speed, Vec3f*, soModuleAccesser* acc) {
    clearSpeed();
    float initialX = speed->m_x;
    float initialY = speed->m_y;
    m_accel = Vec2f(0.0f, 0.0f);
    m_speedTarget = Vec2f(-1.0f, -1.0f);
    m_brake = Vec2f(0.0f, 0.0f);
    m_speedLimit = Vec2f(-1.0f, -1.0f);
    m_facing = 1.0f;
    m_rotation = 0.0f;
    m_speedMul = 1.0f;
    m_speed.m_x = initialX;
    m_speed.m_y = initialY;
    m_motionMode = mode;
    m_facing = acc->getPostureModule().getLr();
    switch (mode) {
    case 0:
    case 1:
    case 2:
    case 3:
        if (mode != 3) {
            Vec2f projected;
            Vec2f::copy(projected, soKineticUtility::projectionGroundSpeed(&getSpeed(), acc));
            m_speed = projected;
        }
        m_brake = Vec2f(soValueAccesser::getConstantFloat(acc, 0xBBC, 0), 0.0f);
        break;
    case 7: {
        int savedMotion = acc->getWorkManageModule().getInt(0x20000001);
        int currentMotion = acc->getMotionModule().getKind();
        if (savedMotion == currentMotion) {
            m_speedMul = acc->getWorkManageModule().getFloat(0x21000009);
        }
        break;
    }
    case 4: case 5: case 6: case 8: case 9: case 10: case 11: case 12: case 13:
        break;
    default:
        return;
    }
    m_prevMotionSpeed = *speed;
    m_hasPrevMotionSpeed = false;
}

void ftKineticEnergyMotion::updateEnergy(soModuleAccesser* acc) {
    Vec2f nextMotionSpeed(0.0f, 0.0f);
    bool hasTranslation;
    if (m_motionMode == 8) hasTranslation = acc->getMotionModule().is2ndTransMove();
    else hasTranslation = acc->getMotionModule().isTransMove();
    m_unk32 = true;
    if (hasTranslation == true) {
        bool groundTracking = false;
        acc->getMotionModule().updateTransMoveSpeed();
        Vec3f translation = acc->getMotionModule().getTransMoveSpeed();
        if (m_motionMode == 8) {
            acc->getMotionModule().update2ndTransMoveSpeed();
            translation = acc->getMotionModule().get2ndTransMoveSpeed();
        }
        Vec3fScale(&translation, &translation, m_speedMul);
        float facing;
        switch (m_motionMode) {
        case 0:
        case 1:
            m_hasPrevMotionSpeed = false;
            // These modes follow a facing change immediately.
        case 5:
        case 7:
            facing = acc->getPostureModule().getLr();
            break;
        default:
            facing = m_facing;
            break;
        }
        Vec2f desired;
        if (facing != 0.0f) {
            desired.m_y = translation.m_y;
            desired.m_x = translation.m_z * facing;
        } else {
            desired.m_y = translation.m_y;
            desired.m_x = translation.m_z;
        }
        switch (m_motionMode) {
        case 2:
        case 3:
            m_unk32 = false;
            // Grounded modes remove the horizontal cap while following translation.
        case 0:
        case 1:
            m_speedLimit = Vec2f(-1.0f, 0.0f);
            break;
        }
        switch (m_motionMode) {
        case 0:
            groundTracking = true;
            break;
        case 1: {
            soWorkManageModule& work = acc->getWorkManageModule();
            soMotionModule& motion = acc->getMotionModule();
            float originalRate = work.getFloat(0x21000007);
            float rate = motion.getRate();
            desired *= 1.0f / (rate / originalRate);
            groundTracking = true;
            break;
        }
        case 2:
            desired = soKineticUtility::projectionGroundSpeed(&desired, acc);
            groundTracking = true;
            break;
        case 3:
            groundTracking = true;
            break;
        case 4:
        case 8:
            m_unk32 = false;
            break;
        case 5:
            rotateMotionSpeed(desired, m_rotation);
            m_unk32 = false;
            break;
        case 7: {
            float stick = acc->getControllerModule().getStickX();
            float deadZone = acc->getWorkManageModule().getFloat(0x21000005);
            double absolute = fabs(stick);
            float magnitude = absolute;
            if (magnitude > deadZone) {
                float strength = (magnitude - deadZone) / (1.0f - deadZone);
                float maxDegrees = acc->getWorkManageModule().getFloat(0x21000006);
                float degrees = strength * maxDegrees;
                float angle;
                if (stick > 0.0f) angle = -(0.017453292f * degrees);
                else angle = 0.017453292f * degrees;
                // Keep the strongest steering angle sampled during the move.
                double wideAngle = fabs(angle);
                double wideRotation = fabs(m_rotation);
                float angleMagnitude = wideAngle;
                float rotationMagnitude = wideRotation;
                if (angleMagnitude > rotationMagnitude) m_rotation = angle;
            }
            rotateMotionSpeed(desired, m_rotation);
            m_unk32 = false;
            break;
        }
        // MATCH-ONLY: keep this independent mode after the steering block.
        case 6:
            desired.m_x = 0.0f;
            m_unk32 = false;
            break;
        case 9:
        case 10:
        case 11: {
            // Follow animation relative to the ledge, rather than accumulating drift.
            Vec3f cliff = acc->getGroundModule().getCliffPos(0);
            Vec3f offset = acc->getMotionModule().getTransNTranslate();
            Vec3f current = acc->getPostureModule().getPos();
            float targetY = cliff.m_y + offset.m_y;
            float targetZ = current.m_z;
            Vec3f target(cliff.m_x + offset.m_z * acc->getPostureModule().getLr(), targetY, targetZ);
            desired.m_x = target.m_x - current.m_x;
            desired.m_y = target.m_y - current.m_y;
            if (m_motionMode == 9) {
                int frames = acc->getWorkManageModule().getInt(0x20000000);
                float remaining = frames + 1;
                float scale = 1.0f / remaining;
                desired *= scale;
            }
            break;
        }
        case 12: {
            desired.m_x = 0.0f;
            float stick = acc->getControllerModule().getStickY();
            double wideStick = fabs(stick);
            float stickMagnitude = wideStick;
            if (stickMagnitude < 0.5f) desired.m_y = 0.0f;
            else if (stick > 0.0f) {
                desired.m_y = soValueAccesser::getConstantFloat(acc, 0xC0E, 0);
                desired.m_y *= acc->getMotionModule().getRate();
            } else {
                desired.m_y = -soValueAccesser::getConstantFloat(acc, 0xC0F, 0);
                desired.m_y *= acc->getMotionModule().getRate();
            }
            break;
        }
        case 13: {
            float baseY = acc->getWorkManageModule().getFloat(0x21000007);
            float baseX = acc->getWorkManageModule().getFloat(0x21000006);
            Vec2f base(baseX, baseY);
            float storedY = acc->getWorkManageModule().getFloat(0x21000009);
            float storedX = acc->getWorkManageModule().getFloat(0x21000008);
            Vec2f stored(storedX, storedY);
            Vec3f offset = acc->getMotionModule().getTransNTranslate();
            Vec3f target(base.m_x + offset.m_z * acc->getPostureModule().getLr(),
                base.m_y + offset.m_y, 0.0f);
            desired.m_x = 0.0f;
            desired.m_y = target.m_y - stored.m_y;
            // Advance the stored position by this frame's animation correction.
            acc->getWorkManageModule().addFloat(desired.m_y, 0x21000009);
            break;
        }
        }
        nextMotionSpeed = desired;
        Vec2f accel;
        Vec2f targetSpeed(-1.0f, -1.0f);
        if (groundTracking == true) {
            if (m_hasPrevMotionSpeed == false) {
                accel = desired - getSpeed();
                m_hasPrevMotionSpeed = true;
            } else if (nextMotionSpeed.m_x == 0.0f && m_prevMotionSpeed.m_x == 0.0f) {
                accel.m_y = 0.0f;
                accel.m_x = 0.0f;
                targetSpeed.m_y = 0.0f;
                targetSpeed.m_x = 0.0f;
            } else {
                accel = desired - m_prevMotionSpeed;
            }
        } else if (m_hasPrevMotionSpeed == false) {
            accel = desired - getSpeed();
            m_hasPrevMotionSpeed = true;
        } else {
            accel = desired - m_prevMotionSpeed;
        }
        m_accel = accel;
        m_speedTarget = targetSpeed;
    } else {
        switch (m_motionMode) {
        case 0: case 1: case 2: case 3:
            m_speedLimit = Vec2f(soValueAccesser::getConstantFloat(acc, 0xBC2, 0), 0.0f);
            m_accel = Vec2f(0.0f, 0.0f);
            break;
        case 4: case 5: case 6: case 7: case 8: case 9: case 10: case 11: case 13:
            m_accel = Vec2f(0.0f, 0.0f);
            break;
        case 12: {
            m_accel = negateMotionSpeed(getSpeed());
            break;
        }
        }
        m_speedTarget = Vec2f(0.0f, 0.0f);
    }
    soKineticEnergyNormal::updateEnergy(acc);
    m_prevMotionSpeed = nextMotionSpeed;
    m_unk32 = true;
}

ftKineticEnergyMotion::~ftKineticEnergyMotion() { }
