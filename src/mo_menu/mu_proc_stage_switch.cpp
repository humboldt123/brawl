#include <gf/gf_application.h>
#include <gm/gm_global.h>
#include <memory.h>
#include <mu/wifi/mu_wifi_interface.h>
#include <nw4r/g3d/g3d_scnmdlsmpl.h>
#include <nw4r/g3d/g3d_scnobj.h>
#include <snd/snd_system.h>
#include <stdio.h>
#include <string.h>
#include <types.h>

#include <mu/mu_proc_stage_switch.h>

// main.dol functions with names that the shared headers do not declare.
// nw4r::g3d::ScnMdlExpand::PushBack(object, node name): attaches the object to the node of the model
extern "C" void PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(void* scnMdl, void* object, const char* name);

// The animations of the cursor and of the header (the first appears with the screen, the second goes away with it).
static MuAnimIdxData sAnims[3] = {
    { 0.0f, 9.0f, 0, 0, 1 },
    { 10.0f, 19.0f, 0, 0, 1 },
    { 0.0f, 30.0f, 1, 0, 0x10 },
};

// The bit of each entry of the grid and of the list in the switches of the stages (and the number of the text of the
// stage); the last one is the random stage.
struct muStageTable {
    int m_bit[42];
};
static muStageTable sStageTable = {
    { 0, 1, 2, 3, 4, 5, 7, 9, 8, 10, 11, 12, 13, 14, 15, 16, 17, 20, 21, 22, 26, 25, 18, 23, 6, 24, 19, 29, 30, 27, 28, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50 }
};

// Takes an object off the node of a model that has room for objects (ScnMdlExpand, a group of the scene).
static inline void removeFromModel(nw4r::g3d::G3dObj* model, MuObject* object) {
    reinterpret_cast<nw4r::g3d::ScnGroup*>(model)->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(object->m_sceneModel));
}

// Takes the last object off a group of the scene (when it has any).
static inline void removeLastItem(nw4r::g3d::ScnGroup* group, u32 count) {
    if (count != 0) {
        group->Remove(count - 1);
    }
}

static inline u8* getMenuData() {
    return reinterpret_cast<u8*>(g_GameGlobal->getGlobalRecordMenuDatap());
}

muProcStageSwitch::muProcStageSwitch(MuObject** objs, nw4r::g3d::G3dObj** mdls, void* msgData) : muProcMenu(objs, mdls) {
    m_msg = MuMsg::create(1, Heaps::MenuInstance, 0x2B);
    m_msg->allocMsgBuf(0x100, 1);
    m_msg->setMsgData(msgData);
    m_msg->attachScnMdlSimple(0, reinterpret_cast<nw4r::g3d::ScnMdlSimple*>(m_objs[0x54]->m_sceneModel), 0, 0.0625f);
    m_msg->setFontWidthModeAuto(0);
    m_msg->setAlignMode(0, MuMsg::Align_Left);
    m_msg->setFontColor(0, 0xFF, 0xFF, 0xFF, 0xFF);
    m_msg->setEdgeEnable(0, true);
    m_msg->setEdge(0, 0x45, 0x45, 0x25, 2.0f);
    m_startIsCancel = true;
    if (g_muWifiInterfaceTask != NULL) {
        m_showRandom = 0;
    } else {
        m_showRandom = 1;
    }
    m_changed = 0;
}

muProcStageSwitch::~muProcStageSwitch() {
    delete m_msg;
}

