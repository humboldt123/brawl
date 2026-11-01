#include <types.h>

#include <gm/gm_common.h>
#include <gm/gm_global.h>
#include <nw4r/g3d/g3d_scnobj.h>
#include <snd/snd_system.h>

#include <mu/mu_proc_opt_sound.h>

// The animations of the two entries.
static MuAnimIdxData sOptSoundAnims[6] = {
    { 0.0f, 9.0f, 0, 2, 0x12 },     // comes in under the cursor
    { 0.0f, 9.0f, 0, 0, 0x12 },     // waits
    { 0.0f, 59.0f, 1, 5, 0x12 },    // blinks under the cursor
    { 0.0f, 29.0f, 0, 4, 0x12 },    // the cursor arrives
    { 0.0f, 9.0f, 0, 3, 0x12 },     // chosen
    { 0.0f, 9.0f, 0, 1, 0x12 },     // goes out
};

void muProcOptSound::init(nw4r::g3d::ScnGroup* group, int from) {
    m_group = group;
    m_volume = g_GameGlobal->getGlobalRecordMenuDatap()->m_volBalance;
    m_holdCount = 20;
    group->Insert(group->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[Entry_MdlSlider]));
    m_panel->changeProcess1(0x18, 0x44);
    switch (from) {
    case 5:
        // From the Options screen
        m_panel->setMsg(0x44, 1, 0);
        m_unk664 = 0;
        m_cursor = Entry_Slider;
        m_code = 5;
        break;
    case 0xF:
        m_panel->setMsg(0x45, 1, 0);
        m_unk664 = 1;
        m_cursor = Entry_Ok;
        m_code = 0xF;
        break;
    }
    for (int i = 0; i < Entry_Count; i++) {
        if (i == m_cursor) {
            m_objs[Entry_ObjBase + i]->setAnimIdx(&sOptSoundAnims[0], true);
        } else {
            m_objs[Entry_ObjBase + i]->setAnimIdx(&sOptSoundAnims[1], true);
        }
    }
    setAnimFrame(Entry_ObjBase + Entry_Slider, m_volume, 9);
    m_objs[Entry_ObjCursor]->setFrameNode(m_cursor);
    m_objs[Entry_ObjCursor]->m_modelAnim->m_anmObjChrRes->SetUpdateRate(0.0f);
}

int muProcOptSound::proc() {
    u32 pad;
    switch (m_unk664) {
    case 0:
    case 1: {
        // The entries come in; the one of the cursor starts to blink when it has arrived
        if (m_objs[Entry_ObjBase + m_cursor]->m_modelAnim->m_anmObjMatClrRes->GetFrame() == 9.0f) {
            m_objs[Entry_ObjBase + m_cursor]->setAnimIdx(&sOptSoundAnims[3], true);
            m_unk664 = 2;
        }
        pad = padUpdate();
        if (selectProc(pad)) {
            m_unk664 = 2;
            return 4;
        }
        if (pad & Pad_Ok) {
            if (m_cursor == Entry_Ok) {
                for (int i = 0; i < Entry_Count; i++) {
                    if (i == m_cursor) {
                        m_objs[Entry_ObjBase + i]->setAnimIdx(&sOptSoundAnims[4], true);
                    } else {
                        m_objs[Entry_ObjBase + i]->setAnimIdx(&sOptSoundAnims[5], true);
                    }
                }
                m_unk664 = 3;
                m_result = 0xF;
                // BYTES: the original keeps an unreachable copy of the save here (the compiler folds the test against the
                // state it has just stored but leaves the body). HYPOTHESIS: the test was on the state, as written here.
                if (m_unk664 == 2) {
                    if ((s8)m_volume != g_GameGlobal->getGlobalRecordMenuDatap()->m_volBalance) {
                        g_GameGlobal->getGlobalRecordMenuDatap()->m_volBalance = m_volume;
                        gmSetAutosaveDirtyFlag();
                    }
                }
                return 2;
            }
        } else if (pad & Pad_Cancel) {
            for (int i = 0; i < Entry_Count; i++) {
                m_objs[Entry_ObjBase + i]->setAnimIdx(&sOptSoundAnims[5], true);
            }
            m_result = 5;
            m_unk664 = 3;
            // BYTES: unreachable save, as above
            if (m_unk664 == 2) {
                if ((s8)m_volume != g_GameGlobal->getGlobalRecordMenuDatap()->m_volBalance) {
                    g_GameGlobal->getGlobalRecordMenuDatap()->m_volBalance = m_volume;
                    gmSetAutosaveDirtyFlag();
                }
            }
            return 1;
        }
        break;
    }
    case 2: {
        // The cursor is on an entry
        MuObject* object = m_objs[Entry_ObjBase + m_cursor];
        if (muGetAnimIdx(object) == 4 && object->m_modelAnim->m_anmObjMatClrRes->GetFrame() >= 29.0f) {
            m_objs[Entry_ObjBase + m_cursor]->setAnimIdx(&sOptSoundAnims[2], true);
        }
        pad = padUpdate();
        selectProc(pad);
        if (pad & Pad_Ok) {
            if (m_cursor == Entry_Ok) {
                for (int i = 0; i < Entry_Count; i++) {
                    if (i == m_cursor) {
                        m_objs[Entry_ObjBase + i]->setAnimIdx(&sOptSoundAnims[4], true);
                    } else {
                        m_objs[Entry_ObjBase + i]->setAnimIdx(&sOptSoundAnims[5], true);
                    }
                }
                if ((s8)m_volume != g_GameGlobal->getGlobalRecordMenuDatap()->m_volBalance) {
                    g_GameGlobal->getGlobalRecordMenuDatap()->m_volBalance = m_volume;
                    gmSetAutosaveDirtyFlag();
                }
                m_result = 0xF;
                m_unk664 = 3;
                return 2;
            }
        } else if (pad & Pad_Cancel) {
            if ((s8)m_volume != g_GameGlobal->getGlobalRecordMenuDatap()->m_volBalance) {
                g_GameGlobal->getGlobalRecordMenuDatap()->m_volBalance = m_volume;
                gmSetAutosaveDirtyFlag();
            }
            for (int i = 0; i < Entry_Count; i++) {
                m_objs[Entry_ObjBase + i]->setAnimIdx(&sOptSoundAnims[5], true);
            }
            m_result = 5;
            m_unk664 = 3;
            return 1;
        }
        break;
    }
    case 3:
        // The entries go out
        if (m_objs[Entry_ObjBase + m_cursor]->m_modelAnim->getUpdateRate() == 0.0f) {
            return 3;
        }
        break;
    }
    return 0;
}

