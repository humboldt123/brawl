#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
// MATCH-ONLY: the -O2,s fighter flags stop the implicit constructor inlining into the static initializer;
// the Matching neighbours build at the default -O4,p.

#include <ft/ft_kinetic_energy.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftStatusUniqProcessCaptureCut : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessCaptureCut() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
};

ftStatusUniqProcessCaptureCut g_ftStatusUniqProcessCaptureCut;

void ftStatusUniqProcessCaptureCut::initStatus(soModuleAccesser* moduleAccesser) {
    // The energy slot depends on whether the situation is ground (0): ground uses slot 3, air slot 2.
    int energyIndex = moduleAccesser->getSituationModule().getKind() == 0 ? 3 : 2;
    soKineticEnergyNormal& normal = dynamic_cast<soKineticEnergyNormal&>(*moduleAccesser->getKineticModule().getEnergy(energyIndex));
    Vec2f speed = normal.getSpeed();
    float lr = moduleAccesser->getPostureModule().getLr();
    float negLr = -lr;
    // HYPOTHESIS: param 0xcc5 is the cut-throw horizontal push scale (shared with catch cut).
    float scale = soValueAccesser::getConstantFloat(moduleAccesser, 0xcc5, 0);
    speed.m_x = speed.m_x + scale * negLr;
    normal.m_speed.m_x = speed.m_x;
    normal.m_speed.m_y = speed.m_y;
    normal.enable();
}
