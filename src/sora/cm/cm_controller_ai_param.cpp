#include <cm/cm_controller_ai_param.h>

void cmCameraParam::clear() {
    m_pos.m_x = 0.0f;
    m_pos.m_y = 10.0f;
    m_pos.m_z = 0.0f;
    m_targetPos.m_x = 0.0f;
    m_targetPos.m_y = 10.0f;
    m_targetPos.m_z = 0.0f;
    m_eye.m_x = 0.0f;
    m_eye.m_y = 8.241425f;
    m_eye.m_z = 90.0f;
    m_targetEye.m_x = 0.0f;
    m_targetEye.m_y = 8.241425f;
    m_targetEye.m_z = 90.0f;
    m_fovy = m_targetFovy = 38.0f;
    unk38 = true;
}

void cmCameraParam::updateFovyNormal(float fovy, float rate) {
    m_targetFovy = fovy;
    m_fovy = m_fovy + rate * (fovy - m_fovy);
}