void muProcStageSwitch::init(nw4r::g3d::ScnGroup* group, int from) {
    m_group = group;
    m_cursor = 0;
    m_unk664 = 0;
    // Which stages are shown: the ones that do not need to be unlocked, and the ones that were unlocked (the number is
    // the bit in the flags of the stages of the game, -1: no bit).
    int gridUnlock[GridStageCount] = { -1, -1, -1, 0, -1, -1, 1, -1, 2, -1, -1, -1, -1, -1, -1, -1, 3, -1, -1, -1, -1, -1, -1, -1, 4, -1, -1, -1, 6, -1, 5 };
    int listUnlock[ListStageCount] = { -1, -1, 8, -1, 9, -1, -1, 11, -1, 10 };
    char name[8];
    memset(m_gridStages, 0, sizeof(m_gridStages));
    memset(m_listStages, 0, sizeof(m_listStages));
    {
        u32 count = 0;
        for (u32 i = 0; i < GridStageCount; i++) {
            int bit = gridUnlock[i];
            if (bit < 0 || (g_GameGlobal->getStageAppFlag()[bit / 32] & (1 << (bit % 32)))) {
                m_gridStages[count] = i;
                count++;
            }
        }
        m_gridCount = count;
    }
    {
        u32 count = 0;
        for (u32 i = 0; i < ListStageCount; i++) {
            int bit = listUnlock[i];
            if (bit < 0 || (g_GameGlobal->getStageAppFlag()[bit / 32] & (1 << (bit % 32)))) {
                m_listStages[count] = i;
                count++;
            }
        }
        m_listCount = count;
    }
    u8* data = getMenuData();
    m_gridRows = (m_gridCount + 4) / 5;
    m_listRows = (m_listCount + 1) / 2;
    m_mask = (u64(*reinterpret_cast<u32*>(data + 0x20)) << 32) | *reinterpret_cast<u32*>(data + 0x24);
    m_gridLastRow = m_gridCount % 5;
    m_col = 0;
    m_row = 0;
    m_listLastRow = m_listCount & 1;
    m_gridMask = 0;
    m_notGridMask = 0;
    m_listMask = 0;
    m_notListMask = 0;
    PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(m_mdls[5], m_objs[0x55]->m_sceneModel, "pos00");
    PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(m_mdls[5], m_objs[0x56]->m_sceneModel, "pos05");
    if (m_showRandom) {
        PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(m_mdls[5], m_objs[0x80]->m_sceneModel, "pos07");
    }
    group->Insert(group->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[5]));
    m_panel->changeProcess1(0x20, 0);
    m_objs[0x54]->setAnimIdx(&sAnims[0], false);
    setAnimFrame(0x54, 0.0f, 4);

    u32 i;
    for (i = 0; i < m_gridCount; i++) {
        sprintf(name, "pos%02d", i + 1);
        PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(m_mdls[5], m_objs[0x57 + i]->m_sceneModel, name);
        setAnimFrame(0x57 + i, 1.0f + (float)m_gridStages[i], 4);
        muStageTable table = sStageTable;
        u64 bit = 1ULL << table.m_bit[m_gridStages[i]];
        if (m_mask & bit) {
            setAnimFrame(0x57 + i, 1.0f, 0x10);
        } else {
            setAnimFrame(0x57 + i, 0.0f, 0x10);
        }
        m_gridMask |= bit;
        m_notListMask |= bit | (1ULL << RandomBit);
    }
    for (; i < GridStageCount; i++) {
        setAnimFrame(0x57 + i, 0.0f, 4);
    }
    for (i = 0; i < m_listCount; i++) {
        sprintf(name, "pos%02d", i + 0x33);
        PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(m_mdls[5], m_objs[0x76 + i]->m_sceneModel, name);
        setAnimFrame(0x76 + i, 1.0f + (float)m_listStages[i], 4);
        u64 bit = 1ULL << (m_listStages[i] + 0x28);
        if (m_mask & bit) {
            setAnimFrame(0x76 + i, 1.0f, 0x10);
        } else {
            setAnimFrame(0x76 + i, 0.0f, 0x10);
        }
        m_listMask |= bit;
        m_notGridMask |= bit | (1ULL << RandomBit);
    }
    for (; i < ListStageCount; i++) {
        setAnimFrame(0x76 + i, 0.0f, 4);
    }
    if ((m_mask >> 32) & 0x40000) {
        setAnimFrame(0x80, 1.0f, 0x10);
    } else {
        setAnimFrame(0x80, 0.0f, 0x10);
    }
    if (m_mask & m_gridMask) {
        setAnimFrame(0x55, 0.0f, 0x10);
    } else {
        setAnimFrame(0x55, 1.0f, 0x10);
    }
    if (m_mask & m_listMask) {
        setAnimFrame(0x56, 0.0f, 0x10);
    } else {
        setAnimFrame(0x56, 1.0f, 0x10);
    }
    // When only one stage is on, the headers cannot turn the stages off
    {
        muStageTable table = sStageTable;
        int selected = 0;
        u32 j;
        for (j = 0; j < GridStageCount; j++) {
            if (m_mask & (1ULL << table.m_bit[j])) {
                selected++;
            }
        }
        for (j = 0; j < ListStageCount; j++) {
            if (m_mask & (1ULL << (j + 0x28))) {
                selected++;
            }
        }
        if (selected == 1) {
            setAnimFrame(0x55, 1.0f, 0x10);
            setAnimFrame(0x56, 1.0f, 0x10);
        }
    }
    PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(m_mdls[5], m_objs[0x81]->m_sceneModel, "pos00");
    m_objs[0x81]->setAnimIdx(&sAnims[2], false);
    setAnimFrame(0x81, 0.0f, 1);
    m_msg->printf(0, "");
    m_changed = 0;
}

