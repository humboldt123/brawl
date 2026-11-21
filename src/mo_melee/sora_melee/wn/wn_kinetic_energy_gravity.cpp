#include <mt/mt_vector.h>
#include <so/so_kinetic_utility.h>
#include <so/so_module_accesser.h>
#include <wn/wn_kinetic_energy_gravity.h>
#include <types.h>

// The vertical-only energy of the falling articles.

void wnKineticEnergyGravity::resetEnergy(int, const Vec2f& speed, const Vec3f&, soModuleAccesser*) {
    m_speedY = speed.m_y;
}

void wnKineticEnergyGravity::updateEnergy(soModuleAccesser*) {
    m_speedY += m_gravity;
    if (m_speedY < 0.0f && m_speedLimit > 0.0f) {
        double limitAbs = __fabs(m_speedLimit);
        double speedAbs = __fabs(m_speedY);
        if ((float)speedAbs > (float)limitAbs) {
            m_speedY = soKineticUtility::limitSpeed(m_speedY, m_speedLimit);
        }
    }
}

// Reflects v about the normal n (out = v - 2 n (n . v)).
static inline void reflectVec(Vec2f& out, const Vec2f& v, const Vec2f& n) {
    out = v - n * 2.0f * (n.m_x * v.m_x + n.m_y * v.m_y);
}

void wnKineticEnergyGravity::reflectSpeed(const Vec3f& normal) {
    Vec2f v(0.0f, m_speedY);
    Vec2f n(normal);
    reflectVec(v, v, n);
    m_speedY = v.m_y;
}

void wnKineticEnergyGravity::reflectAccel(const Vec3f& normal) {
    Vec2f v(0.0f, m_gravity);
    Vec2f n(normal);
    reflectVec(v, v, n);
    m_gravity = v.m_y;
}

void wnKineticEnergyGravity::mulAccel(Vec3f* mul) {
    m_gravity *= mul->m_y;
}

void wnKineticEnergyGravity::mulSpeed(Vec3f* mul) {
    m_speedY *= mul->m_y;
}

void wnKineticEnergyGravity::clearRotSpeed() {
}

void wnKineticEnergyGravity::clearSpeed() {
    m_speedY = 0.0f;
}

Vec3f wnKineticEnergyGravity::getRotation() const {
    return Vec3f(0.0f, 0.0f, 0.0f);
}

Vec2f wnKineticEnergyGravity::getSpeed() const {
    return Vec2f(0.0f, m_speedY);
}

wnKineticEnergyGravity::~wnKineticEnergyGravity() {
}
