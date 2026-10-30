#include <types.h>

#include <nw4r/g3d/g3d_scnobj.h>
#include <snd/snd_system.h>

#include <mu/mu_proc_data.h>

// Where the cursor goes from each icon (up, down, left, right): the three icons are in a row.
static muProcMenuNode sDataNodes[muProcData::Entry_Count] = {
    { { 1, 0 }, { 1, 0 }, { 2, 1 }, { 2, 0 } },
    { { 0, 0 }, { 0, 1 }, { 1, 0 }, { 1, 0 } },
    { { 1, 1 }, { 1, 0 }, { 0, 0 }, { 0, 1 } },
};

void muProcData::init(nw4r::g3d::ScnGroup* group, int from) {
    m_group = group;
    m_nodes = sDataNodes;
    m_objBase = Entry_ObjBase;
    for (u32 i = 0; i < Entry_Count; i++) {
        group->Insert(group->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[Entry_ObjBase + i]->m_sceneModel));
    }

    switch (from) {
    case 0:
        // From the top screen
        m_unk664 = 0;
        m_cursor = 0;
        m_code = 0;
        break;
    case 0xE:
        // Back from one of the three screens: the cursor is on its icon
        m_unk664 = 1;
        m_cursor = 0;
        m_code = 0xE;
        break;
    case 0xF:
        m_unk664 = 1;
        m_cursor = 1;
        m_code = 0xF;
        break;
    case 0x10:
        m_unk664 = 1;
        m_cursor = 2;
        m_code = 0x10;
        break;
    }
    setComeAnim(true);
    m_panel->changeProcess1(0xC, m_cursor + Entry_ObjBase + 1);
}

int muProcData::proc() {
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
            m_panel->setMsg(m_cursor + Entry_ObjBase + 1, 1, 0);
            m_unk664 = 2;
            return 4;
        }
        if (pad & Pad_Ok) {
            m_result = m_cursor + Entry_FirstCode;
            setOutAnim(true);
            m_unk664 = 3;
            return 2;
        } else if (pad & Pad_Cancel) {
            setOutAnim(false);
            m_result = 0;
            m_unk664 = 3;
            return 1;
        }
        break;
    }
    case 1:
        // Back from a screen: the icons wait, the one of the cursor starts to blink
        if (m_objs[Entry_ObjBase + m_cursor]->m_modelAnim->getFrame() == 9.0f) {
            m_objs[Entry_ObjBase + m_cursor]->setAnimIdx(&m_anims[2], true);
            m_unk664 = 2;
        }
        break;
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
            m_panel->setMsg(m_cursor + Entry_ObjBase + 1, 1, 0);
            setSelectAnim(oldCursor, false);
            return 0;
        }
        if (pad & Pad_Ok) {
            m_result = m_cursor + Entry_FirstCode;
            setOutAnim(true);
            m_unk664 = 3;
            return 2;
        } else if (pad & Pad_Cancel) {
            setOutAnim(false);
            m_result = 0;
            m_unk664 = 3;
            return 1;
        }
        break;
    }
    case 3:
        // The icons go out. The object looked at is one past the icon of the cursor (as the game does it).
        if (m_objs[Entry_ObjBase + 1 + m_cursor]->m_modelAnim->getUpdateRate() == 0.0f) {
            return 3;
        }
        break;
    }
    return 0;
}

void muProcData::exit() {
    for (u32 i = 0; i < Entry_Count; i++) {
        m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[Entry_ObjBase + i]->m_sceneModel));
    }
}

void muProcData::playSE_CursorMove() {
    g_sndSystem->playSE(static_cast<SndID>(0x14), -1, 0, 0, -1);
}