int muProcStageSwitch::proc() {
    if ((u32)m_unk664 == 2) {
        if (m_objs[0x54]->m_modelAnim->getUpdateRate() == 0.0f) {
            return 3;
        }
        return 0;
    }
    if (m_objs[0x54]->m_modelAnim->getFrame() == 5.0f) {
        m_panel->clearMsg();
    }
    if (m_unk664 == 0 && m_objs[0x54]->m_modelAnim->getFrame() == 9.0f) {
        m_unk664 = 1;
    }
    if (m_unk664 != 0) {
        u32 pad = padUpdate();
        selectProc(pad);
        u32 mask = Pad_Cancel;
        if (m_startIsCancel) {
            mask |= Pad_Start;
        }
        if (pad & mask) {
            if (m_changed == 1) {
                g_sndSystem->playSE(static_cast<SndID>(2), -1, 0, 0, -1);
            }
            m_objs[0x54]->setAnimIdx(&sAnims[1], false);
            u32 maskHi = m_mask >> 32;
            u32 maskLo = (u32)m_mask;
            u8* data = getMenuData();
            *reinterpret_cast<u32*>(data + 0x24) = maskLo;
            *reinterpret_cast<u32*>(data + 0x20) = maskHi;
            m_result = 0x1F;
            m_unk664 = 2;
            return 1;
        }
    }
    return 0;
}

void muProcStageSwitch::exit() {
    nw4r::g3d::ScnGroup* group = reinterpret_cast<nw4r::g3d::ScnGroup*>(m_mdls[5]);
    while (group->sceneItemsCount != 0) {
        removeLastItem(group, group->sceneItemsCount);
    }
    m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[5]));
}

