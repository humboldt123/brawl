#include <ai/ai_enemy.h>
#include <ai/ai_param.h>
#include <ai/ai_stat.h>
#include <math.h>
#include <gf/gf_memory_util.h>
#include <mt/mt_common.h>

template<class T> static inline T aiMgrField(aiMgr* mgr, int offset) {
    return *(T*)((u8*)mgr + offset);
}

static inline float aiLengthOf(float lengthSq) {
    float length;
    if ((float)fabs(lengthSq) <= 1.17549435e-38f) {
        length = 0.0f;
    } else {
        length = lengthSq * rsqrtf(lengthSq);
    }
    return length;
}

// HYPOTHESIS: aiMgr::get_under_floor and aiMgr::get_via_blk_team, still unnamed in the ai_mgr unit.
extern "C" u32 fn_27_1ED8A4(aiMgr*, Vec3f*, AiClBlock**, void*, int, float, float, float);
// HYPOTHESIS: argument order of get_via_blk_team taken from the register and evaluation order of its callers.
extern "C" AiClBlock* fn_27_1EEAA8(aiMgr*, AiClBlock*, int, float, int, Vec2f*, int, int, float, float, float);

// aiStat: the CPU AI's per-character state snapshot and helper queries.

int aiStat::chk_over_ground() {
    if (!(((m_flags & 1) != 0 || (m_floor->m_flags & 0x20000) != 0) && (u8)(m_situation + 250) > 1)) {
        return 1;
    }
    return ((m_flags >> 1) & 1) ^ 1;
}

