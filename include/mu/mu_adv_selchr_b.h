#pragma once

#include <StaticAssert.h>
#include <gf/gf_file_io_handle.h>
#include <gf/gf_pad_status.h>
#include <gf/gf_task.h>
#include <memory.h>
#include <mt/mt_vector.h>
#include <mu/mu_menu_controller.h>
#include <mu/mu_msg.h>
#include <mu/mu_object.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <types.h>

// What the scene gives to the task.
struct muAdvSelchrBTaskParam {
    u32 m_mode;                 // 0x00 the low byte says which list of the characters is shown, 0x100 says that the second player
                                //      has some of the picks, 0xFFFF shows the result of the picks that were made before
    int m_pad1;                 // 0x04 the pad of the first player (-1: there is none)
    int m_pad2;                 // 0x08 the pad of the second player (-1: there is none)
    void* m_result;             // 0x0C where the picks are stored (0x68 bytes)
};

// A character of the menu: the kind of the character, where the cursor is when it is on the character and where the name
// of the character is shown. The lists are in the file "mu_adv_selchrb_tbl.dat" and end with a kind that is below 0.
// HYPOTHESIS: the names of the fields follow how the cursors use them.
struct muAdvSelchrBChar {
    s8 m_kind;                  // 0x00
    float m_pos[2];             // 0x04 where the cursor is (x, y)
    float m_namePos[2];         // 0x0C where the name is shown (x, y)

    Vec2f getPos() const { return Vec2f(m_pos[0], m_pos[1]); }

    // MATCH-ONLY: the copy of a character reads the fields one by one (the kind is read again).
    void operator=(const muAdvSelchrBChar& other) {
        m_kind = other.m_kind;
        m_pos[0] = other.m_pos[0];
        m_pos[1] = other.m_pos[1];
        m_namePos[0] = other.m_namePos[0];
        m_namePos[1] = other.m_namePos[1];
    }
};
static_assert(sizeof(muAdvSelchrBChar) == 0x14, "Class is wrong size!");

// One model of the menu: the name, the slots of the task it fills (start, end) and the draw priority.
struct muAdvSelchrBModel {
    const char* m_name;
    u8 m_start;
    u8 m_end;
    u8 m_priority;
};

// The result that the menu stores: what each player picked.
struct muAdvSelchrBResult {
    int m_kind[10];             // 0x00 the kinds of the characters in the order that they were picked
    int m_unk28[10];            // 0x28 HYPOTHESIS: 0x3E for each one that was picked
    int m_count;                // 0x50 how many were picked
    int m_unk54;                // 0x54 the kind of the character that the second player picked first (0x28: none)
    int m_unk58;                // 0x58 0x3E
    int m_pad5C;                // 0x5C how many the second player picked
    u8 m_unk60[8];
};
static_assert(sizeof(muAdvSelchrBResult) == 0x68, "Class is wrong size!");

// The adventure mode menu where the players pick the characters (type B): one cursor for each player moves over the
// characters, A picks the character that the cursor is on, B takes the last pick back.
// HYPOTHESIS: the names of the fields follow how the steps use them.
class muAdvSelchrBTask : public gfTask {
public:
    enum Step {
        Step_Appear = 0,
        Step_Select = 1,        // the players pick the characters
        Step_Result = 2,        // the picks are stored and the menu waits a while
        Step_End = 3,
        Step_Count = 4,
    };

    // One step of the menu: what runs when the step starts, and what runs every frame (it returns the next step).
    struct StepFunc {
        void (*m_init)(muAdvSelchrBTask* task);
        int (*m_main)(muAdvSelchrBTask* task);
    };

    // The data of one player: the controller and the picks.
    struct PlayerWork {
        muMenuController m_controller;
        int m_picked[10];       // 0xA8 the characters that the player picked (the numbers of the table), in order
        int m_pickedCount;      // 0xD0
        int m_cursor;           // 0xD4 the character that the cursor is on
    };

    // HYPOTHESIS: the game has a class of its own for the handles (its destructor is a function of the unit).
    struct FileHandle : public gfFileIOHandle {
    };

    nw4r::g3d::ResFile m_resFileCommon;         // 0x40 the models that the other menus of the adventure mode have too
    nw4r::g3d::ResFile m_resFileMenu;           // 0x44 the models of the menu
    nw4r::g3d::ResFile m_resFileUnk;            // 0x48
    MuObject* m_objects[26];                    // 0x4C
    int m_step;                                 // 0xB4
    int m_stepPrev;                             // 0xB8
    PlayerWork m_players[2];                    // 0xBC
    muAdvSelchrBChar m_chars[10];               // 0x26C
    int m_charCount;                            // 0x334
    int m_maxPick[2];                           // 0x338 how many characters each player picks
    int m_timer;                                // 0x340
    int m_state;                                // 0x344 1: loaded, 2: done
    muAdvSelchrBResult* m_result;               // 0x348
    MuMsg* m_msg;                               // 0x34C the names of the characters
    Heaps::HeapType m_instanceHeap;             // 0x350
    Heaps::HeapType m_resourceHeap;             // 0x354
    FileHandle m_handles[3];                // 0x358 the models of the menu, the common models, the list of the characters
    bool m_loaded;                              // 0x364 the files are loaded
    u32 m_mode;                                 // 0x368

    static const muAdvSelchrBModel sMenuModels[2];
    static const muAdvSelchrBModel sCommonModels[5];
    static const u8 sPadTypeFrames[4];

    muAdvSelchrBTask();
    static muAdvSelchrBTask* create(const muAdvSelchrBTaskParam* param, Heaps::HeapType instanceHeap,
                                    Heaps::HeapType resourceHeap);

    virtual void processDefault();
    virtual ~muAdvSelchrBTask();

    void startSelChar();
    void mulScale(float scale);
    int loadSettingTable(int index);
    void createObjResFile(const muAdvSelchrBModel* models, int count, nw4r::g3d::ResFile* resFile);
    void moveOneCursorMain(int player);
    int getNotNumberingPos();
    void selectMain(int player);
    int getNextPos(int player, int pos);
    void dumpResult();
    void storeResult();
    bool isSelected();

    // Whether the player has a pad.
    bool hasPad(int player);
    // Whether some player has picked the character.
    bool isNumbered(int pos);
    // Whether the character is free for the cursor of the player: it is not picked, and the cursors of the players before
    // do not stay on it.
    bool isFree(int pos, int player);

    static void stepAppearInit(muAdvSelchrBTask* task);
    static int stepAppearMain(muAdvSelchrBTask* task);
    static void stepSelectInit(muAdvSelchrBTask* task);
    static int stepSelectMain(muAdvSelchrBTask* task);
    static void stepResultInit(muAdvSelchrBTask* task);
    static int stepResultMain(muAdvSelchrBTask* task);
    static void stepEndInit(muAdvSelchrBTask* task);
    static int stepEndMain(muAdvSelchrBTask* task);
};
static_assert(sizeof(muAdvSelchrBTask) == 0x36C, "Class is wrong size!");
