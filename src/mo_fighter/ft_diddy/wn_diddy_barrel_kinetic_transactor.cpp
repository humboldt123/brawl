#define WN_KINETIC_GENERIC_BUILDER
#define WN_WEAPON_BUILDER
#include <wn/diddy/wn_diddy_barrel_kinetic_transactor.h>
#include <ft/ft_kinetic_energy.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/situation/so_situation_module_impl.h>
#include <so/so_value_accesser.h>
#include <wn/wn_kinetic_energy_gravity.h>
#include <wn/wn_kinetic_transactor.h>
#include <so/so_type_list.h>

extern const float lbl_115_rodata_E8;

// HYPOTHESIS: the transactor reaches an energy of the barrel's instance pools (wnDiddyBarrel's energy type list) through
// an overload selected by an empty soIntToType<pool level> tag; the value-initialized one-byte tag temporary is the zero
// byte stored before each lookup (as in wn_pikmin_pikmin.cpp).
static inline soKineticEnergyNormal* getEnergy(void* pools, const soIntToType<0>&) {
    return reinterpret_cast<soKineticEnergyNormal*>(static_cast<u8*>(pools) + 0x10);
}
static inline wnKineticEnergyGravity* getEnergy(void* pools, const soIntToType<1>&) {
    return reinterpret_cast<wnKineticEnergyGravity*>(static_cast<u8*>(pools) + 0x50);
}
static inline soKineticEnergyWindNormal* getEnergy(void* pools, const soIntToType<4>&) {
    return reinterpret_cast<soKineticEnergyWindNormal*>(static_cast<u8*>(pools) + 0xEC);
}

void wnDiddyBarrelKineticTransactor::addSpeed(
    void*, void*, soModuleAccesser*) {
}

void wnDiddyBarrelKineticTransactor::addSpeedOutside(
    int type, void* speedData, void* pools, soModuleAccesser* moduleAccesser) {
    Vec2f* speed = static_cast<Vec2f*>(speedData);
    soParamAccesser* paramAccesser = static_cast<soParamAccesser*>(
        moduleAccesser->getWorkManageModule().getParamAccesser());

    switch (type) {
    case soKineticEnergy::Outside_Wind: {
        Vec2f delta(lbl_115_rodata_E8, lbl_115_rodata_E8);
        Vec2f brake(lbl_115_rodata_E8, lbl_115_rodata_E8);
        delta.m_x = speed->m_x;
        delta.m_y += speed->m_y;
        brake.m_x = paramAccesser->getParamFloat(moduleAccesser, 0xFA9, 0);
        if (lbl_115_rodata_E8 != delta.m_x || lbl_115_rodata_E8 != delta.m_y) {
            soKineticEnergyWindNormal* wind = getEnergy(pools, soIntToType<4>());
            wind->m_windSpeed.m_x += delta.m_x;
            wind->m_windSpeed.m_y += delta.m_y;
            wind->m_responseRetention = paramAccesser->getParamFloat(moduleAccesser, 0xFA8, 0);
            wind->m_brake = brake;
            wind->enable();
        }
        return;
    }

    case soKineticEnergy::Outside_Attack: {
        Vec2f speedToAdd(lbl_115_rodata_E8, lbl_115_rodata_E8);
        SituationKind situation = moduleAccesser->getSituationModule().getKind();
        switch (situation) {
        case Situation_Ground:
            speedToAdd.m_x = speed->m_x;
            break;
        case Situation_Air:
        case Situation_Water:
            speedToAdd = *speed;
            break;
        }

        if (lbl_115_rodata_E8 != speedToAdd.m_x) {
            soKineticEnergyNormal* normal = getEnergy(pools, soIntToType<0>());
            Vec2f combined(speed->m_x + normal->getSpeed().m_x, lbl_115_rodata_E8);
            normal->m_speed = combined;
            normal->enable();
        }

        if (lbl_115_rodata_E8 != speedToAdd.m_y) {
            wnKineticEnergyGravity* gravity = getEnergy(pools, soIntToType<1>());
            gravity->m_speedY = speedToAdd.m_y + gravity->getSpeed().m_y;
            gravity->enable();
        }
        break;
    }
    case soKineticEnergy::Outside_Movement_Ground:
    default:
        break;
    }
}

void wnDiddyBarrelKineticTransactor::changeKinetic(
    int mode, void* pools, soModuleAccesser* moduleAccesser) {
    if (mode <= 0x1F) {
        wnKineticTransactor::changeKinetic(mode, pools, moduleAccesser);
        return;
    }

    switch (mode) {
    case 0x20: {
        volatile u8 unk9 = 0; // MATCH-ONLY: native initializes this unused local byte.
        moduleAccesser->getKineticModule().unableEnergyAll();
        break;
    }
    case 0x21: {
        wnDiddyBarrelKineticTransactor empty = {};
        empty.changeKineticSub(pools, moduleAccesser);
        break;
    }
    }
}

void wnDiddyBarrelKineticTransactor::changeKineticSub(
    void* pools, soModuleAccesser* moduleAccesser) {
    moduleAccesser->getKineticModule().unableEnergyAll();
    soParamAccesser* paramAccesser = static_cast<soParamAccesser*>(
        moduleAccesser->getWorkManageModule().getParamAccesser());
    soKineticEnergyNormal* normal = getEnergy(pools, soIntToType<0>());
    Vec2f initialSpeed(
        moduleAccesser->getWorkManageModule().getFloat(0x21000004), lbl_115_rodata_E8);
    normal->m_speed = initialSpeed;
    normal->enable();
    wnKineticEnergyGravity* gravity = getEnergy(pools, soIntToType<1>());
    gravity->m_speedY = moduleAccesser->getWorkManageModule().getFloat(0x21000005);
    gravity->m_gravity = -paramAccesser->getParamFloat(moduleAccesser, 0xFA4, 0);
    gravity->m_speedLimit = paramAccesser->getParamFloat(moduleAccesser, 0xFA5, 0);
    gravity->enable();
}
