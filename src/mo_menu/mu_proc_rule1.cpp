#include <gf/gf_application.h>
#include <gm/gm_global.h>
#include <memory.h>
#include <nw4r/g3d/g3d_scnobj.h>
#include <snd/snd_system.h>
#include <string.h>
#include <types.h>

#include <mu/mu_proc_rule1.h>

// main.dol functions with names that the shared headers do not declare.
// nw4r::g3d::ScnMdlExpand::PushBack(object, node name): attaches the object to the node of the model
extern "C" void PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(void* scnMdl, void* object, const char* name);
// GameGlobal::checkHideMenuAdditionalRule()
// Tells the game that the handicap of the rules changed
extern "C" void fn_80050294();

// The animations of each entry of the screen (the entry of the cursor and the others), one row for each icon.
// HYPOTHESIS: the six animations of a row are: 0 the entry the cursor is on appears, 1 an entry appears or waits, 2 the
// entry changes its value, 3 the cursor is on the entry, 4 the entry is picked, 5 the entry goes away.
static MuAnimNameData sAnims[8][6] = {
    {
        { 0.0f, 9.0f, 0, "MenMainIcon0121_TopN__2", 0x11 },
        { 0.0f, 9.0f, 0, "MenMainIcon0121_TopN__0", 0x11 },
        { 0.0f, 59.0f, 1, "MenMainIcon0121_TopN__5", 0x11 },
        { 0.0f, 119.0f, 0, "MenMainIcon0121_TopN__4", 0x11 },
        { 0.0f, 29.0f, 0, "MenMainIcon0121_TopN__3", 0x11 },
        { 0.0f, 19.0f, 0, "MenMainIcon0121_TopN__1", 0x11 },
    },
    {
        { 0.0f, 9.0f, 0, "MenMainIcon0122_TopN__2", 0x11 },
        { 0.0f, 9.0f, 0, "MenMainIcon0122_TopN__0", 0x11 },
        { 0.0f, 59.0f, 1, "MenMainIcon0122_TopN__5", 0x11 },
        { 0.0f, 119.0f, 0, "MenMainIcon0122_TopN__4", 0x11 },
        { 0.0f, 29.0f, 0, "MenMainIcon0122_TopN__3", 0x11 },
        { 0.0f, 19.0f, 0, "MenMainIcon0122_TopN__1", 0x11 },
    },
    {
        { 0.0f, 9.0f, 0, "MenMainIcon0123_TopN__2", 0x11 },
        { 0.0f, 9.0f, 0, "MenMainIcon0123_TopN__0", 0x11 },
        { 0.0f, 59.0f, 1, "MenMainIcon0123_TopN__5", 0x11 },
        { 0.0f, 119.0f, 0, "MenMainIcon0123_TopN__4", 0x11 },
        { 0.0f, 29.0f, 0, "MenMainIcon0123_TopN__3", 0x11 },
        { 0.0f, 19.0f, 0, "MenMainIcon0123_TopN__1", 0x11 },
    },
    {
        { 0.0f, 9.0f, 0, "MenMainIcon0124_TopN__2", 0x11 },
        { 0.0f, 9.0f, 0, "MenMainIcon0124_TopN__0", 0x11 },
        { 0.0f, 59.0f, 1, "MenMainIcon0124_TopN__5", 0x11 },
        { 0.0f, 119.0f, 0, "MenMainIcon0124_TopN__4", 0x11 },
        { 0.0f, 29.0f, 0, "MenMainIcon0124_TopN__3", 0x11 },
        { 0.0f, 19.0f, 0, "MenMainIcon0124_TopN__1", 0x11 },
    },
    {
        { 0.0f, 9.0f, 0, "MenMainIcon0125_TopN__2", 0x11 },
        { 0.0f, 9.0f, 0, "MenMainIcon0125_TopN__0", 0x11 },
        { 0.0f, 59.0f, 1, "MenMainIcon0125_TopN__5", 0x11 },
        { 0.0f, 119.0f, 0, "MenMainIcon0125_TopN__4", 0x11 },
        { 0.0f, 29.0f, 0, "MenMainIcon0125_TopN__3", 0x11 },
        { 0.0f, 19.0f, 0, "MenMainIcon0125_TopN__1", 0x11 },
    },
    {
        { 0.0f, 9.0f, 0, "MenMainIcon0126_TopN__2", 0x11 },
        { 0.0f, 9.0f, 0, "MenMainIcon0126_TopN__0", 0x11 },
        { 0.0f, 59.0f, 1, "MenMainIcon0126_TopN__5", 0x11 },
        { 0.0f, 119.0f, 0, "MenMainIcon0126_TopN__4", 0x11 },
        { 0.0f, 29.0f, 0, "MenMainIcon0126_TopN__3", 0x11 },
        { 0.0f, 19.0f, 0, "MenMainIcon0126_TopN__1", 0x11 },
    },
    {
        { 0.0f, 9.0f, 0, "MenMainIcon0127_TopN__2", 0x11 },
        { 0.0f, 9.0f, 0, "MenMainIcon0127_TopN__0", 0x11 },
        { 0.0f, 59.0f, 1, "MenMainIcon0127_TopN__5", 0x11 },
        { 0.0f, 119.0f, 0, "MenMainIcon0127_TopN__4", 0x11 },
        { 0.0f, 29.0f, 0, "MenMainIcon0127_TopN__3", 0x11 },
        { 0.0f, 19.0f, 0, "MenMainIcon0127_TopN__1", 0x11 },
    },
    {
        { 0.0f, 9.0f, 0, "MenMainIcon0128_TopN__2", 0x11 },
        { 0.0f, 9.0f, 0, "MenMainIcon0128_TopN__0", 0x11 },
        { 0.0f, 59.0f, 1, "MenMainIcon0128_TopN__5", 0x11 },
        { 0.0f, 119.0f, 0, "MenMainIcon0128_TopN__4", 0x11 },
        { 0.0f, 29.0f, 0, "MenMainIcon0128_TopN__3", 0x11 },
        { 0.0f, 19.0f, 0, "MenMainIcon0128_TopN__1", 0x11 },
    },
};

