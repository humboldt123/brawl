#include <types.h>

#include <gm/gm_global.h>
#include <gm/gm_set_rule.h>
#include <nw4r/g3d/g3d_anmclr.h>
#include <nw4r/g3d/g3d_scnobj.h>
#include <snd/snd_system.h>
#include <string.h>

#include <mu/mu_proc_special.h>


// The panel: comes in, goes out.
static MuAnimIdxData sPanelAnims[2] = {
    { 0.0f, 9.0f, 0, 0, 0x1 },
    { 10.0f, 19.0f, 0, 0, 0x1 },
};
// The cursor blinks.
static MuAnimIdxData sCursorAnim = { 0.0f, 30.0f, 1, 0, 0x10 };
// The icons of the settings: come in, go out, and the colour of each value.
static MuAnimIdxData sSettingAnims[5] = {
    { 0.0f, 9.0f, 0, 0, 0x1 },
    { 10.0f, 19.0f, 0, 0, 0x1 },
    { 0.0f, 3.0f, 0, 0, 0x10 },
    { 4.0f, 7.0f, 0, 0, 0x10 },
    { 8.0f, 11.0f, 0, 0, 0x10 },
};
// The model of Mario stands.
static MuAnimIdxData sMarioAnim = { 0.0f, 60.0f, 1, 0, 0x1 };

// The help of the line under the cursor.
static inline void printHelp(muProcSpecial* proc) {
    if (proc->m_cursor == muProcSpecial::Cursor_Start) {
        proc->m_msg->printIndex(muProcSpecial::Msg_Help, muProcSpecial::Msg_HelpStart);
    } else {
        proc->m_msg->printIndex(muProcSpecial::Msg_Help,
                                muProcSpecial::Msg_HelpBase + proc->m_cursor * 3 + proc->m_settings[proc->m_cursor]);
    }
}

// The settings go to the rules of the match.
static inline void storeSettings(muProcSpecial* proc) {
    gmSetRule* rule = g_GameGlobal->m_setRule;
    rule->m_spMeleeSetting1 = proc->m_settings[0];
    rule->m_spMeleeSetting2 = proc->m_settings[1];
    rule->m_spMeleeSetting3 = proc->m_settings[2];
    rule->m_spMeleeSetting4 = proc->m_settings[3];
    rule->m_spMeleeSetting5 = proc->m_settings[4];
    rule->m_spMeleeSetting6 = proc->m_settings[5];
    rule->m_spMeleeSetting7 = proc->m_settings[6];
    rule->m_spMeleeSetting8 = proc->m_settings[7];
}

muProcSpecial::muProcSpecial(MuObject** objs, nw4r::g3d::G3dObj** mdls, void* msgData) : muProcMenu(objs, mdls) {
    m_msg = MuMsg::create(1, Heaps::MenuInstance, 0x2B);
    m_msg->allocMsgBuf(Msg_Buf, 2);
    m_msg->setMsgData(msgData);
    for (int i = 0; i < 2; i++) {
        m_msg->setFontWidthModeAuto(i);
        m_msg->attachScnMdlSimple(i, reinterpret_cast<nw4r::g3d::ScnMdlSimple*>(m_objs[Obj_Panel]->m_sceneModel), i,
                                  1.0f / 16.0f);
        m_msg->setAlignMode(i, MuMsg::Align_Center);
        m_msg->setEdgeEnable(i, false);
    }
    m_msg->setFontColor(Msg_Rules, 0x66, 0x4F, 0x4D, 0xFF);
    m_msg->setFontColor(Msg_Help, 0xE0, 0xE0, 0xE0, 0xFF);
}

