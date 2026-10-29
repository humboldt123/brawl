#define MT_VEC2F_LENGTH_SQ_SINGLE_EXPR
#include <it/kinetic/it_kinetic_energy.h>
#include <mt/mt_vector.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <types.h>

itKineticEnergyStop::itKineticEnergyStop() : m_mode(-1) {
    onConsiderGroundFriction();
}

void itKineticEnergyStop::resetEnergy(int mode, Vec2f* speed, Vec3f*, soModuleAccesser*) {
    clearSpeed();
    m_mode = -1;
    float x = speed->m_x;
    float y = speed->m_y;
    m_accel = Vec2f(0.0f, 0.0f);
    m_stableSpeed = Vec2f(0.0f, 0.0f);
    m_brake = Vec2f(0.0f, 0.0f);
    m_limit = Vec2f(-1.0f, -1.0f);
    m_speed.m_x = x;
    m_speed.m_y = y;
    m_mode = mode;
}

void itKineticEnergyStop::updateEnergy(soModuleAccesser* moduleAccesser) {
    float length = getSpeed().length();
    if (length <= 0.00001f) {
        m_accel = Vec2f(0.0f, 0.0f);
    } else if (length > 0.00001f) {
        float brake;
        if (moduleAccesser->getSituationModule().getKind() == Situation_Ground) {
            brake = soValueAccesser::getConstantFloat(moduleAccesser, 0xD6C, 0);
            m_brake = Vec2f(brake, m_brake.m_y);
        } else {
            brake = soValueAccesser::getConstantFloat(moduleAccesser, 0xD6B, 0);
            m_brake = Vec2f(brake, m_brake.m_y);
        }
        float rest = length - brake;
        if (rest < 0.0f) {
            if (m_mode == 10) {
                Vec2f accel;
                Vec2f::copy(accel, getSpeed());
                Vec2f stableSpeed;
                Vec2f::copy(stableSpeed, m_stableSpeed);
                accel.m_x *= -1.0f;
                accel.m_y *= -1.0f;
                if (accel.m_x > 0.0f) {
                    accel.m_x -= stableSpeed.m_x;
                }
                if (accel.m_x < 0.0f) {
                    accel.m_x += stableSpeed.m_x;
                }
                if (accel.m_y > 0.0f) {
                    accel.m_y -= stableSpeed.m_y;
                }
                if (accel.m_y < 0.0f) {
                    accel.m_y += stableSpeed.m_y;
                }
                m_accel = accel;
            } else {
                m_accel = Vec2f(0.0f, 0.0f);
            }
        } else {
            Vec2f accel;
            Vec2f::copy(accel, getSpeed() * (rest / length - 1.0f));
            m_accel = accel;
        }
    }
    itKineticEnergy::updateEnergy(moduleAccesser);
}
