#include <types.h>

#include <nw4r/g3d/g3d_scnobj.h>
#include <snd/snd_system.h>

#include <mu/mu_proc_figure.h>

// Where the cursor goes from each icon (up, down, left, right).
static muProcMenuNode sFigureNodes[muProcFigure::Entry_Count] = {
    { { 4, 1 }, { 4, 0 }, { 2, 1 }, { 1, 0 } },
    { { 3, 1 }, { 3, 0 }, { 0, 0 }, { 2, 0 } },
    { { 3, 1 }, { 3, 0 }, { 1, 0 }, { 0, 1 } },
    { { 1, 0 }, { 1, 1 }, { 4, 0 }, { 4, 1 } },
    { { 0, 0 }, { 0, 1 }, { 3, 1 }, { 3, 0 } },
};

void muProcFigure::init(nw4r::g3d::ScnGroup* group, int from) {
    int item;
    m_nodes = sFigureNodes;
    m_group = group;
    m_unk664 = 0;
    m_objBase = Entry_ObjBase;
    for (u32 i = 0; i < Entry_Count; i++) {
        group->Insert(group->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[Entry_ObjBase + i]->m_sceneModel));
    }

    switch (from) {
    case Entry_BackCode:
        // From the Vault screen
        m_cursor = 0;
        m_code = Entry_BackCode;
        setComeAnim(true);
        break;
    default:
        // Back from a scene: the cursor is on the icon of the scene that was shown
        m_unk664 = 1;
        switch (getSceneStartCode()) {
        case 0xF:
            item = 4;
            break;
        case 0x10:
            item = 3;
            break;
        case 0x11:
            item = 0;
            break;
        case 0x12:
            item = 1;
            break;
        case 0x13:
            item = 2;
            break;
        }
        m_cursor = item;
        setSelectAnim(item, true);
        m_code = 0x22;
        break;
    }
    m_panel->changeProcess1(0x13, m_cursor + Entry_MsgBase);
}

int muProcFigure::proc() {
    switch (m_unk664) {
    case 0: {
        // The icons come in; the one of the cursor starts to blink when it has arrived
        MuObject* object = m_objs[Entry_ObjBase + m_cursor];
        if (muGetAnimIdx(object) == 2 && object->m_modelAnim->getFrame() == 9.0f) {
            m_objs[Entry_ObjBase + m_cursor]->setAnimIdx(&m_anims[3], true);
            m_unk664 = 1;
        }
        u16 oldCursor;
        u32 pad = padUpdate();
        oldCursor = m_cursor;
        if (selectProc(pad)) {
            setSelectAnim(oldCursor, true);
            m_panel->setMsg(m_cursor + Entry_MsgBase, 1, 0);
            m_unk664 = 1;
            return 4;
        }
        if (pad & Pad_Ok) {
            int code;
            switch (m_cursor) {
            case 0:
                code = 0x11;
                break;
            case 1:
                code = 0x12;
                break;
            case 2:
                code = 0x13;
                break;
            case 4:
                code = 0xF;
                break;
            case 3:
                code = 0x10;
                break;
            default:
                code = 0x2A;
                break;
            }
            if (code != 0x2A) {
                exitScene(code, false);
                bool startsScene;
                switch (m_cursor) {
                case 0:
                case 1:
                case 2:
                case 3:
                case 4:
                    startsScene = true;
                    break;
                default:
                    startsScene = false;
                    break;
                }
                if (startsScene) {
                    g_sndSystem->playSE(static_cast<SndID>(0x13), -1, 0, 0, -1);
                } else {
                    g_sndSystem->playSE(static_cast<SndID>(1), -1, 0, 0, -1);
                }
            }
        } else if (pad & Pad_Cancel) {
            // Back to the Vault screen
            setOutAnim(true);
            m_result = Entry_BackCode;
            m_unk664 = 2;
            return 1;
        }
        break;
    }
    case 1: {
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
            m_panel->setMsg(m_cursor + Entry_MsgBase, 1, 0);
        }
        if (pad & Pad_Ok) {
            int code;
            switch (m_cursor) {
            case 0:
                code = 0x11;
                break;
            case 1:
                code = 0x12;
                break;
            case 2:
                code = 0x13;
                break;
            case 4:
                code = 0xF;
                break;
            case 3:
                code = 0x10;
                break;
            default:
                code = 0x2A;
                break;
            }
            if (code != 0x2A) {
                exitScene(code, false);
                bool startsScene;
                switch (m_cursor) {
                case 0:
                case 1:
                case 2:
                case 3:
                case 4:
                    startsScene = true;
                    break;
                default:
                    startsScene = false;
                    break;
                }
                if (startsScene) {
                    g_sndSystem->playSE(static_cast<SndID>(0x13), -1, 0, 0, -1);
                } else {
                    g_sndSystem->playSE(static_cast<SndID>(1), -1, 0, 0, -1);
                }
            }
        } else if (pad & Pad_Cancel) {
            setOutAnim(true);
            m_result = Entry_BackCode;
            m_unk664 = 2;
            return 1;
        }
        break;
    }
    case 2:
        // The icons go out
        if (m_objs[Entry_ObjBase + m_cursor]->m_modelAnim->getUpdateRate() == 0.0f) {
            return 3;
        }
        break;
    }
    return 0;
}

void muProcFigure::exit() {
    for (u32 i = 0; i < Entry_Count; i++) {
        m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[Entry_ObjBase + i]->m_sceneModel));
    }
}

void muProcFigure::playSE_CursorMove() {
    g_sndSystem->playSE(static_cast<SndID>(0x14), -1, 0, 0, -1);
}
