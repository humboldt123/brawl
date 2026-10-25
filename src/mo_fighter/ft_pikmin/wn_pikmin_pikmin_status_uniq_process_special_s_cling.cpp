#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <math.h>
#include <mt/mt_matrix.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/link/so_link_module_impl.h>
#include <so/model/so_model_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/so_module_accesser.h>
#include <so/so_rot_utility.h>
#include <so/stageobject.h>
#include <so/status/so_status_module_impl.h>
#include <so/visibility/so_visibility_module_simple.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

extern "C" void fn_8003E3BC(const Matrix*, const Vec3f*, Vec3f*);

// HYPOTHESIS: the Pikmin article that clings to a fighter (Special S, the thrown Pikmin that holds on to the victim): the exit
// takes the model constraint off, faces the Pikmin the way the node it hung on pointed and links it back to its owner (the
// task id kept in the work int 0x10000000).
class wnPikminPikminStatusUniqProcessSpecialSCling : public soStatusUniqProcess {
public:
    virtual ~wnPikminPikminStatusUniqProcessSpecialSCling() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

wnPikminPikminStatusUniqProcessSpecialSCling g_wnPikminPikminStatusUniqProcessSpecialSCling;

void wnPikminPikminStatusUniqProcessSpecialSCling::initStatus(soModuleAccesser* moduleAccesser) {
    moduleAccesser->getLinkModule().setAttribute(3, static_cast<soLinkConnection::Attribute>(3), false);
}

void wnPikminPikminStatusUniqProcessSpecialSCling::exitStatus(soModuleAccesser* moduleAccesser, int) {
    moduleAccesser->getWorkManageModule().onFlag(0x22000000);
    if (moduleAccesser->getLinkModule().isModelConstraint() == true) {
        Matrix mtx(true);
        moduleAccesser->getModelModule().getNodeGlobalMtx(4, &mtx, false);
        Vec3f pos(0.0f, 0.0f, 0.0f);
        float z = mtx.m[2][3];
        float y = mtx.m[1][3];
        float x = mtx.m[0][3];
        pos.m_x = x;
        pos.m_y = y;
        pos.m_z = z;
        moduleAccesser->getLinkModule().removeModelConstraint(true);
        pos.m_z = moduleAccesser->getGroundModule().getZ();
        moduleAccesser->getPostureModule().setPos(&pos);
        Vec3f dir(0.0f, 0.0f, 1.0f);
        fn_8003E3BC(&mtx, &dir, &dir);
        float dirX = dir.m_x;
        float dirZ = dir.m_z;
        float angle = soRotUtility::clampRad((float)atan2(dirZ, dirX));
        if (angle >= 0.0f && angle < 1.5707964f) {
            moduleAccesser->getPostureModule().setLr(1.0f);
        } else {
            moduleAccesser->getPostureModule().setLr(-1.0f);
        }
        moduleAccesser->getPostureModule().updateRotYLr();
        moduleAccesser->getStageObject().updateNodeSRT();
    }
    if (moduleAccesser->getLinkModule().isLink(3) == true) {
        moduleAccesser->getLinkModule().unlink(3);
    }
    moduleAccesser->getLinkModule().link(3, moduleAccesser->getWorkManageModule().getInt(0x10000000));
    moduleAccesser->getVisibilityModule().setWhole(1);
}