// The icon (the object of m_objs) of each entry of the cursor. When the rule is the stock, the time entry uses another icon.
static u8 sEntryObjsTime[8] = { 0, 1, 2, 3, 4, 5, 6, 0 };
static u8 sEntryObjsStock[8] = { 0, 7, 2, 3, 4, 5, 6, 0 };

// Takes an object off the node of a model that has room for objects (ScnMdlExpand, a group of the scene).
static inline void removeFromModel(nw4r::g3d::G3dObj* model, MuObject* object) {
    reinterpret_cast<nw4r::g3d::ScnGroup*>(model)->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(object->m_sceneModel));
}

// The settings of the game that the screen shows and changes.
static inline u8* getRuleBytes() {
    return reinterpret_cast<u8*>(g_GameGlobal->m_setRule);
}

muProcRule1::muProcRule1(MuObject** objs, nw4r::g3d::G3dObj** mdls, int mode, int priority) : muProcMenu(objs, mdls) {
    m_priority = priority;
    m_startIsCancel = true;
    m_itemCount = Entry_Count;
    m_mode = mode;
    for (int i = 0; i < Entry_Count; i++) {
        m_enabled[i] = true;
    }
    m_hideAdditionalRule = g_GameGlobal->checkHideMenuAdditionalRule();
}

