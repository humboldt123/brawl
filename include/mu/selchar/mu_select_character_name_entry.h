#pragma once

#include <StaticAssert.h>
#include <gf/gf_pad_status.h>
#include <mu/mu_msg.h>
#include <mu/mu_object.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <types.h>

// Local shadow of the header of the shared headers (this directory comes first in the include paths): the shared header only
// knows the size of the class. The members are the ones that the name entry of the name menu uses.
// The keyboard that the name entry shows: a set of keys (hiragana, katakana, letters, digits, letters of Europe), a cursor that
// moves on the keys, and the name that is typed.
// HYPOTHESIS: the names of the fields follow how the functions use them.
class MuSelctChrNameEntry {
public:
    enum Result {
        Result_Decided = 0,     // the name was decided
        Result_Cancelled = 1,   // the player went back without a name
        Result_Editing = 2,     // the name is still typed
        Result_Closed = 3,      // the entry is not open
    };

    // The keys of the keyboard that have a meaning of their own, the other keys (1 to 11) are the rows of the set of keys.
    enum Cursor {
        Cursor_Back = 0,        // deletes the last letter
        Cursor_Next = 12,       // goes to the next set of keys
        Cursor_Done = 13,       // decides the name
    };

    int m_isOpen;                           // 0x00
    char* m_name;                           // 0x04 the name that is typed
    int m_maxLen;                           // 0x08 how many letters the name can have
    char m_ownBuf[0x18];                    // 0x0C the name, when the entry is not given a buffer
    int m_setList[5];                       // 0x24 the sets of keys that the player can switch between
    int m_setCount;                         // 0x38 how many sets of keys the language uses
    int m_curSet;                           // 0x3C the set of keys that is shown (an index of the list)
    int m_cursor;                           // 0x40 the key that is selected
    MuObject* m_base;                       // 0x44 the window of the keyboard
    MuObject* m_setObj[5];                  // 0x48 the models of the sets of keys
    MuObject* m_cursorObj;                  // 0x5C the cursor on the keys
    MuObject* m_lineObj;                    // 0x60 the line under the letter that is typed (can be missing)
    MuMsg* m_msg;                           // 0x64 the text of the name
    void* m_scnGroup;                       // 0x68 the group of the scene that the keyboard is part of
    nw4r::g3d::ScnMdl* m_scnMdl;            // 0x6C the model that the models of the keyboard are attached to
    int m_msgIndex;                         // 0x70 the line of the text that shows the name
    int m_posNo;                            // 0x74 the place of the keyboard on the screen
    MuObject* m_curSetObj;                  // 0x78 the model of the set of keys that is attached
    bool m_caretAtEnd;                      // 0x7C the line is after the last letter and not on it
    int m_savedCursor;                      // 0x80 the key that the cursor goes back to from the top and from the done key
    int m_caret;                            // 0x84 the place of the letter that the line is under
    bool m_allowEmpty;                      // 0x88 an empty name can be decided (and the random names are not given)
    int m_repeatTimer;                      // 0x8C frames of the cancel button that are left before the name is cleared
    int m_padNo;                            // 0x90 the pad of the player

    MuSelctChrNameEntry();
    void init(void* scnGroup, nw4r::g3d::ScnMdl* scnMdl, MuObject* base, MuObject** setObjs, MuObject* cursorObj, MuObject* lineObj,
              int msgIndex, MuMsg* msg, int posNo, bool allowEmpty);
    void open(const char* initial, char* buf, int maxLen);
    void getCandidacy(char* out, const char* letters, const char* current);
    int main(gfPadStatus pad, char* out, bool allowRandom, bool playSound);
    void close();
    void cursorMain(gfPadStatus* pad);
    int abButtonMain(gfPadStatus* pad, bool allowRandom);
    static bool changeLetter(char* out, const char* current);
    int decideButtonProc(gfPadStatus* pad, bool allowRandom);
    static bool isNameValid(const char* name);
    static bool isNameValidNumeric(const char* name, const char* prefix, int digits, int min, int max);

private:
    void updateCaret();
    void updatePadIcon();
    void setCursorFrame(int cursor);
    void detachSet();
};
static_assert(sizeof(MuSelctChrNameEntry) == 0x94, "Class is wrong size!");
