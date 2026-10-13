#include <gf/gf_pad_system.h>
#include <types.h>

#include <mu/mu_boot_pad.h>

muBootPad::muBootPad() { }

muBootPad::~muBootPad() { }

// Reads the status of the pads.
void muBootPad::update() {
    for (int i = 0; i < 4; i++) {
        g_gfPadSystem->getSysPadStatus(i, &m_pad[i]);
        g_gfPadSystem->getSysPadStatus(i + 4, &m_pad2[i]);
    }
}

// Whether the start button of a pad was pressed (the classic controller's own button, 0x800000, does not count).
static inline u32 isStartPressed(const gfPadStatus* pad) {
    u32 result;
    switch (pad->m_controllerType) {
    case gfPadType::WII_CLASSIC:
        if (pad->m_buttonsPressedThisFrame.bits & 0x800000) {
            result = 0;
            break;
        }
    default:
        result = (pad->m_buttonsPressedThisFrame.bits >> 12) & 1;
        break;
    }
    return result;
}

bool muBootPad::isPressStartButton() {
    gfPadStatus* pad = m_pad;
    for (int i = 0; i < 2; i++) {
        if (isStartPressed(&pad[0]) == 1) {
            return true;
        }
        if (isStartPressed(&pad[4]) == 1) {
            return true;
        }
        if (isStartPressed(&pad[1]) == 1) {
            return true;
        }
        if (isStartPressed(&pad[5]) == 1) {
            return true;
        }
        pad += 2;
    }
    return false;
}

// Whether any button that skips the strap was pressed.
bool muBootPad::isPressStrapSkipButton() {
    gfPadStatus* pad = m_pad;
    gfPadStatus* pad2 = m_pad2;
    for (int i = 0; i < 4; i++) {
        if (isPressStrapSkipButton1(pad) == 1) {
            return true;
        }
        if (isPressStrapSkipButton1(pad2) == 1) {
            return true;
        }
        pad++;
        pad2++;
    }
    return false;
}

// MATCH-ONLY: the game does not inline this function into isPressStrapSkipButton.
#pragma dont_inline on
// Whether a pad pressed a button that skips the strap (any button but a few: L and R of the GameCube controller, the
// button 0x800000 of the classic controller, and the button 0x2000).
bool muBootPad::isPressStrapSkipButton1(const gfPadStatus* pad) {
    if (pad->m_buttonsPressedThisFrame.bits != 0) {
        u32 buttons = pad->m_buttonsPressedThisFrame.bits;
        if (buttons & 0x2000) {
            return false;
        }
        switch (pad->m_controllerType) {
        case gfPadType::GCC:
            if (buttons & GF_PAD_BUTTON_BIT_L) {
                return false;
            }
            if (buttons & GF_PAD_BUTTON_BIT_R) {
                return false;
            }
            break;
        case gfPadType::WII_CLASSIC:
            if (buttons & 0x800000) {
                return false;
            }
            break;
        }
        return true;
    }
    return false;
}

#pragma dont_inline reset

// Whether any button that skips the boot was pressed.
bool muBootPad::isPressBootSkipButton() {
    gfPadStatus* pad = m_pad;
    gfPadStatus* pad2 = m_pad2;
    for (int i = 0; i < 4; i++) {
        if (isPressBootSkipButton1(pad) == 1) {
            return true;
        }
        if (isPressBootSkipButton1(pad2) == 1) {
            return true;
        }
        pad++;
        pad2++;
    }
    return false;
}

// Whether a pad pressed a button that skips the boot (any button but the d-pad, the triggers, and a few more).
bool muBootPad::isPressBootSkipButton1(const gfPadStatus* pad) {
    if (pad->m_buttonsPressedThisFrame.bits != 0) {
        u32 buttons = pad->m_buttonsPressedThisFrame.bits;
        if (buttons & 0x2000) {
            return false;
        }
        if (buttons & GF_PAD_BUTTON_BIT_DPAD_LEFT) {
            return false;
        }
        if (buttons & GF_PAD_BUTTON_BIT_DPAD_RIGHT) {
            return false;
        }
        if (buttons & GF_PAD_BUTTON_BIT_DPAD_UP) {
            return false;
        }
        if (buttons & GF_PAD_BUTTON_BIT_DPAD_DOWN) {
            return false;
        }
        switch (pad->m_controllerType) {
        case gfPadType::GCC:
            if (buttons & GF_PAD_BUTTON_BIT_L) {
                return false;
            }
            if (buttons & GF_PAD_BUTTON_BIT_R) {
                return false;
            }
            if (buttons & GF_PAD_BUTTON_BIT_Z) {
                return false;
            }
            break;
        case gfPadType::WII_CLASSIC:
            if (buttons & 0x800000) {
                return false;
            }
            if (buttons & GF_PAD_BUTTON_BIT_L) {
                return false;
            }
            if (buttons & GF_PAD_BUTTON_BIT_R) {
                return false;
            }
            if (buttons & 0x4000) {
                return false;
            }
            if (buttons & 0x8000) {
                return false;
            }
            break;
        case gfPadType::WIIMOTE:
            break;
        case gfPadType::NUNCHUK:
            if (buttons & 0x40000) {
                return false;
            }
            if (buttons & GF_PAD_BUTTON_BIT_Z) {
                return false;
            }
            break;
        }
        return true;
    }
    return false;
}

// Whether any pad pressed one of the buttons of the mask in this frame.
bool muBootPad::isOnEdge(u32 mask) {
    gfPadStatus* pad = m_pad;
    for (int i = 0; i < 2; i++) {
        if (pad[0].m_buttonsPressedThisFrame.bits & mask) {
            return true;
        }
        if (pad[4].m_buttonsPressedThisFrame.bits & mask) {
            return true;
        }
        if (pad[1].m_buttonsPressedThisFrame.bits & mask) {
            return true;
        }
        if (pad[5].m_buttonsPressedThisFrame.bits & mask) {
            return true;
        }
        pad += 2;
    }
    return false;
}