void muProcRule1::init(nw4r::g3d::ScnGroup* group, int from) {
    m_group = group;
    u8* setting = getRuleBytes();
    for (int i = 0; i < Entry_Count; i++) {
        m_enabled[i] = true;
    }
    // Entries the mode of the scene does not let the player change
    if (m_mode == 1) {
        m_enabled[Entry_Handicap] = false;
        m_enabled[Entry_Stage] = false;
    }
    if (m_mode == 6) {
        if (setting[0x18] == 2) {
            m_enabled[Entry_Rule] = false;
            m_enabled[Entry_Time] = false;
            m_enabled[Entry_Handicap] = false;
        }
        if (setting[0x18] == 1) {
            m_enabled[Entry_Handicap] = false;
        }
    }
    if (m_mode == 2) {
        m_enabled[Entry_Stage] = false;
    }

    switch (from) {
    case 6:
        m_unk664 = 0;
        m_code = 1;
        m_cursor = 0;
        m_side = 0;
        if (m_mode == 1 && setting[2] != 1 && setting[3] == 0) {
            setting[3] = 1;
        }
        while (!m_enabled[m_cursor]) {
            if (m_cursor == Entry_Stage) {
                if (m_side == 0) {
                    m_cursor = Entry_ButtonA;
                }
                if (m_side == 1) {
                    m_cursor = Entry_ButtonB;
                }
            } else if (m_cursor == Entry_ButtonA || m_cursor == Entry_ButtonB) {
                m_cursor = Entry_Rule;
            } else {
                m_cursor++;
            }
        }
        break;
    case 1:
        m_code = 0x1F;
        m_unk664 = 1;
        m_cursor = Entry_ButtonB;
        m_side = 1;
        break;
    case 2:
        m_code = 0x20;
        m_unk664 = 1;
        m_cursor = Entry_ButtonA;
        m_side = 0;
        break;
    }

    switch (setting[2]) {
    case 0:
        m_rule = 0;
        break;
    case 1:
        m_rule = 1;
        break;
    case 2:
        m_rule = 2;
        break;
    }
    switch (setting[5]) {
    case 0:
        m_handicap = 0;
        break;
    case 1:
        m_handicap = 2;
        break;
    case 2:
        m_handicap = 1;
        break;
    }
    m_time = setting[3];
    u8* entryObjs = sEntryObjsStock;
    if (m_rule != 1) {
        entryObjs = sEntryObjsTime;
    }
    m_damage = setting[6];
    m_stage = setting[7];
    m_stock = setting[4];
    m_unk672 = 0;

    // Put the objects of the screen in the scene
    group->Insert(group->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[0]->m_sceneModel));
    group->Insert(group->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[2]->m_sceneModel));
    group->Insert(group->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[4]->m_sceneModel));
    group->Insert(group->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[5]->m_sceneModel));
    group->Insert(group->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[1]));
    if (m_hideAdditionalRule == 1) {
        group->Insert(group->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[6]->m_sceneModel));
    }
    if (m_rule == 1) {
        group->Insert(group->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[2]));
        m_objs[1]->setAnimName(&sAnims[1][1], true);
        m_objs[1]->m_animNameData = NULL;
        setAnimFrame(1, 9.0f, 0x11);
    } else {
        group->Insert(group->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[0]));
        m_objs[7]->setAnimName(&sAnims[7][1], true);
        m_objs[7]->m_animNameData = NULL;
        setAnimFrame(7, 9.0f, 0x11);
    }
    if (m_time == 0) {
        PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(m_mdls[0], m_objs[0x11]->m_sceneModel, "num3N");
    } else {
        PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(m_mdls[0], m_objs[8]->m_sceneModel, "num5N");
        PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(m_mdls[0], m_objs[9]->m_sceneModel, "num4N");
        PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(m_mdls[0], m_objs[0x10]->m_sceneModel, "num3N");
        PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(m_mdls[0], m_objs[10]->m_sceneModel, "num2N");
        PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(m_mdls[0], m_objs[11]->m_sceneModel, "num1N");
    }

    // The numbers are shown dark until the cursor goes to them
    for (int i = 8; i <= 0x11; i++) {
        setAnimFrame(i, 1.0f, 2);
        setAnimFrame(i, 0.0f, 0x10);
    }
    setAnimFrame(8, (float)(m_time / 10), 4);
    setAnimFrame(9, (float)(m_time % 10), 4);
    setAnimFrame(0xC, (float)(m_stock / 10), 4);
    setAnimFrame(0xD, (float)(m_stock % 10), 4);
    setAnimFrame(0xE, (float)(m_damage / 10), 4);
    setAnimFrame(0xF, (float)(m_damage % 10), 4);

    for (int i = 0; i < Entry_Count; i++) {
        if (i == m_cursor) {
            m_objs[entryObjs[i]]->setAnimName(&sAnims[entryObjs[i]][0], true);
            MuObject* object = m_objs[entryObjs[i]];
            int priority = m_priority + 1;
            object->m_scnMdl->SetPriorityDrawOpa(priority);
            object->m_scnMdl->SetPriorityDrawXlu(priority);
        } else {
            m_objs[entryObjs[i]]->setAnimName(&sAnims[entryObjs[i]][1], true);
        }
    }

    // The icons of the entries that have three values show the frame of the value they are on
    if (m_rule == 0) {
        m_objs[0]->setFrameVisible(9.0f);
        m_objs[0]->setFrameTexSrt(9.0f);
    } else {
        m_objs[0]->setFrameVisible(4.0f + 10.0f * (float)(m_rule - 1));
        m_objs[0]->setFrameTexSrt(4.0f + 10.0f * (float)(m_rule - 1));
    }
    m_objs[0]->m_modelAnim->m_anmObjTexSrtRes->SetUpdateRate(0.0f);
    m_objs[0]->m_modelAnim->m_anmObjVisRes->SetUpdateRate(0.0f);
    if (m_handicap == 0) {
        m_objs[2]->setFrameVisible(9.0f);
        m_objs[2]->setFrameTexSrt(9.0f);
    } else {
        m_objs[2]->setFrameVisible(4.0f + 10.0f * (float)(m_handicap - 1));
        m_objs[2]->setFrameTexSrt(4.0f + 10.0f * (float)(m_handicap - 1));
    }
    m_objs[2]->m_modelAnim->m_anmObjTexSrtRes->SetUpdateRate(0.0f);
    m_objs[2]->m_modelAnim->m_anmObjVisRes->SetUpdateRate(0.0f);
    if (m_stage == 0) {
        m_objs[4]->setFrameVisible(9.0f);
        m_objs[4]->setFrameTexSrt(9.0f);
    } else {
        m_objs[4]->setFrameVisible(4.0f + 10.0f * (float)(m_stage - 1));
        m_objs[4]->setFrameTexSrt(4.0f + 10.0f * (float)(m_stage - 1));
    }
    m_objs[4]->m_modelAnim->m_anmObjTexSrtRes->SetUpdateRate(0.0f);
    m_objs[4]->m_modelAnim->m_anmObjVisRes->SetUpdateRate(0.0f);

    m_panel->changeProcess1(0xE, 0x10);

    // The entries that cannot be changed are shown grey
    for (int i = 0; i < Entry_Count; i++) {
        if (!m_enabled[i]) {
            m_objs[i]->changeAnimN(0, 4, false, false);
            m_objs[i]->setFrameTex(1.0f);
            if (i == 1) {
                m_objs[7]->changeAnimN(0, 4, false, false);
                m_objs[7]->setFrameTex(1.0f);
            }
        }
    }
    m_dirty = 1;
    m_repeat = 0;
    setMessage();
}

