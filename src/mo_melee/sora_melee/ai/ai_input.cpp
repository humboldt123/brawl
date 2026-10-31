#include <ai/ai_input.h>
#include <ai/ai_act.h>
#include <ai/ai_enemy.h>
#include <ai/ai_stat.h>
#include <gf/gf_heap_manager.h>

void aiInput::reset() {
    s8 target[16];
    target[0] = -1;
    change_md(0, target, 0, 0);
    m_md = 0;
    m_target = m_chrIdx.m_idx;
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
    m_swingChr = -1;
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
    m_swingChr = -1;
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
