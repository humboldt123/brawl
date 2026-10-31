#include <gf/gf_application.h>
#include <gm/gm_global.h>
#include <memory.h>
#include <nw4r/g3d/g3d_scnobj.h>
#include <snd/snd_system.h>
#include <string.h>
#include <types.h>

#include <mu/mu_proc_rule2.h>

// main.dol functions with names that the shared headers do not declare.
// nw4r::g3d::ScnMdlExpand::PushBack(object, node name): attaches the object to the node of the model
extern "C" void PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(void* scnMdl, void* object, const char* name);
// GameGlobal::checkHideMenuRandomStageSW()

// The animations of each entry of the screen, one row for each icon (see muProcRule1).
static MuAnimNameData sAnims[6][6] = {
    {
        { 0.0f, 9.0f, 0, "MenMainIcon0131_TopN__2", 0x11 },
        { 0.0f, 9.0f, 0, "MenMainIcon0131_TopN__0", 0x11 },
        { 0.0f, 59.0f, 1, "MenMainIcon0131_TopN__5", 0x11 },
        { 0.0f, 119.0f, 0, "MenMainIcon0131_TopN__4", 0x11 },
        { 0.0f, 29.0f, 0, "MenMainIcon0131_TopN__3", 0x11 },
        { 0.0f, 19.0f, 0, "MenMainIcon0131_TopN__1", 0x11 },
    },
    {
        { 0.0f, 9.0f, 0, "MenMainIcon0132_TopN__2", 0x11 },
        { 0.0f, 9.0f, 0, "MenMainIcon0132_TopN__0", 0x11 },
        { 0.0f, 59.0f, 1, "MenMainIcon0132_TopN__5", 0x11 },
        { 0.0f, 119.0f, 0, "MenMainIcon0132_TopN__4", 0x11 },
        { 0.0f, 29.0f, 0, "MenMainIcon0132_TopN__3", 0x11 },
        { 0.0f, 19.0f, 0, "MenMainIcon0132_TopN__1", 0x11 },
    },
    {
        { 0.0f, 9.0f, 0, "MenMainIcon0133_TopN__2", 0x11 },
        { 0.0f, 9.0f, 0, "MenMainIcon0133_TopN__0", 0x11 },
        { 0.0f, 59.0f, 1, "MenMainIcon0133_TopN__5", 0x11 },
        { 0.0f, 119.0f, 0, "MenMainIcon0133_TopN__4", 0x11 },
        { 0.0f, 29.0f, 0, "MenMainIcon0133_TopN__3", 0x11 },
        { 0.0f, 19.0f, 0, "MenMainIcon0133_TopN__1", 0x11 },
    },
    {
        { 0.0f, 9.0f, 0, "MenMainIcon0134_TopN__2", 0x11 },
        { 0.0f, 9.0f, 0, "MenMainIcon0134_TopN__0", 0x11 },
        { 0.0f, 59.0f, 1, "MenMainIcon0134_TopN__5", 0x11 },
        { 0.0f, 119.0f, 0, "MenMainIcon0134_TopN__4", 0x11 },
        { 0.0f, 29.0f, 0, "MenMainIcon0134_TopN__3", 0x11 },
        { 0.0f, 19.0f, 0, "MenMainIcon0134_TopN__1", 0x11 },
    },
    {
        { 0.0f, 9.0f, 0, "MenMainIcon0136_TopN__2", 0x11 },
        { 0.0f, 9.0f, 0, "MenMainIcon0136_TopN__0", 0x11 },
        { 0.0f, 59.0f, 1, "MenMainIcon0136_TopN__5", 0x11 },
        { 0.0f, 119.0f, 0, "MenMainIcon0136_TopN__4", 0x11 },
        { 0.0f, 29.0f, 0, "MenMainIcon0136_TopN__3", 0x11 },
        { 0.0f, 19.0f, 0, "MenMainIcon0136_TopN__1", 0x11 },
    },
    {
        { 0.0f, 9.0f, 0, "MenMainIcon0135_TopN__2", 0x11 },
        { 0.0f, 9.0f, 0, "MenMainIcon0135_TopN__0", 0x11 },
        { 0.0f, 59.0f, 1, "MenMainIcon0135_TopN__5", 0x11 },
        { 0.0f, 119.0f, 0, "MenMainIcon0135_TopN__4", 0x11 },
        { 0.0f, 29.0f, 0, "MenMainIcon0135_TopN__3", 0x11 },
        { 0.0f, 19.0f, 0, "MenMainIcon0135_TopN__1", 0x11 },
    },
};

