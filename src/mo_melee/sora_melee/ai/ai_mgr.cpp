#include <ai/ai_cl_block.h>
#include <ai/ai_enemy.h>
#include <ai/ai_input.h>
#include <ai/ai_mgr.h>
#include <ai/ai_stat.h>
#include <ai/ai_weapon.h>
#include <ai/ai_param.h>
#include <ft/ft_manager.h>
#include <gf/gf_memory_util.h>
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

// Element `index` of a byte/word table at `base`; the index is added to the object first (matches `add r3, r3, r4`).
template <class T> static inline T& aiMgrArray(aiMgr* manager, int base, int index) {
    return *reinterpret_cast<T*>(reinterpret_cast<u8*>(manager) + index + base);
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

static inline bool aiIndexValid(s8 index) {
    bool valid = false;
    if (index >= 0 && index < 14) {
        valid = true;
    }
    return valid;
}

aiStat* aiMgr::get_aiStat(aiChrIdx* idx) {
    aiStat* stat;
    s8 index = idx->m_idx;
    if (index >= 0) {
        if (aiIndexValid(index)) {
            stat = &aiMgrField<aiStat*>(g_aiMgr, 0xF8)[index];
        } else {
            aiEnemy* enemy = aiMgrField<aiEnemyMgr*>(g_aiMgr, 0x100)->getEnemy(index - 14);
            stat = &aiMgrField<aiStat*>(g_aiMgr, 0xF8)[14];
            if (aiMgrField<s8>(g_aiMgr, 0xA4) != idx->m_idx - 14 && enemy != NULL) {
                stat->update_enemy(enemy, idx);
                aiMgrField<s8>(g_aiMgr, 0xA4) = idx->m_idx - 14;
            }
        }
    } else {
        stat = &aiMgrField<aiStat*>(g_aiMgr, 0xF8)[14];
    }
    return stat;
}

void aiMgr::setEnable(int enable) {
    aiMgrField<s8>(g_aiMgr, 0xA0) = enable;
    if (enable == 0) {
        lbl_27_data_47BF8 = -1;
        if (aiMgrField<aiWeaponMgr*>(g_aiMgr, 0xFC) != NULL) {
            aiMgrField<aiWeaponMgr*>(g_aiMgr, 0xFC)->reset();
        }
        if (aiMgrField<aiEnemyMgr*>(g_aiMgr, 0x100) != NULL) {
            aiMgrField<aiEnemyMgr*>(g_aiMgr, 0x100)->reset();
        }
        g_aiMgr->CrearWork();
    }
}

void aiMgr::CrearWork() {
    aiMgrField<int>(this, 0xA8) = 0;
    aiMgrField<s8>(this, 0xA4) = -1;
    aiMgrDangerView* view = reinterpret_cast<aiMgrDangerView*>(this);
    for (int i = 0; i < 24; i++) {
        view->zoneUsed[i] = 0;
    }
    aiMgrField<u8>(this, 0x149) = 0;
    aiMgrField<u8>(this, 0x104) = 0;
    aiMgrField<u8>(this, 0x110) = 0;
    aiMgrField<u8>(this, 0x11C) = 0;
    aiMgrField<u8>(this, 0x128) = 0;
    aiMgrField<int>(this, 0x344) = 1;
    aiMgrField<u16>(this, 0x35C) = 0;
    aiMgrField<u16>(this, 0x35A) = 0;
    aiMgrField<u16>(this, 0x358) = 0;
    aiMgrField<u16>(this, 0x350) = 0;
    aiMgrField<u16>(this, 0x354) = 0;
    aiMgrField<float>(this, 0x360) = -99999.9f;
    aiMgrField<float>(this, 0x36C) = -99999.9f;
    aiMgrField<float>(this, 0x364) = 99999.9f;
    aiMgrField<float>(this, 0x368) = 99999.9f;
    aiMgrField<int>(this, 0x370) = 0;
    aiMgrField<int>(this, 0x378) = 0;
    aiMgrField<int>(this, 0x374) = 0;
    aiMgrField<float>(this, 0x37C) = 0.0f;
    aiMgrField<int>(this, 0x380) = 0;
    aiMgrField<u8>(this, 0x384) = 0;
    aiMgrField<u8>(this, 0x385) = 0;
    aiMgrField<u8>(this, 0x386) = 0;
    gfMemFill(&aiMgrField<u8>(this, 0xAC), 0, 0x4C);
}

void aiMgr::setCpuType(int player, int type) {
    if (m_aiInput != NULL && player >= 0 && player < 7) {
        aiInput* input = m_aiInput[player];
        if (input != NULL) {
            if (aiMgrField<u8>(this, 0xA0) != 1 && aiMgrArray<s8>(this, 0x92, player) >= 0) {
                return;
            }
            input->setCpuType(type);
        }
    }
}

void aiMgr::setCpuRank(int player, s8 rank) {
    if (m_aiInput != NULL && player >= 0 && player < 7) {
        aiInput* input = m_aiInput[player];
        if (input != NULL) {
            if (aiMgrField<u8>(this, 0xA0) == 1 || aiMgrArray<s8>(this, 0x99, player) < 0) {
                aiInput* child = input->m_child;
                input->m_par.setRank(rank, 0, (input->m_flags >> 15) & 1);
                if (child != NULL) {
                    child->m_par.setRank(rank, 0, (child->m_flags >> 15) & 1);
                }
            }
        }
    }
}

void aiMgr::regist_ai_child(int player, aiInput* input, int chrKind) {
    int index = player + 7;
    aiMgrArray<s8>(this, 0x84, index) = 1;
    m_aiInput[index] = input;
    aiMgrField<aiStat*>(this, 0xF8)[index].init(index, chrKind, input, 0);
    if (input != NULL) {
        input->setEnableRumble(false);
    }
}

void aiMgr::notifyEventChangeStart(int entryId, int playerNo, int activeInstanceIndex, ftKind kind) {
    setEnablePlayer(playerNo, 0, 1);
    aiMgrField<u8>(this, 0x385) = 0xF0;
}

void aiMgr::notifyEventChangeEnd(int entryId, int playerNo, int activeInstanceIndex, ftKind kind) {
    if (playerNo < 7) {
        if (m_aiInput[playerNo] != NULL) {
            m_aiInput[playerNo]->changeFtKind(kind);
            aiMgrField<aiStat*>(this, 0xF8)[playerNo].change(playerNo, kind, m_aiInput[playerNo], activeInstanceIndex);
        }
    }
}

void aiMgr::notifyEventDead(int entryId, int deadCount, Fighter::Dead::Reason deadReason, int respawnFrames) {
    int playerNo = g_ftManager->getPlayerNo(entryId);
    removeTgtRefer(playerNo, 1);
    if (playerNo < 0 || playerNo >= 7) {
        return;
    }
    if (g_aiMgr->m_aiInput[playerNo] != NULL) {
        g_aiMgr->m_aiInput[playerNo]->reset();
    }
    if ((&g_aiMgr->m_aiInput[7])[playerNo] != NULL) {
        (&g_aiMgr->m_aiInput[7])[playerNo]->reset();
    }
}

static inline tag_AI_ATK_DATA_HED* aiFindAtkDataHed(aiMgr* mgr, aiInput** inputs, int ftKind) {
    if (inputs != NULL) {
        for (int i = 0; i < 14; i++) {
            aiInput* input = inputs[i];
            if (input != NULL && (input->m_flags & 0x8000) != 0 && ftKind == input->m_ftKind) {
                return (tag_AI_ATK_DATA_HED*)inputs[i]->m_par.m_atkData;
            }
        }
    }
    return NULL;
}

void* aiMgr::searchAtkData(int ftKind, u32 id) {
    tag_AI_ATK_DATA_HED* hed = aiFindAtkDataHed(this, m_aiInput, ftKind);
    if (hed != NULL) {
        return hed->searchAtkData(id);
    }
    return NULL;
}

bool AiClBlock::chk_fl_danger(int team) {
    if ((m_flags & 0x800) != 0) {
        if (team < 0 || team >= 8 || (m_teamMask & (1 << team)) != 0) {
            return true;
        }
    }
    return false;
}

AiClBlock* aiMgr::get_top_block(int team) {
    int count = aiMgrField<u16>(this, 0x350);
    AiClBlock* block = aiMgrField<AiClBlock*>(this, 0x34C);
    float best = -99999.9f;
    AiClBlock* result = NULL;
    for (int i = 0; i < count; i++, block++) {
        u32 flags = block->m_flags;
        float value = block->m_top;
        if ((flags & 2) != 0 && !(value > 100.0f)) {
            if ((flags & 0x10000) != 0) {
                value -= 10000000.0f;
            } else if ((flags & 0x1000) != 0) {
                value -= 100000.0f;
            } else {
                bool danger;
                if ((flags & 0x800) == 0 || (team >= 0 && team < 8 && (block->m_teamMask & (1 << team)) == 0)) {
                    danger = false;
                } else {
                    danger = true;
                }
                if (danger) {
                    value -= 1000.0f;
                }
            }
            if (best < value) {
                best = value;
                result = block;
            }
        }
    }
    return result;
}

AiClBlock* aiMgr::get_right_block(int team) {
    int count = aiMgrField<u16>(this, 0x350);
    AiClBlock* block = aiMgrField<AiClBlock*>(this, 0x34C);
    float best = -99999.9f;
    AiClBlock* result = NULL;
    for (int i = 0; i < count; i++, block++) {
        u32 flags = block->m_flags;
        float value = block->m_edgeL.m_x;
        if ((flags & 2) != 0 && !(value > 100.0f)) {
            if ((flags & 0x10000) != 0) {
                value -= 10000000.0f;
            } else if ((flags & 0x1000) != 0) {
                value -= 100000.0f;
            } else {
                bool danger;
                if ((flags & 0x800) == 0 || (team >= 0 && team < 8 && (block->m_teamMask & (1 << team)) == 0)) {
                    danger = false;
                } else {
                    danger = true;
                }
                if (danger) {
                    value -= 1000.0f;
                }
            }
            if (best < value) {
                best = value;
                result = block;
            }
        }
    }
    return result;
}

AiClBlock* aiMgr::get_center_block(int team) {
    AiClBlock* block = aiMgrField<AiClBlock*>(this, 0x34C);
    AiClBlock* result = NULL;
    float best = 9999979520.0f;
    int count = aiMgrField<u16>(this, 0x350);
    for (int i = 0; i < count; i++, block++) {
        u32 flags = block->m_flags;
        if ((flags & 2) != 0) {
            Vec2f center = (block->m_edgeL + block->m_edgeR) * 0.5f;
            float centerY = center.m_y - 20.0f;
            float score = center.m_x * center.m_x + centerY * centerY;
            if (block->m_edgeL.m_x > 0.0f || block->m_edgeR.m_x < 0.0f) {
                score += 100.0f;
            }
            if (block->m_top < aiMgrField<float>(this, 0x36C) + 40.0f
                || aiMgrField<float>(this, 0x368) - 40.0f < block->m_bottom) {
                score += 10000.0f;
            }
            if ((flags & 0x10000) != 0) {
                score += 10000000.0f;
            } else if ((flags & 0x1000) != 0) {
                score += 100000.0f;
            } else {
                bool danger;
                if ((flags & 0x800) == 0 || (team >= 0 && team < 8 && (block->m_teamMask & (1 << team)) == 0)) {
                    danger = false;
                } else {
                    danger = true;
                }
                if (danger) {
                    score += 1000.0f;
                }
            }
            if (best > score) {
                best = score;
                result = block;
            }
        }
    }
    return result;
}

bool aiMgr::chkUnderFloor(float slope, Vec2f* pos) {
    int count = aiMgrField<u16>(this, 0x350);
    AiClBlock* block = aiMgrField<AiClBlock*>(this, 0x34C);
    for (int i = 0; i < count; i++, block++) {
        if ((block->m_flags & 2) != 0 && !(block->m_top > pos->m_y)) {
            if (block->m_edgeL.m_x > pos->m_x) {
                if (slope > 0.01f) {
                    if (pos->m_y - block->m_edgeL.m_y > (block->m_edgeL.m_x - pos->m_x) / slope) {
                        return true;
                    }
                } else if (pos->m_y > block->m_edgeL.m_y) {
                    return true;
                }
            } else {
                if (block->m_edgeR.m_x >= pos->m_x) {
                    return true;
                }
                if (slope > 0.01f) {
                    if (pos->m_y - block->m_edgeR.m_y > (pos->m_x - block->m_edgeR.m_x) / slope) {
                        return true;
                    }
                } else if (pos->m_y > block->m_edgeR.m_y) {
                    return true;
                }
            }
        }
    }
    return false;
}

AiClBlock* aiMgr::search_blk1(u32* key) {
    if (key[0] != 0) {
        AiClBlock* block = aiMgrField<AiClBlock*>(this, 0x34C);
        for (int count = aiMgrField<u16>(this, 0x350); count > 0; count--) {
            bool match;
            if (block->m_joint == NULL) {
                match = false;
            } else {
                match = false;
                if (key[1] == (u32)block->m_unk48 && key[0] == *(u32*)((u8*)block->m_joint + 0x1C)) {
                    match = true;
                }
            }
            if (match == true) {
                return block;
            }
            block++;
        }
    }
    return NULL;
}

aiStat* aiMgr::get_aiStatFinal() {
    int i = 0;
    do {
        aiChrIdx idx;
        idx.m_idx = i;
        aiStat* stat = get_aiStat(&idx);
        if ((stat->m_flags & 0x8000) == 0) {
            if ((stat->m_flags & 0xC00) != 0) {
                return stat;
            }
            if (stat->chk_chr_stat(2, 0x12)) {
                return stat;
            }
        }
        i++;
    } while (i < 7);
    return NULL;
}

void aiMgr::removePlayer(aiChrIdx* idx) {
    if (aiChrIdx::IsValid(idx)) {
        removeTgtRefer(aiChrIdx::GetPlayerNo(idx), 0);
        aiMgrArray<s8>(this, 0x84, idx->m_idx) = -1;
        aiInput* input = m_aiInput[idx->m_idx];
        if (input != NULL) {
            input->setEnableRumble(false);
        }
        m_aiInput[idx->m_idx] = NULL;
        get_aiStat(idx)->init(idx->m_idx, -1, NULL, 0);
    }
}

void aiMgr::setAdventureMd(u32 md) {
    u32 current = aiMgrField<u8>(g_aiMgr, 0xA1);
    if (current == md) {
        return;
    }
    aiMgrField<u8>(g_aiMgr, 0xA1) = md;
    for (int i = 0; i < 14; i++) {
        aiInput* input = g_aiMgr->m_aiInput[i];
        if (input != NULL) {
            input->m_unk112 = aiMgrField<u8>(g_aiMgr, 0xA1);
        }
    }
}
