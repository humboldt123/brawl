#include <types.h>

#include <gf/gf_model.h>
#include <gf/gf_pad_system.h>
#include <gm/gm_common.h>
#include <memory.h>
#include <nw4r/g3d/g3d_scnmdlsmpl.h>
#include <nw4r/g3d/g3d_scnobj.h>
#include <snd/snd_bgm.h>
#include <snd/snd_system.h>

#include <mu/mu_proc_opt_song.h>

// The animations of the cursors of the four lines.
static MuAnimIdxData sSongCursorAnims[6] = {
    { 0.0f, 9.0f, 0, 2, 0x11 },     // comes in under the cursor
    { 0.0f, 9.0f, 0, 0, 0x11 },     // waits
    { 0.0f, 59.0f, 1, 5, 0x11 },    // blinks under the cursor
    { 0.0f, 29.0f, 0, 4, 0x11 },    // the cursor arrives
    { 0.0f, 9.0f, 0, 3, 0x11 },     // chosen
    { 0.0f, 9.0f, 0, 1, 0x11 },     // goes out
};

// The colors of the lines. HYPOTHESIS: only the last one (the line under the cursor) is used.
static MuAnimIdxData sSongRowAnims[3] = {
    { 1.0f, 2.0f, 0, 0, 0x10 },
    { 3.0f, 4.0f, 0, 0, 0x10 },
    { 10.0f, 40.0f, 1, 0, 0x10 },
};

// The icon of the pad blinks.
static MuAnimIdxData sSongScrollAnim = { 0.0f, 30.0f, 1, 0, 0x10 };

// The category of the songs of each stage (proc copies the table before it reads it).
// HYPOTHESIS: a struct wrapper; a plain local array keeps its initializer in .rodata, the original copies it from .data.
struct StageCategories {
    int m_category[muProcOptSong::StageCount];
};
static StageCategories sStageCategories = { {
    0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D,
    0x0E, 0x0E, 0x0E, 0x0E, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x15, 0x15,
    0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x2D, 0x2D, 0x2D, 0x20,
    0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x0A, 0x2D, 0x2D, 0x2D, 0x2D,
    0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x00, 0x00,
} };

// HYPOTHESIS: an inline helper of sndBgmRateSystem that this unit expands (no category past the 45 of the system).
static inline u32 getCategoryCount(sndBgmRateSystem* system, int category) {
    if (category >= 45) {
        return 0;
    }
    return system->m_categoryCount[category];
}

// The line under the cursor is drawn over the others (the gauge of the My Music lines overlaps the next line).
static inline void addDrawPriority(MuObject* object, int add) {
    int priority = object->m_scnMdl->m_priorityDrawOpa + add;
    object->m_scnMdl->SetPriorityDrawOpa(priority);
    object->m_scnMdl->SetPriorityDrawXlu(priority);
}

// The rate of a song of the list: in My Music the first m_myMusicCount songs are of category 4, the others of 5.
static inline u8 getSongRate(muProcOptSong* proc, u32 index) {
    u32 bgmId = proc->m_bgmIds[index];
    int category = proc->m_category;
    if (category == muProcOptSong::Category_MyMusic) {
        if (index < proc->m_myMusicCount) {
            return lbl_805A01D8->getRate(muProcOptSong::Category_MyMusic, bgmId);
        }
        return lbl_805A01D8->getRate(muProcOptSong::Category_MyMusic2, bgmId);
    }
    return lbl_805A01D8->getRate(category, bgmId);
}

static inline void setSongRate(muProcOptSong* proc, u32 index, u8 rate) {
    u32 bgmId = proc->m_bgmIds[index];
    if (proc->m_category == muProcOptSong::Category_MyMusic) {
        if (index < proc->m_myMusicCount) {
            lbl_805A01D8->setRate(muProcOptSong::Category_MyMusic, bgmId, rate);
        } else {
            lbl_805A01D8->setRate(muProcOptSong::Category_MyMusic2, bgmId, rate);
        }
    } else {
        lbl_805A01D8->setRate(proc->m_category, bgmId, rate);
    }
}