// Takes an object off the node of a model that has room for objects (ScnMdlExpand, a group of the scene).
static inline void removeFromModel(nw4r::g3d::G3dObj* model, MuObject* object) {
    reinterpret_cast<nw4r::g3d::ScnGroup*>(model)->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(object->m_sceneModel));
}

// The setting of the rules of the game that the screen changes.
static inline u8* getRuleBytes() {
    return reinterpret_cast<u8*>(g_GameGlobal->m_setRule);
}

muProcRule2::muProcRule2(MuObject** objs, nw4r::g3d::G3dObj** mdls, int mode, int priority) : muProcMenu(objs, mdls) {
    m_priority = priority;
    m_startIsCancel = true;
    m_mode = mode;
    for (int i = 0; i < Entry_Count; i++) {
        m_enabled[i] = true;
    }
}

inline void muProcRule2::setRule() {
    u8* setting = getRuleBytes();
    setting[8] = m_stockTime;
    setting[9] = m_teamAttack;
    setting[10] = m_pause;
    setting[11] = m_scoreDisplay;
    setting[12] = m_damageGauge;
}

inline void muProcRule2::setMessage() {
    int msg;
    switch (m_cursor) {
    case Entry_StockTime:
        msg = 0x11;
        break;
    case Entry_TeamAttack:
        msg = m_teamAttack + 0x12;
        break;
    case Entry_Pause:
        msg = m_pause + 0x14;
        break;
    case Entry_ScoreDisplay:
        msg = m_scoreDisplay + 0x16;
        break;
    case Entry_DamageGauge:
        msg = m_damageGauge + 0x18;
        break;
    case Entry_Button:
        msg = 0x1A;
        break;
    }
    m_panel->setMsg(msg, 1, 0);
}

