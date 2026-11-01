#define MT_VEC3F_SUB_FRESH_REGS
#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_status_uniq_process_air_lasso_reach.h>
#include <math.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/model/so_model_module_impl.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

ftStatusUniqProcessAirLassoReach g_ftStatusUniqProcessAirLassoReach;

// HYPOTHESIS: when the lasso hooks a ledge the fighter turns towards the ledge (its direction is applied to the posture) and
// the ledge offset is stored in work floats 0x21000005/0x21000006.
void ftStatusUniqProcessAirLassoReach::initStatus(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getGroundModule().isStCliff(0)) {
        float cliffDir = moduleAccesser->getGroundModule().getCliffDir(0);
        Vec3f cliffPos = moduleAccesser->getGroundModule().getCliffPos(0);
        cliffPos.m_x = cliffPos.m_x - cliffDir;
        reinterpret_cast<soPostureModuleSimple&>(moduleAccesser->getPostureModule()).setLr(cliffDir);
        moduleAccesser->getPostureModule().updateRotYLr();
        moduleAccesser->getWorkManageModule().setFloat(cliffPos.m_x, 0x21000005);
        moduleAccesser->getWorkManageModule().setFloat(cliffPos.m_y, 0x21000006);
    }
}

// HYPOTHESIS: the hand of the lasso aims at the ledge (or the stored ledge offset); the fighter's lasso pose is turned towards
// it (motion frame from the angle in degrees) and work flag 0x22000014 records whether the line to the ledge is blocked.
void ftStatusUniqProcessAirLassoReach::execFixPos(soModuleAccesser* moduleAccesser) {
    soWorkManageModule* workManage = &moduleAccesser->getWorkManageModule();
    soGroundModule* ground = &moduleAccesser->getGroundModule();
    Vec3f target;
    if (ground->isStCliff(0)) {
        target = ground->getCliffPos(0);
        target.m_x = target.m_x - reinterpret_cast<soPostureModuleSimple&>(moduleAccesser->getPostureModule()).getLr();
    } else {
        target.m_x = workManage->getFloat(0x21000005);
        target.m_y = workManage->getFloat(0x21000006);
        target.m_z = 0.0f;
    }
    if (!workManage->isFlag(0x22000012)) {
        int node = moduleAccesser->getModelModule().getCorrectNodeId(0x133);
        Vec3f nodePos = moduleAccesser->getModelModule().getNodeGlobalPosition(node, false);
        Vec3f diff = target - nodePos;
        float diffY = diff.m_y;
        float angle = atan2(diff.m_x * reinterpret_cast<soPostureModuleSimple&>(moduleAccesser->getPostureModule()).getLr(), diffY);
        float frame = 57.29578f * angle;
        moduleAccesser->getMotionModule().setFrame(frame);
    }
    float groundZ = ground->getZ();
    Vec3f origin(ground->getCenterPos(0), groundZ);
    Vec3f toTarget = target - origin;
    workManage->setFlag(ground->rayCheck(&origin, &toTarget, true), 0x22000014);
    ftStatusUniqProcessAirLassoCommon::execFixPos(moduleAccesser);
}
