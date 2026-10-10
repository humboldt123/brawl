#include <so/so_module_accesser.h>
#include <so/kinetic/so_kinetic_energy.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftZeldaStatusUniqProcessSpecialS : public soStatusUniqProcess {
public:
    virtual ~ftZeldaStatusUniqProcessSpecialS() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
};

// Grounded start clears the lift-off speed of the first kinetic energy.
void ftZeldaStatusUniqProcessSpecialS::initStatus(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getSituationModule().getKind() != 0) {
        moduleAccesser->getKineticModule().getEnergy(1)->clearSpeed();
    }
}

ftZeldaStatusUniqProcessSpecialS g_ftZeldaStatusUniqProcessSpecialS;
