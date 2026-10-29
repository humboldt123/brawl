#include <ef/ef_effect.h>
#include <ef/ef_manager.h>
#include <types.h>

float lbl_805A0100;

efEffect::efEffect(const char* name, u8 group) {
    m_state = 1;
    m_name = name;
    m_group = group;
    m_slowDivisor = 1;
    m_frame = 0.0f;
    m_nextFrame = 0.0f;
    m_rate = 1.0f;
    m_parent = NULL;
    m_nodeIndex = 0;
    m_pauseRequest = false;
    unkD_b6 = true;
    unkD_b5 = false;
    unk20 = 0;
    m_trans.m_x = m_trans.m_y = m_trans.m_z = 0.0f;
    m_rot.m_x = m_rot.m_y = m_rot.m_z = 0.0f;
    m_scale.m_z = m_scale.m_y = m_scale.m_x = 1.0f;
    efManager::getInstance()->addEffect(this);
}

efEffect::~efEffect() {
    efManager::getInstance()->removeEffect(this);
}

void efEffect::processEffect() {
    if (getState() == 2 || getState() == 1) {
        m_nextFrame = m_frame + lbl_805A0100 * (1.0f / m_slowDivisor);
        if (m_nextFrame >= m_frame) {
            update(m_nextFrame - m_frame);
        }
        m_frame = m_nextFrame;
        if (getState() == 1) {
            m_state = 2;
        }
    }
    if (m_pauseRequest == true) {
        m_state = 4;
        m_pauseRequest = false;
    }
}

void efEffect::render() { }

void efEffect::pause() {
    m_pauseRequest = true;
}

void efEffect::end() {
    m_state = 3;
    endCallback();
}

void efEffect::endCallback() { }

void efEffect::endForce() {
    if (getState() != 6) {
        m_state = 3;
        m_state = 6;
        delete this;
    }
}

void efEffect::getWorldMatrix(Matrix* out) {
    out->setSRT(m_scale, m_rot, m_trans);
    if (m_parent != NULL) {
        Matrix parent(true);
        getParentWorldMatrix(&parent);
        parent.mul(out, out);
    }
    if (unkD_b5) {
        (*out)(2, 3) *= 0.07f;
    }
}
