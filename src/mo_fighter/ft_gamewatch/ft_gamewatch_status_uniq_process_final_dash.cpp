#include <ft/ft_kinetic_energy_controller.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/so_kinetic_energy_normal.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

#include <ft/gamewatch/ft_gamewatch_status_uniq_process_final_swim.h>

class ftGameWatchStatusUniqProcessFinalDash : public ftGameWatchStatusUniqProcessFinalSwim {
public:
    virtual ~ftGameWatchStatusUniqProcessFinalDash() { }
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

ftGameWatchStatusUniqProcessFinalDash g_ftGameWatchStatusUniqProcessFinalDash;

void ftGameWatchStatusUniqProcessFinalDash::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    // These four transitions retain the controller energy's speed.
    switch (nextStatus) {
    case 0x120:
    case 0x125:
    case 0x132:
    case 0x137:
        break;
    default: {
        // Outside that sequence, scale the controller kinetic energy's horizontal speed.
        ftKineticEnergyController& energy =
            dynamic_cast<ftKineticEnergyController&>(*moduleAccesser->getKineticModule().getEnergy(2));
        Vec2f speed = energy.getSpeed();
        speed.m_x *= soValueAccesser::getConstantFloat(moduleAccesser, 0xc35, 0);
        energy.m_speed.m_x = speed.m_x;
        energy.m_speed.m_y = speed.m_y;
        break;
    }
    }
    ftGameWatchStatusUniqProcessFinalSwim::exitStatus(moduleAccesser, nextStatus);
}
