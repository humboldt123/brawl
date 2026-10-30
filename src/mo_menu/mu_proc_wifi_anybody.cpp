#include <types.h>

#include <nt/nt_etc_global.h>
#include <nw4r/g3d/g3d_scnobj.h>
#include <snd/snd_system.h>

#include <mu/mu_proc_wifi_anybody.h>

// Where the cursor goes from each icon (up, down, left, right). init rewires the left/right links of icons 1 and 2 to
// skip icon 0 when it is not shown.
static muProcMenuNode sWifiAnybodyNodes[muProcWifiAnybody::Entry_Count] = {
    { { 2, 1 }, { 2, 0 }, { 1, 1 }, { 1, 0 } },
    { { 2, 0 }, { 2, 1 }, { 0, 0 }, { 0, 1 } },
    { { 1, 0 }, { 1, 1 }, { 0, 0 }, { 1, 1 } },
};

void muProcWifiAnybody::init(nw4r::g3d::ScnGroup* group, int from) {
    int item;
    m_group = group;
    m_nodes = sWifiAnybodyNodes;
    m_objBase = Entry_ObjBase;
    bool showFirst = nteGlobal::isEnableButton(0x10);
    for (u32 i = 0; i < Entry_Count; i++) {
        if (showFirst || i != 0) {
            group->Insert(group->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[Entry_ObjBase + i]->m_sceneModel));
        }
    }

    switch (from) {
    case 3:
        // From the Wi-Fi screen
        m_unk664 = 0;
        m_cursor = 1;
        m_code = 3;
        setComeAnim(true);
        break;
    default:
        // Back from a scene: the cursor is on the icon of the mode that was played
        m_unk664 = 2;
        switch (getSceneStartCode()) {
        case 0x1D:
            item = 0;
            break;
        case 0x1E:
            item = 1;
            break;
        case 0x1F:
            item = 2;
            break;
        }
        m_cursor = item;
        setSelectAnim(item, true);
        m_code = 0x22;
        break;
    }

    if (showFirst == true) {
        sWifiAnybodyNodes[1].m_left.m_next = 0;
        sWifiAnybodyNodes[1].m_right.m_next = 0;
        sWifiAnybodyNodes[2].m_left.m_next = 0;
        sWifiAnybodyNodes[2].m_right.m_next = 1;
        Vec3f pos;
        pos = m_objs[Entry_ObjBase + 1]->m_trans;
        pos.m_x = 0.0f;
        m_objs[Entry_ObjBase + 1]->setTrans(&pos);
        pos = m_objs[Entry_ObjBase + 2]->m_trans;
        pos.m_x = 0.0f;
        m_objs[Entry_ObjBase + 2]->setTrans(&pos);
    }
    if (showFirst == false) {
        sWifiAnybodyNodes[1].m_left.m_next = 2;
        sWifiAnybodyNodes[1].m_right.m_next = 2;
        sWifiAnybodyNodes[2].m_left.m_next = 1;
        sWifiAnybodyNodes[2].m_right.m_next = 1;
        Vec3f pos;
        pos = m_objs[Entry_ObjBase + 1]->m_trans;
        pos.m_x = -5.0f;
        m_objs[Entry_ObjBase + 1]->setTrans(&pos);
        pos = m_objs[Entry_ObjBase + 2]->m_trans;
        pos.m_x = -5.0f;
        m_objs[Entry_ObjBase + 2]->setTrans(&pos);
    }
    m_panel->changeProcess1(0x12, m_cursor + Entry_MsgBase);
}

int muProcWifiAnybody::proc() {
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
            m_panel->setMsg(m_cursor + Entry_MsgBase, 1, 0);
            m_unk664 = 2;
            return 4;
        }
        if (pad & Pad_Ok) {
            int code;
            switch (m_cursor) {
            case 0:
                code = 0x1D;
                break;
            case 1:
                code = 0x1E;
                break;
            case 2:
                code = 0x1F;
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
            // Back to the Wi-Fi screen
            setOutAnim(false);
            m_result = 3;
            m_unk664 = 3;
            return 1;
        }
        break;
    }
    case 1:
        // The icons wait, the one of the cursor starts to blink
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
            m_panel->setMsg(m_cursor + Entry_MsgBase, 1, 0);
            setSelectAnim(oldCursor, false);
        }
        if (pad & Pad_Ok) {
            int code;
            switch (m_cursor) {
            case 0:
                code = 0x1D;
                break;
            case 1:
                code = 0x1E;
                break;
            case 2:
                code = 0x1F;
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
            setOutAnim(false);
            m_result = 3;
            m_unk664 = 3;
            return 1;
        }
        break;
    }
    case 3:
        // The icons go out. The object looked at is the message number, not the icon (as the game does it).
        if (m_objs[Entry_MsgBase + m_cursor]->m_modelAnim->getUpdateRate() == 0.0f) {
            return 3;
        }
        break;
    }
    return 0;
}

void muProcWifiAnybody::exit() {
    for (u32 i = 0; i < Entry_Count; i++) {
        m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[Entry_ObjBase + i]->m_sceneModel));
    }
}

void muProcWifiAnybody::playSE_CursorMove() {
    g_sndSystem->playSE(static_cast<SndID>(0x14), -1, 0, 0, -1);
}
