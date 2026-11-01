#include <memory.h>
#include <string.h>
#include <gm/gm_global.h>
#include <mu/mu_msg.h>
#include <mu/selchar/mu_select_character_list.h>
#include <nw4r/g3d/g3d_scnmdlexpand.h>
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
        m_ends[0]->setFrameVisible(0.0f);
        m_ends[1]->setFrameVisible(0.0f);
    }
    m_unk160 = 0;
    dispList(m_cursor);
    dispScroll();
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

void MuSelctChrList::close() {
    if (m_isOpen) {
        eraseList();
        m_isOpen = false;
    }
}
