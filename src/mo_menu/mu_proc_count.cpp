#include <gm/gm_common.h>
#include <gm/gm_global.h>
#include <nw4r/g3d/g3d_scnobj.h>
#include <snd/snd_system.h>
#include <types.h>

#include <mu/mu_proc_count.h>

// muMenu helpers of main.dol whose symbols take three ints; the callers set only the first (as the matched units do)
extern "C" bool isCharUsable__6muMenuFiii(int muCharKind);
extern "C" int exchangeMuCharKindToMuStockchkind__6muMenuFiii(int muCharKind);
extern "C" int exchangeMuStageKindToGmHideStageKind__6muMenuFiii(int muStageKind);

// MSL's qsort: the stdlib header of this project does not declare it
extern "C" void qsort(void* base, unsigned long count, unsigned long size, int (*compare)(const void*, const void*));

// The lines of the screen, in the order they are shown; the title is the number of the line and the explanation follows
// the titles in the message file.
static const muProcCountItem sCountItems[muProcCount::Item_Count] = {
    { muProcCount::getStartNum, muProcCountItem::Kind_Number, 0, 0x27 },
    { muProcCount::getStartTime, muProcCountItem::Kind_Time, 1, 0x28 },
    { muProcCount::getAllPlayTime, muProcCountItem::Kind_Time, 2, 0x29 },
    { muProcCount::getAdventureAllPlayTime, muProcCountItem::Kind_Time, 3, 0x2A },
    { muProcCount::getAdventureNobePlayTime, muProcCountItem::Kind_Time, 4, 0x2B },
    { muProcCount::getSingleAllPlayTime, muProcCountItem::Kind_Time, 5, 0x2C },
    { muProcCount::getSingleNobePlayTime, muProcCountItem::Kind_Time, 6, 0x2D },
    { muProcCount::getVsAllPlayTime, muProcCountItem::Kind_Time, 7, 0x2E },
    { muProcCount::getVsNobePlayTime, muProcCountItem::Kind_Time, 8, 0x2F },
    { muProcCount::getNumOfVs, muProcCountItem::Kind_Number, 9, 0x30 },
    { muProcCount::getNumOfOfflineVs, muProcCountItem::Kind_Number, 10, 0x31 },
    { muProcCount::getNumOfOnlineVs, muProcCountItem::Kind_Number, 11, 0x32 },
    { muProcCount::getOnlineFriendPlayTime, muProcCountItem::Kind_Time, 12, 0x33 },
    { muProcCount::getOnlineAnyonePlayTime, muProcCountItem::Kind_Time, 13, 0x34 },
    { muProcCount::getNumOfWatchGame, muProcCountItem::Kind_Number, 14, 0x35 },
    { muProcCount::getNumOfTimeVs, muProcCountItem::Kind_Number, 15, 0x36 },
    { muProcCount::getNumOfStockVs, muProcCountItem::Kind_Number, 16, 0x37 },
    { muProcCount::getNumOfCoinVs, muProcCountItem::Kind_Number, 17, 0x38 },
    { muProcCount::getNumOfJoin, muProcCountItem::Kind_Number, 18, 0x39 },
    { muProcCount::getNumOfInterrupt, muProcCountItem::Kind_Number, 19, 0x3A },
    { muProcCount::getTotalDamage, muProcCountItem::Kind_Number, 20, 0x3B },
    { muProcCount::getTotalBeat, muProcCountItem::Kind_Number, 21, 0x3C },
    { muProcCount::getTotalSuicide, muProcCountItem::Kind_Number, 22, 0x3D },
    { muProcCount::getNumOfChara, muProcCountItem::Kind_Number, 23, 0x3E },
    { muProcCount::getNumOfMap, muProcCountItem::Kind_Number, 24, 0x3F },
    { muProcCount::getNumOfFigure, muProcCountItem::Kind_Number, 25, 0x40 },
    { muProcCount::getNumOfSeal, muProcCountItem::Kind_Number, 26, 0x41 },
    { muProcCount::getNumOfName, muProcCountItem::Kind_Number, 27, 0x42 },
    { muProcCount::getNumOfFriend, muProcCountItem::Kind_Number, 28, 0x43 },
    { muProcCount::getNo1UseChara, muProcCountItem::Kind_Chara, 29, 0x44 },
    { muProcCount::getNo2UseChara, muProcCountItem::Kind_Chara, 30, 0x45 },
    { muProcCount::getWorstUseChara, muProcCountItem::Kind_Chara, 31, 0x46 },
    { muProcCount::getNo1ChampChara, muProcCountItem::Kind_Chara, 32, 0x47 },
    { muProcCount::getNo1KetsuChara, muProcCountItem::Kind_Chara, 33, 0x48 },
    { muProcCount::getNo1GiveDamageChara, muProcCountItem::Kind_Chara, 34, 0x49 },
    { muProcCount::getNo1TakeDamageChara, muProcCountItem::Kind_Chara, 35, 0x4A },
    { muProcCount::getBeatKing, muProcCountItem::Kind_Chara, 36, 0x4B },
    { muProcCount::getFallKing, muProcCountItem::Kind_Chara, 37, 0x4C },
    { muProcCount::getSuicideKing, muProcCountItem::Kind_Chara, 38, 0x4D },
};

