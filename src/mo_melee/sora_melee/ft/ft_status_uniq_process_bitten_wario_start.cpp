#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_status_uniq_process_views.h>
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

// Class shapes from the module map; the method bodies are not reconstructed yet.
class ftStatusUniqProcessBittenWarioStart : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessBittenWarioStart() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void*);
};

ftStatusUniqProcessBittenWarioStart g_ftStatusUniqProcessBittenWarioStart;

// HYPOTHESIS: the bitten fighter starts one of two bite motions (0x18a / 0x18d, or 0x29 without a matching scale),
// stores the chosen motion kinds in work ints 0x20000002/0x20000003 and its end frame in work float 0x21000004.
void ftStatusUniqProcessBittenWarioStart::initStatus(soModuleAccesser* moduleAccesser) {
    soPostureModule* posture = moduleAccesser->m_enumerationStart->m_postureModule;
    posture->setLr(-reinterpret_cast<ftLinkModuleSlotView*>(moduleAccesser->m_enumerationStart->m_linkModule)->slotA8(0));
    moduleAccesser->getPostureModule().updateRotYLr();
    soWorkManageModule* work = moduleAccesser->m_enumerationStart->m_workManageModule;
    soMotionModule* motion = moduleAccesser->m_enumerationStart->m_motionModule;
    ftLinkModuleSlotView* linkNode = reinterpret_cast<ftLinkModuleSlotView*>(moduleAccesser->m_enumerationStart->m_linkModule);
    float scale = moduleAccesser->m_enumerationStart->m_postureModule->getScale();
    float nodeScale = linkNode->slot8c(0);
    if (nodeScale == scale) {
        work->setInt(0x18a, 0x20000002);
        work->setInt(0x18d, 0x20000003);
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
        work->setInt(0x29, 0x20000002);
        work->setInt(0x29, 0x20000003);
    }
    motion->setShareAnimObjKind(0x1cb);
    int side = reinterpret_cast<ftLinkModuleSlotView*>(moduleAccesser->m_enumerationStart->m_linkModule)->slotA4(0);
    float endFrame;
    int motionKind;
    if (side == 0) {
        endFrame = motion->getEndFrame(0x18a);
        motionKind = work->getInt(0x20000002);
    } else {
        endFrame = motion->getEndFrame(0x18d);
        motionKind = work->getInt(0x20000003);
    }
    if (motionKind != -1) {
        soMotionChangeParam param;
        param.m_kind = motionKind;
        param.m_frame = 0.0f;
        param.m_rate = 1.0f;
        param._12 = 0;
        param._13 = 0;
        param._14 = 0;
        param._15 = 0;
        motion->changeMotionRequest(&param);
    }
    work->setInt(side, 0x20000000);
    work->setFloat(endFrame, 0x21000004);
    reinterpret_cast<ftCaptureModuleSlotView*>(moduleAccesser->m_enumerationStart->m_captureModule)->slot3c();
}
// HYPOTHESIS: keeps the bitten fighter's motion in step with the side (work int 0x20000000) of the biter's link.
void ftStatusUniqProcessBittenWarioStart::execFixPosCounter(soModuleAccesser* moduleAccesser) {
    ftLinkModuleSlotView* link = reinterpret_cast<ftLinkModuleSlotView*>(moduleAccesser->m_enumerationStart->m_linkModule);
    soWorkManageModule* work = moduleAccesser->m_enumerationStart->m_workManageModule;
    soMotionModule* motion = moduleAccesser->m_enumerationStart->m_motionModule;
    int side = link->slotA4(0);
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
            param._13 = 0;
            param._14 = 1;
            param._15 = 0;
            motion->changeMotionRequest(&param);
        }
        work->setInt(side, 0x20000000);
    }
}

void ftStatusUniqProcessBittenWarioStart::execFixPos(soModuleAccesser* moduleAccesser) {
    reinterpret_cast<ftCaptureModuleSlotView*>(moduleAccesser->m_enumerationStart->m_captureModule)->slot44();
}

// HYPOTHESIS: statuses other than 0xcd drop the bitten fighter at the biter's centre (same shape as the Yoshi capture exit).
void ftStatusUniqProcessBittenWarioStart::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    if (nextStatus != 0xcd) {
        soLinkModule* link = moduleAccesser->m_enumerationStart->m_linkModule;
        if (link->isLink(0)) {
            Vec2f center = link->getParentShapeCenterPos(0);
            reinterpret_cast<ftCaptureModuleSlotView*>(moduleAccesser->m_enumerationStart->m_captureModule)->slot4c(0);
            moduleAccesser->getGroundModule().setShapeSafePos(&center, 0);
        }
    }
}

bool ftStatusUniqProcessBittenWarioStart::checkDamage(soModuleAccesser* moduleAccesser, void*) {
    if (!moduleAccesser->m_enumerationStart->m_damageModule->isCaptureCut()) {
        if (reinterpret_cast<ftLinkModuleSlotView*>(moduleAccesser->m_enumerationStart->m_linkModule)->slotC8(0) == 1) {
            if (!reinterpret_cast<ftLinkModuleSlotView*>(moduleAccesser->m_enumerationStart->m_linkModule)->slotCC(0)) {
                return true;
            }
        } else {
            return true;
        }
    }
    return false;
}