// The icon of the rate of the line under the cursor: full, never, or in between.
static inline void setRateIcon(muProcOptSong* proc, u8 rate) {
    if (rate == 100) {
        proc->setAnimFrame(muProcOptSong::Obj_Row0 + proc->m_row, 1.0f, 2);
    } else if (rate == 0) {
        proc->setAnimFrame(muProcOptSong::Obj_Row0 + proc->m_row, 2.0f, 2);
    } else {
        proc->setAnimFrame(muProcOptSong::Obj_Row0 + proc->m_row, 3.0f, 2);
    }
}

// The icon of the pad shows the buttons of the type of the pad, in their play or stop form.
static inline void setPadIcon(muProcOptSong* proc, bool playing) {
    gfPadStatus status;
    g_gfPadSystem->getSysPadStatus(proc->m_selStage->m_padNo, &status);
    proc->m_padType = status.m_controllerType;
    float frame = playing ? 1.0f : 0.0f;
    switch (proc->m_padType) {
    case gfPadType::WII_CLASSIC:
        frame += 4.0f;
        break;
    case gfPadType::WIIMOTE:
        frame += 6.0f;
        break;
    case gfPadType::NUNCHUK:
        frame += 2.0f;
        break;
    }
    proc->m_objs[muProcOptSong::Obj_Scroll]->setFrameTex(frame);
}

// The songs of a category that can be played.
// HYPOTHESIS: two inline helpers of the class (proc expands the same loops four times).
inline u32 muProcOptSong::countUsableSongs(int category) {
    u32 num = getCategoryCount(lbl_805A01D8, category);
    u32 count = 0;
    for (u32 i = 0; i < num; i++) {
        if (gmCheckBGMIDUseEnable(lbl_805A01D8->getBgmId(category, i)) == true) {
            count++;
        }
    }
    return count;
}

inline void muProcOptSong::addUsableSongs(int category) {
    u32 num = getCategoryCount(lbl_805A01D8, category);
    for (u32 i = 0; i < num; i++) {
        u32 bgmId = lbl_805A01D8->getBgmId(category, i);
        if (gmCheckBGMIDUseEnable(bgmId)) {
            m_bgmIds[m_bgmCount] = bgmId;
            m_bgmCount++;
        }
    }
}

muProcOptSong::muProcOptSong(MuObject** objs, nw4r::g3d::G3dObj** mdls, void* selStageMsgData, void* msgData)
    : muProcMenu(objs, mdls) {
    m_msg = MuMsg::create(1, Heaps::MenuInstance, 0x2B);
    m_msg->allocMsgBuf(0x100, RowCount);
    m_msg->setMsgData(msgData);
    for (int i = 0; i < RowCount; i++) {
        m_msg->attachScnMdlSimple(i, reinterpret_cast<nw4r::g3d::ScnMdlSimple*>(m_objs[Obj_Row0 + i]->m_sceneModel), 0,
                                  1.0f / 16.0f);
        m_msg->setAlignMode(i, MuMsg::Align_Left);
        m_msg->setFontColor(i, 0x80, 0x80, 0x80, 0xFF);
        m_msg->setFontWidthModeAuto(i);
    }
    m_selStageMsgData = selStageMsgData;
}

void muProcOptSong::init(nw4r::g3d::ScnGroup* group, int from) {
    m_group = group;
    m_unk664 = Step_SelectStage;
    m_cursor = 0;
    m_selStage = NULL;
    muSelectStageTask::Param param;
    param.m_mode = 2;
    param.m_group = group;
    param.m_unk18 = 0x20;
    param.m_msgData = m_selStageMsgData;
    param.m_unk30 = NULL;
    param.m_unk34 = NULL;
    param.m_data1 = NULL;
    m_selStage = muSelectStageTask::create(&param);
    m_panel->changeProcess1(0x19, 0x46);
    m_objs[Obj_Scroll]->changeAnimN(0, 0x17, false, false);
    m_objs[Obj_Scroll]->setAnimIdx(&sSongScrollAnim, false);
    lbl_805A01D8->setupBGMList();
}