// The animations of the window and of the arrows: [0] comes in, [1] goes out.
static MuAnimIdxData sCountPanelAnims[2] = {
    { 0.0f, 19.0f, 0, 0, 0x9 },
    { 20.0f, 39.0f, 0, 0, 0x9 },
};
static MuAnimIdxData sCountArrowAnims[2] = {
    { 0.0f, 9.0f, 0, 0, 0x1 },
    { 10.0f, 19.0f, 0, 0, 0x1 },
};

void muProcCount::init(nw4r::g3d::ScnGroup* group, int from) {
    m_panelState = Panel_None;
    m_group = group;
    m_arrowState = Arrow_Init;
    m_row = 0;
    m_top = 0;
    m_unk66C = 0;
    m_panel->changeProcess1(0x22, 0);
    group->Insert(group->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[Obj_Panel]->m_sceneModel));
    group->Insert(group->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[Mdl_Back]));
    m_unk664 = 0;
    m_objs[Obj_Panel]->setFrameTex(2.0f);
    m_objs[Obj_Panel]->m_modelAnim->m_anmObjTexPatRes->SetUpdateRate(0.0f);
    setPanelState(Panel_Opening);
    if (m_arrowState != Arrow_None) {
        m_objs[Obj_Arrow]->setFrameMatCol(0.0f);
        m_objs[Obj_Arrow]->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(0.0f);
        m_arrowState = Arrow_None;
    }
    initMsg();
    makeCharStatistics();
    dispPage(m_top);
    setCursorRow(m_row);
    m_msg->printIndex(Msg_Expl, sCountItems[m_top + m_row].m_explMsg, NULL);
}

void muProcCount::initMsg() {
    m_msg = MuMsg::create(1, Heaps::MenuInstance, Msg_Heap);
    m_msg->allocMsgBuf(Msg_Buf, Msg_Count);
    m_msg->setMsgData(m_msgData);
    u32 line = 0;
    for (int i = 0; i < Rows; i++) {
        u32 valueLine = line + 1;
        m_msg->attachScnMdlSimple(line, reinterpret_cast<nw4r::g3d::ScnMdlSimple*>(m_objs[Obj_Row + i]->m_sceneModel), 0, 0.0625f);
        m_msg->setAlignMode(line, MuMsg::Align_Left);
        m_msg->setWScale(line, 0.8f);
        m_msg->attachScnMdlSimple(valueLine, reinterpret_cast<nw4r::g3d::ScnMdlSimple*>(m_objs[Obj_Row + i]->m_sceneModel), 1, 0.0625f);
        m_msg->setFontColor(valueLine, 0xC4, 0xCE, 0xFF, 0xFF);
        line += 2;
    }
    m_msg->attachScnMdlSimple(Msg_Expl, reinterpret_cast<nw4r::g3d::ScnMdlSimple*>(m_objs[Obj_Arrow]->m_sceneModel), 0, 0.0625f);
}