void muProcSpecial::init(nw4r::g3d::ScnGroup* group, int from) {
    m_group = group;
    m_cursor = 0;
    m_settings[0] = 0;
    m_settings[1] = 0;
    m_settings[2] = 0;
    m_settings[3] = 0;
    m_settings[4] = 0;
    m_settings[5] = 0;
    m_settings[6] = 0;
    m_settings[7] = 0;
    m_unk690 = 0;
    if (static_cast<u32>(from) != 1) {
        // Back from the match: the settings of the last one
        gmSetRule* rule = g_GameGlobal->m_setRule;
        m_settings[0] = rule->m_spMeleeSetting1;
        m_settings[1] = rule->m_spMeleeSetting2;
        m_settings[2] = rule->m_spMeleeSetting3;
        m_settings[3] = rule->m_spMeleeSetting4;
        m_settings[4] = rule->m_spMeleeSetting5;
        m_settings[5] = rule->m_spMeleeSetting6;
        m_settings[6] = rule->m_spMeleeSetting7;
        m_settings[7] = rule->m_spMeleeSetting8;
    } else {
        g_GameGlobal->initSelCharData();
    }

    group->Insert(group->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[Mdl_Panel]));
    for (u32 i = 0; i < SettingCount; i++) {
        group->Insert(group->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[Obj_Setting0 + i]->m_sceneModel));
    }
    group->Insert(group->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[Mdl_Mario]));
    group->Insert(group->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[Obj_Mario0 + 2]->m_sceneModel));

    m_objs[Obj_Panel]->setAnimIdx(&sPanelAnims[0], false);
    setAnimFrame(Obj_Panel, 0.0f, 0x10);
    m_objs[Obj_Cursor]->setAnimIdx(&sCursorAnim, false);
    setAnimFrame(Obj_Cursor, 0.0f, 1);
    setAnimFrame(Obj_Cursor, m_settings[0], 2);
    for (int i = 0; i < SettingCount; i++) {
        int obj = Obj_Setting0 + i;
        m_objs[obj]->setAnimIdx(&sSettingAnims[0], false);
        m_objs[obj]->setFrameMatCol(3.0f + 4.0f * m_settings[i]);
        m_objs[obj]->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(0.0f);
    }
    m_objs[Obj_Mario0]->setAnimIdx(&sMarioAnim, false);
    setMarioModel();

    if (static_cast<u32>(from) == 1) {
        // From the main menu: the panel comes in and the announcer calls the screen
        m_code = 1;
        m_unk664 = Step_Come;
        if (!g_sndSystem->isPlaySEID(static_cast<SndID>(0x202E))) {
            g_sndSystem->playSEwithFixEffect(static_cast<SndID>(0x202E), 1.0f, 1.0f);
        }
        m_panel->changeProcess1(0x22, 0);
    } else {
        m_code = 0x22;
        m_unk664 = Step_Select;
        setAnimFrame(Obj_Panel, 9.0f, 1);
        for (int i = 0; i < SettingCount; i++) {
            setAnimFrame(Obj_Setting0 + i, 9.0f, 1);
        }
        m_panel->changeProcess1(7, 0);
        m_panel->startupSpecialMelee();
    }
    printRuleMsg();
    printHelp(this);
}

int muProcSpecial::proc() {
    switch (m_unk664) {
    case Step_Come:
        if (m_objs[Obj_Panel]->m_modelAnim->getFrame() == 9.0f) {
            m_unk664 = Step_Select;
        }
        break;
    case Step_Select: {
        u32 pad = padUpdate();
        selectProc(pad);
        if (pad & Pad_Start) {
            // Start: the cursor jumps to the start line and the match starts
            m_cursor = Cursor_Start;
            g_sndSystem->playSE(static_cast<SndID>(0x13), -1, 0, 0, -1);
            printHelp(this);
            setAnimFrame(Obj_Panel, 1.0f, 0x10);
            setAnimFrame(Obj_Cursor, 3.0f, 2);
            setAnimFrame(Obj_Cursor, m_cursor, 1);
            m_unk664 = Step_Start;
            storeSettings(this);
        } else if (pad & Pad_Ok) {
            if (m_cursor == Cursor_Start) {
                exitScene(3, false);
                g_sndSystem->playSE(static_cast<SndID>(0x13), -1, 0, 0, -1);
                storeSettings(this);
                m_unk664 = Step_Exit;
            }
        } else if (pad & Pad_Cancel) {
            // Back to the main menu
            m_objs[Obj_Panel]->setAnimIdx(&sPanelAnims[1], false);
            for (u32 i = 0; i < SettingCount; i++) {
                m_objs[Obj_Setting0 + i]->setAnimIdx(&sSettingAnims[1], false);
            }
            m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[Obj_Mario0]->m_sceneModel));
            m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[Obj_Mario0 + 1]->m_sceneModel));
            m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[Obj_Mario0 + 2]->m_sceneModel));
            m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[Obj_Mario0 + 3]->m_sceneModel));
            m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[Obj_Mario0 + 4]->m_sceneModel));
            m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[Mdl_Mario]));
            m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[Mdl_Mario2]));
            m_result = 1;
            m_unk664 = Step_Leave;
            return 1;
        }
        break;
    }
    case Step_Start:
        exitScene(3, false);
        m_unk664 = Step_Exit;
        break;
    case Step_Leave:
        if (m_objs[Obj_Panel]->m_modelAnim->getUpdateRate() == 0.0f) {
            return 3;
        }
        break;
    }
    return 0;
}

void muProcSpecial::exit() {
    m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[Mdl_Panel]));
    for (u32 i = 0; i < SettingCount; i++) {
        m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[Obj_Setting0 + i]->m_sceneModel));
    }
    m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[Mdl_Mario]));
    m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[Obj_Mario0 + 2]->m_sceneModel));
}

