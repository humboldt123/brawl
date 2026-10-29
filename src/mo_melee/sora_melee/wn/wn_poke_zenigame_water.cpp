#include <wn/poke/wn_poke_zenigame_water.h>
#include <wn/wn_class_info_impl.h>
#include <so/so_module_accesser.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/reflect/so_reflect_module.h>
#include <so/status/so_status_module_impl.h>
#include <wn/wn_kinetic_energy_normal.h>
#include <math.h>

// Registers the Pokemon water gun article class (kind 95) in the class info table during static initialization.
wnClassInfoImpl<95, wnPokeZenigameWater, wnPokeZenigameWaterData> g_wnPokeZenigameWaterClassInfo;

// The water model follows the posture and points along its flight direction.
void wnPokeZenigameWater::updateNodeSRT() {
    float value = m_moduleAccesser->getPostureModule().getScale();
    Vec3f scale;
    scale.m_z = value;
    scale.m_y = value;
    scale.m_x = value;
    Vec3f rotation;
    Vec3f position;
    position = m_moduleAccesser->getPostureModule().getPos();
    rotation = m_moduleAccesser->getPostureModule().getRot(0);
    wnKineticEnergyNormal& normal =
        dynamic_cast<wnKineticEnergyNormal&>(*m_moduleAccesser->getKineticModule().getEnergy(0));
    Vec2f speed = normal.getSpeed();
    rotation.m_z = 57.29578f * (float)atan2(speed.m_y, speed.m_x);
    m_moduleAccesser->getModelModule().setNodeSRT(0, &scale, &rotation, &position);
}

// A hit on a target ends the water stream (status 1).
bool wnPokeZenigameWater::notifyEventCollisionAttackCheck(u32 flags) {
    if (flags & 6) {
        m_moduleAccesser->getStatusModule().changeStatusRequest(1, m_moduleAccesser);
        return false;
    }
    return Weapon::notifyEventCollisionAttackCheck(flags);
}

// A reflector turns the water back: the horizontal speed by the reflector's speed factor, the vertical speed flipped.
bool wnPokeZenigameWater::reflect() {
    Vec3f speed;
    speed.m_x = -m_moduleAccesser->getReflectModule().getSpeedMul();
    speed.m_y = -1.0f;
    speed.m_z = 1.0f;
    Vec3f accel;
    accel.m_x = -m_moduleAccesser->getReflectModule().getSpeedMul();
    accel.m_y = 1.0f;
    accel.m_z = 1.0f;
    m_moduleAccesser->getKineticModule().mulSpeed(&speed, soKineticEnergy::AttributeFlag(1));
    m_moduleAccesser->getKineticModule().mulAccel(&accel, soKineticEnergy::AttributeFlag(1));
    return false;
}
