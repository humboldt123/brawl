#include <gm/gm_global.h>
#include <so/so_slow.h>
#include <types.h>

soSlow::soSlow() : gfTask("Slow", Category_None, 0, 13, true) {
    m_kind = 0;
    m_rate = 1.0f;
    m_isOn = false;
}

soSlow::~soSlow() {
    ms_instance = NULL;
}

void soSlow::processBegin() {
    if (g_GameGlobal->getSlowRate() != 1) {
        if (g_GameGlobal->isJustGameFrame() == true) {
            m_kind = 0;
        } else if (g_GameGlobal->isPrevJustGameFrame() == true) {
            m_kind = 2;
        } else {
            m_kind = 1;
        }
        m_isOn = true;
        m_rate = 1.0f / g_GameGlobal->getSlowRate();
    } else {
        m_kind = 0;
        m_isOn = false;
        m_rate = 1.0f;
    }
}

bool soSlow::isEstimate() {
    return !m_isOn || m_kind == 0;
}

bool soSlow::isAdjust() {
    return !m_isOn || m_kind == 2;
}