void muProcRule2::init(nw4r::g3d::ScnGroup* group, int from) {
    m_group = group;
    u8* setting = getRuleBytes();
    m_hideRandomStage = g_GameGlobal->checkHideMenuRandomStageSW();
    for (int i = 0; i < Entry_Count; i++) {
        m_enabled[i] = true;
    }
    // Entries the mode of the scene does not let the player change
    if (m_mode == 6 && setting[0x18] == 2) {
        m_enabled[Entry_StockTime] = false;
    }
    if (m_mode == 2) {
        m_enabled[Entry_Pause] = false;
    }
    m_stockTime = setting[8];
    m_teamAttack = setting[9];
    m_pause = setting[10];
    m_scoreDisplay = setting[11];
    m_damageGauge = setting[12];

    // Put the objects of the screen in the scene
    m_group->Insert(m_group->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[3]));
    for (u32 i = 0x13; i <= 0x17; i++) {
        if (!(m_hideRandomStage == 0 && i == 0x17)) {
            m_group->Insert(m_group->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[i]->m_sceneModel));
        }
    }
    if (m_stockTime == 0) {
        PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(m_mdls[3], m_objs[0x1D]->m_sceneModel, "num3N");
    } else {
        PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(m_mdls[3], m_objs[0x18]->m_sceneModel, "num5N");
        PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(m_mdls[3], m_objs[0x19]->m_sceneModel, "num4N");
        PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(m_mdls[3], m_objs[0x1C]->m_sceneModel, "num3N");
        PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(m_mdls[3], m_objs[0x1A]->m_sceneModel, "num2N");
        PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(m_mdls[3], m_objs[0x1B]->m_sceneModel, "num1N");
    }

    switch (from) {
    case 0:
        m_unk664 = 0;
        m_cursor = 0;
        m_code = 0x1E;
        while (!canSelect()) {
            if (m_cursor == Entry_Button) {
                m_cursor = 0;
            } else {
                m_cursor++;
            }
        }
        break;
    case 3:
        m_unk664 = 1;
        m_cursor = Entry_Button;
        m_panel->changeProcess1(0xF, 0x1A);
        m_code = 0x21;
        break;
    }

    // The numbers are shown dark until the cursor goes to them
    for (u32 i = 0x18; i <= 0x1D; i++) {
        setAnimFrame(i, 1.0f, 2);
        if (m_cursor == 0) {
            setAnimFrame(i, 1.0f, 0x10);
            MuObject* object = m_objs[i];
            int priority = m_priority + 7;
            object->m_scnMdl->SetPriorityDrawOpa(priority);
            object->m_scnMdl->SetPriorityDrawXlu(priority);
        } else {
            setAnimFrame(i, 0.0f, 0x10);
        }
    }
    setAnimFrame(0x18, (float)(m_stockTime / 10), 4);
    setAnimFrame(0x19, (float)(m_stockTime % 10), 4);

    MuAnimNameData* anims = sAnims[0];
    for (int i = 0; i < Entry_Count; i++) {
        if (i == m_cursor) {
            m_objs[0x12 + i]->setAnimName(&sAnims[i][0], true);
            MuObject* object = m_objs[0x12 + i];
            int priority = m_priority + 5;
            object->m_scnMdl->SetPriorityDrawOpa(priority);
            object->m_scnMdl->SetPriorityDrawXlu(priority);
        } else {
            m_objs[0x12 + i]->setAnimName(&sAnims[i][1], true);
        }
    }

    // The icons of the switches show the frame of the value they are on
    m_objs[0x13]->m_modelAnim->m_anmObjTexSrtRes->SetUpdateRate(0.0f);
    m_objs[0x13]->m_modelAnim->m_anmObjVisRes->SetUpdateRate(0.0f);
    m_objs[0x13]->setFrameVisible(4.0f * (float)m_teamAttack);
    m_objs[0x13]->setFrameTexSrt(4.0f * (float)m_teamAttack);
    m_objs[0x14]->m_modelAnim->m_anmObjTexSrtRes->SetUpdateRate(0.0f);
    m_objs[0x14]->m_modelAnim->m_anmObjVisRes->SetUpdateRate(0.0f);
    m_objs[0x14]->setFrameVisible(4.0f * (float)m_pause);
    m_objs[0x14]->setFrameTexSrt(4.0f * (float)m_pause);
    m_objs[0x15]->m_modelAnim->m_anmObjTexSrtRes->SetUpdateRate(0.0f);
    m_objs[0x15]->m_modelAnim->m_anmObjVisRes->SetUpdateRate(0.0f);
    m_objs[0x15]->setFrameVisible(4.0f * (float)m_scoreDisplay);
    m_objs[0x15]->setFrameTexSrt(4.0f * (float)m_scoreDisplay);
    m_objs[0x16]->m_modelAnim->m_anmObjTexSrtRes->SetUpdateRate(0.0f);
    m_objs[0x16]->m_modelAnim->m_anmObjVisRes->SetUpdateRate(0.0f);
    m_objs[0x16]->setFrameVisible(4.0f * (float)m_damageGauge);
    m_objs[0x16]->setFrameTexSrt(4.0f * (float)m_damageGauge);

    m_panel->changeProcess1(0xF, 0x11);

    // The entries that cannot be changed are shown grey
    for (int i = 0; i < Entry_Count; i++) {
        if (!m_enabled[i]) {
            m_objs[0x12 + i]->changeAnimN(0, 4, false, false);
            m_objs[0x12 + i]->setFrameTex(1.0f);
        }
    }
    m_repeat = 0;
    setMessage();
}

