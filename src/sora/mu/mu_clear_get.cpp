#include <mu/mu_clear_get.h>
#include <mu/mu_object.h>
#include <nw4r/g3d/g3d_scnmdlexpand.h>
#include <stdio.h>
#include <types.h>
#include <ut/ut_relocate.h>

extern MuAnimNameData lbl_804569E0[];

bool muClearGetTask::isEnd() {
    return m_state >= 10;
}

void muClearGetTask::setCursor() {
    char name[8];
    if (m_objs[4]->m_modelAnim->m_anmObjVisRes->GetFrame() != 2.0f) {
        if (m_cursorOnGrid) {
            u32 position = m_column + m_row * 4;
            m_objs[4]->setFrameVisible(0.0f);
            sprintf(name, "pos%02d", position);
            m_groups[1]->Remove(m_objs[4]->m_scnMdl);
            m_groups[2]->Remove(m_objs[4]->m_scnMdl);
            m_groups[1]->PushBack(m_objs[4]->m_scnMdl, name);
        } else {
            u32 position = m_hammerSel + 95;
            m_objs[4]->setFrameVisible(1.0f);
            sprintf(name, "pos%02d", position);
            m_groups[1]->Remove(m_objs[4]->m_scnMdl);
            m_groups[2]->Remove(m_objs[4]->m_scnMdl);
            m_groups[2]->PushBack(m_objs[4]->m_scnMdl, name);
        }
        if (m_hammerHeld) {
            m_groups[1]->Remove(m_objs[7 + m_hammerSel]->m_scnMdl);
            m_groups[1]->PushBack(m_objs[7 + m_hammerSel]->m_scnMdl, name);
        }
    }
}

void muClearGetTask::releaseHammer(int mode) {
    u32 index;
    char name[8];
    m_releaseMode = mode;
    m_hammerCancelling = true;
    m_stateFrame = 0;
    m_releaseBox = m_column + m_row * 4;
    switch (mode) {
    case 0:
        m_cursorOnGrid = false;
        index = m_hammerSel;
        break;
    case 2:
        index = m_hammerSel;
        m_objs[7 + m_hammerSel]->setAnimName(lbl_804569E0, true);
        m_objs[7 + m_hammerSel]->setUpdateRateFlg(0.0f, 0x11);
        m_objs[7 + m_hammerSel]->setFrameFlg(0.0f, 0x11);
        break;
    case 1:
        index = m_hammerNum - 1;
        m_objs[6 + m_hammerNum]->setAnimName(lbl_804569E0 + 2, true);
        break;
    }
    sprintf(name, "pos%02d", m_releaseBox);
    m_groups[1]->PushBack(m_objs[7 + index]->m_scnMdl, name);
    m_objs[7 + index]->setTrans(&m_hammerPos);
}

u32 muClearGetTask::setBreakBoxId(u32 id) {
    if (m_dataMgr.m_openState[id] != 2) {
        m_breakQueue[m_breakCount] = id;
        if (m_breakCount == 0 && m_state >= 2) {
            m_state = 6;
            setPosRot(id);
            m_column = id % 4;
            m_row = id / 4;
        }
        ++m_breakCount;
    }
    return m_breakCount;
}

void muClearGetDataManager::setData(u8* buffer, u32 size) {
    utRelocate relocate(buffer, size);
    m_list = static_cast<muClearGetData*>(relocate.getPublicAddress("muClearGetDataList"));
    for (m_count = 0; m_list[m_count].m_index >= 0; m_count++) {
    }
    if (m_count > 128) {
        m_count = 128;
    }
    u8* flags = g_GameGlobal->m_record->unk1528;
    for (int i = 0; i < 128; i++) {
        m_openState[i] = flags[i];
    }
}

void muClearGetDataManager::setOpenState(u32 index) {
    m_openState[index] = 2;
    if (index >= 4 && m_openState[index - 4] == 0) {
        m_openState[index - 4] = 1;
    }
    if (index + 4 < 128 && m_openState[index + 4] == 0) {
        m_openState[index + 4] = 1;
    }
    saveFlags();
}

muClearGetDataManager::~muClearGetDataManager() {
    saveFlags();
}

void muClearGetTask::setPosRot(u32 id) {
    u32 row = id / 4;
    u32 count = 0;
    u32 position = m_scrollRow;
    if (row < m_row) {
        for (u32 current = m_row - 1; current >= row && current != 0; --current) {
            if (position > 3 && position > current + 1) {
                --position;
                ++count;
            }
        }
        m_scrollAngle -= 9.1f * count;
        m_scrollRow -= count;
    } else {
        for (u32 current = m_row + 1; current <= row; ++current) {
            if (position < 28 && current > position + 1) {
                ++position;
                ++count;
            }
        }
        m_scrollAngle += 9.1f * count;
        m_scrollRow += count;
    }
    if (count != 0) {
        m_scrolling = true;
    }
}

void muClearGetTask::setPrizeModel(u32 id, int state) {
    char name[8];
    sprintf(name, "pos%02d", id);
    if (state != 0) {
        switch (m_dataMgr.m_list[id].m_kind) {
        case 0:
            m_groups[1]->PushBack(m_groups[4 + id], name);
            break;
        case 1:
            m_groups[1]->PushBack(m_groups[4 + id], name);
            if (state == 1) {
                m_objs[0x219 + id]->setFrameFlg(1.0f, 0x10);
            } else {
                m_objs[0x219 + id]->setFrameFlg(0.0f, 0x10);
            }
            break;
        case 2:
        case 3:
        case 4:
        case 5:
            m_groups[1]->PushBack(m_objs[0x219 + id]->m_scnMdl, name);
            break;
        }
        for (u32 index = 0; index < 5; ++index) {
            if (id == m_hammerBox[index]) {
                if (state == 1) {
                    m_groups[1]->PushBack(m_objs[12 + index]->m_scnMdl, name);
                } else {
                    m_groups[1]->Remove(m_objs[12 + index]->m_scnMdl);
                }
                break;
            }
        }
    }
}

bool muClearGetTask::calcHammerClose() {
    char name[8];
    u32 index = m_hammerSel + 1;
    sprintf(name, "pos%02d", index + 94);
    Vec3f current;
    Vec3f target;
    target = m_objs[3]->getGlobalPosition(name);
    current = m_objs[7 + index]->getGlobalPosition();
    float distance = (target.m_x - current.m_x) / 6.0f;
    if (static_cast<float>(__fabs(target.m_x - (current.m_x + distance))) < 0.1f) {
        for (; index < m_hammerNum; ++index) {
            m_groups[2]->Remove(m_objs[7 + index]->m_scnMdl);
        }
        m_hammerSel = 0;
        --m_hammerNum;
        for (index = 0; index < m_hammerNum; ++index) {
            m_objs[7 + index]->setTrans(&m_hammerPos);
            sprintf(name, "pos%02d", index + 95);
            m_groups[2]->PushBack(m_objs[7 + index]->m_scnMdl, name);
        }
        return false;
    }
    for (; index < m_hammerNum; ++index) {
        Vec3f translation;
        translation = m_objs[7 + index]->m_trans;
        translation.m_x += distance;
        m_objs[7 + index]->setTrans(&translation);
    }
    return true;
}
