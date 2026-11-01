#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_common_data_accesser.h>
#include <ft/ft_kinetic_energy.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/so_value_accesser.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

// Sheik special-hi parameters live in extendParam[2]. The scale at +0x28
// is applied when the move resolves to its ground or air ending.
class ftSheikStatusUniqProcessSpecialHiEnd : public soStatusUniqProcess {
public:
    virtual ~ftSheikStatusUniqProcessSpecialHiEnd() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
};

void ftSheikStatusUniqProcessSpecialHiEnd::initStatus(soModuleAccesser* moduleAccesser) {
    soSituationModule& situation = moduleAccesser->getSituationModule();
    soKineticModule& kinetic = moduleAccesser->getKineticModule();
    ftData* sheikData = g_ftCommonDataAccesser.getData(Fighter_Sheik);
    const float* p = static_cast<const float*>(sheikData->extendParam[2]);
    ftKineticEnergyGravity& gravity =
        dynamic_cast<ftKineticEnergyGravity&>(*kinetic.getEnergy(1));
    ftKineticEnergyStop& stop =
        dynamic_cast<ftKineticEnergyStop&>(*kinetic.getEnergy(3));
    Vec2f speed = kinetic.getSumSpeed(soKineticEnergy::AttributeFlag(1));
    if (situation.getKind() == 0) {
        kinetic.changeKinetic(6, moduleAccesser);
        speed.m_x *= p[10];
        speed.m_y = 0.0f;
        stop.m_speed = speed;
    } else {
        kinetic.changeKinetic(10, moduleAccesser);
        speed.m_x *= p[10];
        speed.m_y *= p[10];
        gravity.m_speedY = speed.m_y;
        speed.m_y = 0.0f;
        stop.m_speed = speed;
    }
}

void ftSheikStatusUniqProcessSpecialHiEnd::execFixPosCounter(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    soSituationModule& situation = moduleAccesser->getSituationModule();
    soKineticModule& kinetic = moduleAccesser->getKineticModule();
    ftData* sheikData = g_ftCommonDataAccesser.getData(Fighter_Sheik);
    const float* p = static_cast<const float*>(sheikData->extendParam[2]);

    ftKineticEnergyGravity& gravity =
        dynamic_cast<ftKineticEnergyGravity&>(*kinetic.getEnergy(1));
    ftKineticEnergyStop& stop =
        dynamic_cast<ftKineticEnergyStop&>(*kinetic.getEnergy(3));

    if (situation.getKind() != 2) return;
    Vec2f gravitySpeed;
    Vec2f speed;
    if (work.isFlag(0x22000011)) {
        float horizontalSpeed = soValueAccesser::getConstantFloat(moduleAccesser, 0xbd8, 0);
        speed.m_x = horizontalSpeed * p[8];
        speed.m_y = 0.0f;
        stop.m_speedLimit = speed;
        gravity.resume();

        return;
    }

    // The target reduces the current gravity speed by one tenth on each counter tick.
    Vec2f::copy(gravitySpeed, gravity.getSpeed());
    gravitySpeed.m_y -= gravitySpeed.m_y / 10.0f;
    gravity.m_speedY = gravitySpeed.m_y;
    gravity.suspend();
}

ftSheikStatusUniqProcessSpecialHiEnd g_ftSheikStatusUniqProcessSpecialHiEnd;
