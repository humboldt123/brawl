#include <gf/gf_heap_manager.h>
#include <gf/gf_scene.h>
#include <gm/gm_common.h>
#include <gm/gm_global.h>
#include <mu/mu_proc_movie_player.h>
#include <nw4r/g3d/g3d_scnobj.h>
#include <snd/snd_system.h>
#include <types.h>

#include <mu/mu_proc_movie_adv.h>

// The message of the title of each movie of the Adventure Mode, by movie (the first two titles of the list are the other
// way round, like their pictures).
static const u16 sMovieAdvTexts[muProcMovieAdv::Id_Count] = {
    12, 14, 13, 15, 16, 17, 18, 19, 20, 21, 22, 23,
    24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
    36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
    48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59,
    60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71,
    72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83,
    84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95,
    96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107,
    108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119,
    120, 121, 122, 123, 124, 125, 126, 127,
};

// The animations of the list: [0] gone, [1] comes in, [2] waits, [3] goes out.
static MuAnimIdxData sMovieAdvAnims[4] = {
    { 19.0f, 19.0f, 0, 0, 0x11 },
    { 0.0f, 9.0f, 0, 0, 0x11 },
    { 9.0f, 9.0f, 0, 0, 0x11 },
    { 10.0f, 19.0f, 0, 0, 0x11 },
};

// The movie is shown on the screen when the flag that hides it is set (or when it is never hidden).
static inline bool isMovieOpen(const u32* flag, int id) {
    int hide = gmRetHideMovieID(static_cast<GmMovieID>(id));
    if (hide != 0x74) {
        return (flag[hide / 32] & (1 << (hide % 32))) != 0;
    }
    return true;
}

// The frame of the picture of the movie that is under the bar (the first two pictures are the other way round).
static inline int getPictureFrame(muProcMovieAdv* proc) {
    int picture = (proc->m_list + proc->m_cursor)[proc->m_top] - muProcMovieAdv::Id_First;
    if (picture == 1) {
        picture = 2;
    } else if (picture == 2) {
        picture = 1;
    }
    return picture + 1;
}

void muProcMovieAdv::init(nw4r::g3d::ScnGroup* group, int from) {
    m_group = group;
    m_animState = Anim_Out;
    m_list = NULL;
    m_count = 0;
    m_introDone = 0;
    group->Insert(group->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[Mdl_List]));
    m_msg = MuMsg::create(4, Heaps::MenuInstance, Msg_Heap);
    m_msg->allocMsgBuf(Msg_Buf, Lines);
    m_msg->setMsgData(m_msgData);
    for (int i = 0; i < Lines; i++) {
        m_msg->attachScnMdlSimple(i, reinterpret_cast<nw4r::g3d::ScnMdlSimple*>(m_objs[Obj_List]->m_sceneModel), i + 1, 0.0625f);
    }
    m_objs[Obj_Bar]->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(1.0f);
    makeMovieList();
    dispPage(m_top);
    m_objs[Obj_Bar]->setFrameNode(m_cursor);
    m_objs[Obj_Bar]->m_modelAnim->m_anmObjChrRes->SetUpdateRate(0.0f);
    m_panel->setMsg(Msg_Help, 1, reinterpret_cast<int>(m_msg->getBinary()));
    m_objs[Obj_List]->setFrameTex(getPictureFrame(this));
    m_objs[Obj_List]->m_modelAnim->m_anmObjTexPatRes->SetUpdateRate(0.0f);
    m_objs[Obj_List]->setAnimIdx(&sMovieAdvAnims[1], false);
    m_animState = Anim_Coming;
    m_unk664 = 0;
}

