#include <ft/ft_common_data_accesser.h>
#include <ft/ft_entry.h>
#include <ft/ft_kinetic_energy.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Common data record for Zelda; only the scale table at +0x84 is used here.
struct ftZeldaSpecialHiData {
    u8 unk0[0x84];
    float* m_scale;
};

class ftZeldaStatusUniqProcessSpecialHi : public soStatusUniqProcess {
public:
    virtual ~ftZeldaStatusUniqProcessSpecialHi() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
};

// Grounded start clears all speed; an airborne start converts the current
// speed through the per-axis scale table into the stop and gravity energies.
void ftZeldaStatusUniqProcessSpecialHi::initStatus(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getSituationModule().getKind() == 0) {
        moduleAccesser->getKineticModule().clearSpeedAll();
        return;
    }
    float* scale = reinterpret_cast<ftZeldaSpecialHiData*>(g_ftCommonDataAccesser.getData(Fighter_Zelda))->m_scale;
    ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*moduleAccesser->getKineticModule().getEnergy(3));
    ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
    Vec2f sumSpeed = moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(-1));
    moduleAccesser->getKineticModule().clearSpeedAll();
    sumSpeed.m_x /= scale[0];
    sumSpeed.m_y /= scale[1];
    gravity.m_speedY = sumSpeed.m_y;
    gravity.enable();
    stop.m_speed.m_x = sumSpeed.m_x;
    // HYPOTHESIS: constant from lbl_104_rodata_18 (value unverified, guessed 0.0f)
    stop.m_speed.m_y = 0.0f;
    stop.enable();
}

ftZeldaStatusUniqProcessSpecialHi g_ftZeldaStatusUniqProcessSpecialHi;
