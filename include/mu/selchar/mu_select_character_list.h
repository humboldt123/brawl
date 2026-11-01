#pragma once

// Local shadow of the header of the shared headers (this directory comes first in the include paths): the shared header only
// knows a part of the size of the class (it is 0x174 bytes: the area of a player has no gap after it).

#include <StaticAssert.h>
#include <mu/mu_msg.h>
#include <mu/mu_object.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <nw4r/g3d/g3d_scnmdlexpand.h>
#include <types.h>

// The list that opens in the area of a player on the character select screen: the names, the levels of the computer or the
// numbers of the handicap.
// HYPOTHESIS: the names of the fields, from how the functions use them.
class MuSelctChrList {
public:
    enum Kind {
        Kind_Name = 0,          // the saved names (and a first line with no name)
        Kind_CpLevel = 1,       // the nine levels of the computer (lines of text)
        Kind_Handi = 2,         // HYPOTHESIS: the damage handicap, 0 to 300 in steps of 10
        Kind_Hp = 3,            // HYPOTHESIS: the stamina, 1 and then 10 to 300 in steps of 10
    };

    enum {
        Item_Num = 9,           // the lines of the list that are shown
        End_Num = 2,            // the lines above and below the names
        Disable_Num = 4,        // the names that other players already took
        Name_Max = 120,
    };

    int m_kind;                             // 0x000
    int m_isOpen;                           // 0x004
    MuObject* m_base;                       // 0x008 the window of the list
    MuObject* m_items[Item_Num];            // 0x00C
    MuObject* m_ends[End_Num];              // 0x030 HYPOTHESIS: the lines before the first and after the last name
    nw4r::g3d::ScnGroup* m_parentGroup;     // 0x038
    nw4r::g3d::ScnMdlExpand* m_group;       // 0x03C the models of the lines, attached to the nodes of the window
    int m_msgBase;                          // 0x040 the first text of the lines in the message (three columns of nine)
    int m_cursor;                           // 0x044 the line that is selected
    int m_top;                              // 0x048 the line that is at the top of the window
    MuMsg* m_msg;                           // 0x04C the text of the lines
    char m_defName[0x1C];                   // 0x050 the text of the line with no name
    int m_numNames;                         // 0x06C
    u16 m_names[Name_Max];                  // 0x070 the numbers of the saved names, sorted
    int m_unk160;                           // 0x160
    int m_disableNames[Disable_Num];        // 0x164 the names that cannot be picked (-1: none)

    MuSelctChrList();
    void init(nw4r::g3d::ScnGroup* parentGroup, nw4r::g3d::ScnMdlExpand* group, MuObject* base, MuObject** items, MuObject** ends,
              int msgBase, MuMsg* msg, const char* defName);
    int getListStr(char* out, int line);
    void open(int kind, int cursor, Vec3f* pos);
    void dispList(int cursor);
    bool addDisableName(int name);
    bool delDisableName(int name);
    void clearDisableName();
    void eraseList();
    void close();

private:
    // The number of lines of the list.
    int getListNum() {
        int num;
        switch (m_kind) {
        case Kind_Name:
            num = m_numNames + 1;
            break;
        case Kind_CpLevel:
            num = 9;
            break;
        case Kind_Handi:
            num = 31;
            break;
        case Kind_Hp:
            num = 31;
            break;
        }
        return num;
    }

    // The line of a saved name in the sorted list (-1 when it is not there).
    int findName(int name) {
        for (int i = 0; i < m_numNames; i++) {
            if (name == m_names[i]) {
                return i;
            }
        }
        return -1;
    }

    // The arrows of the window show whether the list goes on above and below the cursor.
    // HYPOTHESIS: an inline helper; the name is not known.
    void dispScroll() {
        bool up = (m_kind == Kind_Name ? -1 : 0) < m_cursor;
        int last = getListNum();
        if (m_kind == Kind_Name) {
            last++;
        }
        bool down = m_cursor < last - 1;
        float frame;
        if (down) {
            frame = up ? 1.0f : 2.0f;
        } else {
            frame = up ? 3.0f : 0.0f;
        }
        m_base->setFrameVisible(frame);
    }

    bool isDisableName(int name) {
        if (name < 0) {
            return true;
        }
        for (int i = 0; i < Disable_Num; i++) {
            if (name == m_disableNames[i]) {
                return true;
            }
        }
        return false;
    }
};
static_assert(sizeof(MuSelctChrList) == 0x174, "Wrong size for class!");
