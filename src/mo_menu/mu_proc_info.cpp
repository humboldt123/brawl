#include <gf/gf_heap_manager.h>
#include <gm/gm_common.h>
#include <gm/gm_global.h>
#include <nw4r/g3d/g3d_scnobj.h>
#include <snd/snd_system.h>
#include <string.h>
#include <types.h>

#include <mu/mu_proc_info.h>

// MSL's qsort: the stdlib header of this project does not declare it
extern "C" void qsort(void* base, unsigned long count, unsigned long size, int (*compare)(const void*, const void*));

// muMenu helpers of main.dol whose symbols take three ints; the callers set only the first (as the matched units do)
extern "C" int getInfoMsgID__6muMenuFiii(int info);

// The animations of the window and of the arrows: [0] comes in, [1] goes out.
static MuAnimIdxData sInfoPanelAnims[2] = {
    { 0.0f, 19.0f, 0, 0, 0x9 },
    { 20.0f, 39.0f, 0, 0, 0x9 },
};
static MuAnimIdxData sInfoArrowAnims[2] = {
    { 0.0f, 9.0f, 0, 0, 0x1 },
    { 10.0f, 19.0f, 0, 0, 0x1 },
};

// The notice has been received: the second bit of its two-bit state in the Info flag block.
static inline bool isInfoReceived(const gmGlobalRecord::InfoAppFlagData* flag, int info) {
    int bit = info * 2 + 1;
    return reinterpret_cast<const u32*>(flag)[bit / 32] & (1 << (bit % 32));
}

void muProcInfo::init(nw4r::g3d::ScnGroup* group, int from) {
    m_panelState = Panel_None;
    m_group = group;
    m_arrowState = Arrow_Init;
    m_list = NULL;
    m_listCount = 0;
    m_row = 0;
    m_top = 0;
    m_unk66C = 0;
    m_panel->changeProcess1(0x22, 0);
    group->Insert(group->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[Obj_Panel]->m_sceneModel));
    group->Insert(group->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[Mdl_Back]));
    m_unk664 = 0;
    m_objs[Obj_Panel]->setFrameTex(8.0f);
    m_objs[Obj_Panel]->m_modelAnim->m_anmObjTexPatRes->SetUpdateRate(0.0f);
    setPanelState(Panel_Opening);
    if (m_arrowState != Arrow_None) {
        m_objs[Obj_Arrow]->setFrameMatCol(0.0f);
        m_objs[Obj_Arrow]->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(0.0f);
        m_arrowState = Arrow_None;
    }
    initMsg();
    makeList();
    dispPage(m_top);
    if (m_listCount > 0) {
        setCursorRow(m_row);
    }
}

void muProcInfo::initMsg() {
    m_msg = MuMsg::create(1, Heaps::MenuInstance, Msg_Heap);
    m_msg->allocMsgBuf(Msg_Buf, Msg_Count);
    m_msg->setMsgData(m_msgData);
    u32 line = 0;
    for (int i = 0; i < Rows; i++) {
        u32 dateLine = line + 1;
        m_msg->attachScnMdlSimple(line, reinterpret_cast<nw4r::g3d::ScnMdlSimple*>(m_objs[Obj_Row + i]->m_sceneModel), 0, 0.0625f);
        m_msg->setAlignMode(line, MuMsg::Align_Left);
        m_msg->setFontColor(line, 0xF0, 0xF0, 0xF0, 0xFF);
        m_msg->getWScale(line, 48.0f);
        m_msg->attachScnMdlSimple(dateLine, reinterpret_cast<nw4r::g3d::ScnMdlSimple*>(m_objs[Obj_Row + i]->m_sceneModel), 1, 0.04761905f);
        m_msg->setAlignMode(dateLine, MuMsg::Align_Right);
        m_msg->setFontColor(dateLine, 0xD7, 0xD9, 0xE5, 0xFF);
        m_msg->setFontWidthModeFixed(dateLine, 12.0f);
        m_msg->setHSpace(dateLine, 11.0f);
        line += 2;
    }
}

