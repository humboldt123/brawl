#include <cm/cm_controller_menu_pad.h>
#include <gf/gf_camera.h>
#include <gf/gf_pad_status.h>
#include <gf/gf_pad_system.h>
#include <types.h>

cmMenuPadController::cmMenuPadController() : gfCameraController() {
    unk8 = 0;
    unkC = 0.4f;
    unk14 = 68.0f;
    unk24 = 0.69813f;
    unk10 = false;
    unk18.m_x = 0.0f;
    unk18.m_y = 0.0f;
    unk18.m_z = 0.0f;
}

void cmMenuPadController::storeDefault() {
    unk14 = m_cameraManager->m_cameras[0].unkCC;
    unk18.m_x = m_cameraManager->m_cameras[0].m_targetPos.m_x;
    unk18.m_y = m_cameraManager->m_cameras[0].m_targetPos.m_y;
    unk18.m_z = m_cameraManager->m_cameras[0].m_targetPos.m_z;
    unk24 = m_cameraManager->m_cameras[0].unkD0;
    unk10 = true;
}

void cmMenuPadController::init() {
    gfCamera* camera = &m_cameraManager->m_cameras[0];
    camera->unkCC = unk14;
    camera->unkFA.m_mask |= 0x80;
    camera->m_targetPos = unk18;
    camera->unkFA.m_mask |= 0x2;
    camera->unkD0 = unk24;
}

void cmMenuPadController::update(float) {
    gfCamera* camera = &m_cameraManager->m_cameras[0];
    if (!unk10) {
        storeDefault();
    }
    gfPadStatus status;
    g_gfPadSystem->getSysPadStatus(0, &status);
    g_gfPadSystem->getSysPadStatus(0, &status);
    int i;
    s8 maxX, maxY, minX, minY;
    maxX = 0;
    minX = 0;
    maxY = 0;
    minY = 0;
    for (i = 0; i < 8; i++) {
        g_gfPadSystem->getSysPadStatus(i, &status);
        if (status.m_error == 0) {
            if (maxX < status.m_subStickX) {
                maxX = status.m_subStickX;
            }
            if (minX > status.m_subStickX) {
                minX = status.m_subStickX;
            }
            if (maxY < status.m_subStickY) {
                maxY = status.m_subStickY;
            }
            if (minY > status.m_subStickY) {
                minY = status.m_subStickY;
            }
        }
    }
    int x = maxX + minX;
    x = x > -77 ? x : -77;
    x = x < 77 ? x : 77;
    int absX = x < 0 ? -x : x;
    float tiltX;
    if (absX > 30) {
        tiltX = (float)(absX - 30) / 47.0f;
        if (x < 0) {
            tiltX = -tiltX;
        }
    } else {
        tiltX = 0.0f;
    }
    int y = maxY + minY;
    y = y > -77 ? y : -77;
    y = y < 77 ? y : 77;
    int absY = y < 0 ? -y : y;
    float tiltY;
    if (absY > 30) {
        tiltY = (float)(absY - 30) / 47.0f;
        if (y < 0) {
            tiltY = -tiltY;
        }
    } else {
        tiltY = 0.0f;
    }
    *camera->m_rot.xy() = Vec2f(-(15.0f * tiltY) * 0.017453292f, (15.0f * tiltX) * 0.017453292f);
    camera->m_transformFlag.m_mask = 0xe1;
    camera->m_fovScale = 1.0f;
    camera->calc();
}
