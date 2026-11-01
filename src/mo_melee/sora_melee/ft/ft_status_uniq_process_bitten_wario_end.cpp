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

// Bitten-by-Wario release status (victim side).
class ftStatusUniqProcessBittenWarioEnd : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessBittenWarioEnd() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void*);
};

ftStatusUniqProcessBittenWarioEnd g_ftStatusUniqProcessBittenWarioEnd;

// HYPOTHESIS: ends the bite: picks the release motion pair 0x18c / 0x18f (or -1 without a matching scale) and plays the one
// for the current side of the biter's link.
void ftStatusUniqProcessBittenWarioEnd::initStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule* work = moduleAccesser->m_enumerationStart->m_workManageModule;
    soMotionModule* motion = moduleAccesser->m_enumerationStart->m_motionModule;
    ftLinkModuleSlotView* linkNode = reinterpret_cast<ftLinkModuleSlotView*>(moduleAccesser->m_enumerationStart->m_linkModule);
    float scale = moduleAccesser->m_enumerationStart->m_postureModule->getScale();
    float nodeScale = linkNode->slot8c(0);
    if (nodeScale == scale) {
        work->setInt(0x18c, 0x20000002);
        work->setInt(0x18f, 0x20000003);
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
        param._13 = 0;
        param._14 = 0;
        param._15 = 0;
        motion->changeMotionRequest(&param);
    }
    work->setInt(side, 0x20000000);
}

// HYPOTHESIS: keeps the bitten fighter's motion in step with the side (work int 0x20000000) of the biter's link.
void ftStatusUniqProcessBittenWarioEnd::execFixPosCounter(soModuleAccesser* moduleAccesser) {
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

void ftStatusUniqProcessBittenWarioEnd::execFixPos(soModuleAccesser* moduleAccesser) {
    reinterpret_cast<ftCaptureModuleSlotView*>(moduleAccesser->m_enumerationStart->m_captureModule)->slot44();
}

// HYPOTHESIS: leaving the status always drops the bitten fighter at the biter's centre.
void ftStatusUniqProcessBittenWarioEnd::exitStatus(soModuleAccesser* moduleAccesser, int) {
    soLinkModule* link = moduleAccesser->m_enumerationStart->m_linkModule;
    if (link->isLink(0)) {
        Vec2f center = link->getParentShapeCenterPos(0);
        reinterpret_cast<ftCaptureModuleSlotView*>(moduleAccesser->m_enumerationStart->m_captureModule)->slot4c(0);
        moduleAccesser->getGroundModule().setShapeSafePos(&center, 0);
    }
}

bool ftStatusUniqProcessBittenWarioEnd::checkDamage(soModuleAccesser* moduleAccesser, void* damageInfo) {
    return reinterpret_cast<ftCaptureModuleSlotView*>(moduleAccesser->m_enumerationStart->m_captureModule)->slot38(moduleAccesser, damageInfo);
}
