#include <ft/ft_common_data_accesser.h>
#include <ft/ft_entry.h>
#include <ft/ft_kinetic_energy.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftZeldaStatusUniqProcessSpecialHi : public soStatusUniqProcess {
public:
    virtual ~ftZeldaStatusUniqProcessSpecialHi() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
};

// Grounded start clears all speed; an airborne start converts the current
// speed through the per-axis scale table into the stop and gravity energies.
void ftZeldaStatusUniqProcessSpecialHi::initStatus(soModuleAccesser* moduleAccesser) {
    soSituationModule& situation = moduleAccesser->getSituationModule();
    soKineticModule& kinetic = moduleAccesser->getKineticModule();
    if (situation.getKind() == 0) {
        kinetic.clearSpeedAll();
        return;
    }
    float* scale = static_cast<float*>(g_ftCommonDataAccesser.getData(Fighter_Zelda)->extendParam[2]);
    ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*kinetic.getEnergy(3));
    ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*kinetic.getEnergy(1));
    Vec2f sumSpeed = kinetic.getSumSpeed(soKineticEnergy::AttributeFlag(-1));
    kinetic.clearSpeedAll();
    sumSpeed.m_x /= scale[0];
    sumSpeed.m_y /= scale[1];
    gravity.m_speedY = sumSpeed.m_y;
    gravity.enable();
    stop.m_speed.m_x = sumSpeed.m_x;
    stop.m_speed.m_y = 0.0f; // lbl_104_rodata_18
    stop.enable();
}

ftZeldaStatusUniqProcessSpecialHi g_ftZeldaStatusUniqProcessSpecialHi;
