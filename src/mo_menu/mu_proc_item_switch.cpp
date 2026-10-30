#include <gf/gf_application.h>
#include <gm/gm_global.h>
#include <memory.h>
#include <nw4r/g3d/g3d_scnmdlsmpl.h>
#include <nw4r/g3d/g3d_scnobj.h>
#include <snd/snd_system.h>
#include <stdio.h>
#include <string.h>
#include <types.h>

#include <mu/mu_proc_item_switch.h>

// main.dol functions with names that the shared headers do not declare.
// nw4r::g3d::ScnMdlExpand::PushBack(object, node name): attaches the object to the node of the model
extern "C" void PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(void* scnMdl, void* object, const char* name);
// MuMsg::setAlignMode(number, mode)

// The animations of the icon of the frequency (the first is the one the screen starts with, the second the one it leaves
// with) and of the cursor.
static MuAnimIdxData sFrequencyAnims[2] = {
    { 0.0f, 9.0f, 0, 0, 1 },
    { 10.0f, 19.0f, 0, 0, 1 },
};
static MuAnimIdxData sCursorAnims[2] = {
    { 0.0f, 5.0f, 0, 0, 1 },
    { 0.0f, 30.0f, 1, 0, 0x10 },
};

// The bits of the items in the save data: all the items, and the three above that the menu keeps on.
static const u64 sAllItems = 0x1FFFFFFFFFFFFULL;

// Takes an object off the node of a model that has room for objects (ScnMdlExpand, a group of the scene).
static inline void removeFromModel(nw4r::g3d::G3dObj* model, MuObject* object) {
    reinterpret_cast<nw4r::g3d::ScnGroup*>(model)->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(object->m_sceneModel));
}

// The menu data in the save data of the game: the frequency of the items and the items that are on.
static inline u8* getMenuData() {
    return reinterpret_cast<u8*>(g_GameGlobal->getGlobalRecordMenuDatap());
}

muProcItemSwitch::muProcItemSwitch(MuObject** objs, nw4r::g3d::G3dObj** mdls, void* msgData) : muProcMenu(objs, mdls) {
    m_msg = MuMsg::create(1, Heaps::MenuInstance, 0x2B);
    m_msg->allocMsgBuf(0x100, 3);
    m_msg->setMsgData(msgData);
    for (int i = 0; i < 3; i++) {
        m_msg->attachScnMdlSimple(i, reinterpret_cast<nw4r::g3d::ScnMdlSimple*>(m_objs[30]->m_sceneModel), i, 0.0625f);
        m_msg->setFontWidthModeAuto(i);
        m_msg->setAlignMode(i, MuMsg::Align_Left);
    }
    m_msg->setFontColor(0, 0xFF, 0xFF, 0xFF, 0xFF);
    m_msg->setEdgeEnable(0, true);
    m_msg->setEdge(0, 0x97, 0x1B, 0x21, 2.0f);
    m_msg->setFontColor(1, 0xFB, 0xF9, 0xF9, 0xFF);
    m_msg->setEdgeEnable(1, false);
    m_msg->setFontColor(2, 0x97, 0x1B, 0x21, 0xFF);
    m_msg->setEdgeEnable(2, false);
    m_startIsCancel = true;
}

muProcItemSwitch::~muProcItemSwitch() {
    delete m_msg;
}

void muProcItemSwitch::init(nw4r::g3d::ScnGroup* group, int from) {
    m_group = group;
    m_unk664 = 0;
    m_cursor = 0;
    m_frequency = getMenuData()[0];
    u8* data = getMenuData();
    m_mask = *reinterpret_cast<u64*>(data + 0x8);
    m_row = 0;
    m_col = m_frequency + 3;
    group->Insert(group->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[4]));
    PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(m_mdls[4], m_objs[0x53]->m_sceneModel, "pos00");
    m_panel->changeProcess1(0x20, 0);
    m_objs[30]->setAnimIdx(&sFrequencyAnims[0], false);
    setAnimFrame(0x1F, (float)m_frequency, 0x11);
    for (int i = 0; i < ItemCount; i++) {
        setAnimFrame(0x20 + i, (float)(i + 1), 4);
        if (m_mask & (1ULL << i)) {
            setAnimFrame(0x20 + i, 1.0f, 0x10);
        } else {
            setAnimFrame(0x20 + i, 0.0f, 0x10);
        }
    }
    if ((m_mask & sAllItems) == 0) {
        setAnimFrame(0x51, 1.0f, 0x10);
    } else {
        setAnimFrame(0x51, 0.0f, 0x10);
    }
    setAnimFrame(0x52, 0.0f, 4);
    m_objs[0x53]->setAnimIdx(&sCursorAnims[1], false);
    setAnimFrame(0x53, 0.0f, 1);
    setCursor(false);
}

