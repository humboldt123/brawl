#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_common_data_accesser.h>
#include <ft/ft_kinetic_energy.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Sheik's special-hi parameter block is extendParam[2] in ftData.
class ftSheikStatusUniqProcessSpecialHiStart : public soStatusUniqProcess {
public:
    virtual ~ftSheikStatusUniqProcessSpecialHiStart() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
};

void ftSheikStatusUniqProcessSpecialHiStart::initStatus(soModuleAccesser* moduleAccesser) {
    soSituationModule& situation = moduleAccesser->getSituationModule();
    soKineticModule& kinetic = moduleAccesser->getKineticModule();
    if (situation.getKind() == 0) return;

    ftData* sheikData = g_ftCommonDataAccesser.getData(Fighter_Sheik);
    const float* params = static_cast<const float*>(sheikData->extendParam[2]);
    Vec2f speed = kinetic.getSumSpeed(soKineticEnergy::AttributeFlag(-1));
    kinetic.clearSpeedAll();

    ftKineticEnergyGravity& gravity =
        dynamic_cast<ftKineticEnergyGravity&>(*kinetic.getEnergy(1));
    gravity.m_speedY = params[0];
    gravity.enable();

    ftKineticEnergyStop& stop =
        dynamic_cast<ftKineticEnergyStop&>(*kinetic.getEnergy(3));
    stop.m_speed = Vec2f(speed.m_x, 0.0f);
    stop.enable();
}

ftSheikStatusUniqProcessSpecialHiStart g_ftSheikStatusUniqProcessSpecialHiStart;
