#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#define SO_CATCH_MODULE_CLING_SLOTS
#include <ft/kirby/ft_kirby_copy_ability_id_converter.h>
#include <so/controller/so_controller_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/catch/so_catch_module.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <so/so_value_accesser.h>
#include <types.h>

class ftWarioStatusUniqProcessSpecialNBite : public soStatusUniqProcess {
public:
    virtual ~ftWarioStatusUniqProcessSpecialNBite() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void* damage);
};

ftWarioStatusUniqProcessSpecialNBite g_ftWarioStatusUniqProcessSpecialNBite;

void ftWarioStatusUniqProcessSpecialNBite::initStatus(soModuleAccesser* moduleAccesser) {
    int origKind = g_ftKirbyCopyAbilityIdConverter.convCorrectToOrigId(0, moduleAccesser->getStatusModule().getStatusKind(), moduleAccesser, true);
    if (origKind == 0x119) {
        static_cast<soCatchModule*>(moduleAccesser->m_enumerationStart->m_catchModule)->setCatch(-1);
    }
}

void ftWarioStatusUniqProcessSpecialNBite::execStatus(soModuleAccesser* moduleAccesser) {
    int origKind = g_ftKirbyCopyAbilityIdConverter.convCorrectToOrigId(0, moduleAccesser->getStatusModule().getStatusKind(), moduleAccesser, true);
    if (origKind == 0x119) {
        return;
    }
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    soControllerModule& controller = moduleAccesser->getControllerModule();
    soMotionModule& motion = moduleAccesser->getMotionModule();
    int frames = work.getInt(0x20000004) - 1;
    bool pressed = false;
    if ((controller.getTrigger() & soController::getButtonMask((soController::PadButton)1)) ||
        (controller.getTrigger() & soController::getButtonMask((soController::PadButton)0))) {
        pressed = true;
    }
    if (pressed) {
        frames = (int)moduleAccesser->getConstantFloatKirby(0xfa3);
        motion.setRate(moduleAccesser->getConstantFloatKirby(0xfa2));
    }
    if (frames <= 0) {
        // Return to normal motion speed when the accelerated-bite timer ends.
        motion.setRate(1.0f);
    }
    work.setInt(frames, 0x20000004);
}

void ftWarioStatusUniqProcessSpecialNBite::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    int origKind = g_ftKirbyCopyAbilityIdConverter.convCorrectToOrigId(0, nextStatus, moduleAccesser, true);
    switch (origKind) {
    case 0x3B: case 0x11A: case 0x11B:
        break;
    default:
        static_cast<soCatchModule*>(moduleAccesser->m_enumerationStart->m_catchModule)->catchCut(false, false);
        break;
    }
}

bool ftWarioStatusUniqProcessSpecialNBite::checkDamage(soModuleAccesser* moduleAccesser, void* damage) {
    return static_cast<soCatchModule*>(moduleAccesser->m_enumerationStart->m_catchModule)->checkDamage();
}