int muProcRule1::proc() {
    u8* entryObjs = sEntryObjsStock;
    if (m_rule != 1) {
        entryObjs = sEntryObjsTime;
    }
    switch (m_unk664) {
    case 0: {
        if (m_objs[entryObjs[m_cursor]]->m_modelAnim->getFrame() == 9.0f) {
            m_objs[entryObjs[m_cursor]]->setAnimName(&sAnims[entryObjs[m_cursor]][3], true);
            m_unk664 = 2;
        }
        u32 pad = padUpdate();
        u32 mask = Pad_Cancel;
        if (m_startIsCancel) {
            mask |= Pad_Start;
        }
        if (pad & mask) {
            for (int i = 0; i < Entry_Count; i++) {
                m_objs[entryObjs[i]]->setAnimName(&sAnims[entryObjs[i]][5], true);
            }
            m_result = 1;
            setRule();
            m_unk664 = 3;
            return 1;
        }
        break;
    }
    case 1: {
        if (m_objs[entryObjs[m_cursor]]->m_modelAnim->getFrame() == 9.0f) {
            m_objs[entryObjs[m_cursor]]->setAnimName(&sAnims[entryObjs[m_cursor]][3], true);
            m_unk664 = 2;
        }
        u32 pad = padUpdate();
        if (selectProc(pad)) {
            m_unk664 = 2;
            return 4;
        }
        if (pad & Pad_A) {
            if (m_cursor == Entry_ButtonB) {
                for (int i = 0; i < Entry_Count; i++) {
                    if (i == m_cursor) {
                        m_objs[entryObjs[i]]->setAnimName(&sAnims[entryObjs[i]][4], true);
                    } else {
                        m_objs[entryObjs[i]]->setAnimName(&sAnims[entryObjs[i]][5], true);
                    }
                }
                m_result = 0x1F;
                setRule();
                m_unk664 = 3;
                return 2;
            } else if (m_cursor == Entry_ButtonA) {
                for (int i = 0; i < Entry_Count; i++) {
                    if (i == m_cursor) {
                        m_objs[entryObjs[i]]->setAnimName(&sAnims[entryObjs[i]][4], true);
                    } else {
                        m_objs[entryObjs[i]]->setAnimName(&sAnims[entryObjs[i]][5], true);
                    }
                }
                m_result = 0x20;
                setRule();
                m_unk664 = 3;
                return 2;
            }
        } else {
            u32 mask = Pad_Cancel;
            if (m_startIsCancel) {
                mask |= Pad_Start;
            }
            if (pad & mask) {
                for (int i = 0; i < Entry_Count; i++) {
                    m_objs[entryObjs[i]]->setAnimName(&sAnims[entryObjs[i]][5], true);
                }
                m_result = 1;
                setRule();
                m_unk664 = 3;
                return 1;
            }
        }
        break;
    }
    case 2: {
        int obj = entryObjs[m_cursor];
        if (strcmp(m_objs[obj]->getNodeAnimName(), sAnims[obj][3].m_name) == 0) {
            if (m_objs[entryObjs[m_cursor]]->m_modelAnim->getFrame() >= 119.0f) {
                m_objs[entryObjs[m_cursor]]->setAnimName(&sAnims[entryObjs[m_cursor]][2], true);
            }
        }
        // The icons that were moving stop at the frame of their value
        int animated[3] = { 0, 2, 4 };
        for (int i = 0; i < 3; i++) {
            if (m_objs[animated[i]]->m_modelAnim->m_anmObjTexSrtRes->GetUpdateRate() > 0.0f) {
                if (m_objs[animated[i]]->m_modelAnim->m_anmObjTexSrtRes->GetFrame() >= m_frames[animated[i]]) {
                    m_objs[animated[i]]->setFrameTexSrt(m_frames[animated[i]]);
                    m_objs[animated[i]]->m_modelAnim->m_anmObjTexSrtRes->SetUpdateRate(0.0f);
                    m_objs[animated[i]]->setFrameVisible(m_frames[animated[i]]);
                    m_objs[animated[i]]->m_modelAnim->m_anmObjVisRes->SetUpdateRate(0.0f);
                }
            }
        }
        u32 pad = padUpdate();
        selectProc(pad);
        if (pad & Pad_A) {
            if (m_cursor == Entry_ButtonB) {
                for (int i = 0; i < Entry_Count; i++) {
                    if (i == m_cursor) {
                        m_objs[entryObjs[i]]->setAnimName(&sAnims[entryObjs[i]][4], true);
                    } else {
                        m_objs[entryObjs[i]]->setAnimName(&sAnims[entryObjs[i]][5], true);
                    }
                }
                m_result = 0x1F;
                setRule();
                m_unk664 = 3;
                return 2;
            } else if (m_cursor == Entry_ButtonA) {
                for (int i = 0; i < Entry_Count; i++) {
                    if (i == m_cursor) {
                        m_objs[entryObjs[i]]->setAnimName(&sAnims[entryObjs[i]][4], true);
                    } else {
                        m_objs[entryObjs[i]]->setAnimName(&sAnims[entryObjs[i]][5], true);
                    }
                }
                m_result = 0x20;
                setRule();
                m_unk664 = 3;
                return 2;
            }
        } else {
            u32 mask = Pad_Cancel;
            if (m_startIsCancel) {
                mask |= Pad_Start;
            }
            if (pad & mask) {
                for (int i = 0; i < Entry_Count; i++) {
                    m_objs[entryObjs[i]]->setAnimName(&sAnims[entryObjs[i]][5], true);
                }
                m_result = 1;
                setRule();
                m_unk664 = 3;
                return 1;
            }
        }
        break;
    }
    case 3:
        if (m_objs[entryObjs[m_cursor]]->m_modelAnim->getUpdateRate() == 0.0f) {
            return 3;
        }
        break;
    }
    return 0;
}

