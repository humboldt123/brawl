// Return to the camera center and synchronize position before restoring normal rendering.
#define MT_VEC3F_CTOR_NOINLINE
#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/snake/ft_snake_status_uniq_process_final.h>
#include <cm/cm_camera_controller.h>
#include <cm/cm_controller_ai.h>
#include <mt/mt_vector.h>
#include <so/so_external_value_accesser.h>
#include <so/so_value_accesser.h>
#define SO_PHYSICS_MODULE_IMPL_FULL
#include <so/physics/so_physics_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/work/so_work_manage_module_impl.h>


ftSnakeStatusUniqProcessFinalEnd g_ftSnakeStatusUniqProcessFinalEnd;


void ftSnakeStatusUniqProcessFinalEnd::initStatus(soModuleAccesser* acc) {
    soWorkManageModule& work = acc->getWorkManageModule();
    int duration = soValueAccesser::getConstantInt(acc, 24000, 0);
    work.setInt(duration, 0x20000002);
}

void ftSnakeStatusUniqProcessFinalEnd::execFixPosCounter(soModuleAccesser* acc) {
    if (acc->getWorkManageModule().getInt(0x20000002) > 0)
        acc->getWorkManageModule().decInt(0x20000002);
}

void ftSnakeStatusUniqProcessFinalEnd::execFixPos(soModuleAccesser* acc) {
    if (acc->getWorkManageModule().getInt(0x20000002) > 0) {
        soWorkManageModule& work = acc->getWorkManageModule();
        float duration = static_cast<float>(soValueAccesser::getConstantInt(acc, 24000, 0));
        float remaining = static_cast<float>(work.getInt(0x20000002));
        float cameraScale = remaining / duration;
        ftSnakeStatusUniqProcessFinalCommon::setCameraOffsetZ(acc, cameraScale);
    }
}

void ftSnakeStatusUniqProcessFinalEnd::exitStatus(soModuleAccesser* acc, int next) {
    CameraController* camera = CameraController::getInstance();
    if (camera) {
        soPostureModule& posture = acc->getPostureModule();
        Rect2D& bounds = camera->m_stageCameraBounds;
        posture.initPos(&Vec3f((bounds.m_left + bounds.m_right) / 2.0f, bounds.m_up, 0.0f));
        static_cast<soPhysicsModule*>(acc->m_enumerationStart->m_physicsModule)->sync2ndSavedPosition();
        acc->getStageObject().updateNodeSRT();
        acc->getStageObject().updateRoughPos();
    }
    soPostureModule& posture = acc->getPostureModule();
    posture.setRot(&Vec3f(0.0f, 0.0f, 0.0f), 0);
    if (g_cmAIController) {
        g_cmAIController->setUserDistMin(0.0f);
        ftSnakeStatusUniqProcessFinalCommon::exitStatusCommon(acc, next);
    }
}
