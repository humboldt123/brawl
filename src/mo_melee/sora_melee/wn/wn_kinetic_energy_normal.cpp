#include <mt/mt_vector.h>
#include <so/so_module_accesser.h>
#include <wn/wn_kinetic_energy_normal.h>
#include <types.h>

// The planar energy the generic article (Weapon) shots fly with.

wnKineticEnergyNormal::wnKineticEnergyNormal() : unk34(-1) {
}

wnKineticEnergyNormal::~wnKineticEnergyNormal() {
}

void wnKineticEnergyNormal::resetEnergy(int mode, const Vec2f& speed, const Vec3f&, soModuleAccesser*) {
    m_accel = Vec2f(0.0f, 0.0f);
    m_speedTarget = Vec2f(-1.0f, -1.0f);
    m_brake = Vec2f(0.0f, 0.0f);
    m_speedLimit = Vec2f(-1.0f, -1.0f);
    m_speed = speed;
    unk34 = mode;
    switch (mode) {
    case 0:
        break;
    case 1:
        m_speedTarget = Vec2f(0.0f, 0.0f);
        break;
    }
}

void wnKineticEnergyNormal::updateEnergy(soModuleAccesser* acc) {
    switch (unk34) {
    case 0:
        break;
    case 1: {
        float speedLen = getSpeed().length();
        if (speedLen <= 0.00001f) {
            m_accel = Vec2f(0.0f, 0.0f);
        } else if (speedLen > 0.00001f) {
            float rest = speedLen - m_brake.length();
            const float restFloor = 0.0f;
            if (rest < restFloor) {
                m_accel = Vec2f(0.0f, 0.0f);
            } else {
                Vec2f accel = getSpeed() * (rest / speedLen - 1.0f);
                m_accel = accel;
            }
        }
        break;
    }
    }
    soKineticEnergyNormal::updateEnergy(acc);
}
