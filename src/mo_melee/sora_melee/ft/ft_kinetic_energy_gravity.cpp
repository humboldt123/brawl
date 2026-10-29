#include <ft/ft_kinetic_energy.h>
#include <ft/ft_manager.h>
#include <ft/ft_owner.h>
#include <ft/ft_util.h>
#include <so/so_kinetic_utility.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/so_world.h>

ftKineticEnergyGravity::ftKineticEnergyGravity() :
    m_resetType(-1), m_speedY(0.0f), m_gravity(0.0f),
    m_fallSpeedMax(-1.0f), unk18(-1.0f), unk1C(-1.0f), m_gravityMul(1.0f), m_limitMode(-1) { }

void ftKineticEnergyGravity::resetEnergy(int type, Vec2f* speed, Vec3f*, soModuleAccesser* acc) {
    clearSpeed();
    m_resetType = type;
    m_limitMode = -1;
    int entryId = acc->getWorkManageModule().getInt(0x10000000);
    float gravityMul = ftManager::getInstance()->getOwner(entryId)->getGravity();
    float initialSpeedY = speed->m_y;
    m_gravityMul = gravityMul;
    m_speedY = initialSpeedY;
    switch (m_resetType) {
    case 0:
        m_gravity = -soValueAccesser::getConstantFloat(acc, 3023, 0);
        m_fallSpeedMax = soValueAccesser::getConstantFloat(acc, 3024, 0);
        unk18 = soValueAccesser::getConstantFloat(acc, 3025, 0);
        unk1C = soValueAccesser::getConstantFloat(acc, 3026, 0);
        m_limitMode = 0;
        break;
    case 1:
        m_gravity = soValueAccesser::getConstantFloat(acc, 3023, 0);
        m_fallSpeedMax = soValueAccesser::getConstantFloat(acc, 3024, 0);
        unk18 = soValueAccesser::getConstantFloat(acc, 3025, 0);
        unk1C = soValueAccesser::getConstantFloat(acc, 3026, 0);
        m_limitMode = 1;
        break;
    case 2:
        m_gravity = soValueAccesser::getConstantFloat(acc, 3089, 0);
        m_fallSpeedMax = soValueAccesser::getConstantFloat(acc, 3090, 0);
        unk18 = soValueAccesser::getConstantFloat(acc, 3025, 0);
        unk1C = soValueAccesser::getConstantFloat(acc, 3026, 0);
        m_limitMode = 1;
        break;
    case 3:
        m_gravity = -soValueAccesser::getConstantFloat(acc, 3356, 0);
        m_fallSpeedMax = soValueAccesser::getConstantFloat(acc, 3024, 0);
        unk18 = soValueAccesser::getConstantFloat(acc, 3025, 0);
        unk1C = soValueAccesser::getConstantFloat(acc, 3026, 0);
        m_limitMode = 0;
        break;
    case 4: {
        float mul = soValueAccesser::getConstantFloat(acc, 3094, 0);
        float baseGravity = -soValueAccesser::getConstantFloat(acc, 3023, 0);
        m_gravity = baseGravity * mul;
        m_fallSpeedMax = soValueAccesser::getConstantFloat(acc, 3024, 0);
        unk18 = soValueAccesser::getConstantFloat(acc, 3025, 0);
        unk1C = soValueAccesser::getConstantFloat(acc, 3026, 0);
        m_limitMode = 0;
        break;
    }
    case 5:
        m_gravity = -acc->getWorkManageModule().getFloat(0x21000008);
        m_fallSpeedMax = soValueAccesser::getConstantFloat(acc, 3024, 0);
        unk18 = soValueAccesser::getConstantFloat(acc, 3025, 0);
        unk1C = soValueAccesser::getConstantFloat(acc, 3026, 0);
        m_limitMode = 0;
        break;
    case 6: {
        float mul = soValueAccesser::getConstantFloat(acc, 3337, 0);
        float baseGravity = -soValueAccesser::getConstantFloat(acc, 3023, 0);
        m_gravity = baseGravity * mul;
        m_fallSpeedMax = soValueAccesser::getConstantFloat(acc, 3024, 0);
        unk18 = soValueAccesser::getConstantFloat(acc, 3025, 0);
        unk1C = soValueAccesser::getConstantFloat(acc, 3026, 0);
        m_limitMode = 0;
        break;
    }
    case 7: {
        // HYPOTHESIS: source type/name of this two-value adjustment record.
        struct GravityAdjustment { u32 unk0, unk4; float speedAdd, gravityMul; };
        const GravityAdjustment* adjustment = static_cast<const GravityAdjustment*>(
            soValueAccesser::getConstantIndefinite(acc, 0xA80B, 0));
        m_speedY += adjustment->speedAdd;
        m_gravity = adjustment->gravityMul * -soValueAccesser::getConstantFloat(acc, 3023, 0);
        m_fallSpeedMax = soValueAccesser::getConstantFloat(acc, 3024, 0);
        unk18 = soValueAccesser::getConstantFloat(acc, 3025, 0);
        unk1C = soValueAccesser::getConstantFloat(acc, 3026, 0);
        m_limitMode = 0;
        break;
    }
    case 8:
        m_gravity = -soValueAccesser::getConstantFloat(acc, 3491, 0);
        m_fallSpeedMax = soValueAccesser::getConstantFloat(acc, 3492, 0);
        unk18 = 0.0f;
        unk1C = soValueAccesser::getConstantFloat(acc, 3492, 0);
        m_limitMode = 0;
        break;
    }
}

