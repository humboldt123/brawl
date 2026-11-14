#include <gf/gf_heap_manager.h>
#include <gm/gm_global.h>
#include <mu/menu.h>
#include <nw4r/g3d/g3d_scnmdlexpand.h>
#include <snd/snd_system.h>
#include <types.h>

#include <mu/mu_proc_diagram.h>

// qsort of the library of the program
extern "C" void qsort(void* base, unsigned long count, unsigned long size, int (*compare)(const void*, const void*));

// muMenu helpers of main.dol whose symbols take three ints; the callers set only the first (as the matched units do)
extern "C" bool isCharUsable__6muMenuFiii(int muCharKind);
extern "C" int exchangeMuCharKindToMuStockchkind__6muMenuFiii(int muCharKind);

// HYPOTHESIS: the language test of muMenu::exchangeMuStockchkind2MuCharName (true: the other names are shown too).
extern "C" int fn_8004D9E8(GameGlobal* global);
// HYPOTHESIS: the name of a character (stock kind) in the other language, as muMenu's names (mu_menu.cpp).
extern "C" const char* fn_800AFA8C(int stockchkind);

void muProcDiagram34pBase::setConf(nw4r::g3d::ScnGroup* group, MuMsg* msg, int msgWnd) {
    m_group = group;
    m_state = State_Hidden;
    m_scrollState = Scroll_None;
    m_msg = msg;
    m_msgWnd = msgWnd;
    m_column = Column_Name;
    m_no = 0;
    m_top = 0;
    m_line = 0;
}

void muProcDiagram34pBase::initMsg() {
    m_msg->attachScnMdlSimple(m_msgWnd, reinterpret_cast<nw4r::g3d::ScnMdlSimple*>(m_nameObj->m_sceneModel), 1,
                              1.0f / 16.0f);
    m_msg->setFontType(m_msgWnd, 1);
    m_msg->setFontColor(m_msgWnd, 0xA7, 0xAE, 0xC0, 0xFF);
    m_msg->attachScnMdlSimple(m_msgWnd + Msg_Name, reinterpret_cast<nw4r::g3d::ScnMdlSimple*>(m_nameObj->m_sceneModel),
                              0, 1.0f / 16.0f);
    m_msg->setFontType(m_msgWnd + Msg_Name, 1);
    m_msg->setFontColor(m_msgWnd + Msg_Name, 0x33, 0x3E, 0x58, 0xFF);
    m_msg->attachScnMdlSimple(m_msgWnd + Msg_PrevName,
                              reinterpret_cast<nw4r::g3d::ScnMdlSimple*>(m_nameObj->m_sceneModel), 2, 1.0f / 16.0f);
    m_msg->setFontType(m_msgWnd + Msg_PrevName, 1);
    m_msg->setFontColor(m_msgWnd + Msg_PrevName, 0x93, 0x95, 0x9E, 0xFF);
    m_msg->attachScnMdlSimple(m_msgWnd + Msg_NextName,
                              reinterpret_cast<nw4r::g3d::ScnMdlSimple*>(m_nameObj->m_sceneModel), 3, 1.0f / 16.0f);
    m_msg->setFontType(m_msgWnd + Msg_NextName, 1);
    m_msg->setFontColor(m_msgWnd + Msg_NextName, 0x93, 0x95, 0x9E, 0xFF);
    for (int i = 0; i < LineNum; i++) {
        m_msg->attachScnMdlSimple(i + m_msgWnd + Msg_Title,
                                  reinterpret_cast<nw4r::g3d::ScnMdlSimple*>(m_listObj->m_sceneModel), i, 1.0f / 16.0f);
        m_msg->setFontColor(i + m_msgWnd + Msg_Title, 0x09, 0x17, 0x39, 0xFF);
    }
    for (int i = 0; i < LineNum; i++) {
        int wnd = i + m_msgWnd + Msg_Value;
        m_msg->attachScnMdlSimple(wnd, reinterpret_cast<nw4r::g3d::ScnMdlSimple*>(m_listObj->m_sceneModel),
                                  i + LineNum + 1, 1.0f / 16.0f);
        m_msg->setFontColor(wnd, 0x00, 0x00, 0x19, 0xFF);
        m_msg->setHScale(wnd, 0.8f);
        m_msg->setAlignMode(wnd, MuMsg::AlignMode(2));
        m_msg->setFontWidthModeFixed(wnd, 20.0f);
        wnd = i + m_msgWnd + Msg_Unit;
        m_msg->attachScnMdlSimple(wnd, reinterpret_cast<nw4r::g3d::ScnMdlSimple*>(m_listObj->m_sceneModel),
                                  i + LineNum * 2 + 1, 1.0f / 16.0f);
        m_msg->setFontColor(wnd, 0x00, 0x00, 0x19, 0xFF);
        m_msg->setHScale(wnd, 0.8f);
    }
    m_msg->attachScnMdlSimple(m_msgWnd + Msg_Expl, reinterpret_cast<nw4r::g3d::ScnMdlSimple*>(m_listObj->m_sceneModel),
                              LineNum, 1.0f / 16.0f);
    m_msg->setFontColor(m_msgWnd + Msg_Expl, 0x5C, 0x5C, 0x5C, 0xFF);
}

