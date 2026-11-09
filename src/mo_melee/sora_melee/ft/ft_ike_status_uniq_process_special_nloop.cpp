#include <so/so_module_accesser.h>
#include <so/stageobject.h>
#include <so/sound/so_sound_module_impl.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftIkeStatusUniqProcessSpecialNLoop : public soStatusUniqProcess {
public:
    virtual ~ftIkeStatusUniqProcessSpecialNLoop() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

ftIkeStatusUniqProcessSpecialNLoop g_ftIkeStatusUniqProcessSpecialNLoop;

// Sound ids per charge step: Ike's own, then Kirby's copy (sub kind 5). HYPOTHESIS: these are the charge sound effects.
static s32 s_chargeSoundIds[7] = { 0x1712, 0x1712, 0x1713, 0x1713, 0x1714, 0x1715, 0x1716 };
static s32 s_kirbyChargeSoundIds[7] = { 0xbe0, 0xbe0, 0xbe1, 0xbe1, 0xbe2, 0xbe3, 0xbe4 };

void ftIkeStatusUniqProcessSpecialNLoop::initStatus(soModuleAccesser* acc) {
    // HYPOTHESIS: 0x293 / 0x1cf are the motion end frames of the charge animation for Kirby's copy (sub kind 5) and Ike.
    int endFrame;
    if (acc->getStageObject().soGetSubKind() == 5) {
        endFrame = (int)acc->getMotionModule().getEndFrame(0x293);
    } else {
        endFrame = (int)acc->getMotionModule().getEndFrame(0x1cf);
    }
    // HYPOTHESIS: charge power per end frame is a constant (0x5dc0); the work ids are the charge state.
    acc->getWorkManageModule().setInt(endFrame * acc->getConstantIntKirby(0x5dc0), 0x20000001);
    int handle = acc->getSoundModule().playSE(static_cast<SndID>(0x1712), false, false, 0);
    acc->getWorkManageModule().setInt(handle, 0x20000004);
    acc->getWorkManageModule().setInt(1, 0x20000003);
}



void ftIkeStatusUniqProcessSpecialNLoop::execStatus(soModuleAccesser* acc) {
    int charge = acc->getWorkManageModule().getInt(0x20000000);
    int step = acc->getWorkManageModule().getInt(0x20000003);
    int kind = acc->getStageObject().soGetSubKind();
    int endFrame;
    if (kind == 5) {
        endFrame = (int)acc->getMotionModule().getEndFrame(0x293);
    } else {
        endFrame = (int)acc->getMotionModule().getEndFrame(0x1cf);
    }
    if (charge == endFrame * step) {
        int handle = acc->getWorkManageModule().getInt(0x20000004);
        if (handle >= 0) {
            acc->getSoundModule().stopSEHandle(handle, 0);
        }
        int newHandle;
        if (kind == 5) {
            newHandle = acc->getSoundModule().playSE(static_cast<SndID>(s_kirbyChargeSoundIds[step]), false, false, 0);
        } else {
            newHandle = acc->getSoundModule().playSE(static_cast<SndID>(s_chargeSoundIds[step]), false, false, 0);
        }
        acc->getWorkManageModule().setInt(newHandle, 0x20000004);
        if (step < acc->getConstantIntKirby(0x5dc0) - 1) {
            acc->getWorkManageModule().addInt(1, 0x20000003);
        }
    }
}

void ftIkeStatusUniqProcessSpecialNLoop::exitStatus(soModuleAccesser* acc, int) {
    int handle = acc->getWorkManageModule().getInt(0x20000004);
    if (handle >= 0) {
        acc->getSoundModule().stopSEHandle(handle, 0);
    }
}
