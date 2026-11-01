#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_kinetic_energy.h>
#include <so/item/so_item_manage_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/sound/so_sound_module_impl.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftStatusUniqProcessItemThrow : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessItemThrow() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

ftStatusUniqProcessItemThrow g_ftStatusUniqProcessItemThrow;

void ftStatusUniqProcessItemThrow::initStatus(soModuleAccesser*) {}

void ftStatusUniqProcessItemThrow::execFixPos(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000011)) {
        return;
    }
    if (moduleAccesser->getItemManageModule().getHaveItemKind(0) == 0x48) {
        if (moduleAccesser->getMotionModule().getKind() != 0xfe) {
            moduleAccesser->getSoundModule().playSE(static_cast<SndID>(0x2293), false, false, 0);
        }
    }
    moduleAccesser->getWorkManageModule().onFlag(0x22000011);
}

void ftStatusUniqProcessItemThrow::exitStatus(soModuleAccesser* moduleAccesser, int) {
    if (!moduleAccesser->getItemManageModule().isHaveItem(0)) {
        return;
    }
    if (moduleAccesser->getItemManageModule().getHaveItemSize(0) == 1 ||
        moduleAccesser->getItemManageModule().getHaveItemKind(0) == 0xd) {
        // HYPOTHESIS: the drop arguments are lbl_27_rodata_2370 (90.0f) and lbl_27_rodata_2374 (0.0f).
        moduleAccesser->getItemManageModule().dropItem(90.0f, 0.0f, 0);
    }
}
