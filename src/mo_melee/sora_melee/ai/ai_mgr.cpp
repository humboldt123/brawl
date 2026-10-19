#include <ai/ai_mgr.h>
#include <types.h>

// The CPU player AI manager (aiMgr) and its helpers. HYPOTHESIS: field names are unknown, so members are read through
// byte offsets (aiMgrField).

extern "C" {
// aiMgr readiness flag: -1 while the AI is disabled, 0.. once the floor information has been prepared
extern s8 lbl_27_data_47BF8;
}

// HYPOTHESIS: view of the part of aiMgr that holds the danger zone flags (see aiMgr::setDangerZone)
struct aiMgrDangerView {
    char unk0[0x32C];
    u8 zoneUsed[24];
};

template <class T> static inline T& aiMgrField(aiMgr* manager, int offset) {
    return *reinterpret_cast<T*>(reinterpret_cast<u8*>(manager) + offset);
}

bool aiMgr::isReady() {
    return lbl_27_data_47BF8 > 0;
}

bool aiMgr::isReadyUpdate() {
    return lbl_27_data_47BF8 >= 0;
}

bool aiMgr::isSmallMap() {
    switch (aiMgrField<int>(g_aiMgr, 0x370)) {
    case 1:
    case 2:
    case 3:
    case 4:
    case 6:
    case 9:
    case 0xC:
    case 0xD:
    case 0xE:
    case 0x13:
    case 0x14:
    case 0x15:
    case 0x16:
    case 0x17:
    case 0x18:
    case 0x19:
    case 0x1C:
    case 0x1E:
    case 0x21:
    case 0x24:
    case 0x2B:
    case 0x2E:
    case 0x32:
        return true;
    default:
        return false;
    }
}

void* aiMgr::getAiSystemPar() {
    return aiMgrField<void*>(g_aiMgr, 0x13C);
}

aiReferPar* aiMgr::getAiReferPar(int index) {
    if (index < 0 || index > 0x35) {
        index = 0;
    }
    return aiMgrField<aiReferPar*>(g_aiMgr, 0x138) + index;
}

template <class T> static inline T& aiInputField(aiInput* input, int offset) {
    return *reinterpret_cast<T*>(reinterpret_cast<u8*>(input) + offset);
}

int aiMgr::getCpuType(int player) {
    if (player >= 0 && player < 7 && m_aiInput != NULL && m_aiInput[player] != NULL) {
        return aiInputField<u8>(m_aiInput[player], 0x113);
    }
    return 0;
}

int aiMgr::getCpuRank(int player) {
    if (player >= 0 && player < 7 && m_aiInput != NULL && m_aiInput[player] != NULL) {
        return aiInputField<s8>(m_aiInput[player], 0x98);
    }
    return 0;
}

int aiMgr::getAiCpuTarget(int player) {
    if (player >= 0 && player < 7 && m_aiInput != NULL && m_aiInput[player] != NULL) {
        return aiInputField<s8>(m_aiInput[player], 0x110);
    }
    return -1;
}

bool aiMgr::chkNanaForceMove(int player) {
    if (m_aiInput != NULL && player >= 0 && player < 7 && m_aiInput[player + 7] != NULL) {
        aiInput* nana = m_aiInput[player + 7];
        if (aiInputField<int>(nana, 0x48) == 0x79 && (aiInputField<u16>(nana, 0x3C) & 8) == 0) {
            return true;
        }
    }
    return false;
}

int aiChrIdx::GetPlayerNo(const aiChrIdx* idx) {
    int value = idx->m_idx;
    if (value >= 0 && value < 14) {
        return value % 7;
    }
    return idx->m_idx;
}

int aiChrIdx::GetFighterNo(const aiChrIdx* idx) {
    s8 value = idx->m_idx;
    if (value >= 0 && value < 14) {
        return value / 7;
    }
    return 0;
}

bool aiChrIdx::IsValid(const aiChrIdx* idx) {
    return idx->m_idx >= 0;
}

bool aiChrIdx::IsPlayer(const aiChrIdx* idx) {
    bool result = false;
    if (idx->m_idx >= 0) {
        if (idx->m_idx < 14) {
            result = true;
        }
    }
    return result;
}

// Danger zones: 24 axis-aligned boxes the CPUs avoid (hazards, projectiles, ...). Slots 0-7 are the stage's zones and
// 8-23 the dynamic ones. m_active (+0x32C, one byte per slot, bit 0 = in use, the rest an attribute mask), the
// box corners (+0x14C, 0x14 bytes per slot: minX, minY, maxX, maxY, team) and the in-use count (+0x149).
void aiMgr::delDangerZone(int index) {
    if (index < 0) {
        return;
    }
    if ((u32)index >= 0x18) {
        return;
    }
    aiMgrDangerView* view = reinterpret_cast<aiMgrDangerView*>(this);
    if (view->zoneUsed[index] != 0) {
        view->zoneUsed[index] = 0;
        aiMgrField<s8>(this, 0x149)--;
    }
}