int muProcOptSong::proc() {
    switch (m_unk664) {
    case Step_SelectStage: {
        muSelectStageTask* selStage = m_selStage;
        if (selStage->getDecision() == 1) {
            int kind = selStage->m_selKind;
            if (kind == 2) {
                m_category = Category_Custom;
            } else {
                int stage;
                if (kind == 0) {
                    // HYPOTHESIS: this reads one past the end of the table, as the original does.
                    stage = StageCount;
                } else if (kind == 1) {
                    stage = selStage->m_selStage;
                } else {
                    stage = 0;
                }
                StageCategories categories = sStageCategories;
                m_category = categories.m_category[stage];
            }
            m_cursor = 0;
            m_scroll = 0;
            m_row = 0;
            m_unk664 = Step_Come;
            memset(m_bgmIds, 0, sizeof(m_bgmIds));
            m_bgmCount = 0;
            if (m_category == Category_MyMusic) {
                m_myMusicCount = countUsableSongs(Category_MyMusic);
                m_myMusicCount2 = countUsableSongs(Category_MyMusic2);
                m_categoryCount = 0;
                m_categoryCount += getCategoryCount(lbl_805A01D8, Category_MyMusic);
                m_categoryCount += getCategoryCount(lbl_805A01D8, Category_MyMusic2);
                addUsableSongs(Category_MyMusic);
                addUsableSongs(Category_MyMusic2);
            } else {
                m_categoryCount = getCategoryCount(lbl_805A01D8, m_category);
                addUsableSongs(m_category);
            }
            if (m_category == Category_MyMusic) {
                addDrawPriority(m_objs[Obj_Row0], 2);
            }
            u32 count = m_bgmCount;
            if (count > RowCount) {
                count = RowCount;
            }
            setGageList(count, 0, 1);
            if (m_bgmCount <= RowCount) {
                setAnimFrame(Obj_Scroll, 0.0f, 2);
            } else {
                setAnimFrame(Obj_Scroll, 1.0f, 2);
            }
            setAnimFrame(Obj_Scroll, 0.0f, 1);
            g_sndSystem->stopBGM(0);
            m_isPlaying = false;
            m_playingBgmId = 0;
            setPadIcon(this, m_isPlaying);
            return 4;
        }
        if (selStage->m_step == 1) {
            m_result = 5;
            m_unk664 = Step_Exit;
            return 1;
        }
        break;
    }
    case Step_Come:
        if (m_objs[Obj_Cursor0]->m_modelAnim->getFrame() == 9.0f) {
            m_objs[Obj_Cursor0]->setAnimIdx(&sSongCursorAnims[3], true);
            m_unk664 = Step_Select;
        }
        break;
    case Step_Select: {
        bool blink = static_cast<int>(m_objs[Obj_Scroll]->m_modelAnim->m_anmObjTexPatRes->GetFrame()) % 2 != 0;
        setPadIcon(this, blink);
        MuObject* cursor = m_objs[Obj_Cursor0 + m_row];
        if (muGetAnimIdx(cursor) == 4 && cursor->m_modelAnim->getFrame() >= 29.0f) {
            m_objs[Obj_Cursor0 + m_row]->setAnimIdx(&sSongCursorAnims[2], true);
        }
        u32 pad = padUpdate();
        if (pad & Pad_Cancel) {
            if (!m_isPlaying) {
                lbl_805A01D8->playBGM(Category_Custom, 0, 0);
            }
            g_sndSystem->playSE(static_cast<SndID>(2), -1, 0, 0, -1);
            for (u32 i = 0; i < RowCount; i++) {
                m_objs[Obj_Cursor0 + i]->setAnimIdx(&sSongCursorAnims[4], true);
                if (i == m_row) {
                    m_objs[Obj_Cursor0 + i]->setAnimIdx(&sSongCursorAnims[4], true);
                } else {
                    m_objs[Obj_Cursor0 + i]->setAnimIdx(&sSongCursorAnims[5], true);
                }
            }
            m_unk664 = Step_Leave;
            m_selStage->resumeSelect();
        }
        selectProc(pad);
        break;
    }
    case Step_Leave:
        if (m_objs[Obj_Cursor0]->m_modelAnim->getFrame() == 9.0f) {
            for (int i = 0; i < RowCount; i++) {
                m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[Mdl_Row0 + i]));
            }
            m_unk664 = Step_SelectStage;
        }
        break;
    case Step_Exit:
        if (m_selStage->getDecision() == 2) {
            return 3;
        }
        break;
    }
    return 0;
}

