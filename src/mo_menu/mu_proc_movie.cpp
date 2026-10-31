#include <gf/gf_heap_manager.h>
#include <gf/gf_scene.h>
#include <gm/gm_common.h>
#include <gm/gm_global.h>
#include <mu/mu_proc_movie_player.h>
#include <nw4r/g3d/g3d_scnobj.h>
#include <snd/snd_system.h>
#include <stdio.h>
#include <string.h>
#include <types.h>

#include <mu/mu_proc_movie.h>

// How many icons the first and the second row have, for a page of 1 ... 6 icons.
static const u8 sMovieRows[6][2] = {
    { 1, 0 },
    { 2, 0 },
    { 3, 0 },
    { 2, 2 },
    { 3, 2 },
    { 3, 3 },
};

// The movie icons: the title of the icon and the help that is shown while the cursor is on it.
struct MovieText {
    u16 m_title;
    u16 m_help;
};
static const MovieText sMovieTexts[muProcMovie::Id_Count] = {
    { 0, 6 },
    { 1, 7 },
    { 2, 8 },
    { 3, 9 },
    { 4, 10 },
};

// The animations of the icons: [0] gone, [1] comes in, [2] waits, [3] goes out.
static MuAnimIdxData sMovieAnims[4] = {
    { 0.0f, 0.0f, 0, 0, 0x11 },
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

// Whether any of the movies of the Adventure Mode is on the screen.
static inline bool isAnyAdvMovieOpen(const u32* flag) {
    for (int id = muProcMovie::Id_FirstAdv; id < muProcMovie::Id_Adv; id++) {
        if (isMovieOpen(flag, id)) {
            return true;
        }
    }
    return false;
}

// The icons of the page (the last page may have less than six).
static inline int getPageNum(muProcMovie* proc) {
    int num = proc->m_count - proc->m_page * muProcMovie::Slots;
    if (num > muProcMovie::Slots) {
        num = muProcMovie::Slots;
    }
    return num;
}

// How many rows the page has.
static inline int getRowNum(muProcMovie* proc) {
    const u8* rows = sMovieRows[getPageNum(proc) - 1];
    int num = 0;
    if (rows[0] != 0) {
        num = 1;
    }
    if (rows[1] != 0) {
        num++;
    }
    return num;
}

// The number of the icon in the page that the cursor is on: the icons of the rows before it, then the column.
static inline u8 getCursorNo(muProcMovie* proc, u8 row, u8 col) {
    int no = 0;
    for (int i = 0; i < row; i++) {
        no += sMovieRows[getPageNum(proc) - 1][i];
    }
    return no + col;
}

// Sends all of the icons to an animation.
static inline void setAnims(muProcMovie* proc, MuAnimIdxData* anim) {
    for (int i = 0; i < muProcMovie::Slots; i++) {
        proc->m_objs[muProcMovie::Obj_Slot + i]->setAnimIdx(anim, false);
    }
    proc->m_objs[muProcMovie::Obj_Adv]->setAnimIdx(anim, false);
}

static inline nw4r::g3d::ScnMdlExpand* getSlotGroup(muProcMovie* proc) {
    return reinterpret_cast<nw4r::g3d::ScnMdlExpand*>(proc->m_mdls[muProcMovie::Mdl_Slots]);
}

// Takes all of the icons out of the group, last first.
static inline void removeAllIcons(nw4r::g3d::ScnGroup* slots) {
    while (slots->sceneItemsCount != 0) {
        int last = slots->sceneItemsCount;
        if (last != 0) {
            slots->Remove(static_cast<u32>(last - 1));
        }
    }
}

void muProcMovie::init(nw4r::g3d::ScnGroup* group, int from) {
    m_group = group;
    m_animState = Anim_Out;
    m_list = NULL;
    m_count = 0;
    m_unk66C_0 = 0;
    m_helpShown = 0;
    group->Insert(group->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[Mdl_Slots]));
    m_msg = MuMsg::create(1, Heaps::MenuInstance, Msg_Heap);
    m_msg->allocMsgBuf(Msg_Buf, Msg_Count);
    m_msg->setMsgData(m_msgData);
    for (int i = 0; i < Slots; i++) {
        m_msg->attachScnMdlSimple(i, reinterpret_cast<nw4r::g3d::ScnMdlSimple*>(m_objs[Obj_Slot + i]->m_sceneModel), 0, 0.0625f);
    }
    m_msg->attachScnMdlSimple(Slots, reinterpret_cast<nw4r::g3d::ScnMdlSimple*>(m_objs[Obj_Adv]->m_sceneModel), 0, 0.0625f);
    m_objs[Obj_Cursor]->m_modelAnim->setUpdateRate(1.0f);
    makeMovieList();
    dispPage(m_page);
    setAnims(this, &sMovieAnims[1]);
    m_animState = Anim_Coming;
    if (static_cast<u32>(from) == Code_Back) {
        m_panel->changeProcess1(0x1E, 0x25);
        m_unk664 = 0;
    } else {
        m_unk664 = 1;
    }
}