bool muProcSpecial::selectProc(int dir) {
    u32 change = 0;     // 1: the cursor moved, 2: a value changed
    if (dir & Pad_Up) {
        if (m_cursor == 0) {
            m_cursor = Cursor_Start;
        } else {
            m_cursor--;
            if (m_cursor == 0) {
                padRepeatClear(GF_PAD_BUTTON_BIT_DPAD_UP);
            }
        }
        change = 1;
    } else if (dir & Pad_Down) {
        if (m_cursor == Cursor_Start) {
            m_cursor = 0;
        } else {
            m_cursor++;
            if (m_cursor == Cursor_Start) {
                padRepeatClear(GF_PAD_BUTTON_BIT_DPAD_DOWN);
            }
        }
        change = 1;
    } else if (dir & Pad_Left) {
        if (m_cursor < Cursor_Start && m_settings[m_cursor] != 0) {
            m_settings[m_cursor]--;
            change = 2;
        }
    } else if (dir & Pad_Right) {
        if (m_cursor < Cursor_Start && m_settings[m_cursor] < ValueCount - 1) {
            m_settings[m_cursor]++;
            change = 2;
        }
    } else if (dir & Pad_A) {
        if (m_cursor < Cursor_Start) {
            if (m_settings[m_cursor] == ValueCount - 1) {
                m_settings[m_cursor] = 0;
            } else {
                m_settings[m_cursor]++;
            }
            change = 2;
        }
    }

    if (change == 1) {
        g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
        printHelp(this);
        if (m_cursor == Cursor_Start) {
            setAnimFrame(Obj_Panel, 1.0f, 0x10);
            setAnimFrame(Obj_Cursor, 3.0f, 2);
        } else {
            setAnimFrame(Obj_Panel, 0.0f, 0x10);
            setAnimFrame(Obj_Cursor, m_settings[m_cursor], 2);
        }
        setAnimFrame(Obj_Cursor, m_cursor, 1);
        return true;
    }
    if (change == 2) {
        g_sndSystem->playSE(static_cast<SndID>(0x23), -1, 0, 0, -1);
        printRuleMsg();
        printHelp(this);
        setAnimFrame(Obj_Cursor, m_settings[m_cursor], 2);
        m_objs[Obj_Setting0 + m_cursor]->setAnimIdx(&sSettingAnims[m_settings[m_cursor] + 2], false);
        setMarioModel();
        return true;
    }
    return false;
}

// The model of Mario wears the settings.
void muProcSpecial::setMarioModel() {
    switch (m_settings[6]) {
    case 0:
        setAnimUpdateRate(Obj_Mario0, 1.0f, 1);
        break;
    case 1:
        setAnimUpdateRate(Obj_Mario0, 0.5f, 1);
        break;
    case 2:
        setAnimUpdateRate(Obj_Mario0, 2.0f, 1);
        break;
    }
    setAnimFrame(Obj_Mario0 + 1, m_settings[3], 2);
    setAnimFrame(Obj_Mario0 + 1, m_settings[1], 1);
    switch (m_settings[0]) {
    case 0:
        setAnimFrame(Obj_Mario0 + 2, 0.0f, 2);
        break;
    case 1:
        setAnimFrame(Obj_Mario0 + 2, 2.0f, 2);
        break;
    case 2:
        setAnimFrame(Obj_Mario0 + 2, 1.0f, 2);
        break;
    }
    setAnimFrame(Obj_Mario0 + 3, m_settings[2], 2);
    setAnimFrame(Obj_Mario0 + 4, m_settings[4], 2);
}

// The text of the panel: the names of the values that are on, separated by commas, or "none".
void muProcSpecial::printRuleMsg() {
    char text[0x100];
    int length = 0;
    int size;
    for (int i = 0; i < 0x100; i++) {
        text[i] = 0;
    }
    for (int i = 0; i < SettingCount; i++) {
        if (m_settings[i] != 0) {
            const char* name = m_msg->getMsgData(Msg_RuleBase + i * 2 + m_settings[i] - 1, &size);
            for (int j = i + 1; j < SettingCount; j++) {
                if (m_settings[j] != 0) {
                    name = m_msg->getMsgData(Msg_RuleComma + i * 2 + m_settings[i] - 1, &size);
                    break;
                }
            }
            memcpy(text + length, name, size);
            length += size;
        }
    }
    if (length == 0) {
        const char* none = m_msg->getMsgData(Msg_RuleNone, &size);
        memcpy(text + length, none, size);
        length += size;
    }
    const char* end = m_msg->getMsgData(Msg_RuleEnd, &size);
    memcpy(text + length, end, size);
    m_msg->printData(Msg_Rules, text, length + size);
}

void muProcSpecial::playSE_CursorMove() {
    g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
}