void muProcOptSong::exit() {
    m_selStage->exit();
    m_selStage = NULL;
}

bool muProcOptSong::selectProc(int dir) {
    bool up = false;
    bool down = false;
    u32 scroll = m_bgmCount > RowCount;
    if (scroll == 1) {
        up = true;
        down = true;
        if (m_scroll == 0) {
            up = false;
        }
        if (m_bgmCount <= m_scroll + RowCount) {
            down = false;
        }
    }
    // The original tests the same condition twice (frame 0 is set, then overwritten by frame 1), so the arrows never
    // all go away.
    if (up == false && down == true) {
        setAnimFrame(Obj_Scroll, 0.0f, 2);
    }
    if (up == false && down == true) {
        setAnimFrame(Obj_Scroll, 1.0f, 2);
    }
    if (up == true && down == false) {
        setAnimFrame(Obj_Scroll, 2.0f, 2);
    }
    if (up == true && down == true) {
        setAnimFrame(Obj_Scroll, 3.0f, 2);
    }
    u32 bgmId = m_bgmIds[m_cursor];
    u8 rate = getSongRate(this, m_cursor);
    if (dir & Pad_Up) {
        if (m_cursor != 0) {
            g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
            m_cursor--;
            rate = getSongRate(this, m_cursor);
            if (m_row != 0) {
                m_objs[Obj_Cursor0 + m_row]->setAnimIdx(&sSongCursorAnims[1], true);
                m_objs[Obj_Cursor0 + m_row]->m_animIdxData = NULL;
                setAnimFrame(Obj_Cursor0 + m_row, 9.0f, 0x11);
                m_objs[Obj_Row0 + m_row]->m_animIdxData = NULL;
                setAnimFrame(Obj_Row0 + m_row, 0.0f, 0x12);
                setAnimUpdateRate(Obj_Row0 + m_row, 0.0f, 0x12);
                m_row--;
                if (m_category == Category_MyMusic) {
                    addDrawPriority(m_objs[Obj_Row0 + 1 + m_row], -2);
                    addDrawPriority(m_objs[Obj_Row0 + m_row], 2);
                }
                m_objs[Obj_Cursor0 + m_row]->setAnimIdx(&sSongCursorAnims[3], true);
                m_objs[Obj_Row0 + m_row]->setAnimIdx(&sSongRowAnims[2], false);
                setRateIcon(this, rate);
                setAnimFrame(Obj_Scroll, m_row, 1);
            } else {
                m_scroll--;
                setGageList(RowCount, m_scroll, 0);
            }
        }
    } else if (dir & Pad_Down) {
        if (m_cursor < m_bgmCount - 1) {
            g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
            m_cursor++;
            rate = getSongRate(this, m_cursor);
            if (m_row < RowCount - 1) {
                m_objs[Obj_Cursor0 + m_row]->setAnimIdx(&sSongCursorAnims[1], true);
                m_objs[Obj_Cursor0 + m_row]->m_animIdxData = NULL;
                setAnimFrame(Obj_Cursor0 + m_row, 9.0f, 0x11);
                m_objs[Obj_Row0 + m_row]->m_animIdxData = NULL;
                setAnimFrame(Obj_Row0 + m_row, 0.0f, 0x12);
                setAnimUpdateRate(Obj_Row0 + m_row, 0.0f, 0x12);
                m_row++;
                if (m_category == Category_MyMusic) {
                    addDrawPriority(m_objs[Obj_Row0 - 1 + m_row], -2);
                    addDrawPriority(m_objs[Obj_Row0 + m_row], 2);
                }
                m_objs[Obj_Cursor0 + m_row]->setAnimIdx(&sSongCursorAnims[3], true);
                m_objs[Obj_Row0 + m_row]->setAnimIdx(&sSongRowAnims[2], false);
                setRateIcon(this, rate);
                setAnimFrame(Obj_Scroll, m_row, 1);
            } else {
                m_scroll++;
                setGageList(RowCount, m_scroll, 0);
            }
        }
    } else if (m_pad.m_buttonsCurrentFrame2.bits & 1) {
        // HYPOTHESIS: bit 0 of the held buttons is left (the song plays less often), bit 1 right
        if (rate != 0) {
            rate--;
            setAnimFrame(Obj_Row0 + m_row, 100.0f - rate, 9);
            setSongRate(this, m_cursor, rate);
            gmSetAutosaveDirtyFlag();
            if (rate == 0) {
                setAnimFrame(Obj_Row0 + m_row, 2.0f, 2);
            } else {
                setAnimFrame(Obj_Row0 + m_row, 3.0f, 2);
            }
            if (rate % 3 == 0) {
                g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
            }
        }
    } else if (m_pad.m_buttonsCurrentFrame2.bits & 2) {
        if (rate < 100) {
            rate++;
            setAnimFrame(Obj_Row0 + m_row, 100.0f - rate, 9);
            setSongRate(this, m_cursor, rate);
            gmSetAutosaveDirtyFlag();
            if (rate == 100) {
                setAnimFrame(Obj_Row0 + m_row, 1.0f, 2);
            } else {
                setAnimFrame(Obj_Row0 + m_row, 3.0f, 2);
            }
            if (rate % 3 == 0) {
                g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
            }
        }
    } else if (dir & Pad_A) {
        if (m_isPlaying) {
            g_sndSystem->stopBGM(0);
            m_isPlaying = false;
        }
        if (bgmId != m_playingBgmId) {
            m_playingBgmId = bgmId;
            g_sndSystem->playBGM(bgmId, 0, true);
            m_isPlaying = true;
        } else {
            m_playingBgmId = 0;
        }
    } else if (dir & Pad_Start) {
        g_sndSystem->stopBGM(60);
        m_isPlaying = false;
        m_playingBgmId = 0;
    }
    setPadIcon(this, m_bgmIds[m_cursor] == m_playingBgmId);
    return true;
}