int muProcItemSwitch::proc() {
    if ((u32)m_unk664 == 2) {
        if (m_objs[30]->m_modelAnim->getUpdateRate() == 0.0f) {
            return 3;
        }
        return 0;
    }
    if (m_objs[30]->m_modelAnim->getFrame() == 5.0f) {
        m_panel->clearMsg();
        printHelp();
    }
    if (m_unk664 == 0 && m_objs[30]->m_modelAnim->getFrame() == 9.0f) {
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
            m_objs[30]->setAnimIdx(&sFrequencyAnims[1], false);
            // MATCH-ONLY: the three reserved item bits (49-51) are built in a stack u64 through its high word
            u64 reserved = 0;
            ((u32*)&reserved)[0] |= 0xE0000;
            u64 mask64 = m_mask;
            mask64 |= reserved;
            u8 frequency = m_frequency;
            u8* data = getMenuData();
            data[0] = frequency;
            data = getMenuData();
            *reinterpret_cast<u32*>(data + 0xC) = (u32)mask64;
            *reinterpret_cast<u32*>(data + 0x8) = (u32)(mask64 >> 32);
            m_result = 0x1E;
            m_unk664 = 2;
            return 1;
        }
    }
    return 0;
}

void muProcItemSwitch::exit() {
    removeFromModel(m_mdls[4], m_objs[0x53]);
    m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[4]));
}

bool muProcItemSwitch::selectProc(int dir) {
    int row = m_row;
    bool moved = false;
    u32 index = getIndex();
    if (dir & Pad_Up) {
        if (row == 0) {
            if (m_col > 1) {
                m_row = 7;
            } else {
                m_row = 8;
            }
        } else {
            if (row == 2 && m_col == 1) {
                m_col = 0;
            }
            m_row = m_row - 1;
        }
        g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
        moved = true;
    } else if (dir & Pad_Down) {
        if (row == 8) {
            m_row = 0;
        } else if (row == 7 && m_col > 1) {
            m_row = 0;
        } else {
            m_row = m_row + 1;
        }
        g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
        moved = true;
    } else if (dir & Pad_Left) {
        if (row == 0) {
            if (m_frequency != 0) {
                m_frequency--;
                setAnimFrame(0x1F, (float)m_frequency, 0x11);
                moved = true;
                m_col = m_frequency + 3;
                g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
            }
        } else {
            if (row == 1 && m_col == 2) {
                m_col = 0;
            } else if (m_col == 0) {
                if (row == 8) {
                    m_col = 1;
                } else {
                    m_col = 6;
                }
            } else {
                m_col = m_col - 1;
            }
            g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
            moved = true;
        }
    } else if (dir & Pad_Right) {
        if (row == 0) {
            if (m_frequency != 3) {
                m_frequency++;
                setAnimFrame(0x1F, (float)m_frequency, 0x11);
                moved = true;
                m_col = m_frequency + 3;
                g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
            }
        } else {
            if (row == 1 && m_col == 0) {
                m_col = 2;
            } else if (m_col == 6) {
                m_col = 0;
            } else if (row == 8 && m_col == 1) {
                m_col = 0;
            } else {
                m_col = m_col + 1;
            }
            g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
            moved = true;
        }
    } else if (dir & Pad_A) {
        if (row == 0) {
            m_row = 1;
            g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
            moved = true;
        } else if (index == 1) {
            // The switch of all the items
            if (m_objs[81]->m_modelAnim->m_anmObjMatClrRes->GetFrame() == 1.0f) {
                g_sndSystem->playSE(static_cast<SndID>(0x24), -1, 0, 0, -1);
                m_mask = sAllItems;
                for (u32 i = 0; i < ItemCount; i++) {
                    setAnimFrame(0x20 + i, 1.0f, 0x10);
                }
                setAnimFrame(0x51, 0.0f, 0x10);
            } else {
                g_sndSystem->playSE(static_cast<SndID>(0x24), -1, 0, 0, -1);
                m_mask = 0;
                for (u32 i = 0; i < ItemCount; i++) {
                    setAnimFrame(0x20 + i, 0.0f, 0x10);
                }
                setAnimFrame(0x51, 1.0f, 0x10);
            }
            printHelp();
        } else {
            int item = index - 2;
            moved = true;
            u64 bit = 1ULL << item;
            if (m_mask & bit) {
                g_sndSystem->playSE(static_cast<SndID>(0x24), -1, 0, 0, -1);
                setAnimFrame(0x20 + item, 0.0f, 0x10);
            } else {
                g_sndSystem->playSE(static_cast<SndID>(0x24), -1, 0, 0, -1);
                setAnimFrame(0x20 + item, 1.0f, 0x10);
            }
            m_mask = m_mask ^ bit;
            if ((m_mask & sAllItems) == 0) {
                setAnimFrame(0x51, 1.0f, 0x10);
            } else if ((m_mask & sAllItems) == sAllItems) {
                setAnimFrame(0x51, 0.0f, 0x10);
            }
        }
    }
    if (moved) {
        if (index == getIndex()) {
            setCursor(false);
        } else {
            setCursor(true);
        }
        printHelp();
        return true;
    }
    return false;
}

