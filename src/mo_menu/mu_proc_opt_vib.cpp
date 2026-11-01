#include <types.h>

#include <gf/gf_model.h>
#include <gf/gf_pad_system.h>
#include <gm/gm_common.h>
#include <gm/gm_global.h>
#include <nw4r/g3d/g3d_scnmdlexpand.h>
#include <nw4r/g3d/g3d_scnmdlsmpl.h>
#include <nw4r/g3d/g3d_scnobj.h>
#include <snd/snd_system.h>
#include <stdio.h>

#include <mu/mu_proc_opt_vib.h>

// The animations of the two parts.
static MuAnimIdxData sOptVibAnims[6] = {
    { 0.0f, 9.0f, 0, 2, 0x11 },     // comes in under the cursor
    { 0.0f, 9.0f, 0, 0, 0x11 },     // waits
    { 0.0f, 59.0f, 1, 5, 0x11 },    // blinks under the cursor
    { 0.0f, 29.0f, 0, 4, 0x11 },    // the cursor arrives
    { 0.0f, 9.0f, 0, 3, 0x11 },     // chosen
    { 0.0f, 9.0f, 0, 1, 0x11 },     // goes out
};

// A pad model shakes when its rumble is turned on.
static MuAnimIdxData sPadRumbleAnim = { 0.0f, 10.0f, 0, 0, 1 };

muProcOptVib::muProcOptVib(MuObject** objs, nw4r::g3d::G3dObj** mdls) : muProcMenu(objs, mdls) {
    m_msg = MuMsg::create(2, Heaps::MenuInstance, 0x2B);
    m_msg->allocMsgBuf(0x100, RowCount);
    for (int i = 0; i < RowCount; i++) {
        m_msg->attachScnMdlSimple(i, reinterpret_cast<nw4r::g3d::ScnMdlSimple*>(m_objs[Obj_NameRow0 + i]->m_sceneModel), 0,
                                  1.0f / 12.0f);
        m_msg->setAlignMode(i, MuMsg::Align_Center);
        m_msg->setFontColor(i, 0x14, 0x14, 0x50, 0xFF);
        m_msg->setFontType(i, 1);
    }
}

void muProcOptVib::init(nw4r::g3d::ScnGroup* group, int from) {
    m_group = group;
    m_unk664 = Step_Come;
    m_row = 0;
    m_scroll = 0;
    m_nameNo = 0;
    m_code = 5;
    group->Insert(group->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[Mdl_Pads]));
    group->Insert(group->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[Mdl_Names]));
    for (int i = 0; i < 4; i++) {
        nw4r::g3d::ScnGroup* pads = reinterpret_cast<nw4r::g3d::ScnGroup*>(m_mdls[Mdl_Pads]);
        pads->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[Obj_PadModel0 + i]->m_sceneModel));
        pads = reinterpret_cast<nw4r::g3d::ScnGroup*>(m_mdls[Mdl_Pads]);
        pads->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[Obj_PadModel0 + 4 + i]->m_sceneModel));
        pads = reinterpret_cast<nw4r::g3d::ScnGroup*>(m_mdls[Mdl_Pads]);
        pads->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[Obj_PadModel0 + 8 + i]->m_sceneModel));
        pads = reinterpret_cast<nw4r::g3d::ScnGroup*>(m_mdls[Mdl_Pads]);
        pads->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[Obj_PadModel0 + 12 + i]->m_sceneModel));
    }
    for (int i = 0; i < PadCount; i++) {
        m_padType[i] = -1;
        setPadModel(i);
    }
    m_panel->changeProcess1(0x16, 0x40);
    m_panel->setMsg(0x40, 0, 0);
    m_cursor = Entry_Pads;
    m_objs[Entry_ObjBase + Entry_Pads]->setAnimIdx(&sOptVibAnims[0], true);
    m_objs[Entry_ObjBase + Entry_Names]->setAnimIdx(&sOptVibAnims[1], true);
    char name[0x20];
    for (int i = 0; i < RowCount; i++) {
        if (g_GameGlobal->getName(i + m_scroll, name)) {
            if (g_GameGlobal->getGlobalRecordNameDatap(i + m_scroll)->m_rumble == 1) {
                setAnimFrame(Obj_NameRow0 + i, 2.0f, 4);
            } else {
                setAnimFrame(Obj_NameRow0 + i, 1.0f, 4);
            }
            m_msg->setFontColor(i, 0x80, 0x80, 0x80, 0xFF);
            m_msg->printf(i, name);
        } else {
            setAnimFrame(Obj_NameRow0 + i, 0.0f, 4);
        }
    }
    // The arrow below the list when there are more names
    if (g_GameGlobal->getNumName() > RowCount) {
        setAnimFrame(Entry_ObjBase + Entry_Names, 1.0f, 2);
    } else {
        setAnimFrame(Entry_ObjBase + Entry_Names, 0.0f, 2);
    }
}