void muProcRule1::exit() {
    m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[0]->m_sceneModel));
    m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[2]->m_sceneModel));
    m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[4]->m_sceneModel));
    m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[5]->m_sceneModel));
    m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[6]->m_sceneModel));
    for (u32 i = 0; i <= 2; i++) {
        m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[i]));
    }
}

bool muProcRule1::selectProc(int dir) {
    u16 oldCursor = m_cursor;
    u8 oldValue = m_values[m_cursor];
    u16 digits[5] = { 8, 9, 0x10, 0xA, 0xB };
    bool moved = false;
    bool changed = false;
    bool wrapped = false;
    int step = 0;

    if (dir & Pad_Up) {
        do {
            if (m_cursor == Entry_Rule) {
                if (m_side == 0) {
                    m_cursor = Entry_ButtonA;
                }
                if (m_side == 1) {
                    m_cursor = Entry_ButtonB;
                }
            } else if (m_cursor == Entry_ButtonA || m_cursor == Entry_ButtonB) {
                m_cursor = Entry_Stage;
            } else {
                m_cursor--;
            }
        } while (!m_enabled[m_cursor]);
        moved = true;
    } else if (dir & Pad_Down) {
        do {
            if (m_cursor == Entry_Stage) {
                if (m_side == 0) {
                    m_cursor = Entry_ButtonA;
                }
                if (m_side == 1) {
                    m_cursor = Entry_ButtonB;
                }
            } else if (m_cursor == Entry_ButtonA || m_cursor == Entry_ButtonB) {
                m_cursor = Entry_Rule;
            } else {
                m_cursor++;
            }
        } while (!m_enabled[m_cursor]);
        moved = true;
    } else if (dir & Pad_Left) {
        m_repeat = 0;
        step = -1;
    } else if (dir & Pad_Right) {
        m_repeat = 0;
        step = 1;
    } else if (dir & Pad_A) {
        if (m_repeat > 5) {
            step = 2;
        }
    }
    m_repeat++;

    if (step != 0) {
        switch (m_cursor) {
        case Entry_Rule:
            if (step == 2) {
                m_rule = (m_rule + 1) % 3;
                changed = true;
                float frame;
                if (m_rule == 0) {
                    frame = 0.0f;
                } else {
                    frame = 4.0f + 10.0f * (float)(m_rule - 1);
                }
                m_objs[0]->setFrameTexSrt(frame);
                m_objs[0]->setFrameVisible(frame);
            } else if (step == -1) {
                if (m_rule != 0) {
                    m_rule--;
                    changed = true;
                    MuObject* object = m_objs[0];
                    float start = 5.0f * (float)(m_rule * 2 + 1);
                    float stop = 4.0f + 5.0f * (float)(m_rule * 2 + 1);
                    object->setFrameTexSrt(start);
                    object->setFrameVisible(start);
                    object->m_modelAnim->m_anmObjTexSrtRes->SetUpdateRate(1.0f);
                    object->m_modelAnim->m_anmObjVisRes->SetUpdateRate(1.0f);
                    m_frames[0] = stop;
                }
            } else {
                if (m_rule != 2) {
                    m_rule++;
                    changed = true;
                    MuObject* object = m_objs[0];
                    float start = 5.0f * (float)(m_rule * 2 - 2);
                    float stop = 4.0f + 5.0f * (float)(m_rule * 2 - 2);
                    object->setFrameTexSrt(start);
                    object->setFrameVisible(start);
                    object->m_modelAnim->m_anmObjTexSrtRes->SetUpdateRate(1.0f);
                    object->m_modelAnim->m_anmObjVisRes->SetUpdateRate(1.0f);
                    m_frames[0] = stop;
                }
            }
            if (m_rule != oldValue) {
                if (m_rule == 1) {
                    m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[0]));
                    m_group->Insert(m_group->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[2]));
                } else if (oldValue == 1) {
                    m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[2]));
                    m_group->Insert(m_group->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[0]));
                }
            }
            break;
        case Entry_Time:
            if (step != 2) {
                if (m_rule != 1) {
                    int time = m_time + step;
                    if (time < 0) {
                        m_time = 99;
                    } else {
                        m_time = time % 100;
                    }
                    if (m_time == 0) {
                        if (m_mode != 1) {
                            for (int i = 0; i < 5; i++) {
                                removeFromModel(m_mdls[0], m_objs[digits[i]]);
                            }
                            PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(m_mdls[0], m_objs[0x11]->m_sceneModel, "num3N");
                            setAnimFrame(0x11, 1.0f, 0x12);
                        } else {
                            if (step == 1) {
                                m_time = 1;
                            } else {
                                m_time = 99;
                            }
                            setAnimFrame(8, (float)(m_time / 10), 4);
                            setAnimFrame(9, (float)(m_time % 10), 4);
                            wrapped = true;
                            break;
                        }
                    } else if (oldValue == 0) {
                        removeFromModel(m_mdls[0], m_objs[0x11]);
                        PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(m_mdls[0], m_objs[8]->m_sceneModel, "num5N");
                        PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(m_mdls[0], m_objs[9]->m_sceneModel, "num4N");
                        PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(m_mdls[0], m_objs[0x10]->m_sceneModel, "num3N");
                        PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(m_mdls[0], m_objs[10]->m_sceneModel, "num2N");
                        PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(m_mdls[0], m_objs[11]->m_sceneModel, "num1N");
                        for (int i = 0; i < 5; i++) {
                            setAnimFrame(digits[i], 1.0f, 0x12);
                        }
                    }
                    setAnimFrame(8, (float)(m_time / 10), 4);
                    setAnimFrame(9, (float)(m_time % 10), 4);
                } else {
                    int stock = m_stock + step;
                    if (stock < 1) {
                        m_stock = 99;
                    } else if (stock >= 100) {
                        m_stock = 1;
                    } else {
                        m_stock = stock;
                    }
                    setAnimFrame(0xC, (float)(m_stock / 10), 4);
                    setAnimFrame(0xD, (float)(m_stock % 10), 4);
                }
                wrapped = true;
            }
            break;
        case Entry_Handicap:
            if (step == 2) {
                m_handicap = (m_handicap + 1) % 3;
                changed = true;
                float frame;
                if (m_handicap == 0) {
                    frame = 0.0f;
                } else {
                    frame = 4.0f + 10.0f * (float)(m_handicap - 1);
                }
                m_objs[2]->setFrameTexSrt(frame);
                m_objs[2]->setFrameVisible(frame);
            } else if (step == -1) {
                if (m_handicap != 0) {
                    m_handicap--;
                    changed = true;
                    MuObject* object = m_objs[2];
                    float start = 5.0f * (float)(m_handicap * 2 + 1);
                    float stop = 4.0f + 5.0f * (float)(m_handicap * 2 + 1);
                    object->setFrameTexSrt(start);
                    object->setFrameVisible(start);
                    object->m_modelAnim->m_anmObjTexSrtRes->SetUpdateRate(1.0f);
                    object->m_modelAnim->m_anmObjVisRes->SetUpdateRate(1.0f);
                    m_frames[2] = stop;
                }
            } else {
                if (m_handicap != 2) {
                    m_handicap++;
                    changed = true;
                    MuObject* object = m_objs[2];
                    float start = 5.0f * (float)(m_handicap * 2 - 2);
                    float stop = 4.0f + 5.0f * (float)(m_handicap * 2 - 2);
                    object->setFrameTexSrt(start);
                    object->setFrameVisible(start);
                    object->m_modelAnim->m_anmObjTexSrtRes->SetUpdateRate(1.0f);
                    object->m_modelAnim->m_anmObjVisRes->SetUpdateRate(1.0f);
                    m_frames[2] = stop;
                }
            }
            break;
        case Entry_Damage:
            if (step != 2) {
                int damage = m_damage + step;
                if (damage < 5) {
                    m_damage = 20;
                } else if (damage == 21) {
                    m_damage = 5;
                } else {
                    m_damage = damage;
                }
                setAnimFrame(0xE, (float)(m_damage / 10), 4);
                setAnimFrame(0xF, (float)(m_damage % 10), 4);
                wrapped = true;
            }
            break;
        case Entry_Stage:
            if (step == 2) {
                m_stage = (m_stage + 1) % 5;
                changed = true;
                float frame;
                if (m_stage == 0) {
                    frame = 0.0f;
                } else {
                    frame = 4.0f + 10.0f * (float)(m_stage - 1);
                }
                m_objs[4]->setFrameTexSrt(frame);
                m_objs[4]->setFrameVisible(frame);
            } else if (step == -1) {
                if (m_stage != 0) {
                    m_stage--;
                    changed = true;
                    MuObject* object = m_objs[4];
                    float start = 5.0f * (float)(m_stage * 2 + 1);
                    float stop = 4.0f + 5.0f * (float)(m_stage * 2 + 1);
                    object->setFrameTexSrt(start);
                    object->setFrameVisible(start);
                    object->m_modelAnim->m_anmObjTexSrtRes->SetUpdateRate(1.0f);
                    object->m_modelAnim->m_anmObjVisRes->SetUpdateRate(1.0f);
                    m_frames[4] = stop;
                }
            } else {
                if (m_stage != 4) {
                    m_stage++;
                    changed = true;
                    MuObject* object = m_objs[4];
                    float start = 5.0f * (float)(m_stage * 2 - 2);
                    float stop = 4.0f + 5.0f * (float)(m_stage * 2 - 2);
                    object->setFrameTexSrt(start);
                    object->setFrameVisible(start);
                    object->m_modelAnim->m_anmObjTexSrtRes->SetUpdateRate(1.0f);
                    object->m_modelAnim->m_anmObjVisRes->SetUpdateRate(1.0f);
                    m_frames[4] = stop;
                }
            }
            break;
        case Entry_ButtonA:
        case Entry_ButtonB:
            if (step != 2) {
                if (m_hideAdditionalRule != 0) {
                    moved = true;
                    step = 0;
                    m_side = m_side ^ 1;
                    m_cursor = m_side + 5;
                }
            }
            break;
        }
    }

    if (moved || m_dirty == 1) {
        m_dirty = 0;
        if (step == 0) {
            u8* entryObjs = sEntryObjsStock;
            if (m_rule != 1) {
                entryObjs = sEntryObjsTime;
            }
            // The entry the cursor left goes back to the normal look
            m_objs[entryObjs[oldCursor]]->setAnimName(&sAnims[entryObjs[oldCursor]][1], true);
            m_objs[entryObjs[oldCursor]]->m_animNameData = NULL;
            setAnimFrame(entryObjs[oldCursor], 9.0f, 0x11);
            {
                MuObject* object = m_objs[entryObjs[oldCursor]];
                int priority = m_priority;
                object->m_scnMdl->SetPriorityDrawOpa(priority);
                object->m_scnMdl->SetPriorityDrawXlu(priority);
            }
            if (oldCursor == Entry_Time) {
                for (int i = 0; i < 5; i++) {
                    setAnimFrame(digits[i], 0.0f, 0x10);
                    MuObject* object = m_objs[digits[i]];
                    int priority = m_priority + 2;
                    object->m_scnMdl->SetPriorityDrawOpa(priority);
                    object->m_scnMdl->SetPriorityDrawXlu(priority);
                }
                setAnimFrame(0x11, 0.0f, 0x10);
                {
                    MuObject* object = m_objs[0x11];
                    int priority = m_priority + 2;
                    object->m_scnMdl->SetPriorityDrawOpa(priority);
                    object->m_scnMdl->SetPriorityDrawXlu(priority);
                }
                setAnimFrame(0xC, 0.0f, 0x10);
                {
                    MuObject* object = m_objs[0xC];
                    int priority = m_priority + 2;
                    object->m_scnMdl->SetPriorityDrawOpa(priority);
                    object->m_scnMdl->SetPriorityDrawXlu(priority);
                }
                setAnimFrame(0xD, 0.0f, 0x10);
                {
                    MuObject* object = m_objs[0xD];
                    int priority = m_priority + 2;
                    object->m_scnMdl->SetPriorityDrawOpa(priority);
                    object->m_scnMdl->SetPriorityDrawXlu(priority);
                }
            } else if (oldCursor == Entry_Damage) {
                setAnimFrame(0xE, 0.0f, 0x10);
                {
                    MuObject* object = m_objs[0xE];
                    int priority = m_priority + 2;
                    object->m_scnMdl->SetPriorityDrawOpa(priority);
                    object->m_scnMdl->SetPriorityDrawXlu(priority);
                }
                setAnimFrame(0xF, 0.0f, 0x10);
                {
                    MuObject* object = m_objs[0xF];
                    int priority = m_priority + 2;
                    object->m_scnMdl->SetPriorityDrawOpa(priority);
                    object->m_scnMdl->SetPriorityDrawXlu(priority);
                }
            }
            // The entry the cursor went to is lit
            m_objs[entryObjs[m_cursor]]->setAnimName(&sAnims[entryObjs[m_cursor]][3], true);
            {
                MuObject* object = m_objs[entryObjs[m_cursor]];
                int priority = m_priority + 1;
                object->m_scnMdl->SetPriorityDrawOpa(priority);
                object->m_scnMdl->SetPriorityDrawXlu(priority);
            }
            if (m_cursor == Entry_Time) {
                for (int i = 0; i < 5; i++) {
                    setAnimFrame(digits[i], 1.0f, 0x10);
                    MuObject* object = m_objs[digits[i]];
                    int priority = m_priority + 3;
                    object->m_scnMdl->SetPriorityDrawOpa(priority);
                    object->m_scnMdl->SetPriorityDrawXlu(priority);
                }
                setAnimFrame(0x11, 1.0f, 0x10);
                {
                    MuObject* object = m_objs[0x11];
                    int priority = m_priority + 3;
                    object->m_scnMdl->SetPriorityDrawOpa(priority);
                    object->m_scnMdl->SetPriorityDrawXlu(priority);
                }
                setAnimFrame(0xC, 1.0f, 0x10);
                {
                    MuObject* object = m_objs[0xC];
                    int priority = m_priority + 3;
                    object->m_scnMdl->SetPriorityDrawOpa(priority);
                    object->m_scnMdl->SetPriorityDrawXlu(priority);
                }
                setAnimFrame(0xD, 1.0f, 0x10);
                {
                    MuObject* object = m_objs[0xD];
                    int priority = m_priority + 3;
                    object->m_scnMdl->SetPriorityDrawOpa(priority);
                    object->m_scnMdl->SetPriorityDrawXlu(priority);
                }
            } else if (m_cursor == Entry_Damage) {
                setAnimFrame(0xE, 1.0f, 0x10);
                {
                    MuObject* object = m_objs[0xE];
                    int priority = m_priority + 3;
                    object->m_scnMdl->SetPriorityDrawOpa(priority);
                    object->m_scnMdl->SetPriorityDrawXlu(priority);
                }
                setAnimFrame(0xF, 1.0f, 0x10);
                {
                    MuObject* object = m_objs[0xF];
                    int priority = m_priority + 3;
                    object->m_scnMdl->SetPriorityDrawOpa(priority);
                    object->m_scnMdl->SetPriorityDrawXlu(priority);
                }
            }
            g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
        }
    }

    if (changed) {
        g_sndSystem->playSE(static_cast<SndID>(0x23), -1, 0, 0, -1);
        moved = true;
    } else if (wrapped) {
        g_sndSystem->playSE(static_cast<SndID>(0x25), -1, 0, 0, -1);
        moved = true;
    }
    if (moved) {
        setMessage();
    }
    return moved;
}