inline void muProcDiagram34pBase::changeState(int state) {
    if (state == m_state) {
        return;
    }
    m_state = state;
    if (state == State_Hidden) {
        m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_listMdl));
        m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_frameMdl));
        return;
    }
    if (state == State_Shown) {
        controlInit();
    }
    m_frameObj->setAnimIdx(&sStateAnims[sStateAnimIdx[state]], false);
}

// HYPOTHESIS: side is the side the page comes from, as in muProcDiagram2p::begin.
void muProcDiagram34pBase::begin(int side) {
    m_group->Insert(m_group->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_listMdl));
    m_group->Insert(m_group->sceneItemsCount, reinterpret_cast<nw4r::g3d::ScnObj*>(m_frameMdl));
    m_frameObj->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(1.0f);
    m_nameObj->setAnimIdx(&sCursorAnims[0], false);
    int state;
    switch (side) {
    case 0:
        state = State_Come;
        break;
    case 1:
        state = State_ComeSide;
        break;
    case 2:
        state = State_Come;
        break;
    }
    changeState(state);
}

// HYPOTHESIS: side is the side the page leaves to, as in begin.
void muProcDiagram34pBase::post(int side) {
    int state;
    switch (side) {
    case 0:
        state = State_Leave;
        break;
    case 1:
        state = State_LeaveSide;
        break;
    case 2:
        state = State_Leave;
        break;
    }
    changeState(state);
}

void muProcDiagram34pBase::finish() {
    m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_listMdl));
    m_group->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_frameMdl));
    m_msg->detachMuObject(m_nameObj);
    m_msg->detachMuObject(m_listObj);
}

// When the animation of the page has stopped, the state after it; when the names have scrolled, the names around them.
void muProcDiagram34pBase::main() {
    switch (m_state) {
    case State_Leave:
    case State_LeaveSide:
        if (m_frameObj->m_modelAnim->m_anmObjChrRes->GetUpdateRate() == 0.0f) {
            changeState(State_Hidden);
        }
        break;
    case State_Come:
    case State_ComeSide:
        if (m_frameObj->m_modelAnim->m_anmObjChrRes->GetUpdateRate() == 0.0f) {
            changeState(State_Shown);
        }
        break;
    }
    switch (m_scrollState) {
    case Scroll_Next:
        if (m_nameObj->m_modelAnim->m_anmObjChrRes->GetUpdateRate() == 0.0f) {
            m_nameObj->setAnimIdx(&sCursorAnims[0], false);
            m_scrollState = Scroll_None;
            setNextPrevCharNameVisible(true);
        }
        break;
    case Scroll_Prev:
        if (m_nameObj->m_modelAnim->m_anmObjChrRes->GetUpdateRate() == 0.0f) {
            m_nameObj->setAnimIdx(&sCursorAnims[0], false);
            m_scrollState = Scroll_None;
            setNextPrevCharNameVisible(true);
        }
        break;
    }
}

// HYPOTHESIS: the tables are static members defined after the functions that name them first (as in
// mu_proc_diagram_2p.cpp).
const u8 muProcDiagram34pBase::sStateAnimIdx[State_Num] = { 0, 1, 2, 3, 5, 4 };