bool muProcStageSwitch::selectProc(int dir) {
    u32 index = getIndex();
    muStageTable table = sStageTable;
    muStageTable table2 = table;
    int firstOn = -1;
    int selected = 0;
    int flags = 0;
    bool moved = false;
    u32 j;
    for (j = 0; j < GridStageCount; j++) {
        if (m_mask & (1ULL << table2.m_bit[j])) {
            selected++;
            if (firstOn == -1) {
                firstOn = j;
            }
            flags |= 1;
        }
    }
    for (j = 0; j < ListStageCount; j++) {
        if (m_mask & (1ULL << (j + 0x28))) {
            selected++;
            if (firstOn == -1) {
                firstOn = j + 0x28;
            }
            flags |= 2;
        }
    }
    if ((m_mask >> 32) & 0x40000) {
        if (firstOn == -1) {
            firstOn = RandomBit;
        }
        flags |= 4;
    }

    if (dir & Pad_Up) {
        if (m_row == 0) {
            if (m_col < 5) {
                if (m_gridLastRow != 0 && m_col >= m_gridLastRow) {
                    m_row = m_gridRows - 1;
                } else {
                    m_row = m_gridRows;
                }
            } else if (m_showRandom) {
                m_row = m_listRows + 1;
            } else if (m_col == 6 && m_listLastRow == 1) {
                m_row = m_listRows - 1;
            } else {
                m_row = m_listRows;
            }
        } else if (m_col >= 5 && m_row >= m_listRows + 1) {
            if (m_col == 6 && m_listLastRow == 1) {
                m_row = m_listRows - 1;
            } else {
                m_row = m_listRows;
            }
        } else {
            m_row = m_row - 1;
        }
        moved = true;
    } else if (dir & Pad_Down) {
        if (m_col < 5) {
            if (m_row == m_gridRows || (m_gridLastRow != 0 && m_col >= m_gridLastRow && m_row == m_gridRows - 1)) {
                m_row = 0;
            } else {
                m_row = m_row + 1;
            }
        } else if (m_row >= m_listRows + 1) {
            m_row = 0;
        } else if (m_row == m_listRows - 1 && m_listLastRow != 0 && m_col - 5 >= m_listLastRow) {
            if (m_showRandom) {
                m_row = m_listRows + 1;
            } else {
                m_row = 0;
            }
        } else if (m_row == m_listRows && !m_showRandom) {
            m_row = 0;
        } else {
            m_row = m_row + 1;
        }
        moved = true;
    } else if (dir & Pad_Left) {
        moved = true;
        if (m_row == 0) {
            if (m_col < 5) {
                m_col = 5;
            } else {
                m_col = 0;
            }
        } else if (m_col == 0) {
            if (m_row > m_listRows) {
                if (m_showRandom == 0) {
                    if (m_gridLastRow == 0) {
                        m_col = 4;
                    } else if (m_row == m_gridRows && m_gridLastRow == 1) {
                        moved = false;
                    } else {
                        m_col = m_gridLastRow - 1;
                    }
                } else {
                    m_col = 5;
                }
            } else if (m_row == m_listRows && m_listLastRow == 1) {
                m_col = 5;
            } else {
                m_col = 6;
            }
        } else if (m_row == m_gridRows && m_col >= 5) {
            if (m_gridLastRow == 0) {
                m_col = 4;
            } else {
                m_col = m_gridLastRow - 1;
            }
        } else if (m_col == 6 && m_row == 6) {
            m_col = m_col - 2;
        } else {
            m_col = m_col - 1;
        }
    } else if (dir & Pad_Right) {
        moved = true;
        if (m_row == 0) {
            if (m_col < 5) {
                m_col = 5;
            } else {
                m_col = 0;
            }
        } else if (m_col == 6) {
            m_col = 0;
        } else if (m_col >= 5 && m_row > m_listRows) {
            m_col = 0;
        } else if (m_row == m_listRows && m_col == 5 && m_listLastRow == 1) {
            m_col = 0;
        } else if (m_row == m_gridRows && m_col == m_gridLastRow - 1) {
            if (m_showRandom) {
                m_col = 5;
            } else {
                if (m_col == 0) {
                    moved = false;
                }
                m_col = 0;
            }
        } else if (m_col == 4 && m_row > m_listRows) {
            if (m_showRandom) {
                m_col = m_col + 1;
            } else {
                m_col = 0;
            }
        } else {
            m_col = m_col + 1;
        }
    } else if (dir & Pad_A) {
        if (m_row == 0) {
            if (m_col < 5) {
                // The header of the grid
                if (m_objs[0x55]->m_modelAnim->m_anmObjMatClrRes->GetFrame() == 1.0f) {
                    g_sndSystem->playSE(static_cast<SndID>(0x24), -1, 0, 0, -1);
                    m_mask = m_mask | m_gridMask;
                    for (u32 i = 0; i < GridStageCount; i++) {
                        setAnimFrame(0x57 + i, 1.0f, 0x10);
                    }
                    setAnimFrame(0x55, 0.0f, 0x10);
                } else if (selected == 1) {
                    g_sndSystem->playSE(static_cast<SndID>(3), -1, 0, 0, -1);
                } else {
                    g_sndSystem->playSE(static_cast<SndID>(0x24), -1, 0, 0, -1);
                    m_mask = m_mask & m_notGridMask;
                    for (u32 i = 0; i < GridStageCount; i++) {
                        setAnimFrame(0x57 + i, 0.0f, 0x10);
                    }
                    setAnimFrame(0x55, 1.0f, 0x10);
                    if (flags == 1 || flags == 5) {
                        m_mask = m_mask | (1ULL << firstOn);
                        setAnimFrame(0x57 + firstOn, 1.0f, 0x10);
                    }
                }
            } else {
                // The header of the list
                if (m_objs[0x56]->m_modelAnim->m_anmObjMatClrRes->GetFrame() == 1.0f) {
                    g_sndSystem->playSE(static_cast<SndID>(0x24), -1, 0, 0, -1);
                    m_mask = m_mask | m_listMask;
                    for (u32 i = 0; i < ListStageCount; i++) {
                        setAnimFrame(0x76 + i, 1.0f, 0x10);
                    }
                    setAnimFrame(0x56, 0.0f, 0x10);
                } else if (selected == 1) {
                    g_sndSystem->playSE(static_cast<SndID>(3), -1, 0, 0, -1);
                } else {
                    g_sndSystem->playSE(static_cast<SndID>(0x24), -1, 0, 0, -1);
                    m_mask = m_mask & m_notListMask;
                    for (u32 i = 0; i < ListStageCount; i++) {
                        setAnimFrame(0x76 + i, 0.0f, 0x10);
                    }
                    setAnimFrame(0x56, 1.0f, 0x10);
                    if (flags == 2 || flags == 6) {
                        m_mask = m_mask | (1ULL << firstOn);
                        setAnimFrame(0x4E + firstOn, 1.0f, 0x10);
                    }
                }
            }
        } else {
            // One stage
            int item = index - 2;
            muStageTable table3 = table;
            int bitNo = table3.m_bit[item];
            u64 bit = 1ULL << bitNo;
            bool updateHeaders = true;
            if (m_mask & bit) {
                if (selected == 2) {
                    setAnimFrame(0x55, 1.0f, 0x10);
                    setAnimFrame(0x56, 1.0f, 0x10);
                    updateHeaders = false;
                }
                if (selected == 1 && bitNo != RandomBit) {
                    g_sndSystem->playSE(static_cast<SndID>(3), -1, 0, 0, -1);
                } else {
                    g_sndSystem->playSE(static_cast<SndID>(0x24), -1, 0, 0, -1);
                    setAnimFrame(0x57 + item, 0.0f, 0x10);
                    m_mask = m_mask ^ bit;
                }
            } else {
                g_sndSystem->playSE(static_cast<SndID>(0x24), -1, 0, 0, -1);
                setAnimFrame(0x57 + item, 1.0f, 0x10);
                m_mask = m_mask ^ bit;
            }
            if (updateHeaders) {
                if ((m_mask & m_gridMask) == 0) {
                    setAnimFrame(0x55, 1.0f, 0x10);
                } else if ((m_mask & m_gridMask) == m_gridMask) {
                    setAnimFrame(0x55, 0.0f, 0x10);
                }
                if ((m_mask & m_listMask) == 0) {
                    setAnimFrame(0x56, 1.0f, 0x10);
                } else if ((m_mask & m_listMask) == m_listMask) {
                    setAnimFrame(0x56, 0.0f, 0x10);
                }
            }
        }
    }
    if (moved) {
        g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
        setCursor();
        return true;
    }
    return false;
}

