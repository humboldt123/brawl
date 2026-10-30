#include <types.h>

#include <nw4r/g3d/g3d_scnobj.h>
#include <snd/snd_system.h>

#include <mu/mu_proc_coll.h>

// Where the cursor goes from each icon (up, down, left, right).
static muProcMenuNode sCollNodes[muProcColl::Entry_Count] = {
    { { 3, 1 }, { 3, 0 }, { 2, 1 }, { 1, 0 } },
    { { 4, 1 }, { 4, 0 }, { 0, 0 }, { 2, 0 } },
    { { 5, 1 }, { 5, 0 }, { 1, 0 }, { 0, 1 } },
    { { 0, 0 }, { 0, 1 }, { 6, 1 }, { 4, 0 } },
    { { 1, 0 }, { 1, 1 }, { 3, 0 }, { 5, 0 } },
    { { 2, 0 }, { 2, 1 }, { 4, 0 }, { 6, 0 } },
    { { 2, 0 }, { 2, 1 }, { 5, 0 }, { 3, 1 } },
};

// The message of the help balloon for each icon.
static int sCollMsgs[muProcColl::Entry_Count] = { 0x16, 0x1B, 0x19, 0x17, 0x1A, 0x1C, 0x18 };

void muProcColl::init(nw4r::g3d::ScnGroup* group, int from) {
    int item;
    m_group = group;
    m_nodes = sCollNodes;
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
        setComeAnim(true);
        break;
    case 0x17:
        // Back from the trophy screen
        m_unk664 = 1;
        m_cursor = Entry_Figure;
        m_code = 0x17;
        setComeAnim(true);
        break;
    default:
        // Back from a scene: the cursor is on the icon of the scene that was played
        m_unk664 = 2;
        switch (getSceneStartCode()) {
        case 0xD:
            item = 1;
            break;
        case 0xE:
            item = 4;
            break;
        case 0x14:
            item = 3;
            break;
        case 0x15:
            item = 2;
            break;
        case 0x26:
            item = 5;
            break;
        case 0x27:
            item = 6;
            break;
        }
        m_cursor = item;
        setSelectAnim(item, true);
        m_code = 0x22;
        break;
    }
    m_panel->changeProcess1(10, sCollMsgs[m_cursor]);
}

int muProcColl::proc() {
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
            m_panel->setMsg(sCollMsgs[m_cursor], 1, 0);
            m_unk664 = 2;
            return 4;
        }
        if (pad & Pad_Ok) {
            int code;
            switch (m_cursor) {
            case 2:
                code = 0x15;
                break;
            case 3:
                code = 0x14;
                break;
            case 4:
                code = 0xE;
                break;
            case 1:
                code = 0xD;
                break;
            case 5:
                code = 0x23;
                break;
            case 6:
                code = 0x24;
                break;
            default:
                code = 0x2A;
                break;
            }
            if (code != 0x2A) {
                exitScene(code, false);
                g_sndSystem->playSE(static_cast<SndID>(0x13), -1, 0, 0, -1);
            }
            if (m_cursor == Entry_Figure) {
                setOutAnim(true);
                m_result = 0x17;
                m_unk664 = 3;
                return 2;
            }
        } else if (pad & Pad_Cancel) {
            setOutAnim(false);
            m_result = 0;
            m_unk664 = 3;
            return 1;
        }
        break;
    }
    case 1:
        // Back from the trophy screen: the icons wait, the one of the cursor starts to blink
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
            setSelectAnim(oldCursor, false);
            m_panel->setMsg(sCollMsgs[m_cursor], 1, 0);
        }
        if (pad & Pad_Ok) {
            int code;
            switch (m_cursor) {
            case 2:
                code = 0x15;
                break;
            case 3:
                code = 0x14;
                break;
            case 4:
                code = 0xE;
                break;
            case 1:
                code = 0xD;
                break;
            case 5:
                code = 0x23;
                break;
            case 6:
                code = 0x24;
                break;
            default:
                code = 0x2A;
                break;
            }
            if (code != 0x2A) {
                exitScene(code, false);
                g_sndSystem->playSE(static_cast<SndID>(0x13), -1, 0, 0, -1);
            }
            if (m_cursor == Entry_Figure) {
                setOutAnim(true);
                m_result = 0x17;
                m_unk664 = 3;
                return 2;
            }
        } else if (pad & Pad_Cancel) {
            setOutAnim(false);
            m_result = 0;
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

void muProcColl::exit() {
    for (u32 i = 0; i < Entry_Count; i++) {
        m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[Entry_ObjBase + i]->m_sceneModel));
    }
}

void muProcColl::playSE_CursorMove() {
    g_sndSystem->playSE(static_cast<SndID>(0x14), -1, 0, 0, -1);
}
