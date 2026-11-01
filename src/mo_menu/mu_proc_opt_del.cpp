#include <types.h>

#include <gf/gf_model.h>
#include <mu/mu_delete_nand_task.h>
#include <nw4r/g3d/g3d_anmchr.h>
#include <nw4r/g3d/g3d_scnmdlsmpl.h>
#include <nw4r/g3d/g3d_scnobj.h>
#include <snd/snd_system.h>

#include <mu/mu_proc_opt_del.h>

// Where the cursor goes from each entry: up and down only, wrapping around (holding repeats from the first and the last).
static muProcMenuNode sOptDelNodes[muProcOptDel::Entry_Count] = {
    { { 4, 1, 0 }, { 1, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } },
    { { 0, 0, 0 }, { 2, 0, 0 }, { 1, 0, 0 }, { 1, 0, 0 } },
    { { 1, 0, 0 }, { 3, 0, 0 }, { 2, 0, 0 }, { 2, 0, 0 } },
    { { 2, 0, 0 }, { 4, 0, 0 }, { 3, 0, 0 }, { 3, 0, 0 } },
    { { 3, 0, 0 }, { 0, 1, 0 }, { 4, 0, 0 }, { 4, 0, 0 } },
};

// The animations of the entries.
static MuAnimIdxData sOptDelAnims[6] = {
    { 0.0f, 9.0f, 0, 2, 0x13 },     // comes in under the cursor
    { 0.0f, 9.0f, 0, 0, 0x13 },     // waits
    { 0.0f, 59.0f, 1, 5, 0x13 },    // blinks under the cursor
    { 0.0f, 29.0f, 0, 4, 0x13 },    // the cursor arrives
    { 0.0f, 9.0f, 0, 3, 0x13 },     // chosen
    { 0.0f, 9.0f, 0, 1, 0x13 },     // goes out
};

// The animations of the window.
static MuAnimIdxData sOptDelWindowAnims[4] = {
    { 0.0f, 9.0f, 0, 0, 1 },        // comes in
    { 10.0f, 39.0f, 1, 0, 1 },      // waits
    { 40.0f, 49.0f, 0, 0, 1 },      // goes out
    { 0.0f, 120.0f, 1, 3, 1 },      // deleting
};

// The task that deletes the data while the window says so.
static muDeleteNandTask* s_deleteTask;

muProcOptDel::muProcOptDel(MuObject** objs, nw4r::g3d::G3dObj** mdls, void* msgData) : muProcMenu(objs, mdls) {
    m_no = false;
    m_itemCount = Entry_Count;
    m_msg = MuMsg::create(1, Heaps::MenuInstance, 0x2B);
    m_msg->allocMsgBuf(0x100, 3);
    m_msg->setMsgData(msgData);
    for (int i = 0; i < 3; i++) {
        m_msg->attachScnMdlSimple(i, reinterpret_cast<nw4r::g3d::ScnMdlSimple*>(m_objs[Entry_ObjWindow]->m_sceneModel), i,
                                  0.0625f);
        m_msg->setFontWidthModeAuto(i);
        m_msg->setAlignMode(i, MuMsg::Align_Center);
        m_msg->getWScale(i, 50.0f);
    }
}

muProcOptDel::~muProcOptDel() {
    if (m_msg != NULL) {
        delete m_msg;
    }
}

void muProcOptDel::init(nw4r::g3d::ScnGroup* group, int from) {
    m_nodes = sOptDelNodes;
    m_group = group;
    m_unk664 = Step_Come;
    m_cursor = 0;
    m_objBase = Entry_ObjBase;
    for (u32 i = 0; i < Entry_Count; i++) {
        group->Insert(group->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[Entry_ObjBase + i]->m_sceneModel));
    }
    setComeAnim(true);
    m_panel->changeProcess1(0x1B, 0x49);
}

