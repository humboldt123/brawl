#include <ai/ai_stat.h>
#include <math.h>
#include <mt/mt_common.h>

template<class T> static inline T aiMgrField(aiMgr* mgr, int offset) {
    return *(T*)((u8*)mgr + offset);
}

// aiStat: the CPU AI's per-character state snapshot and helper queries.

int aiStat::chk_over_ground() {
    if (!(((m_flags & 1) != 0 || (m_floor->m_flags & 0x20000) != 0) && (u8)(m_situation + 250) > 1)) {
        return 1;
    }
    return ((m_flags >> 1) & 1) ^ 1;
}

bool aiStat::chk_stat_gr_free() {
    if (m_situation == 1) {
        return true;
    }
    return (m_transition & 0x99) == 0x99;
}

bool aiStat::chk_stat_air_free() {
    if (m_situation == 2) {
        return true;
    }
    return (m_transition & 0x6400) == 0x6400;
}

bool aiStat::chk_transition(u32 mask) {
    return (m_transition & mask) != 0;
}

bool aiStat::chk_chr_stat(u32 mask, int chrKind) {
    if (chrKind == -1 || chrKind == m_chrKind) {
        if ((m_chrStat & mask) != 0) {
            return true;
        }
    }
    return false;
}

bool aiStat::chk_stat_fpass() {
    if (m_unkC0 == 0 && (m_floor->m_flags & 0x20) != 0) {
        return true;
    }
    return false;
}

bool aiStat::chk_stat_ffall() {
    if (m_unkC0 == 0 && (m_floor->m_flags & 0x20000) != 0) {
        return true;
    }
    return false;
}

void aiStat::change(int chrIdx, int chrKind, int input, u8 unk) {
    if (chrIdx < 0 || chrKind == -1 || input == 0) {
        m_chrIdx.m_idx = -1;
        m_chrKind = -1;
        m_input = 0;
    } else {
        m_chrIdx.m_idx = chrIdx;
        m_chrKind = chrKind;
        m_input = input;
    }
    unk1[0] = unk;
}

int aiStat::chk_over_ground1(aiChrIdx* target) {
    aiStat* stat = aiMgr::get_aiStat(target);
    if (!((stat->m_flags & 1) != 0 || (stat->m_floor->m_flags & 0x20000) != 0) || (u8)(stat->m_situation + 250) <= 1) {
        return 1;
    }
    return ((stat->m_flags >> 1) & 1) ^ 1;
}

bool aiStat::chk_near_deadarea() {
    aiMgr* mgr = g_aiMgr;
    float width = aiParam::get_width(&m_chrIdx);
    if (m_pos.m_x - width < 70.0f + aiMgrField<float>(mgr, 0x360)
        || m_pos.m_x + width > aiMgrField<float>(mgr, 0x364) - 70.0f) {
        return true;
    }
    return false;
}

float aiStat::get_lr_tgt(aiChrIdx* target) {
    aiStat* stat = aiMgr::get_aiStat(target);
    float dx = (stat->m_pos - m_pos).m_x;
    if (__fabsf(dx) < 0.01f) {
        return m_lr;
    }
    if (dx < 0.0f) {
        return -1.0f;
    }
    return 1.0f;
}

Vec2f aiStat::getNearFloorEdge() {
    aiFloorInfo* floor = m_floor;
    if (m_pos.m_x < 0.5f * (floor->m_edgeL.m_x + floor->m_edgeR.m_x)) {
        return floor->m_edgeL;
    }
    return floor->m_edgeR;
}

float aiStat::calcArraiveFrameX(float target) {
    return calc_arraive_f(target - m_pos.m_x, m_speedX, m_accelX);
}

float aiStat::calcArraiveFrameY(float target) {
    return calc_arraive_f(target - m_pos.m_y, m_speedY, m_accelY);
}

float aiStat::calcArraivePosX(float frame) {
    float speed = m_speedX;
    float accel = m_accelX;
    float h = 0.5f + frame;
    return m_pos.m_x + (speed * frame + (0.5f * (h * (accel * h)) - 0.125f * accel));
}

float aiStat::calcArraivePosY(float frame) {
    float speed = m_speedY;
    float accel = m_accelY;
    float h = 0.5f + frame;
    return m_pos.m_y + (speed * frame + (0.5f * (h * (accel * h)) - 0.125f * accel));
}

float calc_arraive_f(float dist, float vel, float acc) {
    if (vel * acc >= 0.0f) {
        if (dist > 0.0f) {
            if (vel <= 0.0f && acc <= 0.0f) {
                return -1.0f;
            }
        } else if (vel >= 0.0f && acc >= 0.0f) {
            return -1.0f;
        }
    }
    float absAcc = __fabsf(acc);
    if (absAcc < 0.001f) {
        float absVel = __fabsf(vel);
        if (absVel < 0.001f) {
            return -1.0f;
        }
        return dist / vel;
    }
    float t = -(vel / acc);
    float h = 0.5f + t;
    float r = mtSqrtf(__fabsf((0.125f * acc + (dist - (vel * t + (0.5f * (h * (acc * h)) - 0.125f * acc)))) / (0.5f * acc)));
    return (r - 0.5f) + t;
}
