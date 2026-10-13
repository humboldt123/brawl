#pragma once

#include <StaticAssert.h>
#include <gf/gf_archive.h>
#include <gf/gf_file_io_handle.h>
#include <gf/gf_task.h>
#include <memory.h>
#include <mu/menu.h>
#include <mu/mu_menu_controller.h>
#include <mu/mu_menuroot.h>
#include <mu/mu_msg.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <mu/mu_name.h>
#include <mu/mu_object.h>
#include <mu/mu_rule.h>
#include <mu/mu_tour_table.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <types.h>

// One fighter slot of the tournament while the menu is open (the menu copies it from the data of the game, and back).
// HYPOTHESIS: the names follow how the menu reads the fields.
struct muTourSlot {
    int m_kind;                 // 0x00 0: a player's, 1: a CPU, 2: the slot is not used
    char m_name[0x18];          // 0x04 the name shown in the table
    int m_character;            // 0x1C the kind of fighter in the menu (0x29: none picked)
    u8 m_color;                 // 0x20 the costume
    u8 m_unk21;
    u16 m_handicap;             // 0x22 the handicap (percent)
    u8 m_nameTag;               // 0x24 the number of the saved name (0x78: none)
    u8 m_index;                 // 0x25 the number the name was given (Player 1, CPU 1, ...)
    u8 m_named;                 // 0x26 the name was picked by the player
    u8 m_unk27;
};
static_assert(sizeof(muTourSlot) == 0x28, "Class is wrong size!");

// The menu where the rules of the tournament are set: how many fighters play a match, how many fighters take part, the rules
// of the matches (the menu of the rules is opened for that), and who the fighters are.
class muTourRuleTask : public gfTask {
public:
    enum State {
        State_Load = 0,             // waits for the files
        State_Setup = 1,            // the table is loaded: starts the screen that the mode asks for
        State_SelectPlayers = 2,    // how many fighters play a match (2 ... 4)
        State_SelectEntrants = 3,   // how many fighters take part
        State_Confirm = 4,          // the rules and the start of the tournament
        State_ConfirmBack = 5,      // comes back from another screen to the confirmation
        State_RuleMenu = 6,         // the menu of the rules is open
        State_SelectFighters = 7,   // the slots of the fighters
        State_Warning = 8,          // there are less controllers than players
        State_NameWindow = 9,       // the window where a player picks a name
        State_SelectChar = 10,      // the fighter and costume of a slot
        State_Handicap = 11,        // the handicap of a slot
        State_HandicapEnd = 12,     // the window of the handicap closes
        State_ResumeOpen = 13,
        State_StageMode = 14,       // how the stages are picked
        State_StageModeBack = 15,
        State_StageRuleMenu = 16,   // the menu of the rules is open to set the rules of the stage
        State_Finish = 17,          // the choice is saved in the data of the game
        State_Resume = 18,          // the tournament continues: begins a new one, or goes on
        State_Leave = 19,           // the menu closes
    };

    // How the menu was started.
    enum Mode {
        Mode_New = 0,
        Mode_Edit = 1,
        Mode_Resume = 2,
        Mode_Rules = 3,
    };

    // How the menu closes (m_result).
    enum Result {
        Result_Start = 1,
        Result_Resume = 2,
        Result_Back = 3,
    };

