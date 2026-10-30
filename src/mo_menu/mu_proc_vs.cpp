#include <types.h>

#include <gm/gm_common.h>
#include <gm/gm_syssave_task.h>
#include <nw4r/g3d/g3d_scnobj.h>
#include <snd/snd_system.h>

#include <mu/mu_proc_vs.h>

// Where the cursor goes from each icon (up, down, left, right).
static muProcMenuNode sVsNodes[muProcVs::Entry_Count] = {
    { { 3, 1 }, { 3, 0 }, { 4, 1 }, { 4, 0 } },
    { { 4, 1 }, { 2, 0 }, { 0, 0 }, { 0, 1 } },
    { { 1, 0 }, { 4, 1 }, { 3, 0 }, { 5, 1 } },
    { { 0, 0 }, { 0, 1 }, { 5, 1 }, { 2, 0 } },
    { { 2, 1 }, { 1, 0 }, { 0, 0 }, { 0, 1 } },
    { { 1, 0 }, { 4, 1 }, { 2, 0 }, { 3, 1 } },
};

void muProcVs::init(nw4r::g3d::ScnGroup* group, int from) {
    int item;
    m_group = group;
    m_nodes = sVsNodes;
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
    case 0x18:
        // Back from the Rules screen
        m_unk664 = 1;
        m_cursor = Entry_Rule;
        m_code = 0x18;
        setComeAnim(true);
        break;
    case 0x1E:
        // Back from the Tourney screen
        m_unk664 = 1;
        m_cursor = Entry_Tourney;
        m_code = 0x1E;
        setComeAnim(true);
        break;
    case 0x16:
        // Back from the Names screen: save the names that changed
        if (gmIsAutosaveDirtyFlag()) {
            gmSysSaveTask::create(0x2E);
            gmResetAutosaveDirtyFlag();
        }
        m_unk664 = 1;
        m_cursor = Entry_Name;
        m_code = 0x16;
        setComeAnim(true);
        break;
    case 0x17:
    default:
        // Back from a match: the cursor is on the icon of the mode that was played
        m_unk664 = 2;
        switch (getSceneStartCode()) {
        case 1:
            item = Entry_Brawl;
            break;
        case 2:
            item = Entry_Special;
            break;
        case 0x25:
            item = Entry_Rotation;
            break;
        case 3:
            item = Entry_Rule;
            break;
        }
        m_cursor = item;
        setSelectAnim(item, true);
        m_code = 0x22;
        break;
    }
    m_panel->changeProcess1(7, m_cursor + Entry_ObjBase);
}

int muProcVs::proc() {
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
            case Entry_Brawl:
                code = 1;
                break;
            case Entry_Special:
                code = 2;
                break;
            case Entry_Rotation:
                code = 0x22;
                break;
            default:
                code = 0x2A;
                break;
            }
            if (code != 0x2A) {
                exitScene(code, false);
                bool startsScene;
                switch (m_cursor) {
                case Entry_Brawl:
                case Entry_Rule:
                case Entry_Special:
                case Entry_Rotation:
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
            case Entry_Tourney:
                setOutAnim(true);
                m_unk664 = 3;
                m_result = 0x1E;
                return 2;
            case Entry_Name:
                setOutAnim(true);
                m_unk664 = 3;
                m_result = 0x16;
                gmResetAutosaveDirtyFlag();
                return 2;
            case Entry_Rule:
                setOutAnim(true);
                m_unk664 = 3;
                m_result = 0x18;
                g_sndSystem->playSE(static_cast<SndID>(1), -1, 0, 0, -1);
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
    case 1: {
        // Back from a screen of the menu: the icons wait, the one of the cursor starts to blink
        if (m_objs[Entry_ObjBase + m_cursor]->m_modelAnim->getFrame() == 9.0f) {
            m_objs[Entry_ObjBase + m_cursor]->setAnimIdx(&m_anims[2], true);
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
            case Entry_Brawl:
                code = 1;
                break;
            case Entry_Special:
                code = 2;
                break;
            case Entry_Rotation:
                code = 0x22;
                break;
            default:
                code = 0x2A;
                break;
            }
            if (code != 0x2A) {
                exitScene(code, false);
                bool startsScene;
                switch (m_cursor) {
                case Entry_Brawl:
                case Entry_Rule:
                case Entry_Special:
                case Entry_Rotation:
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
            case Entry_Tourney:
                setOutAnim(true);
                m_unk664 = 3;
                m_result = 0x1E;
                return 2;
            case Entry_Name:
                setOutAnim(true);
                m_unk664 = 3;
                m_result = 0x16;
                gmResetAutosaveDirtyFlag();
                return 2;
            case Entry_Rule:
                setOutAnim(true);
                m_unk664 = 3;
                m_result = 0x18;
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
            case Entry_Brawl:
                code = 1;
                break;
            case Entry_Special:
                code = 2;
                break;
            case Entry_Rotation:
                code = 0x22;
                break;
            default:
                code = 0x2A;
                break;
            }
            if (code != 0x2A) {
                exitScene(code, false);
                bool startsScene;
                switch (m_cursor) {
                case Entry_Brawl:
                case Entry_Rule:
                case Entry_Special:
                case Entry_Rotation:
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
            case Entry_Tourney:
                setOutAnim(true);
                m_unk664 = 3;
                m_result = 0x1E;
                return 2;
            case Entry_Name:
                setOutAnim(true);
                m_unk664 = 3;
                m_result = 0x16;
                gmResetAutosaveDirtyFlag();
                return 2;
            case Entry_Rule:
                setOutAnim(true);
                m_unk664 = 3;
                m_result = 0x18;
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

void muProcVs::exit() {
    for (u32 i = 0; i < Entry_Count; i++) {
        m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[Entry_ObjBase + i]->m_sceneModel));
    }
}

void muProcVs::playSE_CursorMove() {
    g_sndSystem->playSE(static_cast<SndID>(0x14), -1, 0, 0, -1);
}