int muProcOptVib::proc() {
    // BYTES: the state is compared unsigned here (cmplwi), as in muProcItemSwitch::proc
    if ((u32)m_unk664 == Step_Leave) {
        if (m_objs[Entry_ObjBase + m_cursor]->m_modelAnim->getUpdateRate() == 0.0f) {
            return 3;
        }
        return 0;
    }
    MuObject* object = m_objs[Entry_ObjBase + m_cursor];
    int anim = muGetAnimIdx(object);
    switch (anim) {
    case 4:
        if (object->m_modelAnim->getFrame() == 29.0f) {
            m_objs[Entry_ObjBase + m_cursor]->setAnimIdx(&sOptVibAnims[2], true);
        }
        break;
    case 2:
        if (object->m_modelAnim->getFrame() == 9.0f) {
            m_objs[Entry_ObjBase + m_cursor]->setAnimIdx(&sOptVibAnims[3], true);
        }
        break;
    case 3:
        break;
    }
    for (int i = 0; i < PadCount; i++) {
        setPadModel(i);
    }
    u32 pad = padUpdate();
    bool moved = selectProc(pad);
    if (pad & Pad_Cancel) {
        m_objs[Entry_ObjBase + Entry_Pads]->setAnimIdx(&sOptVibAnims[5], true);
        m_objs[Entry_ObjBase + Entry_Names]->setAnimIdx(&sOptVibAnims[5], true);
        setAnimFrame(Obj_NameRow0 + m_row, 0.0f, 0x10);
        m_result = 5;
        m_unk664 = Step_Leave;
        return 1;
    }
    if (moved && m_unk664 == Step_Come) {
        m_unk664 = Step_Select;
        return 4;
    }
    return 0;
}

void muProcOptVib::exit() {
    m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[Mdl_Pads]));
    m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[Mdl_Names]));
    for (int i = 0; i < RowCount; i++) {
        m_msg->printf(i, "");
    }
}

