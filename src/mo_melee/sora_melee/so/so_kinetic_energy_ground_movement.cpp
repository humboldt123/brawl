#define SO_KINETIC_CONTACT_IMPLEMENTATION
#define MT_VEC2F_ADD_ASSIGN_DIRECT // MATCH-ONLY: add attached motion without a copied local.
#define MT_VEC2F_MUL_ASSIGN_DIRECT // MATCH-ONLY: decay the retained velocity in place.
#include <ft/ft_kinetic_energy.h>
#include <math.h>
#include <so/so_module_accesser.h>

soKineticEnergyGroundMovement::soKineticEnergyGroundMovement() :
    m_outsideSpeed(0.0f, 0.0f), m_speed(0.0f, 0.0f), m_brake(1.0f),
    m_brakeThreshold(2.0f), m_outsideRotation(0.0f), m_rotation(0.0f),
    m_prevGroundAngle(0.0f), m_brakeEnabled(false), m_rotationEnabled(false) { }

void soKineticEnergyGroundMovement::updateEnergy(soModuleAccesser* acc) {
    int situation = acc->getSituationModule().getKind();
    if (situation == Situation_Ground) {
        m_speed = m_outsideSpeed + acc->getGroundModule().getDownMovementSpeed(0);
        if (acc->getGroundModule().isAttach(0) == true) {
            m_speed += acc->getGroundModule().getAttachMovementSpeed(0);
        }
    } else if (situation == Situation_Air) {
        if (acc->getGroundModule().isAttach(0) == true) {
            m_speed = m_outsideSpeed + acc->getGroundModule().getAttachMovementSpeed(0);
        } else if (m_brakeEnabled == true) {
            // Preserve a departing surface's movement, then brake toward rest.
            if (m_speed.m_x != 0.0f || m_speed.m_y != 0.0f) {
                float length = m_speed.length();
                float brake = m_brake;
                if (length > m_brakeThreshold) brake += length - m_brakeThreshold;
                if (length > 0.00001f && length > brake) {
                    m_speed *= (length - brake) / length;
                } else {
                    m_speed.m_x = 0.0f;
                    m_speed.m_y = 0.0f;
                }
            }
        } else {
            m_speed = m_outsideSpeed;
        }
    } else {
        m_speed = m_outsideSpeed;
    }

    if (m_rotationEnabled == true) {
        m_rotation = m_outsideRotation;
        Vec2f normal;
        bool attached = false;
        if (acc->getGroundModule().isAttach(0) == true) {
            normal = acc->getGroundModule().getAttachNormal(0);
            attached = true;
        }
        float angle = 0.0f;
        if (attached == true) angle = atan2f(normal.m_y, normal.m_x);
        // The consumer uses degrees per frame; the previous normal is in radians.
        m_rotation += 57.29578f * (m_prevGroundAngle - angle);
        m_prevGroundAngle = angle;
    } else {
        m_rotation = 0.0f;
    }
    m_outsideSpeed.m_x = 0.0f;
    m_outsideSpeed.m_y = 0.0f;
    m_outsideRotation = 0.0f;
}

void soKineticEnergyGroundMovement::enableRot(Vec2f* normal) {
    float angle = atan2f(normal->m_y, normal->m_x);
    m_rotationEnabled = true;
    m_prevGroundAngle = angle;
    m_rotation = 0.0f;
}

// Ground transport is supplied by the surface, rather than reflected or accelerated.
void soKineticEnergyGroundMovement::reflectAccel(Vec3f*) { }
void soKineticEnergyGroundMovement::reflectSpeed(Vec3f*) { }
void soKineticEnergyGroundMovement::mulAccel(Vec3f*) { }
void soKineticEnergyGroundMovement::mulSpeed(Vec3f*) { }
void soKineticEnergyGroundMovement::clearRotSpeed() { }
void soKineticEnergyGroundMovement::clearSpeed() {
    m_speed.m_x = 0.0f;
    m_speed.m_y = 0.0f;
    m_rotation = 0.0f;
}
void soKineticEnergyGroundMovement::resetEnergy(int, Vec2f*, Vec3f*, soModuleAccesser*) { }
Vec3f soKineticEnergyGroundMovement::getRotation() { return Vec3f(m_rotation, 0.0f, 0.0f); }
Vec2f soKineticEnergyGroundMovement::getSpeed() { return m_speed; }
soKineticEnergyGroundMovement::~soKineticEnergyGroundMovement() { }
