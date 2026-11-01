#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_kinetic_energy.h>
#include <ft/ness/ft_ness.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

class ftNessStatusUniqProcessSpecialLw : public soStatusUniqProcess {
public:
    virtual ~ftNessStatusUniqProcessSpecialLw() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
};

ftNessStatusUniqProcessSpecialLw g_ftNessStatusUniqProcessSpecialLw;

void ftNessStatusUniqProcessSpecialLw::initStatus(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getSituationModule().getKind() == 2) {
        soKineticModule& kinetic = moduleAccesser->getKineticModule();
        void** extendedParams = static_cast<ftNess*>(moduleAccesser->m_stageObject)->getExtendParam();
        const float* params = static_cast<const float*>(extendedParams[3]);
        soKineticEnergyNormal& stopEnergy = dynamic_cast<soKineticEnergyNormal&>(*kinetic.getEnergy(3));
        Vec2f adjustedSpeed = stopEnergy.getSpeed();
        adjustedSpeed.m_x *= params[3];
        stopEnergy.m_speed = adjustedSpeed;
        kinetic.getEnergy(1)->clearSpeed();
        kinetic.getEnergy(0)->clearSpeed();
    }

    // The hold status treats negative effect/sound handles as empty and replaces them on entry.
    moduleAccesser->getWorkManageModule().setInt(-1, 0x20000002);
    moduleAccesser->getWorkManageModule().setInt(-1, 0x20000003);
}