MuAnimIdxData muProcDiagram34pBase::sStateAnims[State_Num] = {
    { 0.0f, 0.0f, 0, 0, 0x1 },
    { 9.0f, 9.0f, 0, 0, 0x1 },
    { 0.0f, 9.0f, 0, 0, 0x1 },
    { 10.0f, 19.0f, 0, 0, 0x1 },
    { 20.0f, 29.0f, 0, 0, 0x1 },
    { 30.0f, 39.0f, 0, 0, 0x1 },
};

MuAnimIdxData muProcDiagram34pBase::sCursorAnims[3] = {
    { 0.0f, 0.0f, 0, 0, 0x11 },
    { 1.0f, 5.0f, 0, 0, 0x11 },
    { 6.0f, 10.0f, 0, 0, 0x11 },
};

MuDiagDataKind muProcDiagram3p::sKindOrder[KindNum] = {
    MuDiagData_TotalBeat,    MuDiagData_TotalFall,    MuDiagData_Suicide,      MuDiagData_HitRate,
    MuDiagData_GiveDamage,   MuDiagData_TakeDamage,   MuDiagData_RecoverDamage, MuDiagData_MaxDamage,
    MuDiagData_RoundNum,     MuDiagData_ChampNum,     MuDiagData_KetsuNum,     MuDiagData_PlayTime,
    MuDiagData_UseRate,      MuDiagData_JoinAverage,  MuDiagData_WalkDistance, MuDiagData_JumpDistance,
    MuDiagData_FallDistance, MuDiagData_BlowDistance, MuDiagData_GetCoin,      MuDiagData_MakeDropCoin,
    MuDiagData_DropCoin,     MuDiagData_FallButtocks, MuDiagData_SwimDistance, MuDiagData_SwimFall,
};

MuDiagDataKind muProcDiagram4p::sKindOrder[KindNum] = {
    MuDiagData_TotalBeat,    MuDiagData_TotalFall,    MuDiagData_Suicide,      MuDiagData_HitRate,
    MuDiagData_GiveDamage,   MuDiagData_TakeDamage,   MuDiagData_RecoverDamage, MuDiagData_MaxDamage,
    MuDiagData_RoundNum,     MuDiagData_ChampNum,     MuDiagData_KetsuNum,     MuDiagData_PlayTime,
    MuDiagData_UseRate,      MuDiagData_JoinAverage,  MuDiagData_WalkDistance, MuDiagData_JumpDistance,
    MuDiagData_FallDistance, MuDiagData_BlowDistance, MuDiagData_GetCoin,      MuDiagData_MakeDropCoin,
    MuDiagData_DropCoin,     MuDiagData_FallButtocks, MuDiagData_SwimDistance, MuDiagData_SwimFall,
    MuDiagData_Unk24,        MuDiagData_Unk25,        MuDiagData_Unk26,
};

void muProcDiagram34pBase::controlInit() { }

void muProcDiagram34pBase::setNextPrevCharNameVisible(bool visible) { }

bool muProcDiagram34pBase::isPageChanging() {
    // BYTES: an unsigned compare (the states that are neither hidden nor shown), as in muProcDiagram2p.
    return static_cast<u32>(m_state) > State_Shown;
}

// The largest value a line shows (99:59:59 for times)
// HYPOTHESIS: unit kind 6 is the hours of the play time, as in muProcDiagram2p::updateDisp.
static inline u32 getMaxValue(MuDiagDataKind kind) {
    u32 max;
    switch (muDiagGetDataUnitKind(kind)) {
    case 2:
        max = 99999999;
        break;
    case 6:
        max = 5999999;
        break;
    default:
        max = 99999999;
        break;
    }
    return max;
}

void muProcDiagram34pBase::dispValue(MuDiagDataKind kind, int value, int line) {
    m_msg->printIndex(line + m_msgWnd + Msg_Title, muDiagGetDataTitleMsgID(kind));
    u32 max = getMaxValue(kind);
    const int bufSize = 0x80;
    char buf[bufSize];
    int unitMsgId;
    int length = muDiagTransformDataValue(buf, &unitMsgId, kind, value, max);
    m_msg->printData(line + m_msgWnd + Msg_Value, buf, length + 1);
    if (unitMsgId != -1) {
        m_msg->printIndex(line + m_msgWnd + Msg_Unit, unitMsgId);
    } else {
        m_msg->printf(line + m_msgWnd + Msg_Unit, "");
    }
}

