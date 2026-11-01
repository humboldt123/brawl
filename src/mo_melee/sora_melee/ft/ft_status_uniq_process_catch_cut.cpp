#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_kinetic_energy.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftStatusUniqProcessCatchCut : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessCatchCut() { }
    virtual void initStatus(soModuleAccesser*);
};

ftStatusUniqProcessCatchCut g_ftStatusUniqProcessCatchCut;

void ftStatusUniqProcessCatchCut::initStatus(soModuleAccesser* moduleAccesser) {
    ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*moduleAccesser->getKineticModule().getEnergy(3));
    Vec2f speed = stop.getSpeed();
    float lr = moduleAccesser->getPostureModule().getLr();
    float negLr = -lr;
    // HYPOTHESIS: param 0xcc5 is the cut-throw horizontal push scale.
    float scale = soValueAccesser::getConstantFloat(moduleAccesser, 0xcc5, 0);
    speed.m_x = speed.m_x + scale * negLr;
    stop.m_speed.m_x = speed.m_x;
    stop.m_speed.m_y = speed.m_y;
}