bool muProcOptVib::selectProc(int dir) {
    bool moved = false;
    bool switchPart = false;
    GameGlobal* global = g_GameGlobal;
    char name[0x18];
    if (dir & Pad_Left) {
        if (m_cursor == Entry_Names) {
            switchPart = true;
        }
    } else if (dir & Pad_Right) {
        if (m_cursor == Entry_Pads) {
            switchPart = true;
        }
    } else if (m_cursor == Entry_Pads) {
        // A on a pad switches the rumble of that pad
        for (u16 i = 0; i < PadCount; i++) {
            gfPadStatus status;
            g_gfPadSystem->getSysPadStatus(i, &status);
            if (status.m_buttonsPressedThisFrame.bits & GF_PAD_BUTTON_BIT_A && m_padType[i] != 1) {
                g_sndSystem->playSE(static_cast<SndID>(0x24), -1, 0, 0, -1);
                gmSetAutosaveDirtyFlag();
                if (global->getRumbleSetting(i)) {
                    global->setRumbleSetting(i, 0);
                } else {
                    global->setRumbleSetting(i, 1);
                    g_gfPadSystem->startMotor(i, 0x14);
                    switch (m_padType[i]) {
                    case 0:
                        m_objs[Obj_PadModel0 + i % 4]->setAnimIdx(&sPadRumbleAnim, false);
                        break;
                    case 1:
                        m_objs[Obj_PadModel0 + 4 + i % 4]->setAnimIdx(&sPadRumbleAnim, false);
                        break;
                    case 3:
                        m_objs[Obj_PadModel0 + 8 + i % 4]->setAnimIdx(&sPadRumbleAnim, false);
                        break;
                    case 2:
                        m_objs[Obj_PadModel0 + 12 + i % 4]->setAnimIdx(&sPadRumbleAnim, false);
                        break;
                    }
                }
            }
        }
    } else if (dir & Pad_Up) {
        if (m_nameNo != 0) {
            g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
            m_nameNo--;
            if (m_row != 0) {
                setAnimFrame(Obj_NameRow0 + m_row, 0.0f, 0x10);
                m_msg->setFontColor(m_row, 0x80, 0x80, 0x80, 0xFF);
                global->getName(m_row + m_scroll, name);
                m_msg->printf(m_row, name);
                m_row--;
                setAnimFrame(Obj_NameRow0 + m_row, 1.0f, 0x10);
                m_msg->setFontColor(m_row, 0x19, 0x19, 0x19, 0xFF);
                global->getName(m_row + m_scroll, name);
                m_msg->printf(m_row, name);
            } else {
                m_scroll--;
                if (m_scroll == 0) {
                    setAnimFrame(Entry_ObjBase + Entry_Names, 1.0f, 2);
                } else {
                    setAnimFrame(Entry_ObjBase + Entry_Names, 3.0f, 2);
                }
                for (int i = 0; i < RowCount; i++) {
                    if (global->getName(i + m_scroll, name)) {
                        if (global->getGlobalRecordNameDatap(i + m_scroll)->m_rumble == 1) {
                            setAnimFrame(Obj_NameRow0 + i, 2.0f, 4);
                        } else {
                            setAnimFrame(Obj_NameRow0 + i, 1.0f, 4);
                        }
                        m_msg->printf(i, name);
                    } else {
                        setAnimFrame(Obj_NameRow0 + i, 0.0f, 4);
                    }
                }
            }
        }
    } else if (dir & Pad_Down) {
        if (m_nameNo < global->getNumName() - 1) {
            g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
            m_nameNo++;
            if (m_row < RowCount - 1) {
                setAnimFrame(Obj_NameRow0 + m_row, 0.0f, 0x10);
                m_msg->setFontColor(m_row, 0x80, 0x80, 0x80, 0xFF);
                global->getName(m_row + m_scroll, name);
                m_msg->printf(m_row, name);
                m_row++;
                setAnimFrame(Obj_NameRow0 + m_row, 1.0f, 0x10);
                m_msg->setFontColor(m_row, 0x19, 0x19, 0x19, 0xFF);
                global->getName(m_row + m_scroll, name);
                m_msg->printf(m_row, name);
            } else {
                m_scroll++;
                if (m_scroll == global->getNumName() - RowCount) {
                    setAnimFrame(Entry_ObjBase + Entry_Names, 2.0f, 2);
                } else {
                    setAnimFrame(Entry_ObjBase + Entry_Names, 3.0f, 2);
                }
                for (int i = 0; i < RowCount; i++) {
                    if (global->getName(i + m_scroll, name)) {
                        if (global->getGlobalRecordNameDatap(i + m_scroll)->m_rumble == 1) {
                            setAnimFrame(Obj_NameRow0 + i, 2.0f, 4);
                        } else {
                            setAnimFrame(Obj_NameRow0 + i, 1.0f, 4);
                        }
                        m_msg->printf(i, name);
                    } else {
                        setAnimFrame(Obj_NameRow0 + i, 0.0f, 4);
                    }
                }
            }
        }
    } else if (dir & Pad_A) {
        // A on any pad switches the rumble of the name
        g_sndSystem->playSE(static_cast<SndID>(1), -1, 0, 0, -1);
        gmSetAutosaveDirtyFlag();
        for (int i = 0; i < PadCount; i++) {
            gfPadStatus status;
            g_gfPadSystem->getSysPadStatus(i, &status);
            if (status.m_buttonsPressedThisFrame.bits & GF_PAD_BUTTON_BIT_A) {
                if (global->getGlobalRecordNameDatap(m_nameNo)->m_rumble == 1) {
                    global->getGlobalRecordNameDatap(m_nameNo)->m_rumble = 0;
                    setAnimFrame(Obj_NameRow0 + m_row, 1.0f, 4);
                } else {
                    g_gfPadSystem->startMotor(i, 0x14);
                    global->getGlobalRecordNameDatap(m_nameNo)->m_rumble = 1;
                    setAnimFrame(Obj_NameRow0 + m_row, 2.0f, 4);
                }
            }
        }
    }
    if (switchPart && global->getNumName() > 0) {
        m_cursor ^= 1;
        g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
        m_panel->setMsg(m_cursor + 0x40, 1, 0);
        // The part that is left stops blinking
        int prev = m_cursor ^ 1;
        moved = true;
        m_objs[Entry_ObjBase + prev]->setAnimIdx(&sOptVibAnims[1], true);
        m_objs[Entry_ObjBase + prev]->m_animIdxData = NULL;
        setAnimFrame(Entry_ObjBase + prev, 9.0f, 0x11);
        m_objs[Entry_ObjBase + m_cursor]->setAnimIdx(&sOptVibAnims[3], true);
        if (m_cursor == Entry_Names) {
            setAnimFrame(Obj_NameRow0 + m_row, 1.0f, 0x10);
            m_msg->setFontColor(m_row, 0x19, 0x19, 0x19, 0xFF);
            global->getName(m_row + m_scroll, name);
            m_msg->printf(m_row, name);
        } else {
            setAnimFrame(Obj_NameRow0 + m_row, 0.0f, 0x10);
            m_msg->setFontColor(m_row, 0x80, 0x80, 0x80, 0xFF);
            global->getName(m_row + m_scroll, name);
            m_msg->printf(m_row, name);
        }
    }
    return moved;
}

