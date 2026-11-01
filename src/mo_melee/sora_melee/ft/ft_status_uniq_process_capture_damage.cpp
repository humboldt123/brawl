#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_kinetic_energy.h>
#include <ft/ft_status_uniq_process_capture_wait.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftStatusUniqProcessCaptureDamage : public ftStatusUniqProcessCaptureWait {
public:
    virtual ~ftStatusUniqProcessCaptureDamage() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

ftStatusUniqProcessCaptureDamage g_ftStatusUniqProcessCaptureDamage;

void ftStatusUniqProcessCaptureDamage::initStatus(soModuleAccesser* moduleAccesser) {
    ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*moduleAccesser->getKineticModule().getEnergy(3));
    stop.enable();
    int kind = moduleAccesser->getWorkManageModule().getInt(0x20000000);
    // Damage-state ids 0x7e..0x80 fold to 0x80, 0x81..0x83 fold to 0x83.
    switch (kind) {
    case 0x7E:
    case 0x7F:
    case 0x80:
        kind = 0x80;
        break;
    case 0x81:
    case 0x82:
    case 0x83:
        kind = 0x83;
        break;
    }
    moduleAccesser->getWorkManageModule().setInt(kind, 0x20000000);
    moduleAccesser->getGroundModule().setCorrectIgnoreSlope(1, 0);
}

void ftStatusUniqProcessCaptureDamage::execStatus(soModuleAccesser* moduleAccesser) {
    execStatusCommon(moduleAccesser, 0x80, 0x83, 0x20000000);
}
