#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftMetaknightStatusUniqProcessSpecialNSpin : public soStatusUniqProcess {
public:
    virtual ~ftMetaknightStatusUniqProcessSpecialNSpin() { }
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

ftMetaknightStatusUniqProcessSpecialNSpin g_ftMetaknightStatusUniqProcessSpecialNSpin;

// Mach Tornado spin. While the spin timer (work 0x20000002) runs, the motion rate decays each frame; a special press
// is remembered (flag 0x22000010) and, once enough frames passed (work 0x20000003 >= constant 0x5dc0), it lifts the
// spin: on the ground it first leaves it (air kinetic 0x65, air situation, air ground correction), then the rate gets a
// boost. The rate stays within [0, constant 0xfad]. After the timer the rate only decays.
void ftMetaknightStatusUniqProcessSpecialNSpin::execStatus(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().getInt(0x20000002) >= 0) {
        moduleAccesser->getWorkManageModule().decInt(0x20000002);
        float rate = moduleAccesser->getMotionModule().getRate() - moduleAccesser->getConstantFloatKirby(0xfae);
        soControllerModule& controller = moduleAccesser->getControllerModule();
        u32 specialMask = soController::getButtonMask(soController::Pad_Button_Special);
        int trigger = controller.getTrigger();
        if (trigger & specialMask) {
            moduleAccesser->getWorkManageModule().onFlag(0x22000010);
        }
        soWorkManageModule& work = moduleAccesser->getWorkManageModule();
        int pressFrames = moduleAccesser->getConstantIntKirby(0x5dc0);
        if (work.getInt(0x20000003) >= pressFrames && moduleAccesser->getWorkManageModule().isFlag(0x22000010)) {
            if (moduleAccesser->getSituationModule().getKind() == Situation_Ground) {
                moduleAccesser->getKineticModule().changeKinetic(0x65, moduleAccesser);
                moduleAccesser->getSituationModule().setKind(Situation_Air, false);
                moduleAccesser->getGroundModule().setCorrect(soGroundShapeImpl::Correct_Air, 0);
            }
            rate += moduleAccesser->getConstantFloatKirby(0xfac);
        }
        if (rate < 0.0f) {
            rate = 0.0f;
        } else if (rate > moduleAccesser->getConstantFloatKirby(0xfad)) {
            rate = moduleAccesser->getConstantFloatKirby(0xfad);
        }
        moduleAccesser->getMotionModule().setRate(rate);
    } else {
        float rate = moduleAccesser->getMotionModule().getRate() - moduleAccesser->getConstantFloatKirby(0xfaf);
        if (rate < 0.0f) {
            rate = 0.0f;
        }
        moduleAccesser->getMotionModule().setRate(rate);
    }
}