int muProcRule2::proc() {
    switch (m_unk664) {
    case 0: {
        if (m_objs[0x12 + m_cursor]->m_modelAnim->getFrame() == 9.0f) {
            m_objs[0x12 + m_cursor]->setAnimName(&sAnims[m_cursor][3], true);
            m_unk664 = 2;
        }
        u32 pad = padUpdate();
        if (selectProc(pad)) {
            m_unk664 = 2;
            return 4;
        }
        u32 mask = Pad_Cancel;
        if (m_startIsCancel) {
            mask |= Pad_Start;
        }
        if (pad & mask) {
            for (int i = 0; i < Entry_Count; i++) {
                m_objs[0x12 + i]->setAnimName(&sAnims[i][5], true);
            }
            m_result = 0x1E;
            setRule();
            m_unk664 = 4;
            return 1;
        }
        break;
    }
    case 1: {
        if (m_objs[0x12 + m_cursor]->m_modelAnim->getFrame() == 9.0f) {
            m_objs[0x12 + m_cursor]->setAnimName(&sAnims[m_cursor][3], true);
            m_unk664 = 2;
        }
        u32 pad = padUpdate();
        if (selectProc(pad)) {
            m_unk664 = 2;
            return 4;
        }
        if (pad & Pad_A) {
            if (m_cursor == Entry_Button) {
                for (int i = 0; i < Entry_Count; i++) {
                    if (i == m_cursor) {
                        m_objs[0x12 + i]->setAnimName(&sAnims[i][4], true);
                    } else {
                        m_objs[0x12 + i]->setAnimName(&sAnims[i][5], true);
                    }
                }
                m_result = 0x21;
                setRule();
                m_unk664 = 4;
                return 2;
            }
        } else {
            u32 mask = Pad_Cancel;
            if (m_startIsCancel) {
                mask |= Pad_Start;
            }
            if (pad & mask) {
                for (int i = 0; i < Entry_Count; i++) {
                    m_objs[0x12 + i]->setAnimName(&sAnims[i][5], true);
                }
                m_result = 0x1E;
                setRule();
                m_unk664 = 4;
                return 1;
            }
        }
        break;
    }
    case 2: {
        int cursor = m_cursor;
        if (strcmp(m_objs[0x12 + cursor]->getNodeAnimName(), sAnims[cursor][3].m_name) == 0) {
            if (m_objs[0x12 + m_cursor]->m_modelAnim->getFrame() >= 119.0f) {
                m_objs[0x12 + m_cursor]->setAnimName(&sAnims[m_cursor][2], true);
            }
        }
        // The icons that were moving stop at the frame of their value
        int animated[4] = { 0x13, 0x14, 0x15, 0x16 };
        for (int i = 0; i < 4; i++) {
            if (m_objs[animated[i]]->m_modelAnim->m_anmObjTexSrtRes->GetUpdateRate() > 0.0f) {
                if (m_objs[animated[i]]->m_modelAnim->m_anmObjTexSrtRes->GetFrame() >= m_frames[animated[i] - 0x13]) {
                    m_objs[animated[i]]->setFrameTexSrt(m_frames[animated[i] - 0x13]);
                    m_objs[animated[i]]->m_modelAnim->m_anmObjTexSrtRes->SetUpdateRate(0.0f);
                    m_objs[animated[i]]->setFrameVisible(m_frames[animated[i] - 0x13]);
                    m_objs[animated[i]]->m_modelAnim->m_anmObjVisRes->SetUpdateRate(0.0f);
                }
            }
        }
        u32 pad = padUpdate();
        selectProc(pad);
        if (pad & Pad_A) {
            if (m_cursor == Entry_Button) {
                for (int i = 0; i < Entry_Count; i++) {
                    if (i == m_cursor) {
                        m_objs[0x12 + i]->setAnimName(&sAnims[i][4], true);
                    } else {
                        m_objs[0x12 + i]->setAnimName(&sAnims[i][5], true);
                    }
                }
                m_result = 0x21;
                setRule();
                m_unk664 = 4;
                return 2;
            }
        } else {
            u32 mask = Pad_Cancel;
            if (m_startIsCancel) {
                mask |= Pad_Start;
            }
            if (pad & mask) {
                for (int i = 0; i < Entry_Count; i++) {
                    m_objs[0x12 + i]->setAnimName(&sAnims[i][5], true);
                }
                m_result = 0x1E;
                setRule();
                m_unk664 = 4;
                return 1;
            }
        }
        break;
    }
    case 4:
        if (m_objs[0x12 + m_cursor]->m_modelAnim->getUpdateRate() == 0.0f) {
            return 3;
        }
        break;
    }
    return 0;
}

