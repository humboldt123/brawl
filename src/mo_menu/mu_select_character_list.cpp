#include <memory.h>
#include <string.h>
#include <gm/gm_global.h>
#include <mu/mu_msg.h>
#include <mu/selchar/mu_select_character_list.h>
#include <ms/ms_message.h>
#include <nw4r/g3d/g3d_scnmdlexpand.h>
#include <snd/snd_system.h>
#include <gf/gf_pad_status.h>
#include <stdio.h>
#include <types.h>

// MSL's qsort: the stdlib header of this project does not declare it.
extern "C" void qsort(void* base, unsigned long count, unsigned long size, int (*compare)(const void*, const void*));
#include <types.h>

MuSelctChrList::MuSelctChrList() {
    m_isOpen = false;
    m_base = nullptr;
    for (int i = 0; i < Item_Num; i++) {
        m_items[i] = nullptr;
    }
    for (int i = 0; i < End_Num; i++) {
        m_ends[i] = nullptr;
    }
    m_parentGroup = nullptr;
    m_group = nullptr;
    memset(m_names, 0, sizeof(m_names));
}

void MuSelctChrList::init(nw4r::g3d::ScnGroup* parentGroup, nw4r::g3d::ScnMdlExpand* group, MuObject* base, MuObject** items,
                          MuObject** ends, int msgBase, MuMsg* msg, const char* defName) {
    m_parentGroup = parentGroup;
    m_group = group;
    m_base = base;
    for (int i = 0; i < Item_Num; i++) {
        m_items[i] = items[i];
    }
    m_msgBase = msgBase;
    for (int i = 0; i < End_Num; i++) {
        m_ends[i] = ends[i];
    }
    m_msg = msg;
    for (int i = 0; i < Disable_Num; i++) {
        m_disableNames[i] = -1;
    }
    strcpy(m_defName, defName);
}

// The full-width digits and percent sign of the name entry (sora_menu_name, mu_name_letter_table.cpp).
extern char* g_muNameDigits;
extern char* g_muNamePercent;

// The messages of the nine levels of the computer.
static const int sCpLevelMsg[] = { 21, 22, 24, 25, 27, 28, 30, 31, 32 };

// The order of the names in the list: the newest name first, then by the number of the name.
int muselectcharacterlist_compare(const void* a, const void* b) {
    int nameA = *static_cast<const s16*>(a);
    int nameB = *static_cast<const s16*>(b);
    GameGlobal* global = g_GameGlobal;
    gmGlobalRecord::NameData* dataA = global->getGlobalRecordNameDatap(nameA);
    gmGlobalRecord::NameData* dataB = global->getGlobalRecordNameDatap(nameB);
    int result = nameA - nameB;
    int diff = dataB->m_date - dataA->m_date;
    if (diff != 0) {
        result = diff;
    }
    return result;
}

// Opens the list at a place of the screen, with the cursor on a line (for the names: on a name, 0x78 for no name).
void MuSelctChrList::open(int kind, int cursor, Vec3f* pos) {
    m_kind = kind;
    m_base->setTrans(pos);
    int num;
    if (kind == Kind_Name) {
        num = g_GameGlobal->getNumName();
        for (int i = 0; i < num; i++) {
            m_names[i] = i;
        }
        if (num > 1) {
            qsort(m_names, num, sizeof(m_names[0]), muselectcharacterlist_compare);
        }
        m_numNames = num;
    } else {
        m_numNames = 0;
    }

    num = getListNum();
    if (kind == Kind_Name) {
        // The cursor is now a line of the list.
        if (cursor == Name_Max) {
            cursor = 0;
        } else {
            cursor = findName(cursor) + 1;
        }
        if (cursor < 0 || cursor >= num) {
            m_cursor = -1;
        } else {
            m_cursor = cursor;
        }
    } else if (cursor < 0) {
        m_cursor = 0;
    } else if (cursor >= num) {
        m_cursor = num - 1;
    } else {
        m_cursor = cursor;
    }

    m_isOpen = true;
    if (kind == Kind_Name) {
        setEndsVisible(0.0f);
    }
    m_unk160 = 0;
    dispList(m_cursor);
    dispScroll();
}

// The text of a line: a name or a number is written in the text; a level of the computer returns its message.
int MuSelctChrList::getListStr(char* out, int line) {
    int msgNo = -1;
    switch (m_kind) {
    case Kind_Name:
        if (line <= 0) {
            strcpy(out, m_defName);
        } else {
            Message::utf16to8(out, g_GameGlobal->getGlobalRecordNameDatap(m_names[line - 1])->m_name);
        }
        break;
    case Kind_CpLevel:
        msgNo = sCpLevelMsg[line];
        break;
    case Kind_Handi: {
        out[0] = 0;
        char num[0x20];
        sprintf(num, "%d", line * 10);
        for (char* digit = num; *digit; digit++) {
            Message::appendSubstr(out, g_muNameDigits, *digit - '0', 1);
        }
        Message::appendSubstr(out, g_muNamePercent, 0, 1);
        break;
    }
    case Kind_Hp:
        if (line == 0) {
            out[0] = 0;
            char num[0x20];
            sprintf(num, "%d", 1);
            for (char* digit = num; *digit; digit++) {
                Message::appendSubstr(out, g_muNameDigits, *digit - '0', 1);
            }
        } else {
            out[0] = 0;
            char num[0x20];
            sprintf(num, "%d", line * 10);
            for (char* digit = num; *digit; digit++) {
                Message::appendSubstr(out, g_muNameDigits, *digit - '0', 1);
            }
        }
        break;
    }
    return msgNo;
}