u32 aiStat::chk_over_ground(Vec3f* pos) {
    return fn_27_1ED8A4(g_aiMgr, pos, NULL, NULL, -1, -9999.9f, 0.0f, 0.0f);
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

void aiStat::change(int chrIdx, int chrKind, aiInput* input, int unk) {
    if (chrIdx < 0 || chrKind == -1 || input == NULL) {
        m_chrIdx.m_idx = -1;
        m_chrKind = -1;
        m_input = NULL;
    } else {
        m_chrIdx.m_idx = chrIdx;
        m_chrKind = chrKind;
        m_input = input;
    }
    m_unk1 = unk;
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
    AiClBlock* floor = m_floor;
    if (m_pos.m_x < 0.5f * (floor->m_edgeL.m_x + floor->m_edgeR.m_x)) {
        return floor->m_edgeL;
    }
    return floor->m_edgeR;
}

float aiStat::calcArraiveFrameX(float target) {
    return calc_arraive_f(target - m_pos.m_x, m_speed.m_x, m_accel.m_x);
}

float aiStat::calcArraiveFrameY(float target) {
    return calc_arraive_f(target - m_pos.m_y, m_speed.m_y, m_accel.m_y);
}

float aiStat::calcArraivePosX(float frame) {
    float speed = m_speed.m_x;
    float accel = m_accel.m_x;
    float h = 0.5f + frame;
    return m_pos.m_x + (speed * frame + (0.5f * (h * (accel * h)) - 0.125f * accel));
}

float aiStat::calcArraivePosY(float frame) {
    float speed = m_speed.m_y;
    float accel = m_accel.m_y;
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

void aiStat::init(int chrIdx, int chrKind, aiInput* input, int unk) {
    gfMemFill(this, 0, sizeof(aiStat));
    if (chrIdx < 0 || chrKind == -1 || input == NULL) {
        m_chrIdx.m_idx = -1;
        m_chrKind = -1;
        m_input = NULL;
    } else {
        m_chrIdx.m_idx = chrIdx;
        m_chrKind = chrKind;
        m_input = input;
    }
    m_unkB0 = -1;
    m_unk1 = unk;
    m_flags = 0x8000;
    m_transition = 0;
    m_unkD8 = -1;
    m_unkE0 = 0;
    m_enableAway = -1;
    m_unk1B9 = 0;
    m_unk1BB = 0;
    m_unkAA = 0;
    m_unk2 = 0;
    m_unk8 = 0;
    m_chrStat = 0;
    m_center = Vec3f(0.0f, 0.0f, 0.0f);
    m_pos.m_x = 0.0f;
    m_pos.m_y = 0.0f;
    m_pos.m_z = 0.0f;
    m_scale = Vec3f(1.0f, 1.0f, 1.0f);
    m_accel = Vec2f(0.0f, 0.0f);
    m_speed = Vec2f(0.0f, 0.0f);
    m_unk98.m_x = 0.0f;
    m_unk98.m_y = 0.0f;
    m_unk90.m_x = 0.0f;
    m_unk90.m_y = 0.0f;
    m_lr = 1.0f;
    m_unkA4 = 0.0f;
    m_unkA8 = 0;
    m_unkB8 = 0;
    m_unkB4 = 0;
    m_floor = (AiClBlock*)((u8*)g_aiMgr + 0xAC);
    m_unkC4 = 0.0f;
    m_unkCC = -1;
    m_team = -1;
    m_unkD0 = 0;
    m_unkD4 = 0;
    m_unkDC = 0.0f;
    m_unk4 = 0;
    m_unk1BD = 0;
    m_unk1BC = 0;
}

void aiStat::get_tgt_vector(float scale, Vec3f* out, aiChrIdx* target) {
    aiStat* stat = aiMgr::get_aiStat(target);
    if (m_chrIdx.m_idx == target->m_idx) {
        *out = Vec3f(9999.9f, 0.0f, 0.0f);
    } else {
        Vec2f relSpeed = stat->m_speed - m_speed;
        Vec3f diff = stat->m_center - m_center;
        float speed = relSpeed.length();
        if (speed >= 0.0001f) {
            float dist = aiLengthOf(diff.m_z * diff.m_z + (diff.m_x * diff.m_x + diff.m_y * diff.m_y));
            float time = dist / speed;
            if (time < scale) {
                scale = scale * 0.1f + time * 0.9f;
            }
            Vec3f hit;
            hit.m_x = stat->m_center.m_x + scale * (stat->m_speed.m_x - m_speed.m_x);
            hit.m_y = stat->m_center.m_y + scale * (stat->m_speed.m_y - m_speed.m_y);
            hit.m_z = stat->m_center.m_z;
            *out = hit;
            *out -= m_center;
        } else {
            *out = diff;
        }
    }
}

float aiStat::get_range(float scale, aiChrIdx* target) {
    aiStat* stat = aiMgr::get_aiStat(target);
    float range;
    if (scale <= 0.001f) {
        range = m_lr * (stat->m_pos.m_x - m_pos.m_x);
    } else {
        Vec3f vec;
        get_tgt_vector(scale, &vec, target);
        range = vec.m_x * m_lr;
    }
    if (range >= 0.0f) {
        if (m_input != 0) {
            float targetWidth = aiParam::get_width(target);
            float selfWidth = aiParam::get_width(&m_chrIdx);
            range = range - (selfWidth + targetWidth);
        }
        if (range < 0.0001f) {
            range = 0.0001f;
        }
    } else {
        if (m_input != 0) {
            float targetWidth = aiParam::get_width(target);
            float selfWidth = aiParam::get_width(&m_chrIdx);
            range = range + (selfWidth + targetWidth);
        }
        if (range > -0.0001f) {
            range = -0.0001f;
        }
    }
    return range;
}

float aiStat::get_distance(float scale, aiChrIdx* target) {
    aiStat* stat = aiMgr::get_aiStat(target);
    Vec3f vec;
    if (scale <= 0.001f) {
        vec = stat->m_center - m_center;
    } else {
        get_tgt_vector(scale, &vec, target);
    }
    float dist = aiLengthOf(vec.m_z * vec.m_z + (vec.m_x * vec.m_x + vec.m_y * vec.m_y));
    float targetWidth = aiParam::get_width(target);
    float selfWidth = aiParam::get_width(&m_chrIdx);
    dist = dist - (selfWidth + targetWidth);
    if (dist < 0.0001f) {
        dist = 0.0001f;
    }
    return dist;
}

float aiStat::get_range_floor(float scale, Vec3f* pos) {
    AiClBlock* floor = m_floor;
    Vec2f edgeL = floor->m_edgeL;
    Vec2f edgeR = floor->m_edgeR;
    float sign = 1.0f;
    float range;
    if (m_unkC0 == 2) {
        float x = pos->m_x;
        if (x < edgeL.m_x) {
            sign = -1.0f;
            range = __fabsf(x - edgeL.m_x);
        } else if (x > edgeR.m_x) {
            sign = -1.0f;
            range = __fabsf(x - edgeR.m_x);
        } else {
            float toLeft = __fabsf(x - edgeL.m_x);
            float toRight = __fabsf(x - edgeR.m_x);
            if (toLeft < toRight) {
                toLeft = -toLeft;
            } else {
                toLeft = toRight;
            }
            range = __fabsf(toLeft * scale);
        }
    } else {
        if (scale > 0.0f) {
            range = scale * (edgeR.m_x - pos->m_x);
        } else {
            range = scale * (edgeL.m_x - pos->m_x);
        }
    }
    float width = aiParam::get_width(&m_chrIdx);
    range = range - width;
    if (range < 0.001f) {
        range = 0.001f;
    }
    return range * sign;
}

bool aiStat::chkEnableAway(aiJumpPar* par) {
    if (m_enableAway < 0) {
        aiMgr* mgr = g_aiMgr;
        if (mgr->m_unk358 <= 1) {
            m_enableAway = 0;
        } else {
            Vec2f pos(m_pos.m_x, m_pos.m_y);
            float high = aiParam::get_jump_high(par, this);
            if (m_floor == fn_27_1EEAA8(mgr, m_floor, 0, pos.m_x, m_team, &pos, 1, 0, aiParam::get_jump_length(par, this), high, 0.125f)) {
                m_enableAway = 0;
            } else {
                m_enableAway = 1;
            }
        }
    }
    return m_enableAway > 0;
}

bool aiStat::chkDangerZoneAway() {
    if (m_unkC0 == 0) {
        bool danger = false;
        AiClBlock* floor = m_floor;
        int team = m_team;
        if ((floor->m_flags & 0x1000) != 0 || floor->chk_fl_danger(team)) {
            danger = true;
        }
        if (danger) {
            return true;
        }
    }
    if (chk_stat_gr_free() || chk_stat_air_free()) {
        if (m_floor->chk_fl_danger(m_team) && aiMgr::chkDangerZone((Vec2f*)&m_center, m_team, 0)) {
            return true;
        }
    }
    return false;
}

void aiStat::update_enemy(aiEnemy* enemy, aiChrIdx* chrIdx) {
    init(-1, -1, NULL, 0);
    m_chrIdx = *chrIdx;
    m_flags = 0x80000004;
    if ((enemy->m_flags & 2) == 0) {
        m_flags |= 1;
    }
    m_unkB4 = enemy->m_status;
    m_center.m_x = enemy->m_pos.m_x;
    m_center.m_y = enemy->m_pos.m_y;
    m_center.m_z = enemy->m_pos.m_z;
    m_pos.m_x = m_center.m_x;
    m_pos.m_y = m_center.m_y;
    m_pos.m_z = m_center.m_z;
    m_speed.m_x = enemy->m_move.m_x;
    m_speed.m_y = enemy->m_move.m_y;
    m_lr = enemy->m_lr;
    set_floor_info(0);
}