void muProcOptSong::playSE_CursorMove() {
    g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
}

void muProcOptSong::setGageList(u32 count, u32 start, int come) {
    setRateIcon(this, getSongRate(this, m_cursor));
    for (u32 i = 0; i < count; i++) {
        u8 rate = getSongRate(this, start + i);
        setAnimFrame(Obj_Row0 + i, 100.0f - rate, 9);
        if (come) {
            m_group->Insert(m_group->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[Mdl_Row0 + i]));
        }
        if (come) {
            if (i == 0) {
                m_objs[Obj_Cursor0 + i]->setAnimIdx(&sSongCursorAnims[0], true);
                m_objs[Obj_Row0 + i]->setAnimIdx(&sSongRowAnims[2], false);
            } else {
                m_objs[Obj_Cursor0 + i]->setAnimIdx(&sSongCursorAnims[1], true);
                setAnimFrame(Obj_Row0 + i, 0.0f, 0x12);
                setAnimUpdateRate(Obj_Row0 + i, 0.0f, 0x12);
            }
        }
        m_msg->printIndex(i, lbl_805A01D8->getBgmTitleDataPlayId(m_bgmIds[start + i])->m_msgIndex, NULL);
        if (m_category == Category_MyMusic) {
            if (start + i < m_myMusicCount) {
                m_objs[Obj_Row0 + i]->setFrameTex(1.0f);
            } else {
                m_objs[Obj_Row0 + i]->setFrameTex(2.0f);
            }
        } else {
            m_objs[Obj_Row0 + i]->setFrameTex(0.0f);
        }
    }
}