void muProcDiagram34pBase::dispExpl(MuDiagDataKind kind) {
    m_msg->printIndex(m_msgWnd + Msg_Expl, muDiagGetDataExplMsgID(kind));
}

void muProcDiagram3p::setConf(nw4r::g3d::ScnGroup* group, MuMsg* msg, int msgWnd) {
    muProcDiagram34pBase::setConf(group, msg, msgWnd);
    m_frameObj = m_objs[Obj_Frame];
    m_nameObj = m_objs[Obj_Name];
    m_listObj = m_objs[Obj_List];
    m_iconObj = m_objs[Obj_Icon];
    m_listMdl = reinterpret_cast<nw4r::g3d::ScnMdlExpand*>(m_mdls[Mdl_List]);
    m_frameMdl = m_mdls[Mdl_Frame];
    initMsg();
    setArrowVisible(false);

    // The characters that can be used, in the order of the diagrams
    m_charas = static_cast<CharaList*>(gfHeapManager::alloc(Heaps::MenuInstance, sizeof(CharaList)));
    int num = 0;
    for (int i = 0; i < CharaNum; i++) {
        MuCharKind chara = muDiagGetMuCharKindRaw(i);
        if (isCharUsable__6muMenuFiii(chara)) {
            m_charas->m_charas[num] = chara;
            num++;
        }
    }
    m_charas->m_num = num;
}

void muProcDiagram3p::finish() {
    muProcDiagram34pBase::finish();
    gfHeapManager::free(m_charas);
}

void muProcDiagram3p::begin(int side) {
    muProcDiagram34pBase::begin(side);
    dispUpdate(false);
    setNextPrevCharNameVisible(true);
}

// Writes the name and the icon of the character and the lines of the list.
void muProcDiagram3p::dispUpdate(bool arrows) {
    MuCharKind chara = m_charas->m_charas[m_no];
    m_iconObj->setFrameTex(
        muMenu::exchangeGmCharacterKind2Something(exchangeMuCharKindToMuStockchkind__6muMenuFiii(chara)) * 10 + 1);
    if (fn_8004D9E8(g_GameGlobal)) {
        m_msg->printf(m_msgWnd + Msg_AltName, fn_800AFA8C(exchangeMuCharKindToMuStockchkind__6muMenuFiii(chara)));
    }
    m_msg->printf(m_msgWnd + Msg_Name,
                  muMenu::exchangeMuStockchkind2MuCharName(exchangeMuCharKindToMuStockchkind__6muMenuFiii(chara)));
    int line = 0;
    for (int i = m_top; line < LineNum && i < KindNum; line++, i++) {
        dispValue(sKindOrder[i], muDiagGetDataChar(sKindOrder[i], chara), line);
    }
    m_listObj->setFrameNode(m_line);
    setArrowVisible(arrows);
    dispExpl(sKindOrder[m_top + m_line]);
}

// The names of the characters before and after the one shown.
void muProcDiagram3p::setNextPrevCharNameVisible(bool visible) {
    int no = m_no;
    const char* name;
    if (visible && no < m_charas->m_num - 1) {
        name = muMenu::exchangeMuStockchkind2MuCharName(
            exchangeMuCharKindToMuStockchkind__6muMenuFiii(m_charas->m_charas[no + 1]));
    } else {
        name = "";
    }
    m_msg->printf(m_msgWnd + Msg_NextName, name);
    if (visible && no > 0) {
        name = muMenu::exchangeMuStockchkind2MuCharName(
            exchangeMuCharKindToMuStockchkind__6muMenuFiii(m_charas->m_charas[no - 1]));
    } else {
        name = "";
    }
    m_msg->printf(m_msgWnd + Msg_PrevName, name);
}

