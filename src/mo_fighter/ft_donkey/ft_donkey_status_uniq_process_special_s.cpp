#include <ft/ft_common_data_accesser.h>
#include <ft/ft_kinetic_energy.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <mt/mt_vector.h>
#include <types.h>

class ftDonkeyStatusUniqProcessSpecialS : public soStatusUniqProcess {
public:
    virtual ~ftDonkeyStatusUniqProcessSpecialS() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

void ftDonkeyStatusUniqProcessSpecialS::initStatus(soModuleAccesser* moduleAccesser) {
    soSituationModule* situation = moduleAccesser->m_enumerationStart->m_situationModule;
    soKineticModule* kinetic = moduleAccesser->m_enumerationStart->m_kineticModule;
    float* param = static_cast<float*>(g_ftCommonDataAccesser.getData(static_cast<ftKind>(1))->extendParam[1]);
    ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*kinetic->getEnergy(1));
    ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*kinetic->getEnergy(3));
    Vec2f sumSpeed = kinetic->getSumSpeed(soKineticEnergy::AttributeFlag(-1));
    if (situation->getKind() == 0) {
        kinetic->changeKinetic(6, moduleAccesser);
    } else {
        kinetic->changeKinetic(0xa, moduleAccesser);
        sumSpeed.m_x /= param[0];
        sumSpeed.m_y = 0.0f;
        stop.m_speed = sumSpeed;
        stop.m_brake = Vec2f(param[1], 0.0f);
        gravity.m_speedY = 0.0f;
        gravity.m_gravity = -param[2];
        gravity.disable();
    }
}

void ftDonkeyStatusUniqProcessSpecialS::execStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule* workManage = moduleAccesser->m_enumerationStart->m_workManageModule;
    soSituationModule* situation = moduleAccesser->m_enumerationStart->m_situationModule;
    soKineticModule* kinetic = moduleAccesser->m_enumerationStart->m_kineticModule;
    float* param = static_cast<float*>(g_ftCommonDataAccesser.getData(static_cast<ftKind>(1))->extendParam[1]);
    ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*kinetic->getEnergy(1));
    ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*kinetic->getEnergy(3));
    if (situation->getKind() == 0) {
        kinetic->changeKinetic(6, moduleAccesser);
    } else {
        kinetic->changeKinetic(0xa, moduleAccesser);
        stop.m_brake = Vec2f(param[1], 0.0f);
        gravity.m_gravity = -param[2];
        if (!workManage->isFlag(0x22000011)) {
            gravity.disable();
        }
    }
}

ftDonkeyStatusUniqProcessSpecialS g_ftDonkeyStatusUniqProcessSpecialS;
