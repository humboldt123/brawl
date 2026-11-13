#include <so/damage/so_damage_no_reaction_module.h>

soDamageNoReactionModule::soDamageNoReactionModule(int mode) : m_mode(mode), m_isModeAlways(false), m_isModePerfect(false) { }

// A hit takes no reaction in the perfect mode, or (when it is not absolute) in the always mode, in mode 1, or when one of
// the two rules holds.
bool soDamageNoReactionModule::checkNoReaction(float reaction, float powerMax, bool isAbsolute) {
    if (m_isModePerfect == true) {
        return true;
    }
    if (!isAbsolute) {
        if (m_isModeAlways == true || m_mode == 1) {
            return true;
        }
        for (int i = 0; i < 2; i++) {
            if (m_status[i].m_mode != -1) {
                bool noReaction;
                switch (m_status[i].m_mode) {
                case 1:
                    noReaction = true;
                    break;
                case 2:
                    noReaction = reaction <= m_status[i].m_reaction;
                    break;
                case 3:
                    noReaction = powerMax <= m_status[i].m_powerMax;
                    break;
                case 0:
                    if (reaction <= 0.0f && powerMax >= 0.0f) {
                        noReaction = true;
                    } else {
                        noReaction = false;
                    }
                    break;
                default:
                    noReaction = false;
                    break;
                }
                if (noReaction == true) {
                    return true;
                }
            }
        }
    }
    return false;
}

// Negative limits keep the previous ones.
void soDamageNoReactionModule::set(float reaction, float powerMax, bool is2nd, int mode) {
    m_status[is2nd].m_mode = mode;
    if (reaction >= 0.0f) {
        m_status[is2nd].m_reaction = reaction;
    }
    if (powerMax >= 0.0f) {
        m_status[is2nd].m_powerMax = powerMax;
    }
}

void soDamageNoReactionModule::resetModeStatus() {
    m_status[0].m_mode = 0;
}

void soDamageNoReactionModule::setMode(int mode) {
    m_mode = mode;
}

void soDamageNoReactionModule::resetMode() {
    m_mode = 0;
}

void soDamageNoReactionModule::reset() {
    m_mode = 0;
    m_status[0].m_mode = 0;
    m_status[1].m_mode = -1;
}