// The frame of the visibility: the arrows of the column of the cursor (1 down, 2 up), + 4 in the list; 8 hides them.
void muProcDiagram3p::setArrowVisible(bool visible) {
    int arrows = 0;
    int column;
    if (visible) {
        if (m_column == Column_Name) {
            if (m_no < m_charas->m_num - 1) {
                arrows |= 2;
            }
            if (m_no > 0) {
                arrows |= 1;
            }
        } else {
            if (m_top + LineNum < KindNum) {
                arrows |= 2;
            }
            if (m_top > 0) {
                arrows |= 1;
            }
        }
        column = m_column;
    } else {
        column = 2;
    }
    int frame;
    if (column != 2) {
        frame = 0;
        if (arrows & 1) {
            frame |= 2;
        }
        if (arrows & 2) {
            frame |= 1;
        }
        if (column == Column_List) {
            frame += 4;
        }
    } else {
        frame = 8;
    }
    m_frameObj->setFrameVisible(frame);
}

void muProcDiagram3p::controlInit() {
    setArrowVisible(true);
}

// Right and Left move the cursor between the names and the list; Up and Down choose the character or the line.
bool muProcDiagram3p::controlProc() {
    bool changed = false;
    u32 pad = padUpdate();
    if ((pad & Pad_Right) && m_column == Column_Name) {
        m_column = Column_List;
        changed = true;
    }
    if ((pad & Pad_Left) && m_column == Column_List) {
        m_column = Column_Name;
        changed = true;
    }
    if (m_column == Column_Name) {
        if ((pad & Pad_Down) && m_no < m_charas->m_num - 1) {
            m_no++;
            changed = true;
            m_nameObj->setAnimIdx(&sCursorAnims[2], false);
            setNextPrevCharNameVisible(false);
            m_scrollState = Scroll_Next;
        }
        if ((pad & Pad_Up) && m_no > 0) {
            m_no--;
            changed = true;
            m_nameObj->setAnimIdx(&sCursorAnims[1], false);
            setNextPrevCharNameVisible(false);
            m_scrollState = Scroll_Prev;
        }
    } else {
        if (pad & Pad_Down) {
            if (m_line < LineNum - 1 && m_line < KindNum - 1) {
                m_line++;
                changed = true;
            } else if (m_top + LineNum < KindNum) {
                m_top++;
                changed = true;
            }
        }
        if (pad & Pad_Up) {
            if (m_line > 0) {
                m_line--;
                changed = true;
            } else if (m_top > 0) {
                m_top--;
                changed = true;
            }
        }
    }
    if (changed) {
        g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
        dispUpdate(true);
    }
    return true;
}

void muProcDiagram3p::playSE_CursorMove() {
    g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
}

void muProcDiagram4p::setConf(nw4r::g3d::ScnGroup* group, MuMsg* msg, int msgWnd) {
    muProcDiagram34pBase::setConf(group, msg, msgWnd);
    m_frameObj = m_objs[Obj_Frame];
    m_nameObj = m_objs[Obj_Name];
    m_listObj = m_objs[Obj_List];
    m_iconObj = m_objs[Obj_Icon];
    m_useCharIcons[0] = m_objs[Obj_UseCharIcon];
    m_useCharIcons[1] = m_objs[Obj_UseCharIcon + 1];
    m_useCharIcons[2] = m_objs[Obj_UseCharIcon + 2];
    m_listMdl = reinterpret_cast<nw4r::g3d::ScnMdlExpand*>(m_mdls[Mdl_List]);
    m_frameMdl = m_mdls[Mdl_Frame];
    initMsg();
    setArrowVisible(false);
    m_nameNum = g_GameGlobal->getNumName();
}

void muProcDiagram4p::finish() {
    muProcDiagram34pBase::finish();
}

void muProcDiagram4p::begin(int side) {
    muProcDiagram34pBase::begin(side);
    makeUseCharRanking(m_no);
    dispUpdate(false);
    setNextPrevCharNameVisible(true);
}

