#include <ai/ai_cl_block.h>
#include <ai/ai_enemy.h>
#include <ai/ai_input.h>
#include <ai/ai_mgr.h>
#include <ai/ai_stat.h>
#include <ai/ai_weapon.h>
#include <ai/ai_param.h>
#include <ft/ft_manager.h>
#include <gf/gf_archive.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_memory_util.h>
#include <gm/gm_lib.h>
#include <st/stage.h>
#include <st_madein/st_madein.h>
#include <types.h>

// The CPU player AI manager (aiMgr) and its helpers. HYPOTHESIS: the member names follow how the functions use them.

extern "C" {
// aiMgr readiness flag: -1 while the AI is disabled, 0.. once the floor information has been prepared
extern s8 lbl_27_data_47BF8;
}

// Takes the common AI pac: the refer and system parameter tables and the common script part.
// HYPOTHESIS: owned = the manager frees the data when it is replaced (the caller passes 0).
void aiMgr::pacCmnRegist(void* data, int size, bool owned) {
    if (g_aiMgr != NULL && data != NULL) {
        gfArchive archive;
        archive.setFileImage(data, size, Heaps::Tmp);
        if (g_aiMgr->m_pacData != NULL && g_aiMgr->m_pacOwned) {
            if (g_aiMgr->m_pacData != NULL) {
                gfHeapManager::free(g_aiMgr->m_pacData);
                g_aiMgr->m_pacData = NULL;
            }
            g_aiMgr->m_pacOwned = false;
        }
        g_aiMgr->m_pacData = data;
        g_aiMgr->m_pacOwned = owned;
        aiMgr* mgr = g_aiMgr;
        mgr->m_referPar = static_cast<aiReferPar*>(archive.getData(0, Data_Type_Misc, 0, 0xFFFE));
        mgr->m_systemPar = archive.getData(0, Data_Type_Misc, 1, 0xFFFE);
        mgr->m_unk134 = archive.getData(2, Data_Type_Misc, 0, 0xFFFE);
        if (mgr->m_aiInput != NULL) {
            for (int i = 0; i < NumChrs; i++) {
                if (mgr->m_aiInput[i] != NULL) {
                    mgr->m_aiInput[i]->setCpuType(-1);
                }
            }
        }
    }
}

AiClBlock::AiClBlock() { }

tag_AiClJointList::tag_AiClJointList() { }

aiMgr::~aiMgr() {
    if (m_stat != NULL) {
        delete[] m_stat;
        m_stat = NULL;
    }
    if (m_weaponMgr != NULL) {
        delete m_weaponMgr;
        m_weaponMgr = NULL;
    }
    if (m_enemyMgr != NULL) {
        delete m_enemyMgr;
        m_enemyMgr = NULL;
    }
    if (m_jointLists != NULL) {
        delete[] m_jointLists;
        m_jointLists = NULL;
    }
    if (m_blocks != NULL) {
        delete[] m_blocks;
        m_blocks = NULL;
    }
    if (m_pacOwned == 1 && m_pacData != NULL) {
        gfHeapManager::free(m_pacData);
        m_pacData = NULL;
    }
    g_aiMgr = NULL;
}

bool aiMgr::isReady() {
    return lbl_27_data_47BF8 > 0;
}

bool aiMgr::isReadyUpdate() {
    return lbl_27_data_47BF8 >= 0;
}

char* aiMgr::getMadeinAiData(bool includePreparation) {
    if (g_aiMgr->m_stageKind == Stages::Madein && g_Stage != NULL) {
        stMadeinAiData* data = static_cast<stMadeinAiData*>(g_Stage->getMadeinAiData());
        // The stage sets +1 while the microgame is running; +0 exposes its
        // preparation phase only to callers that request it.
        if (data != NULL && (data->m_unk1 != 0 || (includePreparation && data->m_active != 0))) {
            return reinterpret_cast<char*>(data);
        }
    }
    return NULL;
}

