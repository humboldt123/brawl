#include <gm/gm_global.h>
#include <nw4r/g3d/g3d_scnobj.h>
#include <snd/snd_system.h>
#include <types.h>

#include <mu/mu_proc_1p.h>

// Where the cursor goes from each icon (up, down, left, right). The All-Star icon (2) sits between Classic (0) and Events
// (3); init rewires those two links when All-Star is not shown.
static muProcMenuNode sSoloNodes[muProc1p::Entry_Count] = {
    { { 3, 1 }, { 2, 0 }, { 1, 1 }, { 1, 0 } },
    { { 4, 1 }, { 4, 0 }, { 0, 0 }, { 0, 1 } },
    { { 0, 0 }, { 3, 0 }, { 1, 1 }, { 1, 0 } },
    { { 2, 0 }, { 0, 1 }, { 5, 1 }, { 4, 0 } },
    { { 1, 0 }, { 1, 1 }, { 3, 0 }, { 5, 0 } },
    { { 1, 0 }, { 1, 1 }, { 4, 0 }, { 3, 1 } },
};

void muProc1p::init(nw4r::g3d::ScnGroup* group, int from) {
    m_nodes = sSoloNodes;
    m_group = group;
    m_objBase = Entry_ObjBase;
    for (u32 i = 0; i < Entry_Count; i++) {
        if (g_GameGlobal->checkHideMenuAllstar() || i != Entry_AllStar) {
            group->Insert(group->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[Entry_ObjBase + i]->m_sceneModel));
        }
    }
    if (g_GameGlobal->checkHideMenuAllstar() == true) {
        Vec3f pos(0.0f, 0.0f, 0.0f);
        m_objs[Entry_ObjBase + Entry_Classic]->setTrans(&pos);
        sSoloNodes[Entry_Classic].m_down.m_next = Entry_AllStar;
        sSoloNodes[Entry_Event].m_up.m_next = Entry_AllStar;
    }
    if (g_GameGlobal->checkHideMenuAllstar() == false) {
        Vec3f pos(-2.3f, -4.4f, 0.0f);
        m_objs[Entry_ObjBase + Entry_Classic]->setTrans(&pos);
        sSoloNodes[Entry_Classic].m_down.m_next = Entry_Event;
        sSoloNodes[Entry_Event].m_up.m_next = Entry_Classic;
    }

    int item;
    switch (from) {
    case 0:
        // From the top screen
        m_unk664 = 0;
        m_cursor = Entry_Adventure;
        m_code = 0;
        setComeAnim(true);
        break;
    case 0x19:
        // Back from the Stadium screen
        m_unk664 = 1;
        m_cursor = Entry_Stadium;
        m_code = 0x19;
        setComeAnim(true);
        break;
    default:
        // Back from a scene: the cursor is on the icon of the scene that was played
        m_unk664 = 2;
        switch (getSceneStartCode()) {
        case 4:
            item = Entry_Classic;
            break;
        case 5:
            item = Entry_Adventure;
            break;
        case 6:
            item = Entry_AllStar;
            break;
        case 8:
            item = Entry_Event;
            break;
        case 0xC:
            item = Entry_Training;
            break;
        }
        m_cursor = item;
        setSelectAnim(item, true);
        m_code = 0x22;
        break;
    }
    m_panel->changeProcess1(8, m_cursor + Entry_ObjBase);
}

int muProc1p::proc() {
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
            m_panel->setMsg(m_cursor + Entry_ObjBase, 1, 0);
            m_unk664 = 2;
            return 4;
        }
        if (pad & Pad_Ok) {
            int code;
            switch (m_cursor) {
            case Entry_Classic:
                code = 4;
                break;
            case Entry_Adventure:
                code = 5;
                break;
            case Entry_AllStar:
                code = 6;
                break;
            case Entry_Event:
                code = 8;
                break;
            case Entry_Training:
                code = 0xC;
                break;
            default:
                code = 0x2A;
                break;
            }
            if (code != 0x2A) {
                exitScene(code, false);
                bool startsScene;
                switch (m_cursor) {
                case Entry_Classic:
                case Entry_Adventure:
                case Entry_AllStar:
                case Entry_Event:
                case Entry_Training:
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
            switch (m_cursor) {
            case Entry_Stadium:
                setOutAnim(true);
                m_unk664 = 3;
                m_result = 0x19;
                return 2;
            }
        } else if (pad & Pad_Cancel) {
            setOutAnim(false);
            m_unk664 = 3;
            m_result = 0;
            return 1;
        }
        break;
    }
    case 1:
        // Back from the Stadium screen: the icons wait, the one of the cursor starts to blink
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
            m_panel->setMsg(m_cursor + Entry_ObjBase, 1, 0);
        }
        if (pad & Pad_Ok) {
            int code;
            switch (m_cursor) {
            case Entry_Classic:
                code = 4;
                break;
            case Entry_Adventure:
                code = 5;
                break;
            case Entry_AllStar:
                code = 6;
                break;
            case Entry_Event:
                code = 8;
                break;
            case Entry_Training:
                code = 0xC;
                break;
            default:
                code = 0x2A;
                break;
            }
            if (code != 0x2A) {
                exitScene(code, false);
                bool startsScene;
                switch (m_cursor) {
                case Entry_Classic:
                case Entry_Adventure:
                case Entry_AllStar:
                case Entry_Event:
                case Entry_Training:
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
            switch (m_cursor) {
            case Entry_Adventure:
                break;
            case Entry_Stadium:
                setOutAnim(true);
                m_unk664 = 3;
                m_result = 0x19;
                return 2;
            }
        } else if (pad & Pad_Cancel) {
            setOutAnim(false);
            m_unk664 = 3;
            m_result = 0;
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

void muProc1p::exit() {
    for (u32 i = 0; i < Entry_Count; i++) {
        m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[Entry_ObjBase + i]->m_sceneModel));
    }
}

void muProc1p::playSE_CursorMove() {
    g_sndSystem->playSE(static_cast<SndID>(0x14), -1, 0, 0, -1);
}
