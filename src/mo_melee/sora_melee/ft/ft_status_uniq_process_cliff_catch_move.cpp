#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
// Use the native Fighter observer override before transitive Fighter includes (same as the shoot scope unit).
#define FT_MODULE_BUILDER
#include <ft/ft_owner.h>
#include <ft/fighter.h>
#undef FT_MODULE_BUILDER
#include <ft/ft_status_uniq_process_cliff.h>
#include <ft/ft_status_uniq_process_cliff_catch_move.h>
#include <math.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/model/so_model_module_impl.h>
#include <so/motion/so_motion_change_param.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/stageobject.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

ftStatusUniqProcessCliffCatchMove g_ftStatusUniqProcessCliffCatchMove;

// HYPOTHESIS: the original uses its own inline length helper (float FLT_MIN compare and inline fabs); same as the glide statuses.
static inline float cliffCatchMoveLength(float lengthSq) {
    float length;
    if ((float)__fabs(lengthSq) <= 1.17549435e-38f) {
        length = 0.0f;
    } else {
        length = lengthSq * rsqrtf(lengthSq);
    }
    return length;
}

// HYPOTHESIS: the fighter is placed at the hip node of the ledge-catch pose, facing the ledge, with the shape flag 9 set
// around the repositioning, then hooked on the cliff.
void ftStatusUniqProcessCliffCatchMove::initStatus(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getGroundModule().isStCliff(0) == 1) {
        soModelModule* model = &moduleAccesser->getModelModule();
        int node = model->getCorrectNodeId(0x12D);
        Vec3f pos = model->getNodeGlobalPosition(node, false);
        float cliffDir = moduleAccesser->getGroundModule().getCliffDir(0);
        reinterpret_cast<soPostureModuleSimple&>(moduleAccesser->getPostureModule()).setLr(cliffDir);
        moduleAccesser->getGroundModule().setShapeFlag((soGroundShapeImpl::ShapeFlagId)9, true, 0);
        soMotionChangeParam param;
        param.m_kind = 0xD5;
        param.m_frame = 0.0f;
        param.m_rate = 1.0f;
        param._12 = 0;
        param._13 = 0;
        param._14 = 0;
        param._15 = 0;
        moduleAccesser->getMotionModule().changeMotionRequest(&param);
        moduleAccesser->getMotionModule().setRate(0.0f);
        moduleAccesser->getPostureModule().updateRotYLr();
        moduleAccesser->m_stageObject->updateNodeSRT();
        model = &moduleAccesser->getModelModule();
        node = model->getCorrectNodeId(0x12D);
        pos = model->getTopNodeGlobalPosFromNode(node, &pos);
        pos.m_z = moduleAccesser->getGroundModule().getZ();
        moduleAccesser->getPostureModule().setPos(&pos);
        moduleAccesser->getGroundModule().setShapeFlag((soGroundShapeImpl::ShapeFlagId)9, true, 0);
        execFixPos(moduleAccesser);
        moduleAccesser->m_stageObject->updateNodeSRT();
        moduleAccesser->getGroundModule().entryCliff(0);
        dynamic_cast<Fighter&>(*moduleAccesser->m_stageObject).getOwner()->addLogInt(1, 0x1C, 0);
    }
}

void ftStatusUniqProcessCliffCatchMove::execStatus(soModuleAccesser* moduleAccesser) {
    moduleAccesser->getWorkManageModule().decInt(0x20000000);
}

// HYPOTHESIS: while the catch plays, the fighter is tilted (rotation X, in degrees) so the pose lines up with the direction
// from the fighter to the cliff position, using the motion's translation as the reference direction.
void ftStatusUniqProcessCliffCatchMove::execFixPos(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getGroundModule().isStCliff(0) == 1) {
        Vec3f cliffPos = moduleAccesser->getGroundModule().getCliffPos(0);
        Vec3f pos = moduleAccesser->getPostureModule().getPos();
        Vec2f diff;
        diff.m_x = cliffPos.m_x - pos.m_x;
        diff.m_y = cliffPos.m_y - pos.m_y;
        float lengthSq = diff.m_x * diff.m_x + diff.m_y * diff.m_y;
        float length = cliffCatchMoveLength(lengthSq);
        if (length >= 1e-5f) {
            Vec3f trans = moduleAccesser->getMotionModule().getTransNTranslate();
            double wideX = __fabs(diff.m_x);
            double wideZ = __fabs(trans.m_z);
            Vec3f rot(0.0f, 0.0f, 0.0f);
            float absDiffX = wideX;
            float absTransZ = wideZ;
            if (absTransZ < absDiffX) {
                float lr = reinterpret_cast<soPostureModuleSimple&>(moduleAccesser->getPostureModule()).getLr();
                Vec2f aim;
                aim.m_x = -trans.m_z * lr;
                aim.m_y = -trans.m_y;
                float target = atan2(diff.m_y, diff.m_x);
                float current = atan2(aim.m_y, aim.m_x);
                rot.m_x = -lr * (57.29578f * (target - current));
            }
            moduleAccesser->getPostureModule().setRot(&rot, 0);
        }
    }
}

// Leaving towards a status that releases the cliff also leaves it here and starts the cliff-release counter.
void ftStatusUniqProcessCliffCatchMove::exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind) {
    if (ftStatusUniqProcessCliff::isLeaveCliffStatus(nextStatusKind) == true) {
        moduleAccesser->getGroundModule().leaveCliff(0);
        int releaseFrames = soValueAccesser::getConstantInt(moduleAccesser, 0x5A0A, 0);
        moduleAccesser->getWorkManageModule().setInt(releaseFrames, 0x10000002);
    }
    Vec3f rot(0.0f, 0.0f, 0.0f);
    moduleAccesser->getPostureModule().setRot(&rot, 0);
}
