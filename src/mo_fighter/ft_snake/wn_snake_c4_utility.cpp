#include <math.h>
#include <mt/mt_matrix.h>
#include <so/so_module_accesser.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/link/so_link_module_impl.h>
#include <so/model/so_model_module_impl.h>
#include <so/posture/so_posture_module_impl.h>
#include <so/stageobject.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>
#include <wn/snake/wn_snake_c4_def.h>
#include <wn/snake/wn_snake_c4_utility.h>

void wnSnakeC4Utility::detachConstraint(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getLinkModule().isModelConstraint()) {
        Matrix mtx(true);
        moduleAccesser->getModelModule().getNodeGlobalMtx(wnSnakeC4Def::LinkNo_Parent, &mtx, true);
        Vec3f pos;
        float z = mtx.m[2][3];
        float y = mtx.m[1][3];
        float x = mtx.m[0][3];
        pos.m_x = x;
        pos.m_y = y;
        pos.m_z = z;
        moduleAccesser->getPostureModule().setPos(&pos);
        Vec3f rot;
        mtx.getRotate(&rot);
        moduleAccesser->getPostureModule().setRot(&rot, 0);
        moduleAccesser->getLinkModule().removeModelConstraint(true);
        moduleAccesser->getStageObject().updateNodeSRT();
    }
}

void wnSnakeC4Utility::stickStageRot(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getGroundModule().isTouch(0xff, 0)) {
        float rotY = 90.0f;
        if (moduleAccesser->getPostureModule().getLr() > 0.0f) {
            rotY = -rotY;
        }
        Vec2f normal = moduleAccesser->getGroundModule().getTouchNormal(0xff, 0);
        float angle = atan2f(normal.m_y, normal.m_x);
        Vec3f rot(-90.0f, rotY, 57.29578f * (angle - 1.5707964f));
        moduleAccesser->getPostureModule().setRot(&rot, 0);
        moduleAccesser->getStageObject().updateNodeSRT();
    }
}

void wnSnakeC4Utility::dropFall(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getLinkModule().isModelConstraint() == true) {
        detachConstraint(moduleAccesser);
    }
    moduleAccesser->getStatusModule().changeStatusForce(wnSnakeC4Def::Status_Fly, moduleAccesser);
}
