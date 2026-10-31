#include <ai/ai_input.h>
#include <ai/ai_act.h>
#include <ai/ai_enemy.h>
#include <ai/ai_stat.h>
#include <gf/gf_archive.h>
#include <gf/gf_heap_manager.h>

aiInput::aiInput(u32 contNo, int unk2C, int player, int ftKind, s8 state, aiInput* parent, Heaps::HeapType heap)
    : IpHuman(contNo) {
    m_unk2C = unk2C;
    m_ftKind = ftKind;
    m_chrIdx.m_idx = player + (parent != NULL) * aiMgr::NumPlayers;
    m_unk43.m_idx = 0;
    m_unk112 = g_aiMgr->m_adventureMd;
    m_cpuType = 0;
    m_unk114 = 1;
    m_unk116 = 0x2FF;
    m_flags = 0x8000;
    m_target.m_idx = m_chrIdx.m_idx;
    m_unk58 = 0;
    m_unk5A = 0;
    m_unk6A = 0;
    m_unk6C = 1;
    m_unk74 = 0;
    m_unk60 = 0;
    m_unk61 = 0;
    m_unk62 = 0;
    m_unk63 = 0;
    m_unk64 = 0;
    m_unk65 = 0;
    m_unk66 = 0;
    m_unk68 = 0;
    m_unk67 = -1;
    m_unk75 = 0;
    m_child = NULL;
    m_unk84 = 0;
    m_unk88 = 0;
    if (parent == NULL) {
        aiMgr* mgr = g_aiMgr;
        s8 current;
        if (player < 0 || (u32)player >= 14) {
            current = -1;
        } else {
            current = mgr->m_chrState[player];
        }
        if (current == -1) {
            current = state;
        }
        mgr->set_ai_player(player, current, this, ftKind);
    } else {
        parent->m_child = this;
        aiSystemPar* systemPar = (aiSystemPar*)aiMgr::getAiSystemPar();
        m_par.setRank((s8)systemPar->m_childRank, 0, 1);
        aiMgr* mgr = g_aiMgr;
        mgr->regist_ai_child(aiChrIdx::GetPlayerNo(&parent->m_chrIdx), this, ftKind);
    }
    m_act = new (heap) aiAct(this, aiMgr::get_aiStat(&m_chrIdx));
    s8 target[16];
    target[0] = -1;
    m_act->act_change(0, target, 0, 0);
    m_md = m_mdNext = parent == NULL ? 1 : 0x79;
    m_unk50 = 0;
    m_unk54 = 0;
    m_unk5C = 0;
    m_unk5E = 0;
    m_unk8A = 0;
    m_unk111 = 0;
    m_pacData = NULL;
    m_unk70 = 0;
    m_unk3E = 0;
    m_unk3F = 0;
    m_unk8C = 0.0f;
    m_unk118 = 0.0f;
    m_swingChr.m_idx = -1;
    m_swingState = 0;
    m_unk78 = 0.0f;
    m_unk7C = 0.0f;
    m_unk80 = 0.0f;
    m_unk77 = 0;
    m_unk76 = 0;
}

void aiInput::reset() {
    s8 target[16];
    target[0] = -1;
    change_md(0, target, 0, 0);
    m_md = 0;
    m_target.m_idx = m_chrIdx.m_idx;
    m_unk58 = 0;
    m_unk5A = 0;
    m_unk6A = 0;
    m_unk74 = 0;
    m_unk60 = 0;
    m_unk61 = 0;
    m_unk62 = 0;
    m_unk63 = 0;
    m_unk64 = 0;
    m_unk65 = 0;
    m_unk66 = 0;
    m_unk68 = 0;
    m_unk67 = 0xFF;
    m_unk88 = 0;
    m_swingChr.m_idx = -1;
    m_swingState = 0;
    m_unk8C = 0.0f;
    m_unk78 = 0.0f;
    m_unk7C = 0.0f;
    m_unk80 = 0.0f;
    m_unk77 = 0;
    m_unk76 = 0;
}

void aiInput::swingChkReset(int force) {
    if (force == 0 && m_swingState >= 2) {
        return;
    }
    m_swingChr.m_idx = -1;
    m_swingState = 0;
}

void aiInput::swingChkSet(int hit) {
    if (aiChrIdx::GetFighterNo(&m_chrIdx) <= 0) {
        m_swingChr = m_target;
        if (hit != 0 && m_swingState == 0) {
            m_swingState = 1;
        }
    }
}

void aiInput::setEnable(u32 enable) {
    if (enable == 1 && (m_flags & 0x8000) == 0) {
        reset();
    }
    if (enable != 0) {
        m_flags |= 0x8000;
    } else {
        m_flags &= 0x7FFF;
    }
}

void aiInput::changeFtKind(int ftKind) {
    m_ftKind = ftKind;
    m_unk118 = 0.0f;
    reset();
    m_par.init_par(NULL, -1, NULL);
    if ((m_flags & 0x40) != 0) {
        if (m_pacData != NULL) {
            gfHeapManager::free(m_pacData);
            m_pacData = NULL;
        }
        m_flags &= ~0x40;
    }
}