int muProcMovieAdv::proc() {
    int id;
    int result = 0;
    if (!m_introDone && m_objs[Obj_Intro]->m_modelAnim->m_anmObjChrRes->GetUpdateRate() == 0.0f) {
        m_introDone = 1;
    }
    switch (m_unk664) {
    case 0:
    case 1: {
        if (m_animState == Anim_Shown) {
            result = 4;
            m_unk664 = 2;
        } else {
            u32 pad = padUpdate();
            if (moveCursorMain(pad)) {
                result = 4;
                m_unk664 = 2;
            }
            if (pad & Pad_Ok) {
                result = 4;
                m_unk664 = 2;
            } else if (pad & Pad_Cancel) {
                m_objs[Obj_List]->setAnimIdx(&sMovieAdvAnims[3], false);
                m_animState = Anim_Going;
                result = 1;
                m_result = Code_Back;
                m_unk664 = 3;
            }
        }
        break;
    }
    case 2: {
        u32 pad = padUpdate();
        moveCursorMain(pad);
        if (pad & Pad_Ok) {
            id = (m_list + m_cursor)[m_top];
            muMenuMain* menu = dynamic_cast<muMenuMain*>(gfSceneManager::getInstance()->m_currentScene);
            dynamic_cast<muProcMoviePlayer*>(menu->m_proc)->m_movieId = id;
            m_result = Code_Player;
            g_sndSystem->playSE(static_cast<SndID>(0x13), -1, 0, 0, -1);
            result = 2;
            m_unk664 = 3;
        } else if (pad & Pad_Cancel) {
            m_objs[Obj_List]->setAnimIdx(&sMovieAdvAnims[3], false);
            m_animState = Anim_Going;
            result = 1;
            m_result = Code_Back;
            m_unk664 = 3;
        }
        break;
    }
    case 3:
        if (m_animState == Anim_Out) {
            result = 3;
        }
        break;
    }

    switch (m_animState) {
    case Anim_Out:
        break;
    case Anim_Coming:
        if (m_objs[Obj_List]->m_modelAnim->m_anmObjChrRes->GetUpdateRate() == 0.0f) {
            m_objs[Obj_List]->setAnimIdx(&sMovieAdvAnims[2], false);
            m_animState = Anim_Shown;
        }
        break;
    case Anim_Shown:
        break;
    case Anim_Going:
        if (m_objs[Obj_List]->m_modelAnim->m_anmObjChrRes->GetUpdateRate() == 0.0f) {
            m_objs[Obj_List]->setAnimIdx(&sMovieAdvAnims[0], false);
            m_animState = Anim_Out;
        }
        break;
    }
    return result;
}

void muProcMovieAdv::exit() {
    m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[Mdl_List]));
    if (m_list != NULL) {
        gfHeapManager::free(m_list);
    }
    m_msg->detachMuObject(m_objs[Obj_List]);
    delete m_msg;
}

// Makes the list of the movies of the Adventure Mode that are unlocked.
void muProcMovieAdv::makeMovieList() {
    int count = 0;
    m_list = static_cast<int*>(gfHeapManager::alloc(Heaps::MenuInstance, Id_Count * sizeof(int)));
    const u32* flag = reinterpret_cast<const u32*>(g_GameGlobal->getMovieAppFlag());
    for (int i = 0; i < Id_Count; i++) {
        int id = i + Id_First;
        if (isMovieOpen(flag, id)) {
            m_list[count] = id;
            count++;
        }
    }
    m_count = count;
}

// Writes the titles of the lines from the movie top on, and shows the arrows of the lists above and below.
void muProcMovieAdv::dispPage(u8 top) {
    int line = 0;
    for (int idx = top; line < Lines && idx < m_count; idx++) {
        m_msg->printIndex(line, sMovieAdvTexts[m_list[idx] - Id_First], NULL);
        line++;
    }
    int arrows = 0;
    if (top != 0) {
        arrows |= 1;
    }
    if (top + Lines <= m_count - 1) {
        arrows |= 2;
    }
    m_objs[Obj_List]->setFrameVisible(arrows);
    m_objs[Obj_List]->m_modelAnim->setUpdateRate(0.0f);
}

// Moves the bar a line up or down and scrolls the list at its ends. Returns whether anything moved.
int muProcMovieAdv::moveCursorMain(u32 pad) {
    u32 oldCursor = m_cursor;
    u32 oldTop = m_top;
    if (pad & Pad_Up) {
        if (m_cursor != 0) {
            m_cursor--;
        } else if (m_top != 0) {
            m_top--;
        }
    }
    if (pad & Pad_Down) {
        if (m_cursor < Lines - 1 && m_cursor < m_count - 1) {
            m_cursor++;
        } else if (m_top < m_count - Lines) {
            m_top++;
        }
    }
    if (oldCursor != m_cursor || oldTop != m_top) {
        if (oldCursor != m_cursor) {
            m_objs[Obj_Bar]->setFrameNode(m_cursor);
            m_objs[Obj_Bar]->m_modelAnim->m_anmObjChrRes->SetUpdateRate(0.0f);
        }
        if (oldTop != m_top) {
            dispPage(m_top);
        }
        m_objs[Obj_List]->setFrameTex(getPictureFrame(this));
        m_objs[Obj_List]->m_modelAnim->m_anmObjTexPatRes->SetUpdateRate(0.0f);
        g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
        return 1;
    }
    return 0;
}

void muProcMovieAdv::playSE_CursorMove() {
    g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
}