// Shows the lines of the list around a line (the cursor line is the fifth one of the window).
void MuSelctChrList::dispList(int cursor) {
    int line = cursor - 4;
    int num = getListNum();
    for (int i = 0; i < Item_Num; i++, line++) {
        MuObject* obj;
        if (line >= 0 && line < num) {
            obj = m_items[i];
        } else if (m_kind == Kind_Name && (line == -1 || line == num)) {
            obj = m_ends[line != -1];
        } else {
            continue;
        }
        char nodeName[8];
        sprintf(nodeName, "pos%d", i + 1);
        m_group->PushBack(obj->m_sceneModel, nodeName);
        obj->setFrameMatCol(i == 4 ? 1.0f : 0.0f);
        if (line < 0 || line >= num) {
            continue;
        }

        int msgIndex = m_msgBase + i;
        bool disabled = false;
        if (m_kind == Kind_Name && line > 0 && isDisableName(m_names[line - 1])) {
            disabled = true;
        }
        u8 r, g, b;
        if (disabled) {
            r = 0x7C;
            g = 0x7C;
            b = 0x7C;
        } else if (i == 4) {
            r = 0;
            g = 0;
            b = 0;
        } else {
            r = 0x50;
            g = 0x50;
            b = 0x50;
        }
        m_msg->setFontColor(msgIndex, r, g, b, 0xFF);
        m_msg->setFontColor(msgIndex + Item_Num, r, g, b, 0xFF);
        m_msg->setFontColor(msgIndex + Item_Num * 2, r, g, b, 0xFF);

        char str[40];
        int msgNo = getListStr(str, line);
        if (msgNo >= 0) {
            if (m_kind == Kind_CpLevel) {
                m_msg->printf(msgIndex, "");
                m_msg->printf(msgIndex + Item_Num, "%d", line + 1);
                m_msg->printIndex(msgIndex + Item_Num * 2, msgNo);
            } else {
                m_msg->printIndex(msgIndex, msgNo);
                m_msg->printf(msgIndex + Item_Num, "");
                m_msg->printf(msgIndex + Item_Num * 2, "");
            }
        } else {
            m_msg->printf(msgIndex, str);
            m_msg->printf(msgIndex + Item_Num, "");
            m_msg->printf(msgIndex + Item_Num * 2, "");
        }
    }
    m_parentGroup->Insert(m_parentGroup->sceneItemsCount, m_group);
    m_top = cursor;
}

// Takes a name out of the list (another player took it).
bool MuSelctChrList::addDisableName(int name) {
    if (isDisableName(name)) {
        return true;
    }
    for (int i = 0; i < Disable_Num; i++) {
        if (m_disableNames[i] == -1) {
            m_disableNames[i] = name;
            eraseList();
            dispList(m_cursor);
            return true;
        }
    }
    return false;
}

bool MuSelctChrList::delDisableName(int name) {
    for (int i = 0; i < Disable_Num; i++) {
        if (name == m_disableNames[i]) {
            m_disableNames[i] = -1;
            eraseList();
            dispList(m_cursor);
            return true;
        }
    }
    return false;
}

void MuSelctChrList::clearDisableName() {
    for (int i = 0; i < Disable_Num; i++) {
        m_disableNames[i] = -1;
    }
    eraseList();
    dispList(m_cursor);
}

// Takes the lines of the list and the list itself out of the scene.
void MuSelctChrList::eraseList() {
    int i;
    int line = m_top - 4;
    int num = getListNum();
    for (i = 0; i < Item_Num; i++, line++) {
        MuObject* obj;
        if (line >= 0 && line < num) {
            obj = m_items[i];
        } else if (m_kind == Kind_Name && (line == -1 || line == num)) {
            obj = m_ends[line != -1];
        } else {
            continue;
        }
        m_group->Remove(obj->m_sceneModel);
    }
    m_parentGroup->Remove(m_group);
}