void aiInput::pacRegist(void* data, int size, int owned, int check) {
    if (check != 0) {
        gfArchive archive;
        archive.setFileImage(data, size, Heaps::Tmp);
        aiParam::check_revision(static_cast<char*>(archive.getData(1, Data_Type_Misc, 0, 0xFFFE)));
    }
    m_par.init_par(NULL, -1, NULL);
    if (m_pacData != NULL && (m_flags & 0x40) != 0) {
        if (m_pacData != NULL) {
            gfHeapManager::free(m_pacData);
            m_pacData = NULL;
        }
        m_flags &= ~0x40;
    }
    if (m_ftKind <= -1 || (u32)m_ftKind >= 0x36) {
        return;
    }
    m_pacData = data;
    if (owned != 0) {
        m_flags |= 0x40;
    } else {
        m_flags &= ~0x40;
    }
    if (m_pacData != NULL) {
        gfArchive archive;
        archive.setFileImage(data, size, Heaps::Tmp);
        aiParData* par = static_cast<aiParData*>(archive.getData(1, Data_Type_Misc, 0, 0xFFFE));
        if (par != NULL) {
            char* atkData = static_cast<char*>(archive.getData(3, Data_Type_Misc, 0, 0xFFFE));
            m_par.init_par(par, m_ftKind, atkData);
            m_act->script_reload(archive.getData(2, Data_Type_Misc, 0, 0xFFFE));
        }
    }
}

void aiInput::setCpuType(int cpuType) {
    if (cpuType >= 0 && cpuType < 0x1D) {
        if (m_cpuType != cpuType && m_par.m_par != NULL) {
            reset();
        }
        m_cpuType = cpuType;
    }
    aiSystemPar* systemPar = (aiSystemPar*)aiMgr::getAiSystemPar();
    if (systemPar != NULL) {
        aiCpuTypePar* row;
        if (m_cpuType < 0x19) {
            row = &systemPar->m_cpuType[m_cpuType];
        } else {
            row = &systemPar->m_cpuType[0];
        }
        m_unk116 = row->m_unk0;
        m_unk114 = row->m_unk2;
    }
}

aiInput::~aiInput() {
    g_aiMgr->removePlayer(&m_chrIdx);
    if ((m_flags & 0x40) != 0) {
        if (m_pacData != NULL) {
            gfHeapManager::free(m_pacData);
            m_pacData = NULL;
        }
        m_flags &= ~0x40;
    }
    if (m_act != NULL) {
        delete m_act;
        m_act = NULL;
    }
}

// The CPU only fills in the input of its own character: the controller getters return the members of IpHuman that the
// AI wrote this frame.
Vec2f aiInput::getStickMain() const {
    return m_stickMain;
}

Vec2f aiInput::getStickSub() const {
    return m_stickSub;
}

ipPadButton aiInput::getButton() const {
    return m_button;
}

ipPadTrigger aiInput::getTrigger() const {
    return m_trigger;
}

s32 aiInput::getContNo() const {
    return m_contNo;
}

// The character slot that this CPU has to attack no matter what (the four CPU types 0xE..0x11 of the Subspace Emissary
// "follow player N" scripts), or, when the stage is 0x1C, the slot of the enemy nearest to it. Falls back to its own slot.
// HYPOTHESIS: the meaning of the aiStat flags 0x8000 / 0x40000000 and of m_unkCC (probably the team) is not known.
s8 aiInput::get_fix_target(float* distance) {
    u32 cpuType = m_cpuType;
    if (cpuType >= 0xE && cpuType <= 0x11) {
        s8 target = cpuType - 0xE;
        aiStat* self = aiMgr::get_aiStat(&m_chrIdx);
        aiChrIdx targetIdx;
        targetIdx.m_idx = target;
        aiStat* other = aiMgr::get_aiStat(&targetIdx);
        if (target != m_chrIdx.m_idx) {
            u32 flags = other->m_flags;
            if ((flags & 0x8000) == 0 || (flags & 0x40000000) != 0) {
                if (other->m_unkCC != self->m_unkCC) {
                    if (distance != NULL) {
                        aiChrIdx distIdx;
                        distIdx.m_idx = target;
                        *distance = self->get_distance(0.0f, &distIdx);
                    }
                    return target;
                }
            }
        }
    }
    switch (g_aiMgr->m_stageKind) {
    case 0x1C: {
        aiStat* self = aiMgr::get_aiStat(&m_chrIdx);
        aiEnemy* enemy = g_aiMgr->m_enemyMgr->getEnemyNear(&self->m_center, distance);
        if (enemy != NULL) {
            return enemy->m_index + 0xE;
        }
        break;
    }
    }
    return m_chrIdx.m_idx;
}

// Defined here, between the aiInput functions of the original unit.
aiWeaponMgr* aiMgr::getWeaponMgr() {
    return g_aiMgr->m_weaponMgr;
}