void ftKineticEnergyGravity::clearSpeed() { m_speedY = 0.0f; }

void ftKineticEnergyGravity::updateEnergy(soModuleAccesser* acc) {
    float worldSpeed = g_soWorld->m_gravityDown;
    // Dive uses a fixed downward speed instead of integrating acceleration.
    if (acc->getWorkManageModule().isFlag(0x22000002) == true) {
        float diveSpeed = -soValueAccesser::getConstantFloat(acc, 3031, 0);
        m_speedY = m_gravityMul * (diveSpeed * worldSpeed);
    } else {
        float worldGravity = g_soWorld->m_gravityUp;
        float accel = worldGravity * (m_gravity * m_gravityMul);
        if (acc->getWorkManageModule().isFlag(0x22000000) == true && ftUtil::isEnableSpeedOperation(acc) == true) {
            // This mode only accelerates against existing movement.
            if (m_speedY * accel < 0.0f) m_speedY += accel;
        } else {
            if (accel * (m_speedY + accel) > 0.0f) {
                accel = soKineticUtility::brakeSpeed(m_speedY, accel,
                    m_gravityMul * (m_fallSpeedMax * worldSpeed), unk18);
            }
            m_speedY += accel;
        }
        if (m_limitMode != -1 && !(unk1C <= 0.0f)) {
            if (m_limitMode == 2) {
                m_speedY = soKineticUtility::limitSpeed(m_speedY, unk1C);
            } else if (m_limitMode == 0) {
                if (m_speedY < 0.0f) m_speedY = soKineticUtility::limitSpeed(m_speedY, unk1C);
            } else if (m_limitMode == 1) {
                if (m_speedY > 0.0f) m_speedY = soKineticUtility::limitSpeed(m_speedY, unk1C);
            }
        }
    }
}

void ftKineticEnergyGravity::mulSpeed(Vec3f* mul) { m_speedY *= mul->m_y; }
void ftKineticEnergyGravity::mulAccel(Vec3f* mul) { m_gravity *= mul->m_y; }

void ftKineticEnergyGravity::reflectSpeed(Vec3f* normal) {
    Vec2f n = *normal->xy();
    Vec2f speed(0.0f, m_speedY);
    Vec2f reflected = speed - n * 2.0f * (n.m_x * speed.m_x + n.m_y * speed.m_y);
    m_speedY = reflected.m_y;
}

void ftKineticEnergyGravity::reflectAccel(Vec3f* normal) {
    Vec2f n = *normal->xy();
    Vec2f accel(0.0f, m_gravity);
    Vec2f reflected = accel - n * 2.0f * (n.m_x * accel.m_x + n.m_y * accel.m_y);
    m_gravity = reflected.m_y;
}

void ftKineticEnergyGravity::clearRotSpeed() { }
Vec3f ftKineticEnergyGravity::getRotation() { return Vec3f(0.0f, 0.0f, 0.0f); }
Vec2f ftKineticEnergyGravity::getSpeed() { return Vec2f(0.0f, m_speedY); }