int muProcOptDel::proc() {
    switch (m_unk664) {
    case Step_Come: {
        MuObject* object = m_objs[Entry_ObjBase + m_cursor];
        if (muGetAnimIdx(object) == 2 && object->m_modelAnim->getFrame() == 9.0f) {
            m_objs[Entry_ObjBase + m_cursor]->setAnimIdx(&sOptDelAnims[3], true);
            m_unk664 = Step_Select;
        }
        u16 cursor; // BYTES: declared before pad, which puts the two in the registers of the original
        u32 pad = padUpdate();
        cursor = m_cursor;
        if (selectProc(pad)) {
            setSelectAnim(cursor, true);
            m_panel->setMsg(m_cursor + 0x49, 1, 0);
            m_unk664 = Step_Select;
            return 4;
        }
        if (pad & Pad_Ok) {
            m_group->Insert(m_group->sceneItemsCount,
                            reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[Entry_ObjWindow]->m_sceneModel));
            m_objs[Entry_ObjWindow]->setAnimIdx(&sOptDelWindowAnims[0], true);
            setAnimFrame(Entry_ObjWindow, 1.0f, 1);
            m_msg->printIndex(0, 0, NULL);
            m_msg->printIndex(1, 4, NULL);
            m_msg->printIndex(2, 5, NULL);
            m_no = true;
            m_unk664 = Step_Confirm1;
            m_seHandle = g_sndSystem->playSE(static_cast<SndID>(0x2048), -1, 0, 0, -1);
        } else if (pad & Pad_Cancel) {
            setOutAnim(false);
            m_result = 5;
            m_unk664 = Step_Leave;
            return 1;
        }
        break;
    }
    case Step_Select: {
        MuObject* object = m_objs[Entry_ObjBase + m_cursor];
        if (muGetAnimIdx(object) == 4 && object->m_modelAnim->getFrame() >= 29.0f) {
            m_objs[Entry_ObjBase + m_cursor]->setAnimIdx(&sOptDelAnims[2], true);
        }
        u16 cursor; // BYTES: declared before pad, which puts the two in the registers of the original
        u32 pad = padUpdate();
        cursor = m_cursor;
        if (selectProc(pad)) {
            m_panel->setMsg(m_cursor + 0x49, 1, 0);
            setSelectAnim(cursor, false);
        }
        if (pad & Pad_Ok) {
            m_group->Insert(m_group->sceneItemsCount,
                            reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[Entry_ObjWindow]->m_sceneModel));
            m_objs[Entry_ObjWindow]->changeAnimN(0, 1, false, false);
            m_objs[Entry_ObjWindow]->setAnimIdx(&sOptDelWindowAnims[0], true);
            m_objs[Entry_ObjWindow]->changeAnimN(0, 0x10, false, false);
            m_objs[Entry_ObjWindow]->setFrameMatCol(1.0f);
            m_msg->printIndex(0, 0, NULL);
            m_msg->printIndex(1, 4, NULL);
            m_msg->printIndex(2, 5, NULL);
            m_no = true;
            m_unk664 = Step_Confirm1;
            m_seHandle = g_sndSystem->playSE(static_cast<SndID>(0x2048), -1, 0, 0, -1);
        } else if (pad & Pad_Cancel) {
            setOutAnim(false);
            m_result = 5;
            m_unk664 = Step_Leave;
            return 1;
        }
        break;
    }
    case Step_Confirm1: {
        if (m_objs[Entry_ObjWindow]->m_modelAnim->getFrame() == 9.0f) {
            m_objs[Entry_ObjWindow]->setAnimIdx(&sOptDelWindowAnims[1], true);
        }
        u32 pad = padUpdate();
        if (pad & (Pad_Left | Pad_Right)) {
            g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
            m_no ^= 1;
            m_objs[Entry_ObjWindow]->setFrameMatCol(m_no);
        } else if (pad & Pad_Ok) {
            g_sndSystem->stopSE(m_seHandle, 0);
            if (!m_no) {
                m_msg->printIndex(0, 1, NULL);
                m_no = true;
                m_objs[Entry_ObjWindow]->setFrameMatCol(m_no);
                m_unk664 = Step_Confirm2;
                m_seHandle = g_sndSystem->playSE(static_cast<SndID>(0x2049), -1, 0, 0, -1);
            } else {
                m_objs[Entry_ObjWindow]->setAnimIdx(&sOptDelWindowAnims[2], true);
                m_unk664 = Step_Close;
                g_sndSystem->playSE(static_cast<SndID>(2), -1, 0, 0, -1);
            }
        } else if (pad & Pad_Cancel) {
            m_objs[Entry_ObjWindow]->setAnimIdx(&sOptDelWindowAnims[2], true);
            m_unk664 = Step_Close;
            g_sndSystem->playSE(static_cast<SndID>(2), -1, 0, 0, -1);
            g_sndSystem->stopSE(m_seHandle, 0);
        }
        break;
    }
    case Step_Confirm2: {
        u32 pad = padUpdate();
        if (pad & (Pad_Left | Pad_Right)) {
            g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
            m_no ^= 1;
            m_objs[Entry_ObjWindow]->setFrameMatCol(m_no);
        } else if (pad & Pad_Ok) {
            g_sndSystem->stopSE(m_seHandle, 0);
            if (!m_no) {
                if (m_cursor == Entry_All) {
                    // All the data: one more question
                    m_msg->printIndex(0, 2, NULL);
                    m_no = true;
                    m_objs[Entry_ObjWindow]->setFrameMatCol(m_no);
                    m_unk664 = Step_Confirm3;
                    m_seHandle = g_sndSystem->playSE(static_cast<SndID>(0x2053), -1, 0, 0, -1);
                } else {
                    m_unk664 = Step_Delete;
                }
            } else {
                m_objs[Entry_ObjWindow]->setAnimIdx(&sOptDelWindowAnims[2], true);
                m_unk664 = Step_Close;
                g_sndSystem->playSE(static_cast<SndID>(2), -1, 0, 0, -1);
            }
        } else if (pad & Pad_Cancel) {
            m_objs[Entry_ObjWindow]->setAnimIdx(&sOptDelWindowAnims[2], true);
            m_unk664 = Step_Close;
            g_sndSystem->playSE(static_cast<SndID>(2), -1, 0, 0, -1);
            g_sndSystem->stopSE(m_seHandle, 0);
        }
        break;
    }
    case Step_Confirm3: {
        u32 pad = padUpdate();
        if (pad & (Pad_Left | Pad_Right)) {
            g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
            m_no ^= 1;
            m_objs[Entry_ObjWindow]->setFrameMatCol(m_no);
        } else if (pad & Pad_Ok) {
            g_sndSystem->stopSE(m_seHandle, 0);
            if (!m_no) {
                m_unk664 = Step_Delete;
            } else {
                m_objs[Entry_ObjWindow]->setAnimIdx(&sOptDelWindowAnims[2], true);
                m_unk664 = Step_Close;
                g_sndSystem->playSE(static_cast<SndID>(2), -1, 0, 0, -1);
            }
        } else if (pad & Pad_Cancel) {
            g_sndSystem->stopSE(m_seHandle, 0);
            m_objs[Entry_ObjWindow]->setAnimIdx(&sOptDelWindowAnims[2], true);
            m_unk664 = Step_Close;
            g_sndSystem->playSE(static_cast<SndID>(2), -1, 0, 0, -1);
        }
        break;
    }
    case Step_Close:
        if (m_objs[Entry_ObjWindow]->m_modelAnim->m_anmObjChrRes->GetFrame() == 49.0f) {
            m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[Entry_ObjWindow]->m_sceneModel));
            m_unk664 = Step_Select;
        }
        break;
    case Step_Delete:
        m_objs[Entry_ObjWindow]->setAnimIdx(&sOptDelWindowAnims[3], true);
        m_objs[Entry_ObjWindow]->changeAnimN(3, 1, true, false);
        m_msg->printIndex(0, 3, NULL);
        m_unk664 = Step_Deleting;
        if (s_deleteTask == NULL) {
            s_deleteTask = muDeleteNandTask::create(Heaps::MenuInstance, m_cursor);
        }
        // fall through
    case Step_Deleting:
        if (s_deleteTask->getStep() == muDeleteNandTask::Step_Done) {
            g_sndSystem->playSE(static_cast<SndID>(0x204B), -1, 0, 0, -1);
            m_objs[Entry_ObjWindow]->changeAnimN(1, 1, true, false);
            m_unk664 = Step_Deleted;
            s_deleteTask->exit();
            s_deleteTask = NULL;
        }
        break;
    case Step_Deleted:
        if (padUpdate() & (Pad_Up | Pad_Down | Pad_Left | Pad_Right | Pad_Ok | Pad_Cancel)) {
            m_objs[Entry_ObjWindow]->changeAnimN(2, 1, true, false);
            m_unk664 = Step_Select;
            g_sndSystem->playSE(static_cast<SndID>(2), -1, 0, 0, -1);
        }
        break;
    case Step_Leave:
        if (m_objs[Entry_ObjBase + m_cursor]->m_modelAnim->getUpdateRate() == 0.0f) {
            return 3;
        }
        break;
    }
    return 0;
}

void muProcOptDel::exit() {
    if (s_deleteTask != NULL) {
        s_deleteTask->exit();
        s_deleteTask = NULL;
    }
    for (u32 i = 0; i < Entry_Count; i++) {
        m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[Entry_ObjBase + i]->m_sceneModel));
    }
}

void muProcOptDel::playSE_CursorMove() {
    g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
}
