#define MT_VEC2F_PLAIN_BASE // MATCH-ONLY: retain native aggregate vector copies.
#define MT_VEC2F_LENGTH_SQ_SINGLE_EXPR // MATCH-ONLY: retain the native squared-length expression.
#define SO_KINETIC_WIND_IMPLEMENTATION
#include <ft/ft_kinetic_energy.h>
#include <math.h>
#include <so/so_module_accesser.h>

soKineticEnergyWindNormal::soKineticEnergyWindNormal() :
    m_windSpeed(0.0f, 0.0f), m_responseRetention(0.0f) {
    m_unk30 = true;
}

void soKineticEnergyWindNormal::updateEnergy(soModuleAccesser* acc) {
    Vec2f current = getSpeed();
    if (m_windSpeed.lengthSq() != 0.0f || current.lengthSq() != 0.0f) {
        Vec2f limit(-1.0f, -1.0f);
        double absoluteY = fabs(m_windSpeed.m_y);
        double absoluteX = fabs(m_windSpeed.m_x);
        float targetY = absoluteY;
        float targetX = absoluteX;
        Vec2f target(targetX, targetY);
        Vec2f accel(0.0f, 0.0f);
        // Vertical wind transport is allowed only while airborne.
        if (acc->getSituationModule().getKind() != Situation_Air) limit.m_y = 0.0f;

        if (float(fabs(target.lengthSq())) > float(fabs(current.lengthSq()))) {
            // Stronger wind approaches the supplied velocity with a retained fraction.
            Vec2f delta = m_windSpeed - current;
            accel = delta * (1.0f - m_responseRetention);
        } else {
            float length = current.length();
            if (length > 0.00001f) {
                float remaining = length - m_brake.length();
                if (remaining >= 0.0f) accel = current * ((remaining / length) - 1.0f);
            }
        }
        m_accel = accel;
        m_speedTarget = target;
        m_speedLimit = limit;
        soKineticEnergyNormal::updateEnergy(acc);
        // Snap tiny residual velocities to rest after normal braking/projection.
        if (getSpeed().length() < 0.00001f) m_speed = Vec2f(0.0f, 0.0f);
        m_windSpeed.m_x = 0.0f;
        m_windSpeed.m_y = 0.0f;
    }
}

void soKineticEnergyWindNormal::init() {
    soKineticEnergyNormal::init();
    m_windSpeed = Vec2f(0.0f, 0.0f);
    m_responseRetention = 0.0f;
}

soKineticEnergyWindNormal::~soKineticEnergyWindNormal() { }