// Writes the name tag and the lines of the list.
void muProcDiagram4p::dispUpdate(bool arrows) {
    int nameNo = m_no;
    // HYPOTHESIS: the size (a name tag of six characters as UTF-8).
    char name[0x14];
    g_GameGlobal->getName(nameNo, name);
    m_msg->printf(m_msgWnd + Msg_Name, name);
    m_listMdl->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_useCharIcons[0]->m_sceneModel));
    m_listMdl->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_useCharIcons[1]->m_sceneModel));
    m_listMdl->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_useCharIcons[2]->m_sceneModel));
    int line = 0;
    for (int i = m_top; line < LineNum && i < KindNum; line++, i++) {
        u32 value;
        MuDiagDataKind kind = sKindOrder[i];
        if (kind >= 0 && kind < MuDiagData_Unk24) {
            value = muDiagGetDataName(kind, nameNo);
        } else {
            switch (kind) {
            case MuDiagData_Unk24:
                value = m_useChars[0];
                break;
            case MuDiagData_Unk25:
                value = m_useChars[1];
                break;
            case MuDiagData_Unk26:
                value = m_useChars[2];
                break;
            }
        }
        dispValue(sKindOrder[i], value, line);
    }
    m_listObj->setFrameNode(m_line);
    setArrowVisible(arrows);
    dispExpl(sKindOrder[m_top + m_line]);
}

// The frame of the visibility, as in muProcDiagram3p::setArrowVisible.
void muProcDiagram4p::setArrowVisible(bool visible) {
    int arrows = 0;
    int column;
    if (visible) {
        if (m_column == Column_Name) {
            if (m_no < m_nameNum - 1) {
                arrows |= 2;
            }
            if (m_no > 0) {
                arrows |= 1;
            }
        } else {
            // BYTES: the original tests the 24 records of 3p here.
            if (m_top + LineNum < muProcDiagram3p::KindNum) {
                arrows |= 2;
            }
            if (m_top > 0) {
                arrows |= 1;
            }
        }
        column = m_column;
    } else {
        column = 2;
    }
    int frame;
    if (column != 2) {
        frame = 0;
        if (arrows & 1) {
            frame |= 2;
        }
        if (arrows & 2) {
            frame |= 1;
        }
        if (column == Column_List) {
            frame += 4;
        }
    } else {
        frame = 8;
    }
    m_frameObj->setFrameVisible(frame);
}

// The name tags before and after the one shown.
void muProcDiagram4p::setNextPrevCharNameVisible(bool visible) {
    GameGlobal* global;
    int no = m_no;
    global = g_GameGlobal;
    char name[0x14];
    if (visible && no < m_nameNum - 1) {
        global->getName(no + 1, name);
    } else {
        name[0] = '\0';
    }
    m_msg->printf(m_msgWnd + Msg_NextName, name);
    if (visible && no > 0) {
        global->getName(no - 1, name);
    } else {
        name[0] = '\0';
    }
    m_msg->printf(m_msgWnd + Msg_PrevName, name);
}

void muProcDiagram4p::controlInit() {
    setArrowVisible(true);
}

// As muProcDiagram3p::controlProc, with the name tags; the ranking is made again for the new name tag.
bool muProcDiagram4p::controlProc() {
    bool changed = false;
    u32 pad = padUpdate();
    if ((pad & Pad_Right) && m_column == Column_Name) {
        m_column = Column_List;
        changed = true;
    }
    if ((pad & Pad_Left) && m_column == Column_List) {
        m_column = Column_Name;
        changed = true;
    }
    if (m_column == Column_Name) {
        if ((pad & Pad_Down) && m_no < m_nameNum - 1) {
            m_no++;
            changed = true;
            m_nameObj->setAnimIdx(&sCursorAnims[2], false);
            setNextPrevCharNameVisible(false);
            m_scrollState = Scroll_Next;
            makeUseCharRanking(m_no);
        }
        if ((pad & Pad_Up) && m_no > 0) {
            m_no--;
            changed = true;
            m_nameObj->setAnimIdx(&sCursorAnims[1], false);
            setNextPrevCharNameVisible(false);
            m_scrollState = Scroll_Prev;
            makeUseCharRanking(m_no);
        }
    } else {
        if (pad & Pad_Down) {
            if (m_line < LineNum - 1 && m_line < KindNum - 1) {
                m_line++;
                changed = true;
            } else if (m_top + LineNum < KindNum) {
                m_top++;
                changed = true;
            }
        }
        if (pad & Pad_Up) {
            if (m_line > 0) {
                m_line--;
                changed = true;
            } else if (m_top > 0) {
                m_top--;
                changed = true;
            }
        }
    }
    if (changed) {
        dispUpdate(true);
        g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
    }
    return true;
}

static int compare(const void* a, const void* b);

