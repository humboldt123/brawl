#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_common_data_accesser.h>
#include <ft/ft_entry.h>
#include <ft/ft_kinetic_energy.h>
#include <so/kinetic/so_kinetic_energy.h>
#include <so/so_kinetic_energy_normal.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftSheikStatusUniqProcessSpecialLw : public soStatusUniqProcess {
public:
    virtual ~ftSheikStatusUniqProcessSpecialLw() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
};

void ftSheikStatusUniqProcessSpecialLw::initStatus(soModuleAccesser* moduleAccesser) {
    // HYPOTHESIS: 0x115 is the status kind this transition is entered from.
    if (moduleAccesser->getStatusModule().getStatusKind() == 0x115) {
        soSituationModule& situation = moduleAccesser->getSituationModule();
        soKineticModule& kinetic = moduleAccesser->getKineticModule();
        // HYPOTHESIS: extendParam[3] of the Sheik data points to two float divisors (x, y).
        float* divisor = static_cast<float*>(g_ftCommonDataAccesser.getData(Fighter_Sheik)->extendParam[3]);
        ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*kinetic.getEnergy(1));
        ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*kinetic.getEnergy(3));
        Vec2f sumSpeed = kinetic.getSumSpeed(soKineticEnergy::AttributeFlag(-1));
        kinetic.clearSpeedAll();
        sumSpeed.m_x = sumSpeed.m_x / divisor[0];
        stop.m_speed = Vec2f(sumSpeed.m_x, 0.0f);
        stop.enable();
        if (situation.getKind() != 0) {
            sumSpeed.m_y = sumSpeed.m_y / divisor[1];
            gravity.m_speedY = sumSpeed.m_y;
            gravity.enable();
        }
    }
}

ftSheikStatusUniqProcessSpecialLw g_ftSheikStatusUniqProcessSpecialLw;
