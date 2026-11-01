#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_status_uniq_process_views.h>
#include <so/controller/so_controller_impl.h>
#include <so/controller/so_controller_module_impl.h>
#include <so/damage/so_damage_module_impl.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/link/so_link_module_impl.h>
#include <so/motion/so_motion_change_param.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

// Bitten-by-Wario (Wario's side special, victim side) main hold status. Class shape from the module map.
class ftStatusUniqProcessBittenWario : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessBittenWario() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void*);
};

ftStatusUniqProcessBittenWario g_ftStatusUniqProcessBittenWario;

// HYPOTHESIS: the clatter (button mash) time grows with the damage taken (constants 0xd2f..0xd31); the bite then starts the
// biter-side motion pair 0x18b / 0x18e (or -1 when the scale of the link does not match).
void ftStatusUniqProcessBittenWario::initStatus(soModuleAccesser* moduleAccesser) {
    float base = soValueAccesser::getConstantFloat(moduleAccesser, 0xd2f, 0);
    float damage = moduleAccesser->m_enumerationStart->m_damageModule->getDamage(0);
    damage = damage * soValueAccesser::getConstantFloat(moduleAccesser, 0xd30, 0);
    ftControllerClatterView* controller = reinterpret_cast<ftControllerClatterView*>(moduleAccesser->m_enumerationStart->m_controllerModule);
    float p3 = soValueAccesser::getConstantFloat(moduleAccesser, 0xd31, 0);
    controller->startClatter(base + damage, 1.0f, p3, 0, -1, 0, false);
    soWorkManageModule* work = moduleAccesser->m_enumerationStart->m_workManageModule;
    soMotionModule* motion = moduleAccesser->m_enumerationStart->m_motionModule;
    ftLinkModuleSlotView* linkNode = reinterpret_cast<ftLinkModuleSlotView*>(moduleAccesser->m_enumerationStart->m_linkModule);
    float scale = moduleAccesser->m_enumerationStart->m_postureModule->getScale();
    float nodeScale = linkNode->slot8c(0);
    if (nodeScale == scale) {
        work->setInt(0x18b, 0x20000002);
        work->setInt(0x18e, 0x20000003);
        int kindOffset = 0;
        switch (soValueAccesser::getConstantInt(moduleAccesser, 0x5a98, 0)) {
        case 1:
            kindOffset = 6;
            break;
        case 2:
            kindOffset = 0xc;
            break;
        }
        motion->setKindOffset(kindOffset);
    } else {
        work->setInt(-1, 0x20000002);
        work->setInt(-1, 0x20000003);
    }
    motion->setShareAnimObjKind(0x1cb);
    int side = reinterpret_cast<ftLinkModuleSlotView*>(moduleAccesser->m_enumerationStart->m_linkModule)->slotA4(0);
    int motionKind;
    if (side == 0) {
        motionKind = work->getInt(0x20000002);
    } else {
        motionKind = work->getInt(0x20000003);
    }
    if (motionKind != -1) {
        soMotionChangeParam param;
        param.m_kind = motionKind;
        param.m_frame = 0.0f;
        param.m_rate = 1.0f;
        param._12 = 0;
        param._13 = 1;
        param._14 = 0;
        param._15 = 0;
        motion->changeMotionRequest(&param);
    }
    work->setInt(side, 0x20000000);
}

