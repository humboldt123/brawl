#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_status_uniq_process_gimmick.h>
#include <so/so_module_accesser.h>
#include <so/visibility/so_visibility_module_simple.h>
#include <types.h>

void ftStatusUniqProcessGimmickBarrel::exitStatus(soModuleAccesser* moduleAccesser, int next) {
    ftStatusUniqProcessGimmick::exitStatus(moduleAccesser, next);
    moduleAccesser->getVisibilityModule().setWhole(true);
}
