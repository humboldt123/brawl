#include <types.h>

#include <gm/gm_common.h>
#include <gm/gm_global.h>
#include <gm/gm_syssave_task.h>
#include <nw4r/g3d/g3d_scnobj.h>
#include <snd/snd_system.h>

#include <mu/mu_proc_option.h>

// Where the cursor goes from each icon (up, down, left, right).
static muProcMenuNode sOptionNodes[muProcOption::Entry_Count] = {
    { { 4, 1 }, { 4, 0 }, { 3, 1 }, { 1, 0 } },
    { { 5, 1 }, { 5, 0 }, { 0, 0 }, { 2, 0 } },
    { { 6, 1 }, { 6, 0 }, { 1, 0 }, { 3, 0 } },
    { { 6, 1 }, { 6, 0 }, { 2, 0 }, { 0, 1 } },
    { { 0, 0 }, { 0, 1 }, { 6, 1 }, { 5, 0 } },
    { { 1, 0 }, { 1, 1 }, { 4, 0 }, { 6, 0 } },
    { { 2, 0 }, { 2, 1 }, { 5, 0 }, { 4, 1 } },
};

// The animations of the icons of this screen (the Options icons also have a color animation, flag 0x10).
static MuAnimIdxData sOptionAnims[6] = {
    { 0.0f, 9.0f, 0, 2, 0x13 },
    { 0.0f, 9.0f, 0, 0, 0x13 },
    { 0.0f, 59.0f, 1, 5, 0x13 },
    { 0.0f, 179.0f, 0, 4, 0x13 },
    { 0.0f, 29.0f, 0, 3, 0x13 },
    { 0.0f, 19.0f, 0, 1, 0x13 },
};

void muProcOption::init(nw4r::g3d::ScnGroup* group, int from) {
    m_nodes = sOptionNodes;
    m_group = group;
    m_anims = sOptionAnims;
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
        gmResetAutosaveDirtyFlag();
        break;
    default:
        // Back from one of the option screens: save what changed, the cursor is on its icon
        if (gmIsAutosaveDirtyFlag()) {
            gmSysSaveTask::create(0x2E);
            gmResetAutosaveDirtyFlag();
        }
        m_unk664 = 1;
        m_cursor = from - Entry_FirstCode;
        m_code = from;
        break;
    }
    setComeAnim(true);
    int msgs[Entry_Count] = { 0x1D, 0x1E, 0x1F, 0x20, 0x22, 0x21, 0x24 };
    m_panel->changeProcess1(0xB, msgs[m_cursor]);
    // The Screen icon shows whether the game runs in widescreen
    m_objs[Entry_ObjBase]->setFrameTex((float)(u32)g_GameGlobal->getGlobalRecordMenuDatap()->m_isWidescreen);
}

int muProcOption::proc() {
    // The message of the help balloon for each icon
    int msgs[Entry_Count] = { 0x1D, 0x1E, 0x1F, 0x20, 0x22, 0x21, 0x24 };
    switch (m_unk664) {
    case 0: {
        // The icons come in; the one of the cursor starts to blink when it has arrived
        MuObject* object = m_objs[Entry_ObjBase + m_cursor];
        if (muGetAnimIdx(object) == 2 && object->m_modelAnim->getFrame() == 9.0f) {
            m_objs[Entry_ObjBase + m_cursor]->setAnimIdx(&sOptionAnims[3], true);
            m_unk664 = 2;
        }
        u16 oldCursor;
        u32 pad = padUpdate();
        oldCursor = m_cursor;
        if (selectProc(pad)) {
            setSelectAnim(oldCursor, true);
            m_panel->setMsg(msgs[m_cursor], 1, 0);
            m_unk664 = 2;
            return 4;
        }
        if (pad & Pad_Ok) {
            setOutAnim(true);
            m_unk664 = 3;
            m_result = m_cursor + Entry_FirstCode;
            return 2;
        } else if (pad & Pad_Cancel) {
            setOutAnim(false);
            m_result = 0;
            m_unk664 = 3;
            return 1;
        }
        break;
    }
    case 1: {
        // Back from an option screen: the icons wait, the one of the cursor starts to blink
        if (m_objs[Entry_ObjBase + m_cursor]->m_modelAnim->getFrame() == 9.0f) {
            m_objs[Entry_ObjBase + m_cursor]->setAnimIdx(&sOptionAnims[2], true);
            m_unk664 = 2;
        }
        u16 oldCursor;
        u32 pad = padUpdate();
        oldCursor = m_cursor;
        if (selectProc(pad)) {
            m_panel->setMsg(msgs[m_cursor], 1, 0);
            setSelectAnim(oldCursor, true);
            m_unk664 = 2;
            return 4;
        }
        if (pad & Pad_Ok) {
            setOutAnim(true);
            m_unk664 = 3;
            m_result = m_cursor + Entry_FirstCode;
            return 2;
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
            m_objs[Entry_ObjBase + m_cursor]->setAnimIdx(&sOptionAnims[2], true);
        }
        u16 oldCursor;
        u32 pad = padUpdate();
        oldCursor = m_cursor;
        if (selectProc(pad)) {
            m_panel->setMsg(msgs[m_cursor], 1, 0);
            setSelectAnim(oldCursor, false);
        }
        if (pad & Pad_Ok) {
            setOutAnim(true);
            m_unk664 = 3;
            m_result = m_cursor + Entry_FirstCode;
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
        // The icons go out
        if (m_objs[Entry_ObjBase + m_cursor]->m_modelAnim->getUpdateRate() == 0.0f) {
            return 3;
        }
        break;
    }
    return 0;
}

void muProcOption::exit() {
    for (u32 i = 0; i < Entry_Count; i++) {
        m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[Entry_ObjBase + i]->m_sceneModel));
    }
}

void muProcOption::playSE_CursorMove() {
    g_sndSystem->playSE(static_cast<SndID>(0x14), -1, 0, 0, -1);
}