void aiMgr::clearDangerZone(int index) {
    if (index == 0 && (aiMgrField<u16>(this, 0xA2) & 0x100) != 0) {
        return;
    }
    int last = 0x18;
    int first = 0;
    switch (index) {
    case 0:
        last = 8;
        break;
    case 1:
        first = 8;
        break;
    }
    for (int i = first; i < last; i++) {
        delDangerZone(i);
    }
}

int aiMgr::setDangerZone(Vec2f* p1, Vec2f* p2, int index, bool unk, bool attr) {
    int mode = unk;
    if (mode == 0 && (aiMgrField<u16>(this, 0xA2) & 0x100) != 0) {
        return -1;
    }
    int first;
    int last;
    if (mode == 1) {
        first = 8;
        last = 0x18;
    } else {
        if (aiMgrField<int>(this, 0x370) == 0x1F) {
            return -1;
        }
        first = 0;
        last = 8;
    }
    if (index < first || last <= index) {
        if (index >= 0) {
            return -1;
        }
        index = first;
        for (int i = first; i < last; i++) {
            if (aiMgrField<u8>(this, 0x32C + index) == 0) {
                break;
            }
            index++;
        }
        if ((u32)index >= 0x18) {
            return -1;
        }
    }
    float x0 = p1->m_x;
    float x1 = p2->m_x;
    if (x0 <= x1) {
        aiMgrField<float>(this, 0x14C + index * 0x14) = x0;
        aiMgrField<float>(this, 0x154 + index * 0x14) = x1;
    } else {
        aiMgrField<float>(this, 0x154 + index * 0x14) = x0;
        aiMgrField<float>(this, 0x14C + index * 0x14) = x1;
    }
    float y0 = p1->m_y;
    float y1 = p2->m_y;
    if (y0 <= y1) {
        aiMgrField<float>(this, 0x150 + index * 0x14) = y0;
        aiMgrField<float>(this, 0x158 + index * 0x14) = y1;
    } else {
        aiMgrField<float>(this, 0x158 + index * 0x14) = y0;
        aiMgrField<float>(this, 0x150 + index * 0x14) = y1;
    }
    if (aiMgrField<u8>(this, 0x32C + index) == 0) {
        aiMgrField<s8>(this, 0x149)++;
        aiMgrField<u8>(this, 0x32C + index) = attr | 1;
        aiMgrField<int>(this, 0x15C + index * 0x14) = -1;
    }
    return index;
}

void aiMgr::setDangerAttr(int index, u8 attr) {
    if (index < 0) {
        return;
    }
    if ((u32)index >= 0x18) {
        return;
    }
    aiMgrDangerView* view = reinterpret_cast<aiMgrDangerView*>(this);
    u8 current = view->zoneUsed[index];
    if (current != 0) {
        view->zoneUsed[index] = current | attr;
    }
}

void aiMgr::setDangerTeam(int index, int team) {
    if (index < 0) {
        return;
    }
    if ((u32)index >= 0x18) {
        return;
    }
    if (reinterpret_cast<aiMgrDangerView*>(this)->zoneUsed[index] != 0) {
        aiMgrField<int>(this, 0x15C + index * 0x14) = team;
    }
}

// Whether a point lies inside an active danger zone (other than the ones registered for the given team, if team >= 0,
// and only those carrying one of the attribute bits, if attr != 0).
bool aiMgr::chkDangerZone(Vec2f* pos, int team, u8 attr) {
    aiMgr* manager = g_aiMgr;
    if (aiMgrField<u8>(manager, 0x149) != 0) {
        u8* base = reinterpret_cast<u8*>(manager);
        float* box = &aiMgrField<float>(manager, 0x14C);
        // two zones per iteration (the original loop is unrolled by two)
        for (int i = 0; i < 12; i++, box += 10, base += 2) {
            if (base[0x32C] != 0 && (team < 0 || *reinterpret_cast<int*>(&box[4]) != team)) {
                if (attr == 0 || (attr & base[0x32C]) != 0) {
                    if (!(pos->m_x > box[2]) && !(pos->m_x < box[0]) && !(pos->m_y > box[3]) && !(pos->m_y < box[1])) {
                        return true;
                    }
                }
            }
            if (base[0x32D] != 0 && (team < 0 || *reinterpret_cast<int*>(&box[9]) != team)) {
                if (attr == 0 || (attr & base[0x32D]) != 0) {
                    if (!(pos->m_x > box[7]) && !(pos->m_x < box[5]) && !(pos->m_y > box[8]) && !(pos->m_y < box[6])) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}