void muProcStageSwitch::setCursor() {
    u32 index = getIndex();
    u32 entry = getIndex();
    u32 stage = 0;
    if (entry == 1) {
        entry = 0x32;
    } else if (entry == 0x2B) {
        entry = 0x46;
    } else if (entry >= 0x21) {
        entry = entry + 0x12;
        stage = m_listStages[entry - 0x33] + 0x32;
    } else if (entry != 0) {
        entry = entry - 1;
        stage = m_gridStages[entry - 1] + 2;
    }
    if (index < 2) {
        m_msg->printf(0, "");
    } else if (index == 0x2B) {
        m_msg->printIndex(0, 0x29, NULL);
    } else if (index >= 0x21) {
        m_msg->printIndex(0, stage - 0x13, NULL);
    } else {
        muStageTable table = sStageTable;
        m_msg->printIndex(0, table.m_bit[stage], NULL);
    }
    removeFromModel(m_mdls[5], m_objs[0x81]);
    char name[16];
    sprintf(name, "pos%02d", entry);
    PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(m_mdls[5], m_objs[0x81]->m_sceneModel, name);
    if (entry == 0) {
        setAnimFrame(0x81, 0.0f, 1);
    } else if (entry == 0x32) {
        setAnimFrame(0x81, 2.0f, 1);
    } else if (entry == 0x46) {
        setAnimFrame(0x81, 4.0f, 1);
    } else if (entry < 0x21) {
        setAnimFrame(0x81, 1.0f, 1);
    } else {
        setAnimFrame(0x81, 3.0f, 1);
    }
    setAnimFrame(0x54, (float)stage, 4);
}

void muProcStageSwitch::playSE_CursorMove() {
    g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
}