    MenuRoot* m_root;                           // 0x040 the root of the scene the models are put in
    int m_state;                                // 0x044
    int m_result;                               // 0x048
    int m_timer;                                // 0x04C the frames the warning is shown
    muMenuController m_ctrl;                    // 0x050
    MuMsg* m_msg;                               // 0x0F8 the texts of the screen
    MuMsg* m_msgSlot;                           // 0x0FC the names and handicaps
    MuMsg* m_msgCpu;                            // 0x100 the level of the CPU
    muTourSlot m_slots[0x20];                   // 0x104
    u8 m_edited;                                // 0x604 the player changed the fighters (they are kept when the numbers change)
    u8 m_nameDone;                              // 0x605
    nw4r::g3d::ResFile m_resRuleA;              // 0x608 the models of the screen
    nw4r::g3d::ResFile m_resRuleB;              // 0x60C
    MuObject* m_objs[0xC3];                     // 0x610
    nw4r::g3d::ScnGroup* m_mdls[0x28];          // 0x91C the models that the objects are put in
    muObjProcessTask* m_objProc;                // 0x9BC updates the objects
    muTourTable* m_table;                       // 0x9C0
    bool m_padWarning;                          // 0x9C4 the warning of the controllers is shown
    nw4r::g3d::ResFile m_resC;                  // 0x9C8
    nw4r::g3d::ResFile m_resNameEntry;          // 0x9CC "name_entry.brres"
    nw4r::g3d::ResFile m_resD;                  // 0x9D0
    nw4r::g3d::ResFile m_resWifiWnd;            // 0x9D4 "wifi_notice_wnd.brres"
    nw4r::g3d::ResFile m_resCharaFace;          // 0x9D8 "CharaFaceTex.brres"
    nw4r::g3d::ResFile m_resStockFace;          // 0x9DC "StockFaceTex.brres"
    int m_playersPerMatch;                      // 0x9E0 2 ... 4
    int m_sizeIndex;                            // 0x9E4 which size of the tournament (see the tables of the table)
    int m_entrants;                             // 0x9E8 how many fighters take part
    int m_cursor;                               // 0x9EC
    bool m_transition;                          // 0x9F0 an animation of the screen is running
    int m_ctrlPad;                              // 0x9F4 the pad that picked the slot (-1 ... 7)
    gfTask* m_panel;                            // 0x9F8
    muRuleTask* m_ruleTask;                     // 0x9FC the menu of the rules while it is open
    int m_charCount;                            // 0xA00 how many fighters can be picked (and the random one)
    int m_charCursor;                           // 0xA04
    int m_slotCursor;                           // 0xA08 the slot that is on
    int m_cpuLevel;                             // 0xA0C the level of the CPU (9: random)
    bool m_onPaging;                            // 0xA10 the cursor is on the buttons that change the page
    bool m_onCpuLevel;                          // 0xA11 the cursor is on the level of the CPU
    bool m_hasCpu;                              // 0xA12 a CPU takes part
    u8 m_cpuAnim;                               // 0xA13 0: the level of the CPU is not shown, 1: it is, 2: it goes away
    int m_padCount;                             // 0xA14 how many controllers are plugged in
    muNameTask* m_nameWnd;                      // 0xA18 the window where a name is picked
    int m_handicap;                             // 0xA1C
    int m_handicapTop;                          // 0xA20
    int m_page;                                 // 0xA24 the page of the slots
    float m_pageY;                              // 0xA28 where the slots are moving to
    bool m_pageMoving;                          // 0xA2C
    int m_charColumns;                          // 0xA30 how many fighters are in a row of the picker
    int m_charScroll;                           // 0xA34
    float m_charScrollX;                        // 0xA38
    bool m_charScrollMoving;                    // 0xA3C
    int m_charGridCursor;                       // 0xA40 the place of the fighter that is on in the picker
    int m_selChar;                              // 0xA44 the fighter that is on (0x29: random)
    u8 m_selColor;                              // 0xA48
    int m_charList[0x24];                       // 0xA4C the fighters that can be picked (the kinds of the menu)
    int m_stageMode;                            // 0xADC
    int m_confirmSel;                           // 0xAE0 which button is on in the screen of the resume
    gfArchive* m_archiveA;                      // 0xAE4
    gfArchive* m_archiveB;                      // 0xAE8
    bool m_loaded[11];                          // 0xAEC which of the files are loaded (the files, then the two archives, ...)
    gfFileIOHandle m_files[8];                  // 0xAF8 the files that the menu reads
    int m_mode;                                 // 0xB18 how the menu was started
    bool m_colorShown;                          // 0xB1C the button that changes the costume is shown

    explicit muTourRuleTask(int mode);
    virtual void processAnim();
    virtual ~muTourRuleTask();

    void pushbackPanel(int mode);
    void drawSelectTotalPlayer(bool first);
    void drawSelectConf(bool first);
    void drawSelectName(u8 onPaging, u8 onCpuLevel, bool first);
    void initPlayerData();
    void setPlayerWindow(int slot);
    void drawHandiWindow();
    void drawSelectChar();
    void drawColorIcon(int character, u8 color);
    int charSelOption(int character);
    void setWarning();
    void drawAllConf();
    void setGlobalData();
    void initMdlData();
    void setEntryButton();
    void setupTournament();
};
static_assert(sizeof(muTourRuleTask) == 0xB20, "Class is wrong size!");