// Writes the notices of the page that starts at top, and shows the arrows of the pages before and after it.
void muProcInfo::dispPage(int top) {
    gmGlobalRecord::InfoAppFlagData* flag = g_GameGlobal->getInfoAppFlag();
    u32 line = 0;
    u32 dateLine = 1;
    for (int i = 0, idx = top; i < Rows && idx < m_listCount; i++, idx++) {
        int info = m_list[idx];
        m_msg->printIndex(line, getInfoMsgID__6muMenuFiii(info), NULL);
        OSCalendarTime time;
        gmGetCalendarTime(&time, flag->m_recvTime[info]);
        if (g_GameGlobal->getLanguage() == 0) {
            m_msg->printf(dateLine, "%4d/%2d/%2d\n%2d:%02d:%02d", time.year, time.month + 1, time.mday, time.hour, time.min, time.sec);
        } else {
            m_msg->printf(dateLine, "%2d/%2d/%4d\n%2d:%02d:%02d", time.month + 1, time.mday, time.year, time.hour, time.min, time.sec);
        }
        line += 2;
        dateLine += 2;
    }

    // Frame 1 shows the down arrow, frame 2 the up arrow, frame 3 both
    u8 arrows = 0;
    if (top > 0) {
        arrows |= 2;
    }
    if (top + Rows < m_listCount) {
        arrows |= 1;
    }
    m_objs[Obj_Arrow]->setFrameVisible(arrows);
    m_objs[Obj_Arrow]->m_modelAnim->m_anmObjVisRes->SetUpdateRate(0.0f);
}

void muProcInfo::setPanelState(int state) {
    if (state != m_panelState) {
        MuObject* arrow;
        MuObject* panel;
        switch (state) {
        case Panel_Closed:
            arrow = m_objs[Obj_Arrow];
            arrow->m_animIdxData = NULL;
            m_objs[Obj_Arrow]->setFrameNode(19.0f);
            m_objs[Obj_Arrow]->m_modelAnim->m_anmObjChrRes->SetUpdateRate(0.0f);
            panel = m_objs[Obj_Panel];
            panel->m_animIdxData = NULL;
            m_objs[Obj_Panel]->setFrameNode(39.0f);
            m_objs[Obj_Panel]->m_modelAnim->m_anmObjChrRes->SetUpdateRate(0.0f);
            m_objs[Obj_Panel]->setFrameTexSrt(39.0f);
            m_objs[Obj_Panel]->m_modelAnim->m_anmObjTexSrtRes->SetUpdateRate(0.0f);
            break;
        case Panel_Opening:
            m_objs[Obj_Arrow]->setAnimIdx(&sInfoArrowAnims[0], false);
            m_objs[Obj_Panel]->setAnimIdx(&sInfoPanelAnims[0], false);
            break;
        case Panel_Open:
            arrow = m_objs[Obj_Arrow];
            arrow->m_animIdxData = NULL;
            m_objs[Obj_Arrow]->setFrameNode(9.0f);
            m_objs[Obj_Arrow]->m_modelAnim->m_anmObjChrRes->SetUpdateRate(0.0f);
            panel = m_objs[Obj_Panel];
            panel->m_animIdxData = NULL;
            m_objs[Obj_Panel]->setFrameNode(19.0f);
            m_objs[Obj_Panel]->m_modelAnim->m_anmObjChrRes->SetUpdateRate(0.0f);
            m_objs[Obj_Panel]->setFrameTexSrt(19.0f);
            m_objs[Obj_Panel]->m_modelAnim->m_anmObjTexSrtRes->SetUpdateRate(0.0f);
            break;
        case Panel_Closing:
            m_objs[Obj_Arrow]->setAnimIdx(&sInfoArrowAnims[1], false);
            m_objs[Obj_Panel]->setAnimIdx(&sInfoPanelAnims[1], false);
            break;
        }
        m_panelState = state;
    }
}

void muProcInfo::exit() {
    m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[Obj_Panel]->m_sceneModel));
    m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[Mdl_Back]));
    if (m_list != NULL) {
        gfHeapManager::free(m_list);
        m_list = NULL;
    }
    m_listCount = 0;
    for (int i = 0; i < Rows; i++) {
        m_msg->detachMuObject(m_objs[Obj_Row + i]);
    }
    if (m_msg != NULL) {
        delete m_msg;
        m_msg = NULL;
    }
}

// Moves the cursor up or down a row, or scrolls the page at its ends; the arrow that was used flashes.
int muProcInfo::moveCursor(u32 pad) {
    int moved = 0;
    bool up = false;
    if (pad & Pad_Up) {
        if (m_row > 0) {
            m_row--;
            setCursorRow(m_row);
            up = true;
        } else if (m_top > 0) {
            m_top--;
            dispPage(m_top);
            up = true;
        }
    }
    if (up) {
        if (m_arrowState != Arrow_Up) {
            m_objs[Obj_Arrow]->setFrameMatCol(1.0f);
            m_objs[Obj_Arrow]->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(1.0f);
            m_arrowState = Arrow_Up;
        }
        moved = 1;
    }

    bool down = false;
    if (pad & Pad_Down) {
        if (m_top + m_row < m_listCount - 1) {
            if (m_row < Rows - 1) {
                m_row++;
                setCursorRow(m_row);
                down = true;
            } else if (m_top < m_listCount - Rows) {
                m_top++;
                dispPage(m_top);
                down = true;
            }
        }
    }
    if (down) {
        if (m_arrowState != Arrow_Down) {
            m_objs[Obj_Arrow]->setFrameMatCol(6.0f);
            m_objs[Obj_Arrow]->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(1.0f);
            m_arrowState = Arrow_Down;
        }
        moved = 1;
    }

    if (moved) {
        g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
    }
    return moved;
}