void muProcOptVib::setPadModel(int pad) {
    int* type = &m_padType[pad];
    int old = *type;
    int column = pad % 4;
    char node[0x20];
    gfPadStatus status;
    g_gfPadSystem->getSysPadStatus(pad, &status);
    if (status.m_error != 0) {
        *type = -1;
        setAnimFrame(Obj_PadSlot0 + pad, 0.0f, 0x16);
    } else {
        *type = status.m_controllerType;
        if (g_GameGlobal->getRumbleSetting(pad) == true && *type != 1) {
            setAnimFrame(Obj_PadSlot0 + pad, 2.0f * (1.0f + column), 0x16);
        } else {
            setAnimFrame(Obj_PadSlot0 + pad, 2.0f * (1.0f + column) - 1.0f, 0x16);
        }
    }
    if (old == *type) {
        return;
    }
    if (old != -1) {
        switch (old) {
        case 0:
            reinterpret_cast<nw4r::g3d::ScnMdlExpand*>(m_mdls[Mdl_Pads])->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[Obj_PadModel0 + column]->m_sceneModel));
            break;
        case 1:
            reinterpret_cast<nw4r::g3d::ScnMdlExpand*>(m_mdls[Mdl_Pads])->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[Obj_PadModel0 + 4 + column]->m_sceneModel));
            break;
        case 3:
            reinterpret_cast<nw4r::g3d::ScnMdlExpand*>(m_mdls[Mdl_Pads])->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[Obj_PadModel0 + 8 + column]->m_sceneModel));
            break;
        case 2:
            reinterpret_cast<nw4r::g3d::ScnMdlExpand*>(m_mdls[Mdl_Pads])->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[Obj_PadModel0 + 12 + column]->m_sceneModel));
            break;
        }
    }
    if (*type == -1) {
        return;
    }
    sprintf(node, "pos%02d", pad);
    switch (*type) {
    case 0:
        reinterpret_cast<nw4r::g3d::ScnMdlExpand*>(m_mdls[Mdl_Pads])->PushBack(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[Obj_PadModel0 + column]->m_sceneModel), node);
        break;
    case 1:
        reinterpret_cast<nw4r::g3d::ScnMdlExpand*>(m_mdls[Mdl_Pads])->PushBack(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[Obj_PadModel0 + 4 + column]->m_sceneModel), node);
        break;
    case 3:
        reinterpret_cast<nw4r::g3d::ScnMdlExpand*>(m_mdls[Mdl_Pads])->PushBack(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[Obj_PadModel0 + 8 + column]->m_sceneModel), node);
        break;
    case 2:
        reinterpret_cast<nw4r::g3d::ScnMdlExpand*>(m_mdls[Mdl_Pads])->PushBack(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[Obj_PadModel0 + 12 + column]->m_sceneModel), node);
        break;
    }
}

void muProcOptVib::playSE_CursorMove() {
    g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
}