int muProcMovie::proc() {
    int result = 0;
    switch (m_unk664) {
    case 0:
    case 1: {
        if (m_animState == Anim_Shown) {
            result = 4;
            m_unk664 = 2;
        } else {
            u32 pad = padUpdate();
            if (moveCursorMain(pad)) {
                m_unk664 = 2;
                result = 4;
                g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
            }
            if (pad & Pad_Ok) {
                result = 4;
                m_unk664 = 2;
            }
            if (pad & Pad_Cancel) {
                getSlotGroup(this)->Remove(m_objs[Obj_Cursor]->m_sceneModel);
                setAnims(this, &sMovieAnims[3]);
                m_animState = Anim_Going;
                result = 1;
                m_result = Code_Back;
                m_unk664 = 3;
            }
        }
        break;
    }
    case 2: {
        if (!m_helpShown) {
            u8 row = m_row;
            u8 col = m_col;
            nw4r::g3d::ScnObj* cursor = m_objs[Obj_Cursor]->m_sceneModel;
            getSlotGroup(this)->Remove(cursor);
            char name[16];
            sprintf(name, "pos%02d", getCursorNo(this, row, col));
            getSlotGroup(this)->PushBack(cursor, name);
            int id = m_list[m_page * Slots + getCursorNo(this, row, col)];
            m_panel->setMsg(id == Id_Adv ? Msg_AdvHelp : sMovieTexts[id].m_help, 1, reinterpret_cast<int>(m_msg->getBinary()));
            m_helpShown = 1;
        }
        u32 pad = padUpdate();
        moveCursorMain(pad);
        if (pad & Pad_Ok) {
            getSlotGroup(this)->Remove(m_objs[Obj_Cursor]->m_sceneModel);
            setAnims(this, &sMovieAnims[3]);
            m_animState = Anim_Going;
            int id = m_list[m_page * Slots + getCursorNo(this, m_row, m_col)];
            if (id == Id_Adv) {
                m_result = Code_Adv;
            } else {
                g_sndSystem->playSE(static_cast<SndID>(0x13), -1, 0, 0, -1);
                muMenuMain* menu = dynamic_cast<muMenuMain*>(gfSceneManager::getInstance()->m_currentScene);
                dynamic_cast<muProcMoviePlayer*>(menu->m_proc)->m_movieId = id;
                m_result = Code_Player;
            }
            result = 2;
            m_unk664 = 3;
        } else if (pad & Pad_Cancel) {
            getSlotGroup(this)->Remove(m_objs[Obj_Cursor]->m_sceneModel);
            setAnims(this, &sMovieAnims[3]);
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
        if (m_objs[Obj_Slot]->m_modelAnim->m_anmObjChrRes->GetUpdateRate() == 0.0f) {
            setAnims(this, &sMovieAnims[2]);
            m_animState = Anim_Shown;
        }
        break;
    case Anim_Shown:
        break;
    case Anim_Going:
        if (m_objs[Obj_Slot]->m_modelAnim->m_anmObjChrRes->GetUpdateRate() == 0.0f) {
            setAnims(this, &sMovieAnims[0]);
            m_animState = Anim_Out;
        }
        break;
    }
    return result;
}

void muProcMovie::exit() {
    nw4r::g3d::ScnGroup* slots = reinterpret_cast<nw4r::g3d::ScnGroup*>(m_mdls[Mdl_Slots]);
    removeAllIcons(slots);
    m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[Mdl_Slots]));
    if (m_list != NULL) {
        gfHeapManager::free(m_list);
    }
    for (int i = 0; i < Slots; i++) {
        m_msg->detachMuObject(m_objs[Obj_Slot + i]);
    }
    m_msg->detachMuObject(m_objs[Obj_Adv]);
    delete m_msg;
}

