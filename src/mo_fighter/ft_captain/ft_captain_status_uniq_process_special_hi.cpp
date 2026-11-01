#include <ft/ft_kinetic_energy.h>
#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <types.h>

class ftCaptainStatusUniqProcessSpecialHi : public soStatusUniqProcess {
public:
    virtual ~ftCaptainStatusUniqProcessSpecialHi() { }
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

void ftCaptainStatusUniqProcessSpecialHi::exitStatus(soModuleAccesser* moduleAccesser, int) {
    soKineticEnergyNormal& motion = dynamic_cast<soKineticEnergyNormal&>(*moduleAccesser->getKineticModule().getEnergy(0));
    float limit = soValueAccesser::getConstantFloat(moduleAccesser, 0xbd0, 0);
    Vec2f speed = motion.getSpeed();
    if (speed.m_y < -limit) {
        speed.m_y = -limit;
        motion.m_speed.m_x = speed.m_x;
        motion.m_speed.m_y = speed.m_y;
    }
}

ftCaptainStatusUniqProcessSpecialHi g_ftCaptainStatusUniqProcessSpecialHi;
