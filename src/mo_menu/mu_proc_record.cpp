#include <types.h>

#include <nw4r/g3d/g3d_scnobj.h>
#include <snd/snd_system.h>

#include <mu/mu_proc_record.h>

// Where the cursor goes from each icon (up, down, left, right).
static muProcMenuNode sRecordNodes[muProcRecord::Entry_Count] = {
    { { 2, 1 }, { 2, 0 }, { 1, 1 }, { 1, 0 } },
    { { 2, 1 }, { 2, 0 }, { 0, 0 }, { 0, 1 } },
    { { 0, 0 }, { 0, 1 }, { 0, 1 }, { 1, 0 } },
};

// The code of the screen each icon opens, and the message of the help balloon of each icon.
static const u8 sRecordCodes[muProcRecord::Entry_Count] = { 0x15, 0x12, 0x11 };
static const u16 sRecordMsgs[muProcRecord::Entry_Count] = { 0x4F, 0x4E, 0x50 };

void muProcRecord::init(nw4r::g3d::ScnGroup* group, int from) {
    m_group = group;
    m_nodes = sRecordNodes;
    m_objBase = Entry_ObjBase;
    for (u32 i = 0; i < Entry_Count; i++) {
        group->Insert(group->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[Entry_ObjBase + i]->m_sceneModel));
    }

    // From the Data screen the icons come in; back from one of the screens they wait
    if ((u32)from == Entry_BackCode) {
        m_unk664 = 0;
    } else {
        m_unk664 = 1;
    }
    u16 cursor;
    if ((u32)from == Entry_BackCode) {
        cursor = 0;
    } else {
        // The cursor is on the icon of the screen that was shown (or past the last one)
        for (cursor = 0; cursor < Entry_Count; cursor++) {
            if ((u32)from == sRecordCodes[cursor]) {
                break;
            }
        }
    }
    m_cursor = cursor;
    m_panel->changeProcess1(0x1C, sRecordMsgs[cursor]);
    setComeAnim(true);
}

int muProcRecord::proc() {
    switch (m_unk664) {
    case 0: {
        // The icons come in; the one of the cursor starts to blink when it has arrived
        MuObject* object = m_objs[Entry_ObjBase + m_cursor];
        if (muGetAnimIdx(object) == 2 && object->m_modelAnim->getFrame() == 9.0f) {
            m_objs[Entry_ObjBase + m_cursor]->setAnimIdx(&m_anims[3], true);
            m_unk664 = 2;
        }
        u16 oldCursor;
        u32 pad = padUpdate();
        oldCursor = m_cursor;
        if (selectProc(pad)) {
            setSelectAnim(oldCursor, true);
            m_panel->setMsg(sRecordMsgs[m_cursor], 1, 0);
            m_unk664 = 2;
        }
        if (pad & Pad_Ok) {
            m_result = sRecordCodes[m_cursor];
            setOutAnim(true);
            m_unk664 = 3;
            return 2;
        } else if (pad & Pad_Cancel) {
            setOutAnim(false);
            m_result = Entry_BackCode;
            m_unk664 = 3;
            return 1;
        }
        break;
    }
    case 1: {
        // Back from a screen: the icons wait, the one of the cursor starts to blink
        if (m_objs[Entry_ObjBase + m_cursor]->m_modelAnim->getFrame() == 9.0f) {
            m_objs[Entry_ObjBase + m_cursor]->setAnimIdx(&m_anims[2], true);
            m_unk664 = 2;
        }
        u16 oldCursor;
        u32 pad = padUpdate();
        oldCursor = m_cursor;
        if (selectProc(pad)) {
            setSelectAnim(oldCursor, true);
            m_panel->setMsg(sRecordMsgs[m_cursor], 1, 0);
            m_unk664 = 2;
            return 4;
        }
        if (pad & Pad_Ok) {
            m_result = sRecordCodes[m_cursor];
            setOutAnim(true);
            m_unk664 = 3;
            return 2;
        } else if (pad & Pad_Cancel) {
            setOutAnim(false);
            m_result = Entry_BackCode;
            m_unk664 = 3;
            return 1;
        }
        break;
    }
    case 2: {
        // The cursor is on an icon
        MuObject* object = m_objs[Entry_ObjBase + m_cursor];
        if (muGetAnimIdx(object) == 4 && object->m_modelAnim->getFrame() >= 179.0f) {
            m_objs[Entry_ObjBase + m_cursor]->setAnimIdx(&m_anims[2], true);
        }
        u16 oldCursor;
        u32 pad = padUpdate();
        oldCursor = m_cursor;
        if (selectProc(pad)) {
            setSelectAnim(oldCursor, false);
            m_panel->setMsg(sRecordMsgs[m_cursor], 1, 0);
        }
        if (pad & Pad_Ok) {
            m_result = sRecordCodes[m_cursor];
            setOutAnim(true);
            m_unk664 = 3;
            return 2;
        } else if (pad & Pad_Cancel) {
            setOutAnim(false);
            m_result = Entry_BackCode;
            m_unk664 = 3;
            return 1;
        }
        break;
    }
    case 3:
        // The icons go out
        if (m_objs[Entry_ObjBase + m_cursor]->m_modelAnim->getUpdateRate() == 0.0f) {
            return 3;
        }
        break;
    }
    return 0;
}

void muProcRecord::exit() {
    for (u32 i = 0; i < Entry_Count; i++) {
        m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[Entry_ObjBase + i]->m_sceneModel));
    }
}

void muProcRecord::playSE_CursorMove() {
    g_sndSystem->playSE(static_cast<SndID>(0x14), -1, 0, 0, -1);
}