int muProcInfo::proc() {
    int result = 0;
    switch (m_unk664) {
    case 0: {
        // The window comes in; the cursor can move as soon as it is open
        if (m_panelState != Panel_Opening) {
            m_unk664 = 2;
        }
        u32 pad = padUpdate();
        if (moveCursor(pad)) {
            setPanelState(Panel_Open);
            result = 4;
            m_unk664 = 2;
        }
        if (pad & Pad_Cancel) {
            setPanelState(Panel_Closing);
            m_unk664 = 3;
            result = 1;
            m_result = Code_Back;
        }
        break;
    }
    case 1:
        if (m_panelState != Panel_Opening) {
            m_unk664 = 2;
        }
        break;
    case 2: {
        u32 pad = padUpdate();
        moveCursor(pad);
        if (pad & Pad_Cancel) {
            setPanelState(Panel_Closing);
            m_result = Code_Back;
            result = 1;
            m_unk664 = 3;
        }
        break;
    }
    case 3:
        // The window goes out
        if (m_panelState == Panel_Closed) {
            result = 3;
        }
        break;
    }

    switch (m_panelState) {
    case Panel_Closed:
        break;
    case Panel_Opening:
        if (m_objs[Obj_Arrow]->m_modelAnim->m_anmObjChrRes->GetFrame() >= 9.0f &&
            m_objs[Obj_Panel]->m_modelAnim->m_anmObjChrRes->GetFrame() >= 19.0f) {
            setPanelState(Panel_Open);
        }
        break;
    case Panel_Open:
        break;
    case Panel_Closing:
        if (m_objs[Obj_Arrow]->m_modelAnim->m_anmObjChrRes->GetFrame() >= 19.0f &&
            m_objs[Obj_Panel]->m_modelAnim->m_anmObjChrRes->GetFrame() >= 39.0f) {
            setPanelState(Panel_Closed);
        }
        break;
    }

    // The flash of the arrow that was pressed ends
    switch (m_arrowState) {
    case Arrow_None:
        break;
    case Arrow_Up:
        if (m_objs[Obj_Arrow]->m_modelAnim->m_anmObjMatClrRes->GetFrame() >= 5.0f && m_arrowState != Arrow_None) {
            m_objs[Obj_Arrow]->setFrameMatCol(0.0f);
            m_objs[Obj_Arrow]->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(0.0f);
            m_arrowState = Arrow_None;
        }
        break;
    case Arrow_Down:
        if (m_objs[Obj_Arrow]->m_modelAnim->m_anmObjMatClrRes->GetFrame() >= 10.0f && m_arrowState != Arrow_None) {
            m_objs[Obj_Arrow]->setFrameMatCol(0.0f);
            m_objs[Obj_Arrow]->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(0.0f);
            m_arrowState = Arrow_None;
        }
        break;
    }
    return result;
}

// Sorts the notices by the date they arrived, the oldest first; notices of the same date keep their number order.
static int compare(const void* a, const void* b) {
    gmGlobalRecord::InfoAppFlagData* flag = g_GameGlobal->getInfoAppFlag();
    u32 timeA = flag->m_recvTime[*static_cast<const int*>(a)];
    u32 timeB = flag->m_recvTime[*static_cast<const int*>(b)];
    if (timeA != timeB) {
        if (timeA < timeB) {
            return -1;
        }
    } else if (*static_cast<const int*>(a) < *static_cast<const int*>(b)) {
        return -1;
    }
    return 1;
}

// Collects the notices that were received into m_list.
void muProcInfo::makeList() {
    int count = 0;
    int list[Info_Max];
    gmGlobalRecord::InfoAppFlagData* flag = g_GameGlobal->getInfoAppFlag();
    int* entry = list;
    for (int i = 0; i < Info_Count; i++) {
        if (isInfoReceived(flag, i)) {
            *entry++ = i;
            count++;
        }
    }
    if (count > 0) {
        qsort(list, count, sizeof(int), compare);
        u32 size = count * sizeof(int);
        m_list = static_cast<int*>(gfHeapManager::calloc(Heaps::MenuInstance, size));
        memcpy(m_list, list, size);
    } else {
        m_list = NULL;
    }
    m_listCount = count;
}

void muProcInfo::playSE_CursorMove() {
    g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
}