void muProcOptSound::exit() {
    m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[Entry_MdlSlider]));
}

bool muProcOptSound::selectProc(int dir) {
    bool changed = false;
    if ((dir & Pad_Up && m_cursor == Entry_Ok) || (dir & Pad_Down && m_cursor == Entry_Slider)) {
        g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
        changed = true;
        m_cursor ^= 1;
        m_panel->setMsg(m_cursor + 0x44, 1, 0);
        m_objs[Entry_ObjCursor]->setFrameNode(m_cursor);
        m_objs[Entry_ObjBase + (m_cursor ^ 1)]->setAnimIdx(&sOptSoundAnims[1], true);
        m_objs[Entry_ObjBase + (m_cursor ^ 1)]->m_animIdxData = NULL;
        setAnimFrame(Entry_ObjBase + (m_cursor ^ 1), 9.0f, 0x12);
        m_objs[Entry_ObjBase + m_cursor]->setAnimIdx(&sOptSoundAnims[3], true);
    } else if (m_cursor == Entry_Slider) {
        // Left and right (held, with repeat) move the slider; it rests at 50 for 20 frames
        u32 held = m_pad.m_buttonsCurrentFrame2.bits;
        if (held & GF_PAD_BUTTON_BIT_DPAD_LEFT && m_volume < 100) {
            if (m_volume == 50 && m_holdCount < 20) {
                m_holdCount++;
            } else {
                m_holdCount = 0;
                m_volume++;
                setAnimFrame(Entry_ObjBase + Entry_Slider, m_volume, 9);
                g_sndSystem->setVolBalance((100 - m_volume) / 100.0f, m_volume / 100.0f);
                changed = true;
            }
        } else if (held & GF_PAD_BUTTON_BIT_DPAD_RIGHT && m_volume != 0) {
            if (m_volume == 50 && m_holdCount < 20) {
                m_holdCount++;
            } else {
                m_holdCount = 0;
                m_volume--;
                setAnimFrame(Entry_ObjBase + Entry_Slider, m_volume, 9);
                g_sndSystem->setVolBalance((100 - m_volume) / 100.0f, m_volume / 100.0f);
                changed = true;
            }
        }
    }
    // A tick every third step
    if (changed && m_volume % 3 == 0) {
        g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
    }
    return changed;
}

void muProcOptSound::playSE_CursorMove() { }
