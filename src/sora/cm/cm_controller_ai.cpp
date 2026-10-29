#include <cm/cm_controller_ai.h>
#include <gf/gf_camera.h>
#include <sr/sr_common.h>
#include <types.h>

cmAIController* g_cmAIController;

// HYPOTHESIS: one table holds both default blocks (the constructor addresses the second from the first).
static struct {
    cmAIController::DefaultParam m_param;
    cmAIController::DefaultParam2 m_param2;
} s_defaultParam = {
    { -30.0f, 5.0f, -7.0f, 17.5f, -17.5f, 0.0f, 0.0682f, 60.0f, 120.0f, 0.05f, 0.1f, 120.0f, 900.0f,
      0.15f, 30.0f, 0.1f, 0.2f, 60.0f, 0.1f, 0.001f, 0.1f, 1.0f, 1.0f, 0.6f, 0.6f },
    { 0.02f, 1.0f, 0.14f, 1200.0f, -0.2f, 1.2f },
};

cmAIController* cmAIController::create() {
    if (!g_cmAIController) {
        g_cmAIController = new (Heaps::System) cmAIController();
    }
    return g_cmAIController;
}

cmAIController::cmAIController() {
    unk8 = 0;
    unk94_80 = false;
    unk94_40 = false;
    unk94_20 = false;
    unk94_10 = true;
    unk94_08 = true;
    unk94_04 = false;
    unk94_02 = false;
    unk94_01 = true;
    unk95_80 = true;
    m_devilOffset.m_x = 0.0f;
    m_devilOffset.m_y = 0.0f;
    unk17C = 0.0f;
    unk180 = 1.0f;
    unk184 = 0.0f;
    unk88 = 0;
    unk84 = 0;
    unk90 = 0;
    unk8C = 0;
    unkBC = s_defaultParam.m_param;
    unk120 = s_defaultParam.m_param2;
    unk138 = nullptr;
    unk13C = nullptr;
    unk140 = nullptr;
    m_aspect = gfCameraManager::getManager()->m_aspectRatio;
    m_param.clear();
    m_internalParam.clear();
    m_userRot.m_z = 0.0f;
    m_userRot.m_y = 0.0f;
    m_userRot.m_x = 0.0f;
    m_userPos.m_y = 0.0f;
    m_userPos.m_x = 0.0f;
    unk15C.m_left = -10.0f;
    unk15C.m_right = 10.0f;
    unk15C.m_up = 20.0f;
    unk15C.m_down = -20.0f;
    unk16C.m_left = -10.0f;
    unk16C.m_right = 10.0f;
    unk16C.m_up = 20.0f;
    unk16C.m_down = -20.0f;
    m_userDistMin = 0.0f;
    unk188.m_left = -3.4028235e38f;
    unk188.m_right = 3.4028235e38f;
    unk188.m_up = 3.4028235e38f;
    unk188.m_down = -3.4028235e38f;
}

cmAIController::~cmAIController() {
    g_cmAIController = NULL;
}

void cmAIController::reset() {
    unk88 = 0;
    unk8 = 0;
    unk84 = 0;
    unk90 = 0;
    unk8C = 0;
    unk17C = 0.0f;
    unk180 = 1.0f;
    unk184 = 0.0f;
    unk94_80 = false;
    unk94_10 = true;
    unk94_08 = true;
    unk94_40 = false;
    unk94_20 = false;
    unk94_04 = false;
    unk94_02 = false;
    unk94_01 = true;
    unk95_80 = true;
    m_devilOffset.m_x = 0.0f;
    m_devilOffset.m_y = 0.0f;
    m_userRot.m_z = 0.0f;
    m_userRot.m_y = 0.0f;
    m_userRot.m_x = 0.0f;
    m_userPos.m_y = 0.0f;
    m_userPos.m_x = 0.0f;
    m_userDistMin = 0.0f;
    m_param.clear();
    m_internalParam.clear();
    unk15C.m_left = -10.0f;
    unk15C.m_right = 10.0f;
    unk15C.m_up = 20.0f;
    unk15C.m_down = -20.0f;
    unk16C.m_left = -10.0f;
    unk16C.m_right = 10.0f;
    unk16C.m_up = 20.0f;
    unk16C.m_down = -20.0f;
    unk188.m_left = -3.4028235e38f;
    unk188.m_right = 3.4028235e38f;
    unk188.m_up = 3.4028235e38f;
    unk188.m_down = -3.4028235e38f;
    gfCamera* camera = &m_cameraManager->m_cameras[0];
    if (camera) {
        camera->init();
    }
    camera = gfCameraManager::getManager()->getCamera(1);
    if (camera) {
        camera->init();
        camera->m_aspect = 1.4848485f;
    }
}

void cmAIController::setUserOffset(const Vec3f& rot, const Vec2f& pos) {
    m_userRot = rot;
    m_userPos = pos;
}

void cmAIController::updateCameraPosTarget() {
    gfCamera* camera = &m_cameraManager->m_cameras[0];
    m_param.m_pos = camera->m_targetPos;
    m_param.m_eye = camera->m_centerPos;
    m_internalParam.m_pos = m_param.m_pos;
    m_internalParam.m_eye = m_param.m_eye;
}

void cmAIController::resetDevilOffset() {
    m_devilOffset.m_y = 0.0f;
    m_devilOffset.m_x = 0.0f;
}

void cmAIController::accDevilOffset(Vec2f* offset) {
    m_devilOffset.m_x += offset->m_x;
    m_devilOffset.m_y += offset->m_y;
}

void cmAIController::setUserDistMin(float distance) {
    m_userDistMin = distance;
}