// HYPOTHESIS: stick flicks below the threshold or a shield/attack press shake the bitten fighter free: it resets the counter
// (work int 0x20000001) and speeds up the motion (constants 0xd32/0xd33); the counter then runs the motion rate to zero.
void ftStatusUniqProcessBittenWario::execStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule* work = moduleAccesser->m_enumerationStart->m_workManageModule;
    soMotionModule* motion = moduleAccesser->m_enumerationStart->m_motionModule;
    soControllerModule* controller = moduleAccesser->m_enumerationStart->m_controllerModule;
    int flickLimit = soValueAccesser::getConstantInt(moduleAccesser, 0x5a52, 0);
    soValueAccesser::getConstantFloat(moduleAccesser, 0xd34, 0);
    bool flicked = false;
    if ((u8)controller->getFlickX() < flickLimit) {
        flicked = true;
    }
    if ((u8)controller->getFlickY() < flickLimit) {
        flicked = true;
    }
    bool pressed = false;
    if ((controller->getTrigger() & soController::getButtonMask((soController::PadButton)1)) ||
        (controller->getTrigger() & soController::getButtonMask((soController::PadButton)0))) {
        pressed = true;
    }
    int frames = work->getInt(0x20000001) - 1;
    if (flicked || pressed) {
        frames = (int)soValueAccesser::getConstantFloat(moduleAccesser, 0xd33, 0);
        float rate = soValueAccesser::getConstantFloat(moduleAccesser, 0xd32, 0);
        motion->setRate(rate);
    }
    if (frames <= 0) {
        motion->setRate(1.0f);
    }
    work->setInt(frames, 0x20000001);
}

// HYPOTHESIS: keeps the bitten fighter's motion in step with the side of the biter's link (like the Start status).
void ftStatusUniqProcessBittenWario::execFixPosCounter(soModuleAccesser* moduleAccesser) {
    soWorkManageModule* work = moduleAccesser->m_enumerationStart->m_workManageModule;
    soMotionModule* motion = moduleAccesser->m_enumerationStart->m_motionModule;
    int phase = work->getInt(0x20000001);
    int side = reinterpret_cast<ftLinkModuleSlotView*>(moduleAccesser->m_enumerationStart->m_linkModule)->slotA4(0);
    if (side != work->getInt(0x20000000)) {
        int kind;
        if (side == 0) {
            kind = work->getInt(0x20000002);
        } else {
            kind = work->getInt(0x20000003);
        }
        if (kind != -1) {
            float frame = motion->getFrame();
            soMotionChangeParam param;
            param.m_kind = kind;
            param.m_frame = frame;
            param.m_rate = 1.0f;
            param._12 = 0;
            param._13 = 1;
            param._14 = 1;
            param._15 = 0;
            motion->changeMotionRequest(&param);
        }
        if (phase > 0) {
            float rate = soValueAccesser::getConstantFloat(moduleAccesser, 0xd32, 0);
            motion->setRate(rate);
        }
        work->setInt(side, 0x20000000);
    }
}

void ftStatusUniqProcessBittenWario::execFixPos(soModuleAccesser* moduleAccesser) {
    reinterpret_cast<ftCaptureModuleSlotView*>(moduleAccesser->m_enumerationStart->m_captureModule)->slot44();
}

// HYPOTHESIS: statuses other than 0xce drop the bitten fighter at the biter's centre (same shape as the Start status).
void ftStatusUniqProcessBittenWario::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    if (nextStatus != 0xce) {
        soLinkModule* link = moduleAccesser->m_enumerationStart->m_linkModule;
        if (link->isLink(0)) {
            Vec2f center = link->getParentShapeCenterPos(0);
            reinterpret_cast<ftCaptureModuleSlotView*>(moduleAccesser->m_enumerationStart->m_captureModule)->slot4c(0);
            moduleAccesser->getGroundModule().setShapeSafePos(&center, 0);
        }
    }
}

bool ftStatusUniqProcessBittenWario::checkDamage(soModuleAccesser* moduleAccesser, void*) {
    if (!moduleAccesser->m_enumerationStart->m_damageModule->isCaptureCut()) {
        ftLinkModuleSlotView* link = reinterpret_cast<ftLinkModuleSlotView*>(moduleAccesser->m_enumerationStart->m_linkModule);
        if (link->slotC8(0) == 1) {
            if (!link->slotCC(0)) {
                return true;
            }
        } else {
            return true;
        }
    }
    return false;
}