// The list starts to scroll: the line of the cursor is drawn plain until the scroll ends.
void MuSelctChrList::scrollStart(bool up) {
    // The animations of the window when the list scrolls up or down.
    static MuAnimNameData sScrollAnims[2] = {
        { 1.0f, 5.0f, false, "MenSelchrWselbase", 1 },
        { 6.0f, 10.0f, false, "MenSelchrWselbase", 1 },
    };
    m_base->setAnimName(&sScrollAnims[!up], false);
    MuObject* obj;
    if (m_kind == Kind_Name && m_cursor == -1) {
        obj = m_ends[0];
    } else {
        obj = m_items[4];
    }
    obj->setFrameMatCol(0.0f);
    if (m_cursor >= 0 && m_cursor < getListNum()) {
        int msgIndex = m_msgBase + 4;
        m_msg->setFontColor(msgIndex, 0x50, 0x50, 0x50, 0xFF);
        m_msg->setFontColor(msgIndex + Item_Num, 0x50, 0x50, 0x50, 0xFF);
        m_msg->setFontColor(msgIndex + Item_Num * 2, 0x50, 0x50, 0x50, 0xFF);
        char str[40];
        int msgNo = getListStr(str, m_cursor);
        if (msgNo >= 0) {
            if (m_kind == Kind_CpLevel) {
                m_msg->printf(msgIndex, "");
                m_msg->printf(msgIndex + Item_Num, "%d", m_cursor + 1);
                m_msg->printIndex(msgIndex + Item_Num * 2, msgNo);
            } else {
                m_msg->printIndex(msgIndex, msgNo);
                m_msg->printf(msgIndex + Item_Num, "");
                m_msg->printf(msgIndex + Item_Num * 2, "");
            }
        } else {
            m_msg->printf(msgIndex, str);
            m_msg->printf(msgIndex + Item_Num, "");
            m_msg->printf(msgIndex + Item_Num * 2, "");
        }
    }
}

// Moves the cursor with the pad and decides or cancels. Returns 0 when the list is closed, 1 while it is open, 2 when a line
// was picked (written to out), 3 when it was cancelled, 4 to type a new name and 5 for no name.
int MuSelctChrList::main(gfPadStatus* pad, int* out) {
    if (!m_isOpen) {
        return 0;
    }
    u32 trigger = pad->m_buttonsPressedThisFrame.bits;
    if (trigger & gfPadButtons::A || trigger & gfPadButtons::B) {
        int result;
        if (trigger & gfPadButtons::A) {
            int se = 0;
            if (m_kind == Kind_Name) {
                if (m_cursor == -1 || m_cursor >= getListNum()) {
                    // The line after the names: a new name, if there is room for one.
                    if (g_GameGlobal->getNameCapacityRest() <= 0) {
                        m_unk160 = 0x78;
                        setEndsVisible(1.0f);
                        se = 3;
                        result = 1;
                    } else {
                        result = 4;
                    }
                } else if (m_cursor == 0) {
                    result = 5;
                } else {
                    int name = m_names[m_cursor - 1];
                    if (isDisableName(name)) {
                        se = 3;
                        result = 1;
                    } else {
                        *out = name;
                        result = 2;
                    }
                }
            } else {
                *out = m_cursor;
                result = 2;
            }
            g_sndSystem->playSE(static_cast<SndID>(se), -1, 0, 0, -1);
        } else if (m_kind != Kind_Name) {
            result = 2;
            *out = m_cursor;
            g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
        } else {
            result = 3;
            g_sndSystem->playSE(static_cast<SndID>(2), -1, 0, 0, -1);
        }
        if (result != 1) {
            eraseList();
            m_base->setFrame(0.0f);
            m_base->m_modelAnim->setUpdateRate(0.0f);
            m_isOpen = false;
            return result;
        }
    }

    if ((u32)(m_isOpen - State_ScrollUp) <= State_ScrollDown - State_ScrollUp) {
        // The list scrolls: when the animation of the window stops, the lines are put at their new places.
        bool done;
        if (m_base->m_modelAnim->m_anmObjChrRes->GetUpdateRate() != 0.0f) {
            done = false;
        } else {
            eraseList();
            dispList(m_cursor);
            if (m_kind == Kind_Name && m_unk160 > 0) {
                setEndsVisible(0.0f);
                m_unk160 = 0;
            }
            m_base->setFrameNode(0.0f);
            done = true;
        }
        if (done) {
            m_isOpen = State_Open;
        }
    }

    if (m_isOpen == State_Open) {
        // HYPOTHESIS: the repeat bits of the pad (8 up, 4 down).
        if (pad->m_buttonsPressedThisFrame2.bits & 8 && m_cursor > (m_kind == Kind_Name ? -1 : 0)) {
            scrollStart(true);
            m_cursor--;
            m_isOpen = State_ScrollUp;
            dispScroll();
            g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
        }
        if (pad->m_buttonsPressedThisFrame2.bits & 4 && m_cursor < getLastLine()) {
            scrollStart(false);
            m_cursor++;
            m_isOpen = State_ScrollDown;
            dispScroll();
            g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
        }
    }

    if (m_unk160 > 0) {
        // The lines that say that there is no room for a name stay for a while.
        if (--m_unk160 <= 0) {
            setEndsVisible(0.0f);
        }
    }
    return 1;
}

void MuSelctChrList::close() {
    if (m_isOpen) {
        eraseList();
        m_isOpen = false;
    }
}
