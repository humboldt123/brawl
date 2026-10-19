#include <ai/ai_input.h>
#include <gf/gf_heap_manager.h>

void aiInput::reset() {
    s8 target[16];
    target[0] = -1;
    change_md(0, target, 0, 0);
    m_md = 0;
    m_unk110 = m_chrIdx.m_idx;
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
        m_swingChr = m_unk110;
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