// The characters the name tag played most, second most and least (a place is NoChara when there is a tie for it or the
// time is zero).
void muProcDiagram4p::makeUseCharRanking(int nameNo) {
    gmGlobalRecord::NameData* data = g_GameGlobal->getGlobalRecordNameDatap(nameNo);
    UseChar useChars[CharaNum];
    UseChar* useChar = useChars;
    int num = 0;
    for (int chara = 0; chara < CharaNum; chara++) {
        if (isCharUsable__6muMenuFiii(chara)) {
            useChar->m_chara = static_cast<MuCharKind>(chara);
            num++;
            useChar->m_time = data->m_charPlayTime[chara];
            useChar++;
        }
    }
    qsort(useChars, num, sizeof(UseChar), compare);
    m_useChars[0] = static_cast<MuCharKind>(NoChara);
    m_useChars[1] = static_cast<MuCharKind>(NoChara);
    m_useChars[2] = static_cast<MuCharKind>(NoChara);

    if (num > 0 && useChars[0].m_time != 0) {
        if (num > 1) {
            if (useChars[0].m_time != useChars[1].m_time) {
                m_useChars[0] = useChars[0].m_chara;
            }
        } else {
            m_useChars[0] = useChars[0].m_chara;
        }
    }

    if (m_useChars[0] != NoChara) {
        int i;
        for (i = 1; i < num; i++) {
            if (useChars[i - 1].m_time != useChars[i].m_time) {
                break;
            }
        }
        if (i < num && useChars[i].m_time != 0) {
            if (num > i + 1) {
                if (useChars[i].m_time != useChars[i + 1].m_time) {
                    m_useChars[1] = useChars[i].m_chara;
                }
            } else {
                m_useChars[1] = useChars[i].m_chara;
            }
        }
    }

    if (num > 1) {
        if (useChars[num - 1].m_time != useChars[num - 2].m_time) {
            m_useChars[2] = useChars[num - 1].m_chara;
        }
    } else {
        m_useChars[2] = useChars[num - 1].m_chara;
    }
}

// The longest play time first
static int compare(const void* a, const void* b) {
    return static_cast<const muProcDiagram4p::UseChar*>(b)->m_time -
           static_cast<const muProcDiagram4p::UseChar*>(a)->m_time;
}

// The records of the name tag as in muProcDiagram34pBase::dispValue; the three places of the ranking show the icon of
// the character (or "-").
void muProcDiagram4p::dispValue(MuDiagDataKind kind, int value, int line) {
    const char* text;
    if (kind < MuDiagData_Unk24) {
        m_msg->printIndex(line + m_msgWnd + Msg_Title, muDiagGetDataTitleMsgID(kind));
        u32 max = getMaxValue(kind);
        const int bufSize = 0x80;
        char buf[bufSize];
        int unitMsgId;
        int length = muDiagTransformDataValue(buf, &unitMsgId, kind, value, max);
        m_msg->printData(line + m_msgWnd + Msg_Value, buf, length + 1);
        if (unitMsgId != -1) {
            m_msg->printIndex(line + m_msgWnd + Msg_Unit, unitMsgId);
        } else {
            m_msg->printf(line + m_msgWnd + Msg_Unit, "");
        }
        return;
    }
    m_msg->printIndex(line + m_msgWnd + Msg_Title, muDiagGetDataTitleMsgID(kind));
    if (value == NoChara) {
        text = "-";
    } else {
        text = "";
        MuObject* icon;
        switch (kind) {
        case MuDiagData_Unk24:
            icon = m_useCharIcons[0];
            break;
        case MuDiagData_Unk25:
            icon = m_useCharIcons[1];
            break;
        case MuDiagData_Unk26:
            icon = m_useCharIcons[2];
            break;
        }
        m_listMdl->PushBack(reinterpret_cast<nw4r::g3d::ScnObj*>(icon->m_sceneModel), "pos01");
        icon->setFrameTex(
            muMenu::exchangeGmCharacterKind2Something(exchangeMuCharKindToMuStockchkind__6muMenuFiii(value)) * 10 + 1);
        icon->setFrameNode(line);
    }
    m_msg->printf(line + m_msgWnd + Msg_Value, text);
    m_msg->printf(line + m_msgWnd + Msg_Unit, "");
}

void muProcDiagram4p::playSE_CursorMove() {
    g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
}
