#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_common_data_accesser.h>
#include <ft/ft_entry.h>
#include <ft/ft_kinetic_energy.h>
#include <so/kinetic/so_kinetic_energy.h>
#include <so/so_kinetic_energy_normal.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/visibility/so_visibility_module_simple.h>
#include <types.h>

class ftZeldaStatusUniqProcessSpecialHi3 : public soStatusUniqProcess {
public:
    virtual ~ftZeldaStatusUniqProcessSpecialHi3() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
};

void ftZeldaStatusUniqProcessSpecialHi3::initStatus(soModuleAccesser* moduleAccesser) {
    soKineticModule& kinetic = moduleAccesser->getKineticModule();
    soSituationModule& situation = moduleAccesser->getSituationModule();
    // Farore's Wind parameters are stored in ftData::extendParam[2] (+0x84).
    float* params = static_cast<float*>(g_ftCommonDataAccesser.getData(Fighter_Zelda)->extendParam[2]);
    Vec2f sumSpeed = kinetic.getSumSpeed(soKineticEnergy::AttributeFlag(-1));
    kinetic.clearSpeedAll();
    ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*kinetic.getEnergy(3));
    ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*kinetic.getEnergy(1));
    if (situation.getKind() == 0) {
        kinetic.changeKinetic(6, moduleAccesser);
        sumSpeed.m_x = sumSpeed.m_x * params[11];
        sumSpeed.m_y = 0.0f; // lbl_104_rodata_50
        stop.m_speed = sumSpeed;
        stop.enable();
    } else {
        kinetic.changeKinetic(0xb, moduleAccesser);
        sumSpeed.m_x = sumSpeed.m_x * params[11];
        sumSpeed.m_y = sumSpeed.m_y * params[11];
        stop.m_speed = sumSpeed;
        stop.enable();
        gravity.disable();
    }
    moduleAccesser->getVisibilityModule().setWhole(1);
}

void ftZeldaStatusUniqProcessSpecialHi3::execFixPosCounter(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& workManage = moduleAccesser->getWorkManageModule();
    soSituationModule& situation = moduleAccesser->getSituationModule();
    soKineticModule& kinetic = moduleAccesser->getKineticModule();
    if (situation.getKind() != 0) {
        ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*kinetic.getEnergy(3));
        ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*kinetic.getEnergy(1));
        Vec2f sumSpeed = kinetic.getSumSpeed(soKineticEnergy::AttributeFlag(1));
        if (workManage.isFlag(0x22000011)) {
            // HYPOTHESIS: the native limit value is a float in the parameter record at +0x7C.
            float* params = static_cast<float*>(g_ftCommonDataAccesser.getData(Fighter_Zelda)->extendParam[2]);
            ftParam* param = g_ftCommonDataAccesser.getParam(Fighter_Zelda);
            gravity.m_speedY = sumSpeed.m_y;
            gravity.enable();
            float limit = *reinterpret_cast<float*>(reinterpret_cast<u8*>(param) + 0x7C) * params[9];
            if (sumSpeed.m_x < -limit) {
                sumSpeed.m_x = -limit;
            } else if (sumSpeed.m_x > limit) {
                sumSpeed.m_x = limit;
            }
            sumSpeed.m_y = 0.0f; // lbl_104_rodata_50
            stop.m_speed.m_x = sumSpeed.m_x;
            stop.m_speed.m_y = sumSpeed.m_y;
        } else {
            sumSpeed = stop.getSpeed();
            sumSpeed.m_y = sumSpeed.m_y - sumSpeed.m_y / 10.0f;
            stop.m_speed = sumSpeed;
        }
    }
}

ftZeldaStatusUniqProcessSpecialHi3 g_ftZeldaStatusUniqProcessSpecialHi3;
