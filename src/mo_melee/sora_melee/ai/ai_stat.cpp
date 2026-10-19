#include <ai/ai_stat.h>
#include <mt/mt_common.h>

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
        m_chrIdx = -1;
        m_chrKind = -1;
        m_input = 0;
    } else {
        m_chrIdx = chrIdx;
        m_chrKind = chrKind;
        m_input = input;
    }
    unk1[0] = unk;
}
