#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_common_data_accesser.h>
#include <ft/ft_kinetic_energy.h>
#include <so/collision/so_collision_shield_module_impl.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/so_kinetic_energy_normal.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

// HYPOTHESIS: extendParam group 4 of the GameWatch data (ftData::extendParam[3]); the divisor sits at +0x0.
struct ftGameWatchSpecialLwGroup {
    float m_0;
};

class ftGamewatchStatusUniqProcessSpecialLw : public soStatusUniqProcess {
public:
    virtual ~ftGamewatchStatusUniqProcessSpecialLw() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

ftGamewatchStatusUniqProcessSpecialLw g_ftGamewatchStatusUniqProcessSpecialLw;

void ftGamewatchStatusUniqProcessSpecialLw::initStatus(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getSituationModule().getKind() == 2) {
        if (moduleAccesser->getStatusModule().getPrevStatusKind(0) != 0x11A) {
            ftGameWatchSpecialLwGroup* group = static_cast<ftGameWatchSpecialLwGroup*>(
                g_ftCommonDataAccesser.getData(Fighter_GameWatch)->extendParam[3]);
            ftKineticEnergyStop& energy = dynamic_cast<ftKineticEnergyStop&>(
                *moduleAccesser->getKineticModule().getEnergy(3));
            Vec2f speed = moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1));
            speed.m_x = speed.m_x / group->m_0;
            energy.m_speed = speed;
        }
    }
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000013)) {
        moduleAccesser->getCollisionAbsorberModule().setStatus(0, 1, 0);
    }
}

void ftGamewatchStatusUniqProcessSpecialLw::execStatus(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000012)) {
        if (!moduleAccesser->getWorkManageModule().isFlag(0x22000013)) {
            moduleAccesser->getCollisionAbsorberModule().setStatus(0, 1, 0);
            moduleAccesser->getWorkManageModule().onFlag(0x22000013);
        }
        moduleAccesser->getWorkManageModule().offFlag(0x22000012);
    }
}