void muProcItemSwitch::printHelp() {
    // HYPOTHESIS: the original getIndex has this if/else shape; calling the header getIndex() compiles 4 bytes short
    u32 index;
    if (m_row == 0) {
        index = 0;
    } else if (m_row == 1) {
        if (m_col == 0) {
            index = 1;
        } else {
            index = m_col;
        }
    } else {
        u32 row = m_row - 1;
        u32 base = row * 7;
        index = base + m_col;
    }
    int itemInfo[ItemCount] = {
        0x92, 0x87, 0x87, 0x88, 0x8E, 0x91, 0x8C, 0x8C, 0x8C, 0x8F, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8D, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0x8A, 0x8A, 0x8A, 0x8A, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89, 0x90, 0x90
    };
    int itemName[ItemCount] = {
        0x30, 0x0, 0x1, 0x2, 0x2F, 0x1A, 0x3, 0x4, 0x5, 0x2E, 0x6, 0x7, 0x8, 0xA, 0x9, 0x19, 0x1C, 0x1B, 0x1D, 0xB, 0xC, 0x10, 0xE, 0x11, 0xD, 0x1E, 0x12, 0x13, 0x14, 0x1F, 0x16, 0x15, 0x21, 0x22, 0x24, 0xF, 0x23, 0x28, 0x25, 0x17, 0x18, 0x20, 0x26, 0x27, 0x2C, 0x2D, 0x29, 0x2A, 0x2B
    };
    if (index == 0) {
        m_msg->printIndex(0, m_frequency + 0x4C, NULL);
        m_msg->printf(1, "");
        m_msg->printIndex(2, m_frequency + 0x81, NULL);
    } else if (index == 1) {
        m_msg->printf(0, "");
        m_msg->printf(1, "");
        if (m_objs[81]->m_modelAnim->m_anmObjMatClrRes->GetFrame() == 0.0f) {
            m_msg->printIndex(2, 0x86, NULL);
        } else {
            m_msg->printIndex(2, 0x85, NULL);
        }
    } else {
        int item = index - 2;
        int name = itemName[item];
        m_msg->printIndex(0, name + 0x1B, NULL);
        m_msg->printIndex(1, itemInfo[item], NULL);
        m_msg->printIndex(2, name + 0x50, NULL);
    }
}

void muProcItemSwitch::setCursor(bool changed) {
    u32 index = getIndex();
    if (changed) {
        removeFromModel(m_mdls[4], m_objs[0x53]);
        char name[16];
        sprintf(name, "pos%02d", index);
        PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(m_mdls[4], m_objs[0x53]->m_sceneModel, name);
        if (index < 2) {
            setAnimFrame(0x53, (float)index, 1);
            setAnimFrame(0x52, 0.0f, 4);
        } else {
            setAnimFrame(0x53, 2.0f, 1);
            setAnimFrame(0x52, (float)index - 1.0f, 4);
            m_objs[82]->setAnimIdx(&sCursorAnims[0], false);
        }
    }
    if (index == 0) {
        switch (m_frequency) {
        case 0:
            setAnimFrame(0x53, 2.0f, 2);
            break;
        case 3:
            setAnimFrame(0x53, 1.0f, 2);
            break;
        default:
            setAnimFrame(0x53, 3.0f, 2);
            break;
        }
    } else {
        setAnimFrame(0x53, 0.0f, 2);
    }
}

void muProcItemSwitch::playSE_CursorMove() {
    g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
}