void muProcRule1::setMessage() {
    int msg;
    switch (m_cursor) {
    case Entry_Rule:
        switch (m_rule) {
        case 0:
            msg = 0;
            break;
        case 1:
            msg = 2;
            break;
        case 2:
            msg = 1;
            break;
        }
        break;
    case Entry_Time:
        if (m_rule != 1) {
            if (m_time == 0) {
                msg = 4;
            } else {
                msg = 3;
            }
        } else {
            msg = 5;
        }
        break;
    case Entry_Handicap:
        msg = m_handicap + 6;
        break;
    case Entry_Damage:
        msg = 9;
        break;
    case Entry_Stage:
        msg = m_stage + 10;
        break;
    case Entry_ButtonA:
        msg = 0xF;
        break;
    case Entry_ButtonB:
        msg = 0x10;
        break;
    }
    m_panel->setMsg(msg, 1, 0);
}

void muProcRule1::setRule() {
    u8* setting = getRuleBytes();
    switch (m_rule) {
    case 0:
        setting[2] = 0;
        break;
    case 2:
        setting[2] = 2;
        break;
    case 1:
        setting[2] = 1;
        break;
    }
    bool handicapChanged = false;
    switch (m_handicap) {
    case 0:
        if (setting[5] != 0) {
            setting[5] = 0;
            handicapChanged = true;
        }
        break;
    case 1:
        if (setting[5] != 2) {
            setting[5] = 2;
            handicapChanged = true;
        }
        break;
    case 2:
        if (setting[5] != 1) {
            setting[5] = 1;
            handicapChanged = true;
        }
        break;
    }
    if (handicapChanged) {
        fn_80050294();
    }
    setting[3] = m_time;
    setting[6] = m_damage;
    setting[7] = m_stage;
    setting[4] = m_stock;
}

void muProcRule1::playSE_CursorMove() {
    g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
}
