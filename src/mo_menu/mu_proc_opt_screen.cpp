#include <types.h>

#include <cm/cm_camera_controller.h>
#include <gf/gf_camera.h>
#include <gf/gf_camera_controller.h>
#include <gm/gm_common.h>
#include <gm/gm_global.h>
#include <nw4r/g3d/g3d_scnobj.h>
#include <snd/snd_system.h>

#include <mu/mu_proc_opt_screen.h>

// The animations of the two icons.
static MuAnimIdxData sOptScreenAnims[6] = {
    { 0.0f, 9.0f, 0, 2, 0x11 },     // comes in under the cursor
    { 0.0f, 9.0f, 0, 0, 0x11 },     // waits
    { 0.0f, 59.0f, 1, 5, 0x11 },    // blinks under the cursor
    { 0.0f, 29.0f, 0, 4, 0x11 },    // the cursor arrives
    { 0.0f, 9.0f, 0, 3, 0x11 },
    { 0.0f, 9.0f, 0, 1, 0x11 },     // goes out
};

void muProcOptScreen::init(nw4r::g3d::ScnGroup* group, int from) {
    m_group = group;
    m_unk664 = 0;
    m_cursor = g_GameGlobal->getGlobalRecordMenuDatap()->m_isWidescreen;
    m_panel->changeProcess1(0x14, m_cursor + 0x3C);
    for (u32 i = 0; i < Entry_Count; i++) {
        group->Insert(group->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[Entry_ObjBase + i]->m_sceneModel));
    }
    for (int i = 0; i < Entry_Count; i++) {
        if (i == m_cursor) {
            m_objs[Entry_ObjBase + i]->setAnimIdx(&sOptScreenAnims[0], true);
        } else {
            m_objs[Entry_ObjBase + i]->setAnimIdx(&sOptScreenAnims[1], true);
        }
    }
}

int muProcOptScreen::proc() {
    // BYTES: the state is compared unsigned here (cmplwi), as in muProcItemSwitch::proc
    if ((u32)m_unk664 == 2) {
        // The icons go out
        if (m_objs[Entry_ObjBase]->m_modelAnim->getUpdateRate() == 0.0f) {
            return 3;
        }
        return 0;
    }

    MuObject* object = m_objs[Entry_ObjBase + m_cursor];
    int animIdx = muGetAnimIdx(object);
    switch (animIdx) {
    case 4:
        if (object->m_modelAnim->getFrame() == 29.0f) {
            m_objs[Entry_ObjBase + m_cursor]->setAnimIdx(&sOptScreenAnims[2], true);
        }
        break;
    case 2:
        if (object->m_modelAnim->getFrame() == 9.0f) {
            m_objs[Entry_ObjBase + m_cursor]->setAnimIdx(&sOptScreenAnims[3], true);
        }
        break;
    }

    u32 pad = padUpdate();
    if ((pad & Pad_Left && m_cursor == Entry_Wide) || (pad & Pad_Right && m_cursor == Entry_Standard)) {
        g_sndSystem->playSE(static_cast<SndID>(0x23), -1, 0, 0, -1);
        m_cursor ^= 1;
        m_panel->setMsg(m_cursor + 0x3C, 1, 0);
        // Switch the camera to the new ratio at once
        float aspect;
        int ratio;
        if (m_cursor == Entry_Standard) {
            aspect = 1.35f;
            ratio = 1;
        } else {
            ratio = 0;
            aspect = 1.73232f;
        }
        gfCameraManager::getManager()->setAspectRatio(ratio);
        CameraController::getInstance()->m_currentController->getCameraManager()->getCamera(0)->m_aspect = aspect;
        m_objs[Entry_ObjBase + (m_cursor ^ 1)]->setAnimIdx(&sOptScreenAnims[1], true);
        m_objs[Entry_ObjBase + (m_cursor ^ 1)]->m_animIdxData = NULL;
        setAnimFrame(Entry_ObjBase + (m_cursor ^ 1), 9.0f, 0x11);
        m_objs[Entry_ObjBase + m_cursor]->setAnimIdx(&sOptScreenAnims[3], true);
    } else if (pad & Pad_Cancel) {
        for (int i = 0; i < Entry_Count; i++) {
            m_objs[Entry_ObjBase + i]->setAnimIdx(&sOptScreenAnims[5], true);
        }
        if ((u8)m_cursor != g_GameGlobal->getGlobalRecordMenuDatap()->m_isWidescreen) {
            g_GameGlobal->getGlobalRecordMenuDatap()->m_isWidescreen = (u8)m_cursor;
            gmSetAutosaveDirtyFlag();
        }
        m_result = 5;
        m_unk664 = 2;
        return 1;
    }
    return 0;
}

void muProcOptScreen::exit() {
    m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[Entry_ObjBase + Entry_Standard]->m_sceneModel));
    m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[Entry_ObjBase + Entry_Wide]->m_sceneModel));
}

void muProcOptScreen::playSE_CursorMove() {
    g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
}