// Makes the list of the movies to show: the Adventure Mode icon if any of its movies is unlocked, then the unlocked movies.
void muProcMovie::makeMovieList() {
    int list[Slots];
    int count = 0;
    const u32* flag = reinterpret_cast<const u32*>(g_GameGlobal->getMovieAppFlag());
    bool anyAdv = isAnyAdvMovieOpen(flag);
    if (anyAdv) {
        list[0] = Id_Adv;
        count = 1;
    }
    flag = reinterpret_cast<const u32*>(g_GameGlobal->getMovieAppFlag());
    for (int id = 0; id < Id_Count; id++) {
        if (isMovieOpen(flag, id)) {
            list[count++] = id;
        }
    }
    u32 size = count * sizeof(int);
    m_list = static_cast<int*>(gfHeapManager::alloc(Heaps::MenuInstance, size));
    memcpy(m_list, list, size);
    m_count = count;
}

// Writes the icons and the titles of a page, and the page markers.
void muProcMovie::dispPage(int page) {
    int first = page * Slots;
    int num = getPageNum(this);
    nw4r::g3d::ScnGroup* slots = getSlotGroup(this);
    removeAllIcons(slots);
    int i = 0;
    int* movie = &m_list[first];
    u32 slot = 0;
    for (; i < num; i++, movie++) {
        nw4r::g3d::ScnObj* icon;
        u32 line;
        u32 text;
        if (*movie != Id_Adv) {
            int obj = Obj_Slot + slot;
            m_objs[obj]->setFrameTex(*movie + 1);
            m_objs[obj]->m_modelAnim->m_anmObjTexPatRes->SetUpdateRate(0.0f);
            icon = m_objs[obj]->m_sceneModel;
            text = sMovieTexts[*movie].m_title;
            line = slot;
            slot++;
        } else {
            line = Slots;
            text = Msg_AdvTitle;
            icon = m_objs[Obj_Adv]->m_sceneModel;
        }
        char name[8];
        sprintf(name, "pos%02d", i);
        getSlotGroup(this)->PushBack(icon, name);
        m_msg->printIndex(line, text, NULL);
    }
    m_objs[Obj_Page]->setFrameNode(num);
    m_objs[Obj_Page]->m_modelAnim->m_anmObjChrRes->SetUpdateRate(0.0f);
    int arrows = 0;
    if (page != 0) {
        arrows |= 1;
    }
    if (page < (m_count + Slots - 1) / Slots - 1) {
        arrows |= 2;
    }
    m_objs[Obj_Page]->setFrameVisible(arrows);
    m_objs[Obj_Page]->m_modelAnim->m_anmObjVisRes->SetUpdateRate(0.0f);
}

// Moves the cursor in the page, and to the next or the previous page at the ends of a row. Returns whether it moved.
int muProcMovie::moveCursorMain(u32 pad) {
    int moved = 0;
    if ((pad & Pad_Up) && m_row != 0) {
        u8 row = m_row - 1;
        m_row = row;
        int len = sMovieRows[getPageNum(this) - 1][row];
        if (m_col >= len) {
            m_col = len - 1;
        }
        moved = 1;
    }
    if (pad & Pad_Down) {
        if (m_row < getRowNum(this) - 1) {
            u8 row = m_row + 1;
            m_row = row;
            int len = sMovieRows[getPageNum(this) - 1][row];
            if (m_col >= len) {
                m_col = len - 1;
            }
            moved = 1;
        }
    }
    if (pad & Pad_Left) {
        if (m_col != 0) {
            m_col--;
            moved = 1;
        } else if (m_page != 0) {
            m_page--;
            dispPage(m_page);
            int rows = getRowNum(this);
            if (m_row >= rows) {
                m_row = rows - 1;
            }
            moved = 1;
            m_col = sMovieRows[getPageNum(this) - 1][m_row] - 1;
        }
    }
    if (pad & Pad_Right) {
        if (m_col < sMovieRows[getPageNum(this) - 1][m_row] - 1) {
            m_col++;
            moved = 1;
        } else if (m_page < (m_count + Slots - 1) / Slots - 1) {
            m_page++;
            dispPage(m_page);
            int rows = getRowNum(this);
            if (m_row >= rows) {
                m_row = rows - 1;
            }
            moved = 1;
            m_col = 0;
        }
    }
    if (moved) {
        nw4r::g3d::ScnObj* cursor = m_objs[Obj_Cursor]->m_sceneModel;
        getSlotGroup(this)->Remove(cursor);
        u8 row = m_row;
        u8 col = m_col;
        char name[16];
        sprintf(name, "pos%02d", getCursorNo(this, row, col));
        getSlotGroup(this)->PushBack(cursor, name);
        int id = m_list[m_page * Slots + getCursorNo(this, row, col)];
        m_panel->setMsg(id == Id_Adv ? Msg_AdvHelp : sMovieTexts[id].m_help, 1, reinterpret_cast<int>(m_msg->getBinary()));
        g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
    }
    return moved;
}

void muProcMovie::playSE_CursorMove() {
    g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
}
