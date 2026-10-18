#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
// MATCH-ONLY: the -O2,s fighter flags stop the implicit constructor inlining into the static initializer;
// the Matching neighbours build at the default -O4,p.
#include <ft/ft_kinetic_energy.h>
#include <ft/ft_status_uniq_process_capture_pulled.h>
#include <ft/ft_status_uniq_process_views.h>
#include <so/controller/so_controller_module_impl.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/situation/so_situation_module_impl.h>
#include <so/stageobject.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

// HYPOTHESIS: heights (in fighter scales) above the captured fighter's feet that the pull has to reach; plain data in the
// original (they sit in .data in front of the vtable).
static float sPullHeightHigh = 12.5f;
static float sPullHeightLow = 8.5f;

ftStatusUniqProcessCapturePulled g_ftStatusUniqProcessCapturePulled;

bool ftStatusUniqProcessCapturePulled::checkPullHeight(soModuleAccesser* moduleAccesser, int nearStatus, int farStatus, int* outStatus) {
    float progress = reinterpret_cast<ftCaptureModuleSlotView*>(moduleAccesser->m_enumerationStart->m_captureModule)->slot2c();
    float scale = moduleAccesser->getPostureModule().getScale();
    Vec3f position = moduleAccesser->getPostureModule().getPos();
    float high;
    float low = position.m_y;
    high = low;
    switch (soValueAccesser::getConstantInt(moduleAccesser, 0x5a99, 0)) {
    case 0:
        high = low + sPullHeightHigh * scale;
        low = low + sPullHeightLow * scale;
        break;
    case 1:
        high = low + sPullHeightLow * scale;
        low = high;
        break;
    }
    float margin = soValueAccesser::getConstantFloat(moduleAccesser, 0xcbf, 0);
    if (progress >= low + margin) {
        *outStatus = nearStatus;
    } else {
        *outStatus = farStatus;
    }
    if (progress >= high + margin) {
        return true;
    }
    return false;
}