void muProcRule2::exit() {
    m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[3]));
    for (u32 i = 0x13; i <= 0x17; i++) {
        m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[i]->m_sceneModel));
    }
}

bool muProcRule2::selectProc(int dir) {
    u16 digits[5] = { 0x18, 0x19, 0x1C, 0x1A, 0x1B };
    const char* names[5] = { "num5N", "num4N", "num3N", "num2N", "num1N" };
    bool moved = false;
    bool changed = false;
    bool wrapped = false;
    int step = 0;
    u16 oldCursor = m_cursor;

    if (dir & Pad_Up) {
        do {
            if (m_cursor == 0) {
                m_cursor = Entry_Button;
            } else {
                m_cursor--;
            }
        } while (!canSelect());
        moved = true;
    } else if (dir & Pad_Down) {
        do {
            if (m_cursor == Entry_Button) {
                m_cursor = 0;
            } else {
                m_cursor++;
            }
        } while (!canSelect());
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
        case Entry_StockTime:
            if (step != 2) {
                u8 oldValue = m_stockTime;
                int time = m_stockTime + step;
                if (time < 0) {
                    m_stockTime = 99;
                } else {
                    m_stockTime = time % 100;
                }
                if (m_stockTime == 0) {
                    for (int i = 0; i < 5; i++) {
                        removeFromModel(m_mdls[3], m_objs[digits[i]]);
                    }
                    PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(m_mdls[3], m_objs[0x1D]->m_sceneModel, "num3N");
                    setAnimFrame(0x1D, 1.0f, 0x12);
                } else {
                    if (oldValue == 0) {
                        removeFromModel(m_mdls[3], m_objs[0x1D]);
                        for (int i = 0; i < 5; i++) {
                            PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(m_mdls[3], m_objs[digits[i]]->m_sceneModel, names[i]);
                            setAnimFrame(digits[i], 1.0f, 0x12);
                        }
                    }
                    setAnimFrame(0x18, (float)(m_stockTime / 10), 4);
                    setAnimFrame(0x19, (float)(m_stockTime % 10), 4);
                }
                wrapped = true;
            }
            break;
        case Entry_TeamAttack:
            if (step == 2) {
                m_teamAttack = (m_teamAttack + 1) % 2;
                changed = true;
                float frame;
                if (m_teamAttack == 0) {
                    frame = 0.0f;
                } else {
                    frame = 4.0f + 10.0f * (float)(m_teamAttack - 1);
                }
                m_objs[0x13]->setFrameTexSrt(frame);
                m_objs[0x13]->setFrameVisible(frame);
            } else if (step == -1) {
                if (m_teamAttack != 0) {
                    m_teamAttack--;
                    changed = true;
                    MuObject* object = m_objs[0x13];
                    float start = 5.0f * (float)(m_teamAttack * 2 + 1);
                    float stop = 4.0f + 5.0f * (float)(m_teamAttack * 2 + 1);
                    object->setFrameTexSrt(start);
                    object->setFrameVisible(start);
                    object->m_modelAnim->m_anmObjTexSrtRes->SetUpdateRate(1.0f);
                    object->m_modelAnim->m_anmObjVisRes->SetUpdateRate(1.0f);
                    m_frames[0] = stop;
                }
            } else {
                if (m_teamAttack != 1) {
                    m_teamAttack++;
                    changed = true;
                    MuObject* object = m_objs[0x13];
                    float start = 5.0f * (float)(m_teamAttack * 2 - 2);
                    float stop = 4.0f + 5.0f * (float)(m_teamAttack * 2 - 2);
                    object->setFrameTexSrt(start);
                    object->setFrameVisible(start);
                    object->m_modelAnim->m_anmObjTexSrtRes->SetUpdateRate(1.0f);
                    object->m_modelAnim->m_anmObjVisRes->SetUpdateRate(1.0f);
                    m_frames[0] = stop;
                }
            }
            break;
        case Entry_Pause:
            if (step == 2) {
                m_pause = (m_pause + 1) % 2;
                changed = true;
                float frame;
                if (m_pause == 0) {
                    frame = 0.0f;
                } else {
                    frame = 4.0f + 10.0f * (float)(m_pause - 1);
                }
                m_objs[0x14]->setFrameTexSrt(frame);
                m_objs[0x14]->setFrameVisible(frame);
            } else if (step == -1) {
                if (m_pause != 0) {
                    m_pause--;
                    changed = true;
                    MuObject* object = m_objs[0x14];
                    float start = 5.0f * (float)(m_pause * 2 + 1);
                    float stop = 4.0f + 5.0f * (float)(m_pause * 2 + 1);
                    object->setFrameTexSrt(start);
                    object->setFrameVisible(start);
                    object->m_modelAnim->m_anmObjTexSrtRes->SetUpdateRate(1.0f);
                    object->m_modelAnim->m_anmObjVisRes->SetUpdateRate(1.0f);
                    m_frames[1] = stop;
                }
            } else {
                if (m_pause != 1) {
                    m_pause++;
                    changed = true;
                    MuObject* object = m_objs[0x14];
                    float start = 5.0f * (float)(m_pause * 2 - 2);
                    float stop = 4.0f + 5.0f * (float)(m_pause * 2 - 2);
                    object->setFrameTexSrt(start);
                    object->setFrameVisible(start);
                    object->m_modelAnim->m_anmObjTexSrtRes->SetUpdateRate(1.0f);
                    object->m_modelAnim->m_anmObjVisRes->SetUpdateRate(1.0f);
                    m_frames[1] = stop;
                }
            }
            break;
        case Entry_ScoreDisplay:
            if (step == 2) {
                m_scoreDisplay = (m_scoreDisplay + 1) % 2;
                changed = true;
                float frame;
                if (m_scoreDisplay == 0) {
                    frame = 0.0f;
                } else {
                    frame = 4.0f + 10.0f * (float)(m_scoreDisplay - 1);
                }
                m_objs[0x15]->setFrameTexSrt(frame);
                m_objs[0x15]->setFrameVisible(frame);
            } else if (step == -1) {
                if (m_scoreDisplay != 0) {
                    m_scoreDisplay--;
                    changed = true;
                    MuObject* object = m_objs[0x15];
                    float start = 5.0f * (float)(m_scoreDisplay * 2 + 1);
                    float stop = 4.0f + 5.0f * (float)(m_scoreDisplay * 2 + 1);
                    object->setFrameTexSrt(start);
                    object->setFrameVisible(start);
                    object->m_modelAnim->m_anmObjTexSrtRes->SetUpdateRate(1.0f);
                    object->m_modelAnim->m_anmObjVisRes->SetUpdateRate(1.0f);
                    m_frames[2] = stop;
                }
            } else {
                if (m_scoreDisplay != 1) {
                    m_scoreDisplay++;
                    changed = true;
                    MuObject* object = m_objs[0x15];
                    float start = 5.0f * (float)(m_scoreDisplay * 2 - 2);
                    float stop = 4.0f + 5.0f * (float)(m_scoreDisplay * 2 - 2);
                    object->setFrameTexSrt(start);
                    object->setFrameVisible(start);
                    object->m_modelAnim->m_anmObjTexSrtRes->SetUpdateRate(1.0f);
                    object->m_modelAnim->m_anmObjVisRes->SetUpdateRate(1.0f);
                    m_frames[2] = stop;
                }
            }
            break;
        case Entry_DamageGauge:
            if (step == 2) {
                m_damageGauge = (m_damageGauge + 1) % 2;
                changed = true;
                float frame;
                if (m_damageGauge == 0) {
                    frame = 0.0f;
                } else {
                    frame = 4.0f + 10.0f * (float)(m_damageGauge - 1);
                }
                m_objs[0x16]->setFrameTexSrt(frame);
                m_objs[0x16]->setFrameVisible(frame);
            } else if (step == -1) {
                if (m_damageGauge != 0) {
                    m_damageGauge--;
                    changed = true;
                    MuObject* object = m_objs[0x16];
                    float start = 5.0f * (float)(m_damageGauge * 2 + 1);
                    float stop = 4.0f + 5.0f * (float)(m_damageGauge * 2 + 1);
                    object->setFrameTexSrt(start);
                    object->setFrameVisible(start);
                    object->m_modelAnim->m_anmObjTexSrtRes->SetUpdateRate(1.0f);
                    object->m_modelAnim->m_anmObjVisRes->SetUpdateRate(1.0f);
                    m_frames[3] = stop;
                }
            } else {
                if (m_damageGauge != 1) {
                    m_damageGauge++;
                    changed = true;
                    MuObject* object = m_objs[0x16];
                    float start = 5.0f * (float)(m_damageGauge * 2 - 2);
                    float stop = 4.0f + 5.0f * (float)(m_damageGauge * 2 - 2);
                    object->setFrameTexSrt(start);
                    object->setFrameVisible(start);
                    object->m_modelAnim->m_anmObjTexSrtRes->SetUpdateRate(1.0f);
                    object->m_modelAnim->m_anmObjVisRes->SetUpdateRate(1.0f);
                    m_frames[3] = stop;
                }
            }
            break;
        }
    }

    if (moved && step == 0) {
        if (m_cursor == 0) {
            padRepeatClear(GF_PAD_BUTTON_BIT_DPAD_UP);
        } else if (m_cursor == Entry_Button) {
            padRepeatClear(GF_PAD_BUTTON_BIT_DPAD_DOWN);
        }
        // The entry the cursor left goes back to the normal look
        m_objs[0x12 + oldCursor]->setAnimName(&sAnims[oldCursor][1], true);
        m_objs[0x12 + oldCursor]->m_animNameData = NULL;
        setAnimFrame(0x12 + oldCursor, 9.0f, 0x11);
        {
            MuObject* object = m_objs[0x12 + oldCursor];
            int priority = m_priority + 4;
            object->m_scnMdl->SetPriorityDrawOpa(priority);
            object->m_scnMdl->SetPriorityDrawXlu(priority);
        }
        if (oldCursor == 0) {
            for (int i = 0; i < 5; i++) {
                setAnimFrame(digits[i], 0.0f, 0x10);
                MuObject* object = m_objs[digits[i]];
                int priority = m_priority + 6;
                object->m_scnMdl->SetPriorityDrawOpa(priority);
                object->m_scnMdl->SetPriorityDrawXlu(priority);
            }
            setAnimFrame(0x1D, 0.0f, 0x10);
            {
                MuObject* object = m_objs[0x1D];
                int priority = m_priority + 6;
                object->m_scnMdl->SetPriorityDrawOpa(priority);
                object->m_scnMdl->SetPriorityDrawXlu(priority);
            }
        }
        // The entry the cursor went to is lit
        m_objs[0x12 + m_cursor]->setAnimName(&sAnims[m_cursor][3], true);
        {
            MuObject* object = m_objs[0x12 + m_cursor];
            int priority = m_priority + 5;
            object->m_scnMdl->SetPriorityDrawOpa(priority);
            object->m_scnMdl->SetPriorityDrawXlu(priority);
        }
        if (m_cursor == 0) {
            for (int i = 0; i < 5; i++) {
                setAnimFrame(digits[i], 1.0f, 0x10);
                MuObject* object = m_objs[digits[i]];
                int priority = m_priority + 7;
                object->m_scnMdl->SetPriorityDrawOpa(priority);
                object->m_scnMdl->SetPriorityDrawXlu(priority);
            }
            setAnimFrame(0x1D, 1.0f, 0x10);
            {
                MuObject* object = m_objs[0x1D];
                int priority = m_priority + 7;
                object->m_scnMdl->SetPriorityDrawOpa(priority);
                object->m_scnMdl->SetPriorityDrawXlu(priority);
            }
        }
        g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
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

void muProcRule2::playSE_CursorMove() {
    g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
}