bool aiMgr::isSmallMap() {
    switch (g_aiMgr->m_stageKind) {
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
    return g_aiMgr->m_systemPar;
}

aiReferPar* aiMgr::getAiReferPar(int index) {
    if (index < 0 || index > 0x35) {
        index = 0;
    }
    return g_aiMgr->m_referPar + index;
}

int aiMgr::getCpuType(int player) {
    if (player >= 0 && player < 7 && m_aiInput != NULL && m_aiInput[player] != NULL) {
        return m_aiInput[player]->m_cpuType;
    }
    return 0;
}

int aiMgr::getCpuRank(int player) {
    if (player >= 0 && player < 7 && m_aiInput != NULL && m_aiInput[player] != NULL) {
        return m_aiInput[player]->m_par.m_rank;
    }
    return 0;
}

int aiMgr::getAiCpuTarget(int player) {
    if (player >= 0 && player < 7 && m_aiInput != NULL && m_aiInput[player] != NULL) {
        return m_aiInput[player]->m_target.m_idx;
    }
    return -1;
}

bool aiMgr::chkNanaForceMove(int player) {
    if (m_aiInput != NULL && player >= 0 && player < 7 && m_aiInput[player + 7] != NULL) {
        aiInput* nana = m_aiInput[player + 7];
        if (nana->m_md == 0x79 && (nana->m_flags & 8) == 0) {
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
// 8-23 the dynamic ones. m_dangerAttr (one byte per slot, bit 0 = in use, the rest an attribute mask), m_dangerZone (the
// box corners and the team) and m_numDangerZones (the in-use count).
void aiMgr::delDangerZone(int index) {
    if (index < 0 || (u32)index >= 0x18 || m_dangerAttr[index] == 0) {
        return;
    }
    m_dangerAttr[index] = 0;
    m_numDangerZones--;
}

void aiMgr::clearDangerZone(int index) {
    if (index == 0 && (m_flags & 0x100) != 0) {
        return;
    }
    int first = 0;
    int last = 0x18;
    switch (index) {
    case 0:
        last = 8;
        break;
    case 1:
        first = 8;
        break;
    }
    for (; first < last; first++) {
        delDangerZone(first);
    }
}

int aiMgr::setDangerZone(Vec2f* p1, Vec2f* p2, int index, bool unk, bool attr) {
    int mode = unk;
    if (mode == 0 && (m_flags & 0x100) != 0) {
        return -1;
    }
    int first;
    int last;
    if (mode == 1) {
        first = 8;
        last = 0x18;
    } else {
        if (m_stageKind == 0x1F) {
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
            if (m_dangerAttr[index] == 0) {
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
        m_dangerZone[index].m_minX = x0;
        m_dangerZone[index].m_maxX = x1;
    } else {
        m_dangerZone[index].m_maxX = x0;
        m_dangerZone[index].m_minX = x1;
    }
    float y0 = p1->m_y;
    float y1 = p2->m_y;
    if (y0 <= y1) {
        m_dangerZone[index].m_minY = y0;
        m_dangerZone[index].m_maxY = y1;
    } else {
        m_dangerZone[index].m_maxY = y0;
        m_dangerZone[index].m_minY = y1;
    }
    if (m_dangerAttr[index] == 0) {
        m_numDangerZones++;
        m_dangerAttr[index] = attr | 1;
        m_dangerZone[index].m_team = -1;
    }
    return index;
}

void aiMgr::setDangerAttr(int index, u8 attr) {
    u8 current;
    if (index < 0 || (u32)index >= 0x18 || (current = m_dangerAttr[index]) == 0) {
        return;
    }
    m_dangerAttr[index] = current | attr;
}

void aiMgr::setDangerTeam(int index, int team) {
    if (index < 0 || (u32)index >= 0x18 || m_dangerAttr[index] == 0) {
        return;
    }
    u8* zone = reinterpret_cast<u8*>(this) + index * 0x14;
    *reinterpret_cast<int*>(zone + 0x15C) = team;
}

// Whether a point lies inside an active danger zone (other than the ones registered for the given team, if team >= 0,
// and only those carrying one of the attribute bits, if attr != 0).
bool aiMgr::chkDangerZone(Vec2f* pos, int team, u8 attr) {
    aiMgr* manager = g_aiMgr;
    if (manager->m_numDangerZones != 0) {
        // MATCH-ONLY: the attribute bytes are read through a byte pointer that starts at the manager (m_dangerAttr is at
        // +0x32C), and the zones through a float pointer; typed array indexing did not reproduce the loop (the original
        // steps one pointer by 2 bytes and one by 0x28 per iteration). Two zones per iteration (the loop is unrolled by two).
        u8* base = reinterpret_cast<u8*>(manager);
        float* box = &manager->m_dangerZone[0].m_minX;
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
            stat = &g_aiMgr->m_stat[index];
        } else {
            aiEnemy* enemy = g_aiMgr->m_enemyMgr->getEnemy(index - 14);
            stat = &g_aiMgr->m_stat[14];
            if (g_aiMgr->m_unkA4 != idx->m_idx - 14 && enemy != NULL) {
                stat->update_enemy(enemy, idx);
                g_aiMgr->m_unkA4 = idx->m_idx - 14;
            }
        }
    } else {
        stat = &g_aiMgr->m_stat[14];
    }
    return stat;
}

void aiMgr::setEnable(int enable) {
    g_aiMgr->m_enable = enable;
    if (enable == 0) {
        lbl_27_data_47BF8 = -1;
        if (g_aiMgr->m_weaponMgr != NULL) {
            g_aiMgr->m_weaponMgr->reset();
        }
        if (g_aiMgr->m_enemyMgr != NULL) {
            g_aiMgr->m_enemyMgr->reset();
        }
        g_aiMgr->CrearWork();
    }
}

void aiMgr::CrearWork() {
    m_unkA8 = 0;
    m_unkA4 = -1;
    for (int i = 0; i < 24; i++) {
        m_dangerAttr[i] = 0;
    }
    m_numDangerZones = 0;
    m_unk104 = 0;
    m_unk110 = 0;
    m_unk11C = 0;
    m_unk128 = 0;
    m_unk344 = 1;
    m_unk35C = 0;
    m_unk35A = 0;
    m_unk358 = 0;
    m_numBlocks = 0;
    m_unk354 = 0;
    m_areaLeft = -99999.9f;
    m_areaBottom = -99999.9f;
    m_areaRight = 99999.9f;
    m_areaTop = 99999.9f;
    m_stageKind = 0;
    m_unk378 = 0;
    m_unk374 = 0;
    m_unk37C = 0.0f;
    m_unk380 = 0;
    m_unk384 = 0;
    m_unk385 = 0;
    m_unk386 = 0;
    gfMemFill(m_work, 0, sizeof(m_work));
}

void aiMgr::set_ai_player(int player, s8 state, aiInput* input, int ftKind) {
    if (m_aiInput != NULL && player >= 0 && player < NumPlayers) {
        m_chrState[player] = state;
        if (input != NULL) {
            m_aiInput[player] = input;
            m_stat[player].init(player, ftKind, input, 0);
            if (m_cpuTypeLock[player] >= 0) {
                m_aiInput[player]->setCpuType(m_cpuTypeLock[player]);
            }
            if (m_cpuRankLock[player] >= 0) {
                aiInput* cpu = m_aiInput[player];
                cpu->m_par.setRank(m_cpuRankLock[player], 0, (cpu->m_flags >> 15) & 1);
            }
        }
        if (m_aiInput[player] != NULL) {
            if (state != 0 && state != m_chrState[player] && input == NULL) {
                m_aiInput[player]->reset();
            }
            aiInput* child = m_aiInput[player]->m_child;
            if (child != NULL) {
                if (state == 0) {
                    aiSystemPar* systemPar = static_cast<aiSystemPar*>(getAiSystemPar());
                    child->m_par.setRank((s8)systemPar->m_childRank, 0, (child->m_flags >> 15) & 1);
                } else {
                    child->m_par.setRank(m_aiInput[player]->m_par.m_rank, 0, (child->m_flags >> 15) & 1);
                }
                child->setCpuType(0);
            }
            m_aiInput[player]->setEnableRumble(state == 0);
        }
    }
}

void aiMgr::setCpuType(int player, int type) {
    if (m_aiInput != NULL && player >= 0 && player < 7) {
        aiInput* input = m_aiInput[player];
        if (input != NULL) {
            if (m_enable != 1 && m_cpuTypeLock[player] >= 0) {
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
            if (m_enable == 1 || m_cpuRankLock[player] < 0) {
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
    m_chrState[index] = 1;
    m_aiInput[index] = input;
    m_stat[index].init(index, chrKind, input, 0);
    if (input != NULL) {
        input->setEnableRumble(false);
    }
}

void aiMgr::notifyEventChangeStart(int entryId, int playerNo, int activeInstanceIndex, ftKind kind) {
    setEnablePlayer(playerNo, 0, 1);
    m_unk385 = 0xF0;
}

void aiMgr::notifyEventChangeEnd(int entryId, int playerNo, int activeInstanceIndex, ftKind kind) {
    if (playerNo < 7) {
        if (m_aiInput[playerNo] != NULL) {
            m_aiInput[playerNo]->changeFtKind(kind);
            m_stat[playerNo].change(playerNo, kind, m_aiInput[playerNo], activeInstanceIndex);
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
    int count = m_numBlocks;
    AiClBlock* block = m_blocks;
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
                if ((flags & 0x800) != 0 && (team < 0 || team >= 8 || (block->m_teamMask & (1 << team)) != 0)) {
                    danger = true;
                } else {
                    danger = false;
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
    int count = m_numBlocks;
    AiClBlock* block = m_blocks;
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
                if ((flags & 0x800) != 0 && (team < 0 || team >= 8 || (block->m_teamMask & (1 << team)) != 0)) {
                    danger = true;
                } else {
                    danger = false;
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
    AiClBlock* block = m_blocks;
    AiClBlock* result = NULL;
    float best = 9999979520.0f;
    int count = m_numBlocks;
    for (int i = 0; i < count; i++, block++) {
        u32 flags = block->m_flags;
        if ((flags & 2) != 0) {
            Vec2f center = (block->m_edgeL + block->m_edgeR) * 0.5f;
            float centerY = center.m_y - 20.0f;
            float score = center.m_x * center.m_x + centerY * centerY;
            if (block->m_edgeL.m_x > 0.0f || block->m_edgeR.m_x < 0.0f) {
                score += 100.0f;
            }
            if (block->m_top < m_areaBottom + 40.0f
                || m_areaTop - 40.0f < block->m_bottom) {
                score += 10000.0f;
            }
            if ((flags & 0x10000) != 0) {
                score += 10000000.0f;
            } else if ((flags & 0x1000) != 0) {
                score += 100000.0f;
            } else {
                bool danger;
                if ((flags & 0x800) != 0 && (team < 0 || team >= 8 || (block->m_teamMask & (1 << team)) != 0)) {
                    danger = true;
                } else {
                    danger = false;
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
    int count = m_numBlocks;
    AiClBlock* block = m_blocks;
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
        AiClBlock* block = m_blocks;
        for (int count = m_numBlocks; count > 0; count--) {
            bool match;
            if (block->m_joint == NULL) {
                match = false;
            } else {
                match = false;
                if (key[1] == (u32)block->m_unk48 && key[0] == block->m_joint->m_id) {
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
            if ((stat->m_flags & 0xC00) != 0 || stat->chk_chr_stat(2, 0x12)) {
                return stat;
            }
        }
        i++;
    } while (i < 7);
    return NULL;
}

void aiMgr::removePlayer(aiChrIdx* idx) {
    if (aiChrIdx::IsPlayer(idx)) {
        removeTgtRefer(aiChrIdx::GetPlayerNo(idx), 0);
        int slot = idx->m_idx;
        m_chrState[slot] = -1;
        aiInput* input = m_aiInput[slot];
        if (input != NULL) {
            input->setEnableRumble(false);
        }
        m_aiInput[idx->m_idx] = NULL;
        get_aiStat(idx)->init(idx->m_idx, -1, NULL, 0);
    }
}

AiClBlock* aiMgr::get_via_blk(AiClBlock* from, int unk1, float x, Vec2f* pos, int unk2, int unk3, float f1, float f2, float f3) {
    return get_via_blk_team(from, unk1, x, -1, pos, unk2, unk3, f1, f2, f3);
}

bool aiMgr::chk_cl_cross(Vec2f* from, Vec2f* to, u32 mask, Vec2f* hitPos, AiClBlock* ignore) {
    return chk_cl_cross2(from, to, mask, hitPos, ignore) != NULL;
}

// Whether the input of the character in this slot still comes from its controller: true until the AI has its floor
// information, and for every slot that the AI has not registered.
bool aiMgr::chkIpHumanUpdate(aiChrIdx* idx) {
    if (aiChrIdx::IsPlayer(idx) && aiChrIdx::GetFighterNo(idx) == 0) {
        int player = aiChrIdx::GetPlayerNo(idx);
        if (m_jointLists == NULL || m_chrState[player] <= 0) {
            return true;
        }
    }
    return false;
}

bool aiMgr::chkChildOptionMd(int player) {
    if (player >= 0 && player < 7 && m_aiInput != NULL && m_aiInput[player] != NULL) {
        aiInput* child = m_aiInput[player]->m_child;
        if (child != NULL && child->m_md == 0x79) {
            if ((get_aiStat(&child->m_chrIdx)->m_flags & 0x8000) == 0) {
                return true;
            }
        }
    }
    return false;
}

void aiMgr::renderDebug() {
}

void aiMgr::setAdventureMd(u32 md) {
    u32 current = g_aiMgr->m_adventureMd;
    if (current == md) {
        return;
    }
    g_aiMgr->m_adventureMd = md;
    for (int i = 0; i < 14; i++) {
        aiInput* input = g_aiMgr->m_aiInput[i];
        if (input != NULL) {
            input->m_unk112 = g_aiMgr->m_adventureMd;
        }
    }
}
