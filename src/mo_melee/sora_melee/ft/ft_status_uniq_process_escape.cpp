#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_kinetic_energy.h>
#include <ft/ft_status_uniq_process_escape.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

ftStatusUniqProcessEscape g_ftStatusUniqProcessEscape;

void ftStatusUniqProcessEscape::initStatus(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getStatusModule().getStatusKind() == 0x21) {
        int prevKind = moduleAccesser->getStatusModule().getPrevStatusKind(0);
        switch (prevKind) {
        case 0x49:
            moduleAccesser->getWorkManageModule().onFlag(0x22000012);
            break;
        case 0x45:
        case 0x46:
        case 0x47:
        case 0x48:
        case 0xc0:
            moduleAccesser->getWorkManageModule().onFlag(0x22000012);
            moduleAccesser->getWorkManageModule().onFlag(0x22000013);
            break;
        }
    } else {
        soKineticEnergyNormal& energy = dynamic_cast<soKineticEnergyNormal&>(*moduleAccesser->getKineticModule().getEnergy(4));
        energy.clearSpeed();
    }
}

void ftStatusUniqProcessEscape::execStatus(soModuleAccesser*) {}

void ftStatusUniqProcessEscape::exitStatus(soModuleAccesser*, int) {}
