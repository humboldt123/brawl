#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
// Use the native Fighter observer override before transitive Fighter includes;
// this preserves the verified primary vtable slot of getOwner.
#define FT_MODULE_BUILDER
#include <ft/ft_owner.h>
#include <ft/fighter.h>
#undef FT_MODULE_BUILDER
#include <ft/ft_log_transactor.h>
#include <ft/ft_status_uniq_process_item_shoot.h>
#include <ft/ft_status_uniq_process_shoot.h>
#include <ft/ft_util.h>
#include <math.h>
#include <nw4r/math/math_arithmetic.h>
#include <so/controller/so_controller_impl.h>
#include <so/controller/so_controller_module_impl.h>
#include <so/effect/so_effect_module_impl.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/item/so_item_manage_module_impl.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/situation/so_situation_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/stageobject.h>
#include <so/status/so_status_module_impl.h>
#include <so/visibility/so_visibility_module_simple.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

ftStatusUniqProcessItemShoot g_ftStatusUniqProcessItemShoot;

void ftStatusUniqProcessItemShoot::initStatus(soModuleAccesser* moduleAccesser) {
    int itemKind = moduleAccesser->getItemManageModule().getHaveItemKind(0);
    moduleAccesser->getWorkManageModule().setInt(itemKind, 0x20000000);
    moduleAccesser->getWorkManageModule().setInt(0xA4, 0x20000001);
    moduleAccesser->getWorkManageModule().setInt(0xA5, 0x20000002);
    moduleAccesser->getWorkManageModule().setInt(0xA7, 0x20000003);
    moduleAccesser->getWorkManageModule().setInt(0xA6, 0x20000004);
    moduleAccesser->getWorkManageModule().setInt(0xA8, 0x20000005);
    moduleAccesser->getWorkManageModule().setInt(0xA9, 0x20000006);
    moduleAccesser->getWorkManageModule().setInt(0xAA, 0x20000007);
    moduleAccesser->getWorkManageModule().setInt(0xAD, 0x20000008);
    moduleAccesser->getWorkManageModule().setInt(0xAC, 0x20000009);
    moduleAccesser->getWorkManageModule().setInt(0xAB, 0x2000000A);
    moduleAccesser->getWorkManageModule().setInt(0xAE, 0x2000000B);
    switch (itemKind) {
    case 0x2C:
        initStatus_LGun(moduleAccesser);
        break;
    case 0x3D:
        initStatus_SScope(moduleAccesser);
        break;
    case 0x18:
        initStatus_FFlower(moduleAccesser);
        break;
    case 0xD:
        initStatus_CLauncher(moduleAccesser);
        break;
    }
}

void ftStatusUniqProcessItemShoot::detachHoldAnim(soModuleAccesser* moduleAccesser) {
    dynamic_cast<Fighter&>(*moduleAccesser->m_stageObject).detachItemHoldAnim(true);
}

void ftStatusUniqProcessItemShoot::attachHoldAnim(soModuleAccesser* moduleAccesser, int holdKind) {
    dynamic_cast<Fighter&>(*moduleAccesser->m_stageObject).attachItemHoldAnim(holdKind, 0);
}

void ftStatusUniqProcessItemShoot::execStatus(soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getWorkManageModule().getInt(0x20000000)) {
    case 0x2C:
        execStatus_LGun(moduleAccesser);
        break;
    case 0x3D:
        execStatus_SScope(moduleAccesser);
        break;
    case 0x18:
        execStatus_FFlower(moduleAccesser);
        break;
    case 0xD:
        execStatus_CLauncher(moduleAccesser);
        break;
    }
}

void ftStatusUniqProcessItemShoot::execFixPosCounter(soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getWorkManageModule().getInt(0x20000000)) {
    case 0x2C:
        execFixPosCounter_LGun(moduleAccesser);
        break;
    case 0x3D:
        execFixPosCounter_SScope(moduleAccesser);
        break;
    case 0x18:
        execFixPosCounter_FFlower(moduleAccesser);
        break;
    case 0xD:
        execFixPosCounter_CLauncher(moduleAccesser);
        break;
    }
}

void ftStatusUniqProcessItemShoot::execFixPos(soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getWorkManageModule().getInt(0x20000000)) {
    case 0x2C:
        execFixPos_LGun(moduleAccesser);
        break;
    case 0x3D:
        execFixPos_SScope(moduleAccesser);
        break;
    case 0x18:
        execFixPos_FFlower(moduleAccesser);
        break;
    case 0xD:
        execFixPos_CLauncher(moduleAccesser);
        break;
    }
}

void ftStatusUniqProcessItemShoot::exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind) {
    switch (moduleAccesser->getWorkManageModule().getInt(0x20000000)) {
    case 0x2C:
        exit_LGun(moduleAccesser, nextStatusKind);
        break;
    case 0x3D:
        exit_SScope(moduleAccesser, nextStatusKind);
        break;
    case 0x18:
        exit_FFlower(moduleAccesser, nextStatusKind);
        break;
    case 0xD:
        exit_CLauncher(moduleAccesser, nextStatusKind);
        break;
    }
}
