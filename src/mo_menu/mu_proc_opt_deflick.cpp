#include <types.h>

#include <gf/gf_video.h>
#include <gm/gm_common.h>
#include <gm/gm_global.h>
#include <nw4r/g3d/g3d_scnobj.h>
#include <snd/snd_system.h>

#include <mu/mu_proc_opt_deflick.h>

// The animations of the switch icon.
static MuAnimIdxData sOptDeflickAnims[3] = {
    { 0.0f, 9.0f, 0, 0, 0x13 },     // comes in
    { 0.0f, 29.0f, 1, 2, 0x13 },    // waits
    { 0.0f, 9.0f, 0, 1, 0x13 },     // goes out
};

void muProcOptDeflick::init(nw4r::g3d::ScnGroup* group, int from) {
    m_group = group;
    m_unk664 = 0;
    m_cursor = g_GameGlobal->getGlobalRecordMenuDatap()->m_deflicker;
    group->Insert(group->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[Entry_Obj]->m_sceneModel));
    m_objs[Entry_Obj]->setAnimIdx(&sOptDeflickAnims[0], true);
    // The frame of the texture pattern shows the setting (0 off, 1 on); it does not play by itself
    m_objs[Entry_Obj]->changeTexPatAnim(0);
    m_objs[Entry_Obj]->setFrameTex(m_cursor);
    m_objs[Entry_Obj]->m_modelAnim->m_anmObjTexPatRes->SetUpdateRate(0.0f);
    m_panel->changeProcess1(0x15, 0x3F - m_cursor);
}

int muProcOptDeflick::proc() {
    // BYTES: the state is compared unsigned here (cmplwi), as in muProcItemSwitch::proc
    if ((u32)m_unk664 == 2) {
        // The icon goes out
        if (m_objs[Entry_Obj]->m_modelAnim->getUpdateRate() == 0.0f) {
            return 3;
        }
        return 0;
    }

    MuObject* object = m_objs[Entry_Obj];
    if (muGetAnimIdx(object) == 0 && object->m_modelAnim->getFrame() == 9.0f) {
        m_objs[Entry_Obj]->setAnimIdx(&sOptDeflickAnims[1], true);
    }

    u32 pad = padUpdate();
    if (pad & Pad_Ok) {
        g_sndSystem->playSE(static_cast<SndID>(0x24), -1, 0, 0, -1);
        m_cursor ^= 1;
        lbl_805A0098->setDeflicker(m_cursor == 1);
        m_panel->setMsg(0x3F - m_cursor, 1, 0);
        m_objs[Entry_Obj]->setFrameTex(m_cursor);
    } else if (pad & Pad_Cancel) {
        if ((u8)m_cursor != g_GameGlobal->getGlobalRecordMenuDatap()->m_deflicker) {
            g_GameGlobal->getGlobalRecordMenuDatap()->m_deflicker = m_cursor;
            gmSetAutosaveDirtyFlag();
        }
        m_objs[Entry_Obj]->setAnimIdx(&sOptDeflickAnims[2], true);
        m_result = 5;
        m_unk664 = 2;
        return 1;
    }
    return 0;
}

void muProcOptDeflick::exit() {
    m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[Entry_Obj]->m_sceneModel));
}

void muProcOptDeflick::playSE_CursorMove() {
    g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
}