// Writes the lines of the page that starts at top, and shows the arrows of the pages before and after it.
void muProcCount::dispPage(int top) {
    const muProcCountItem* item = &sCountItems[top];
    u32 line = 0;
    for (int i = 0; i < Rows; i++) {
        u32 valueLine = line + 1;
        m_msg->printIndex(line, item->m_titleMsg, NULL);
        u32 value = item->m_get(this);
        switch (item->m_kind) {
        case muProcCountItem::Kind_Number:
            if (value > Number_Max) {
                value = Number_Max;
            }
            m_msg->setFontWidthModeFixed(valueLine, 12.0f);
            m_msg->setHSpace(valueLine, 9.0f);
            m_msg->setAlignMode(valueLine, MuMsg::Align_Right);
            m_msg->printf(valueLine, "%d", value);
            break;
        case muProcCountItem::Kind_Time:
            if (value > Time_Max) {
                value = Time_Max;
            }
            u32 hours = value / 3600;
            value %= 3600;
            m_msg->setFontWidthModeFixed(valueLine, 12.0f);
            m_msg->setHSpace(valueLine, 9.0f);
            m_msg->setAlignMode(valueLine, MuMsg::Align_Right);
            m_msg->printf(valueLine, "%d:%02d", hours, value / 60);
            break;
        case muProcCountItem::Kind_Chara:
            if (value == Char_None) {
                m_msg->setFontWidthModeFixed(valueLine, 12.0f);
                m_msg->setHSpace(valueLine, 9.0f);
                m_msg->setAlignMode(valueLine, MuMsg::Align_Right);
                m_msg->printf(valueLine, "- ");
            } else {
                const char* name = muMenu::exchangeMuStockchkind2MuCharName(exchangeMuCharKindToMuStockchkind__6muMenuFiii(value));
                m_msg->setFontWidthModeAuto(valueLine);
                m_msg->setAlignMode(valueLine, MuMsg::Align_Right);
                m_msg->setHSpace(valueLine, 0.0f);
                m_msg->printf(valueLine, name);
            }
            break;
        }
        line += 2;
        item++;
    }

    // Frame 1 shows the down arrow, frame 2 the up arrow, frame 3 both
    u8 arrows = 0;
    if (top > 0) {
        arrows |= 2;
    }
    if (top + Rows < Item_Count) {
        arrows |= 1;
    }
    m_objs[Obj_Arrow]->setFrameVisible(arrows);
    m_objs[Obj_Arrow]->m_modelAnim->m_anmObjVisRes->SetUpdateRate(0.0f);
}

void muProcCount::setPanelState(int state) {
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
            m_objs[Obj_Arrow]->setAnimIdx(&sCountArrowAnims[0], false);
            m_objs[Obj_Panel]->setAnimIdx(&sCountPanelAnims[0], false);
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
            m_objs[Obj_Arrow]->setAnimIdx(&sCountArrowAnims[1], false);
            m_objs[Obj_Panel]->setAnimIdx(&sCountPanelAnims[1], false);
            break;
        }
        m_panelState = state;
    }
}

void muProcCount::exit() {
    int i;
    m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_objs[Obj_Panel]->m_sceneModel));
    m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_mdls[Mdl_Back]));
    for (i = 0; i < Rows; i++) {
        m_msg->detachMuObject(m_objs[Obj_Row + i]);
    }
    m_msg->detachMuObject(m_objs[Obj_Arrow]);
    if (m_msg != NULL) {
        delete m_msg;
        m_msg = NULL;
    }
}