// Being pulled in: the stop energy is cleared, the fighter faces the grabber, the controller clatters (time grows with the
// grab's capture level) and the near/far capture status is stored in work int 0x20000000.
void ftStatusUniqProcessCapturePulled::initStatus(soModuleAccesser* moduleAccesser) {
    ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*moduleAccesser->getKineticModule().getEnergy(3));
    stop.resetEnergy(0x16, &Vec2f(0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
    stop.m_speedTarget.m_x = -1.0f;
    stop.m_speedTarget.m_y = -1.0f;
    Vec2f target(-1.0f, -1.0f);
    *((u8*)&stop + 5) |= 0x80;
    soPostureModuleSimple* posture = reinterpret_cast<soPostureModuleSimple*>(moduleAccesser->m_enumerationStart->m_postureModule);
    posture->setLr(-reinterpret_cast<ftLinkModuleSlotView*>(moduleAccesser->m_enumerationStart->m_linkModule)->slotA8(0));
    posture->updateRotYLr();
    moduleAccesser->m_stageObject->updateNodeSRT();
    soDamageModule* damageModule = &moduleAccesser->getDamageModule();
    float scale = soValueAccesser::getConstantFloat(moduleAccesser, 0xcc2, 0);
    float damage = damageModule->getDamage(0);
    float clatterTime = damage * scale;
    float base = (float)soValueAccesser::getConstantInt(moduleAccesser, 0x5a2d, 0);
    clatterTime = base + clatterTime;
    soControllerModule* controller = &moduleAccesser->getControllerModule();
    float cCbc = soValueAccesser::getConstantFloat(moduleAccesser, 0xcbc, 0);
    float cCbb = soValueAccesser::getConstantFloat(moduleAccesser, 0xcbb, 0);
    reinterpret_cast<ftControllerClatterView*>(controller)->startClatter(clatterTime, cCbb, cCbc, 0, -1, 0, false);
    int status;
    checkPullHeight(moduleAccesser, 0x7e, 0x81, &status);
    moduleAccesser->getWorkManageModule().setInt(status, 0x20000000);
    int level = reinterpret_cast<ftCaptureModuleSlotView*>(moduleAccesser->m_enumerationStart->m_captureModule)->slot64();
    if (level > 0) {
        switch (soValueAccesser::getConstantInt(moduleAccesser, 0x5a98, 0)) {
        case 1:
            level += 6;
            break;
        case 2:
            level += 12;
            break;
        }
        moduleAccesser->getWorkManageModule().setInt(level, 0x20000002);
    }
    moduleAccesser->getGroundModule().setCorrectIgnoreSlope(true, 0);
}

// Per frame: the stop energy follows the pull offset of the grab; while airborne-free (situation 0) the near/far status is
// re-evaluated and, when the pull is done, the fighter is put on the ground; otherwise the new status's motion is started.
void ftStatusUniqProcessCapturePulled::execStatusCommon(soModuleAccesser* moduleAccesser, int nearStatus, int farStatus, u32 workIndex) {
    Vec2f pull;
    Vec2f::copy(pull, reinterpret_cast<ftCaptureModuleSlotView*>(moduleAccesser->m_enumerationStart->m_captureModule)->slot28(1));
    dynamic_cast<ftKineticEnergyStop&>(*moduleAccesser->getKineticModule().getEnergy(3)).m_speed = pull;
    if (moduleAccesser->getSituationModule().getKind() == 0) {
        int status;
        if (checkPullHeight(moduleAccesser, nearStatus, farStatus, &status) == true) {
            moduleAccesser->getGroundModule().setCorrect(static_cast<soGroundShapeImpl::CorrectKind>(5), 0);
        } else if (status != moduleAccesser->getWorkManageModule().getInt(workIndex)) {
            moduleAccesser->getWorkManageModule().setInt(status, workIndex);
            soMotionChangeParam param;
            param.m_frame = moduleAccesser->getMotionModule().getFrame();
            param.m_kind = status;
            param.m_rate = 1.0f;
            param._12 = 0;
            param._13 = 0;
            param._14 = 1;
            param._15 = 0;
            moduleAccesser->getMotionModule().changeMotionRequest(&param);
        }
    }
}


static inline void execPullCommon(ftStatusUniqProcessCapturePulled* self, soModuleAccesser* moduleAccesser, int nearStatus, int farStatus, u32 workIndex) {
    Vec2f pull;
    Vec2f::copy(pull, reinterpret_cast<ftCaptureModuleSlotView*>(moduleAccesser->m_enumerationStart->m_captureModule)->slot28(1));
    dynamic_cast<ftKineticEnergyStop&>(*moduleAccesser->getKineticModule().getEnergy(3)).m_speed = pull;
    if (moduleAccesser->getSituationModule().getKind() == 0) {
        int status;
        if (self->checkPullHeight(moduleAccesser, nearStatus, farStatus, &status) == true) {
            moduleAccesser->getGroundModule().setCorrect(static_cast<soGroundShapeImpl::CorrectKind>(5), 0);
        } else if (status != moduleAccesser->getWorkManageModule().getInt(workIndex)) {
            moduleAccesser->getWorkManageModule().setInt(status, workIndex);
            soMotionChangeParam param;
            param.m_frame = moduleAccesser->getMotionModule().getFrame();
            param.m_kind = status;
            param.m_rate = 1.0f;
            param._12 = 0;
            param._13 = 0;
            param._14 = 1;
            param._15 = 0;
            moduleAccesser->getMotionModule().changeMotionRequest(&param);
        }
    }
}

void ftStatusUniqProcessCapturePulled::execStatus(soModuleAccesser* moduleAccesser) {
    execPullCommon(this, moduleAccesser, 0x7e, 0x81, 0x20000000);
}

// Leaving the pulled status clears the stop energy (and releases the capture) unless the next status is one of the
// capture family itself.
void ftStatusUniqProcessCapturePulled::exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind) {
    switch (nextStatusKind) {
    case 0x3d:
    case 0x3e:
    case 0x3f:
    case 0x42:
    case 0xd9:
    case 0xda:
    case 0x107:
    case 0x108:
    case 0x109:
        break;
    default:
        {
            ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*moduleAccesser->getKineticModule().getEnergy(3));
            stop.m_speed.m_x = 0.0f;
            stop.m_speed.m_y = 0.0f;
            Vec2f zero;
            zero.m_x = 0.0f;
            zero.m_y = 0.0f;
        }
        reinterpret_cast<ftCaptureModuleSlotView*>(moduleAccesser->m_enumerationStart->m_captureModule)->slot30(0, 0);
        break;
    }
}

bool ftStatusUniqProcessCapturePulled::checkDamage(soModuleAccesser* moduleAccesser, void*) {
    return reinterpret_cast<ftCaptureModuleSlotView*>(moduleAccesser->m_enumerationStart->m_captureModule)->slot34(0x3f);
}
