#include <so/controller/so_controller_module_impl.h>
#include <types.h>

void soControllerClatter::reset() {
    m_unk0D = 0;
    m_time = m_unk04 = m_unk08 = 0.0f;
    m_unk0C = 0;
    m_unk0E = 0;
    m_unk0F = 0;
}

// HYPOTHESIS: 0x0C/0x0D hold the last main-stick direction (-1/0/1) on X/Y; a direction
// change counts as one mash (m_unk08 is the per-mash decrement, m_unk04 the per-frame one).
void soControllerClatter::update(soController* controller, int pressed, float threshold) {
    s8 dirX = m_unk0C;
    s8 dirY = m_unk0D;
    if (controller->getStickX() < -threshold) {
        dirX = -1;
    } else if (controller->getStickX() > threshold) {
        dirX = 1;
    }
    if (controller->getStickY() < -threshold) {
        dirY = -1;
    } else if (controller->getStickY() > threshold) {
        dirY = 1;
    }
    if (dirX != m_unk0C || dirY != m_unk0D) {
        m_unk0C = dirX;
        m_unk0D = dirY;
        m_time -= m_unk08;
        if (m_unk0F) {
            m_unk0E++;
        }
    }
    if ((u32)pressed == 1) { // MATCH-ONLY: target compares unsigned (cmplwi)
        m_time -= m_unk08;
    }
    m_time -= m_unk04;
    if (m_time < 0.0f) {
        m_time = 0.0f;
    }
}

int soControllerClatter::checkButton(soController* controller) {
    u32 mask = soController::getButtonMask(soController::Pad_Button_Attack) |
               soController::getButtonMask(soController::Pad_Button_Special) |
               soController::getButtonMask(soController::Pad_Button_Jump) |
               soController::getButtonMask(soController::Pad_Button_Guard);
    int trigger = controller->getTrigger();
    return (trigger & mask) != 0;
}