// Moves the cursor up or down a row, or scrolls the page at its ends; the arrow that was used flashes and the
// explanation follows the cursor.
int muProcCount::moveCursor(u32 pad) {
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
        if (m_top + m_row < Item_Count - 1) {
            if (m_row < Rows - 1) {
                m_row++;
                setCursorRow(m_row);
                down = true;
            } else if (m_top <= Item_Count - Rows) {
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
        m_msg->printIndex(Msg_Expl, sCountItems[m_top + m_row].m_explMsg, NULL);
        g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
    }
    return moved;
}

int muProcCount::proc() {
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

// The per-character records the characters are ranked by (muProcCount::makeCharRankList).
static u32 getCharUseTime(MuCharKind kind) {
    return g_GameGlobal->getGlobalRecordCharaNumDatap(kind)->m_diagram.m_playTime;
}

static u32 getCharNumOfChamp(MuCharKind kind) {
    return g_GameGlobal->getGlobalRecordCharaNumDatap(kind)->m_diagram.m_champNum;
}

static u32 getCharNumOfKetsu(MuCharKind kind) {
    return g_GameGlobal->getGlobalRecordCharaNumDatap(kind)->m_diagram.m_ketsuNum;
}

static u32 getCharGiveDamage(MuCharKind kind) {
    return g_GameGlobal->getGlobalRecordCharaNumDatap(kind)->m_diagram.m_giveDamage;
}

static u32 getCharTakeDamage(MuCharKind kind) {
    return g_GameGlobal->getGlobalRecordCharaNumDatap(kind)->m_diagram.m_takeDamage;
}

// The KOs of every character that can be played
static u32 getCharNumOfBeat(MuCharKind kind) {
    gmGlobalRecord::CharaNumData* data = g_GameGlobal->getGlobalRecordCharaNumDatap(kind);
    int i;
    u32 count = 0;
    for (i = 0; i < muProcCount::Char_Count; i++) {
        if (isCharUsable__6muMenuFiii(i)) {
            count += data->m_beatCount[i];
        }
    }
    return count;
}

static u32 getCharNumOfFall(MuCharKind kind) {
    return g_GameGlobal->getGlobalRecordCharaNumDatap(kind)->m_fallCount;
}

static u32 getCharNumOfSuicide(MuCharKind kind) {
    return g_GameGlobal->getGlobalRecordCharaNumDatap(kind)->m_diagram.m_suicide;
}

// Finds the characters of the records: the most, second most and least played, and the first in each count.
void muProcCount::makeCharStatistics() {
    u32 usable;
    int i;
    int count;
    muProcCountRank list[Char_Count];
    m_no1UseChara = Char_None;
    m_no2UseChara = Char_None;
    m_worstUseChara = Char_None;
    m_no1ChampChara = Char_None;
    m_no1KetsuChara = Char_None;
    m_no1GiveDamageChara = Char_None;
    m_no1TakeDamageChara = Char_None;
    m_beatKing = Char_None;
    m_fallKing = Char_None;
    m_suicideKing = Char_None;

    count = makeCharRankList(list, getCharUseTime);
    if (count > 0) {
        m_no1UseChara = findRankChar(list, count, 1);
        if (m_no1UseChara != Char_None) {
            m_no2UseChara = findRankChar(list, count, 2);
        } else {
            m_no2UseChara = Char_None;
        }
        // The least played character needs three characters to pick from
        usable = 0;
        for (i = 0; i < Char_Count; i++) {
            if (isCharUsable__6muMenuFiii(i)) {
                usable++;
            }
        }
        if (usable >= 3) {
            m_worstUseChara = findRankChar(list, count, -1);
        }
    }
    count = makeCharRankList(list, getCharNumOfChamp);
    if (count > 0) {
        m_no1ChampChara = findRankChar(list, count, 1);
    }
    count = makeCharRankList(list, getCharNumOfKetsu);
    if (count > 0) {
        m_no1KetsuChara = findRankChar(list, count, 1);
    }
    count = makeCharRankList(list, getCharGiveDamage);
    if (count > 0) {
        m_no1GiveDamageChara = findRankChar(list, count, 1);
    }
    count = makeCharRankList(list, getCharTakeDamage);
    if (count > 0) {
        m_no1TakeDamageChara = findRankChar(list, count, 1);
    }
    count = makeCharRankList(list, getCharNumOfBeat);
    if (count > 0) {
        m_beatKing = findRankChar(list, count, 1);
    }
    count = makeCharRankList(list, getCharNumOfFall);
    if (count > 0) {
        m_fallKing = findRankChar(list, count, 1);
    }
    count = makeCharRankList(list, getCharNumOfSuicide);
    if (count > 0) {
        m_suicideKing = findRankChar(list, count, 1);
    }
}

// Sorts the characters by their value, the largest first.
static int compare(const void* a, const void* b) {
    if (static_cast<const muProcCountRank*>(a)->m_value > static_cast<const muProcCountRank*>(b)->m_value) {
        return -1;
    }
    return 1;
}

// Lists the characters that were played with the value get reads for each, the largest first.
int muProcCount::makeCharRankList(muProcCountRank* list, u32 (*get)(MuCharKind kind)) {
    int i;
    muProcCountRank* entry = list;
    int count = 0;
    for (i = 0; i < Char_Count; i++) {
        if (g_GameGlobal->getGlobalRecordCharaNumDatap(i)->m_diagram.m_playTime != 0) {
            entry->m_kind = i;
            entry->m_value = get(static_cast<MuCharKind>(i));
            entry++;
            count++;
        }
    }
    if (count >= 2) {
        qsort(list, count, sizeof(muProcCountRank), compare);
    }
    return count;
}

// The character at a rank of the list (1 the first, 2 the next, -1 the last ...), or Char_None when the rank does not
// exist or is shared. A rank counts the entries whose value differs from the one at the end the search starts from.
int muProcCount::findRankChar(const muProcCountRank* list, int count, int rank) {
    if (rank > 0) {
        int found = 1;
        int i = 0;
        u32 first = list->m_value;
        for (; i < count; i++, list++) {
            if (first != list->m_value) {
                found++;
            }
            if (found == rank) {
                break;
            }
        }
        if (i >= count) {
            return Char_None;
        }
        if (i < count - 1 && list->m_value == list[1].m_value) {
            return Char_None;
        }
        return list->m_kind;
    } else {
        int target = -rank;
        int found = 1;
        const muProcCountRank* entry = list + count - 1;
        u32 last = entry->m_value;
        int i = count - 1;
        for (; i >= 0; i--, entry--) {
            if (last != entry->m_value) {
                found++;
            }
            if (found == target) {
                break;
            }
        }
        if (i < 0) {
            return Char_None;
        }
        if (i > 0 && entry->m_value == entry[-1].m_value) {
            return Char_None;
        }
        return entry->m_kind;
    }
}

void muProcCount::playSE_CursorMove() {
    g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
}

u32 muProcCount::getStartNum(muProcCount* proc) {
    return *g_GameGlobal->getGlobalRecordBootCountDatap();
}

u32 muProcCount::getStartTime(muProcCount* proc) {
    return *g_GameGlobal->getGlobalRecordWorkingTimeDatap();
}

// All the time spent in matches: the melees, the Adventure (in frames) and the online matches
u32 muProcCount::getAllPlayTime(muProcCount* proc) {
    u32 total = gmGetAllMeleeTimeTotal();
    u32 adventure = g_GameGlobal->m_record->m_advTimeCount / 60;
    u32 friends = *g_GameGlobal->getGlobalRecordOnlineFriendTimeCountDatap();
    u32 anybody = *g_GameGlobal->getGlobalRecordOnlineAnybodyTimeCountDatap();
    addMaxCheck(&total, adventure, 359999999);
    addMaxCheck(&total, friends, 359999999);
    addMaxCheck(&total, anybody, 359999999);
    return total;
}

u32 muProcCount::getAdventureAllPlayTime(muProcCount* proc) {
    return g_GameGlobal->m_record->m_advTimeCount / 60;
}

u32 muProcCount::getAdventureNobePlayTime(muProcCount* proc) {
    return g_GameGlobal->m_record->m_advTimeTotal / 60;
}

u32 muProcCount::getSingleAllPlayTime(muProcCount* proc) {
    u32 total = *g_GameGlobal->getGlobalRecordSingleMeleeTimeCountDatap();
    addMaxCheck(&total, g_GameGlobal->m_record->m_advTimeCount / 60, 0xFFFFFFFF);
    return total;
}

u32 muProcCount::getSingleNobePlayTime(muProcCount* proc) {
    u32 total = *g_GameGlobal->getGlobalRecordSingleMeleeTimeTotalDatap();
    addMaxCheck(&total, g_GameGlobal->m_record->m_advTimeTotal / 60, 0xFFFFFFFF);
    return total;
}

u32 muProcCount::getVsAllPlayTime(muProcCount* proc) {
    return *g_GameGlobal->getGlobalRecordVsMeleeTimeCountDatap();
}

u32 muProcCount::getVsNobePlayTime(muProcCount* proc) {
    return *g_GameGlobal->getGlobalRecordVsMeleeTimeTotalDatap();
}

u32 muProcCount::getNumOfVs(muProcCount* proc) {
    return gmGetVsMeleeTotalCount();
}

u32 muProcCount::getNumOfOfflineVs(muProcCount* proc) {
    return gmGetOfflineVsMeleeTotalCount();
}

u32 muProcCount::getNumOfOnlineVs(muProcCount* proc) {
    return gmGetOnlineVsMeleeTotalCount();
}

u32 muProcCount::getOnlineFriendPlayTime(muProcCount* proc) {
    return *g_GameGlobal->getGlobalRecordOnlineFriendTimeCountDatap();
}

u32 muProcCount::getOnlineAnyonePlayTime(muProcCount* proc) {
    return *g_GameGlobal->getGlobalRecordOnlineAnybodyTimeCountDatap();
}

u32 muProcCount::getNumOfWatchGame(muProcCount* proc) {
    return *g_GameGlobal->getGlobalRecordOnlineAnybodyWatchCountDatap();
}

u32 muProcCount::getNumOfTimeVs(muProcCount* proc) {
    return *g_GameGlobal->getGlobalRecordVsTimeMeleeCountDatap();
}

u32 muProcCount::getNumOfStockVs(muProcCount* proc) {
    return *g_GameGlobal->getGlobalRecordVsStockMeleeCountDatap();
}

u32 muProcCount::getNumOfCoinVs(muProcCount* proc) {
    return *g_GameGlobal->getGlobalRecordVsCoinMeleeCountDatap();
}

u32 muProcCount::getNumOfJoin(muProcCount* proc) {
    return *g_GameGlobal->getGlobalRecordVsPartyPlayerCountDatap();
}

u32 muProcCount::getNumOfInterrupt(muProcCount* proc) {
    return *g_GameGlobal->getGlobalRecordVsResetMeleeCountDatap();
}

u32 muProcCount::getTotalDamage(muProcCount* proc) {
    return *g_GameGlobal->getGlobalRecordDamageTotalCountDatap();
}

u32 muProcCount::getTotalBeat(muProcCount* proc) {
    return *g_GameGlobal->getGlobalRecordKillTotalCountDatap();
}

u32 muProcCount::getTotalSuicide(muProcCount* proc) {
    return *g_GameGlobal->getGlobalRecordSuicideTotalCountDatap();
}

u32 muProcCount::getNumOfChara(muProcCount* proc) {
    int i;
    u32 count = 0;
    for (i = 0; i < Char_Count; i++) {
        if (isCharUsable__6muMenuFiii(i)) {
            count++;
        }
    }
    return count;
}

// The stages that can be played: the ones that are always there (no hide kind) and the ones that were unlocked
u32 muProcCount::getNumOfMap(muProcCount* proc) {
    u32* flag = g_GameGlobal->getStageAppFlag();
    int i;
    u32 count = 0;
    for (i = 0; i < 41; i++) {
        int hide = exchangeMuStageKindToGmHideStageKind__6muMenuFiii(i);
        if (hide != 12) {
            if (flag[hide / 32] & (1 << (hide % 32))) {
                count++;
            }
        } else {
            count++;
        }
    }
    return count;
}

u32 muProcCount::getNumOfFigure(muProcCount* proc) {
    return gmGetKindCountFigureExist();
}

u32 muProcCount::getNumOfSeal(muProcCount* proc) {
    return gmGetKindCountSealExist();
}

u32 muProcCount::getNumOfName(muProcCount* proc) {
    return g_GameGlobal->getNumName();
}

u32 muProcCount::getNumOfFriend(muProcCount* proc) {
    return gmGetFriendCount();
}

u32 muProcCount::getNo1UseChara(muProcCount* proc) {
    return proc->m_no1UseChara;
}

u32 muProcCount::getNo2UseChara(muProcCount* proc) {
    return proc->m_no2UseChara;
}

u32 muProcCount::getWorstUseChara(muProcCount* proc) {
    return proc->m_worstUseChara;
}

u32 muProcCount::getNo1ChampChara(muProcCount* proc) {
    return proc->m_no1ChampChara;
}

u32 muProcCount::getNo1KetsuChara(muProcCount* proc) {
    return proc->m_no1KetsuChara;
}

u32 muProcCount::getNo1GiveDamageChara(muProcCount* proc) {
    return proc->m_no1GiveDamageChara;
}

u32 muProcCount::getNo1TakeDamageChara(muProcCount* proc) {
    return proc->m_no1TakeDamageChara;
}

u32 muProcCount::getBeatKing(muProcCount* proc) {
    return proc->m_beatKing;
}

u32 muProcCount::getFallKing(muProcCount* proc) {
    return proc->m_fallKing;
}

u32 muProcCount::getSuicideKing(muProcCount* proc) {
    return proc->m_suicideKing;
}
