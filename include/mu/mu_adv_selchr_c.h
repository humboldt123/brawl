#pragma once

#include <StaticAssert.h>
#include <gf/gf_file_io_handle.h>
#include <gf/gf_task.h>
#include <mu/mu_menu_controller.h>
#include <mu/mu_object.h>
#include <nw4r/g3d/g3d_scnmdlexpand.h>
#include <sr/sr_common.h>
#include <types.h>

// The adventure mode menu where the players pick the team members (type C): the teams are listed in lines of seven
// faces, and each player moves a cursor over the characters that are free.
// The layout comes from the constructor (0xC60 bytes, the size create allocates) and from the accessors.
// HYPOTHESIS: the names of the fields follow how the accessors use them; the names of the functions follow their order.
class muAdvSelchrCTask : public gfTask {
public:
    enum Step {
        Step_Appear = 0,
        Step_SelectTeam = 1,
        Step_SelectChar = 2,
        Step_Selected = 3,
        Step_Zombie = 4,            // the menu waits to be removed
        Step_Count = 5,
    };

    // The menu kinds of the characters that change form: the menu shows one entry for each of them.
    enum CharKind {
        CharKind_Samus = 3,         // 3 and 4 (Zero Suit Samus)
        CharKind_Zelda = 0xE,       // 0xE and 0xF (Sheik)
        CharKind_PokeTrainer = 0x1B, // 0x1B to 0x1E (the trainer and the three Pokemon)
        CharKind_None = 0x28,
    };

    // Indices of m_objects. HYPOTHESIS: the names follow how moveCharCursor and dispNumber use them.
    enum Obj {
        Obj_Face = 13,              // the faces of the members of a team, from its m_top on
        Obj_Cursor = 0x36,          // the cursor of each player (two)
        Obj_Number1 = 0x38,         // the numbers that show the order of the picks of the first player (40)
        Obj_Number2 = 0x60,         // the same for the second player
    };

    // One step of the menu: what runs when the step starts, and what runs every frame (it returns the next step).
    struct StepFunc {
        void (*m_init)(muAdvSelchrCTask* task);
        int (*m_main)(muAdvSelchrCTask* task);
    };

    // A team: the characters in it and the first one that the list shows.
    struct Team {
        int m_id;                   // 0x00 what getCharTeam gives back
        int m_members[40];          // 0x04 the menu kinds of the characters
        int m_count;                // 0xA4
        int m_top;                  // 0xA8 the first member that the list shows
    };

    // The data of one player: the controller, the characters the player picked and the cursor.
    struct PlayerWork {
        muMenuController m_controller;
        int m_members[40];          // 0xA8 the characters that the player picked (the positions in the team)
        int m_count;                // 0x148
        int m_cursor;               // 0x14C the character that the cursor is on
    };

    // HYPOTHESIS: the game has a class of its own for the handles (its destructor is a function of the unit).
    struct FileHandle : public gfFileIOHandle {
    };

    Team m_teams[10];               // 0x40
    int m_numTeams;                 // 0x6F8
    int m_unk6FC;                   // 0x6FC
    int m_unk700[4];                // 0x700
    MuObject* m_objects[137];       // 0x710 the faces of the members are from index 13 on
    nw4r::g3d::ScnMdlExpand* m_expands[12]; // 0x934 from index 2 on, the faces of each team (the cursors and numbers go on them)
    int m_unk964;                   // 0x964
    int m_step;                     // 0x968
    int m_stepPrev;                 // 0x96C
    PlayerWork m_players[2];        // 0x970
    int m_unkC10[2];                // 0xC10
    int m_timer;                    // 0xC18 how many frames the selected step has run
    int m_teamCurPos;               // 0xC1C
    bool m_unkC20;                  // 0xC20
    int m_state;                    // 0xC24 2: done
    bool m_unkC28[7];               // 0xC28
    int m_pokeTrainerKind;          // 0xC30 the form that is shown for each character that changes form
    int m_zeldaKind;                // 0xC34
    int m_samusKind;                // 0xC38
    int m_unkC3C;                   // 0xC3C
    FileHandle m_handles[5];        // 0xC40
    bool m_loaded;                  // 0xC54
    Heaps::HeapType m_instanceHeap; // 0xC58
    Heaps::HeapType m_resourceHeap; // 0xC5C

    static int normalizeCharKind(int kind, bool* pokeTrainerSeen, bool* zeldaSeen, bool* samusSeen);
    int getCharTeam(int* index, int kind);
    int getTeamMember(int team, int index);
    int getTeamMemberDetail(int team, int index);
    int getNumTeamMember(int team);
    MuObject* getFaceObj(int team, int index);
    bool isPlayerJoined(int player);
    int getTeamCurPos();
    int getNumTeamLine(int team);
    int getCharCursorPos(int player);
    void setCharCursorPos(int player, int pos);
    int getMemberNumber(int pos);
    bool canLocateCursor(int player, int pos);
    int getNumFreeChar();
    void moveCharCursor(int player, int pos);
    void dispNumber(int player, int pos, int number);
    void removeNumber(int player, int number);
    bool isSelected();

    static void mainStepSelectedInit(muAdvSelchrCTask* task);
    static int mainStepSelectedMain(muAdvSelchrCTask* task);
    static void mainStepZombieInit(muAdvSelchrCTask* task);
    static int mainStepZombieMain(muAdvSelchrCTask* task);
};
static_assert(sizeof(muAdvSelchrCTask) == 0xC60, "Class is wrong size!");
