#pragma once

#include <StaticAssert.h>
#include <gf/gf_archive.h>
#include <gf/gf_pad_status.h>
#include <gf/gf_task.h>
#include <memory.h>
#include <mu/mu_menu_controller.h>
#include <mu/mu_msg.h>
#include <mu/mu_object.h>
#include <nw4r/g3d/g3d_obj.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <types.h>

// What the scene gives to the task, and what it keeps of the choice: the event that the cursor is on, the level, the players
// and their pads.
// HYPOTHESIS: the names follow how the menu uses the fields.
struct muEventSetting {
    int m_cursor;               // 0x00 the place of the event in the list
    int m_level;                // 0x04 the level (0 ... 2; 3: none)
    int m_playerCount;          // 0x08 how many players are in the menu
    int m_pad[2];               // 0x0C the pad of each player (-1: there is none)
    int m_name[2];              // 0x14 the name that each player picked (-1: none)
    int m_eventNo;              // 0x1C the event (the number the game uses for it)
    u8 m_unk20;                 // 0x20
};
static_assert(sizeof(muEventSetting) == 0x24, "Class is wrong size!");

// An event of the list. The lists are in the file "muEventTbl" and end with an entry that starts with -1.
// HYPOTHESIS: the names follow how the menu uses the fields.
struct muEventEntry {
    int m_msgNo;                // 0x00 the number of the text of the name of the event
    int m_descNo;               // 0x04 the number of the text of the description
    u8 m_kind1P;                // 0x08 the kind of the fighter of the first player (0x28: none)
    u8 m_unk09;
    u8 m_kind2P;                // 0x0A the kind of the fighter of the second player (0x28: none)
    union {
        u8 m_flags;             // 0x0B
        struct {
            u8 m_unk0B_7 : 1;
            u8 m_isTime : 1;    // the score is a time
            u8 m_unk0B_5 : 6;
        };
    };
    u8 m_eventNo;               // 0x0C the number of the event in the data of the game
};

// What the player did in one level of an event: it was cleared, and the best score.
struct muEventLevel {
    u8 m_cleared : 1;           // 0x00
    u8 m_unk0 : 7;
    u8 m_pad[3];
    u32 m_score;                // 0x04
};
static_assert(sizeof(muEventLevel) == 8, "Class is wrong size!");

// The three levels of an event.
struct muEventRecord {
    muEventLevel m_levels[3];
};
static_assert(sizeof(muEventRecord) == 0x18, "Class is wrong size!");

// The events of one list.
struct muEventList {
    muEventEntry m_entries[0x40];
    u8 m_count;                 // 0x400
    u8 m_pad[3];
};
static_assert(sizeof(muEventList) == 0x404, "Class is wrong size!");

// One model of the menu: the name, the slots of the task it fills (start, end) and the draw priority.
struct muEventModel {
    const char* m_name;
    u8 m_start;
    u8 m_end;
    u8 m_priority;
};

// The window that the second player opens to pick a name.
// HYPOTHESIS: this is a class of the module of the name menus; only the fields the event menu reads are known.
class muEventNameWnd : public gfTask {
public:
    u8 m_unk40[0x5C];
    int m_unk9C;                // 0x9C
    u8 m_unkA0[0x348];
    int m_state;                // 0x3E8
    int m_value;                // 0x3EC the name that was picked
    virtual void vf64();
};

// The menu where the player picks an event (the events of the Wii version: one fighter fights with a rule) and the level.
class muEventTask : public gfTask {
public:
    enum Step {
        Step_Loading = 0,
        Step_Appear = 1,
        Step_SelectEvent = 2,
        Step_SelectLevel = 3,
        Step_Decided = 4,
        Step_Count = 5,
    };

    // One step of the menu: what runs when the step starts, and what runs every frame (it returns the next step).
    struct StepFunc {
        void (*m_init)(muEventTask* task);
        int (*m_main)(muEventTask* task);
    };

    MuObject* m_objects[0x56];                  // 0x40
    nw4r::g3d::G3dObj* m_scnMdls[2];            // 0x198 the models that the panels and the thumbnails are attached to
    int m_step;                                 // 0x1A0
    int m_stepPrev;                             // 0x1A4
    nw4r::g3d::ScnGroup* m_group;               // 0x1A8 the group of the scene that the models are put in
    nw4r::g3d::ResFile m_resFileMenu;           // 0x1AC the models of the menu
    nw4r::g3d::ResFile m_resFileWndA;           // 0x1B0 the window of the name
    nw4r::g3d::ResFile m_resFileWndB;           // 0x1B4 its texts
    muMenuController m_controllers[3];          // 0x1B8 the controller of all the players, then one for each player
    muEventRecord m_records[2][0x40];           // 0x3B0 what each player did in each event of the lists
    muEventList m_lists[2];                     // 0xFB0 the events of the two lists (one player, two players)
    int m_cursor;                               // 0x17B8 the event the cursor is on
    int m_mode;                                 // 0x17BC 0: one player, 1: two players
    MuMsg* m_msg;                               // 0x17C0
    int m_arrowDir;                             // 0x17C4 the direction the cursor moved
    int m_level;                                // 0x17C8
    muEventNameWnd* m_wnds[2];                  // 0x17CC the windows where the players pick a name
    bool m_wndShown[2];                         // 0x17D4
    int m_pad[2];                               // 0x17D8
    int m_name[2];                              // 0x17E0
    bool m_unk17E8;
    bool m_unk17E9;
    int m_wait;                                 // 0x17EC
    gfArchive* m_archive;                       // 0x17F0

    static const muEventModel sModels[13];

    muEventTask(gfArchive* archive);
    static muEventTask* create(nw4r::g3d::ScnGroup* group, const muEventSetting* setting, gfArchive* archive);

    virtual void processDefault();
    virtual ~muEventTask();

    void initProc(nw4r::g3d::ScnGroup* group, const muEventSetting* setting);
    void setEventRecord();
    void createModel();
    void createObjResFile(const muEventModel* models, int count, nw4r::g3d::ResFile* resFile);
    void createPanelExpMdl();
    void initMsg();
    void initDisp();
    void dispPadType(int player, bool force);
    bool nameSelectMainPlayer(int player);
    bool acceptIntruder();
    void makeEventList(muEventList* lists);
    void initPanel(int mode);
    void pasteThumb(int pos, int mode);
    void setCurEventListPos(int pos, int mode);
    int makeHighScoreMsgData(char* out, bool hasScore, u32 score, int mode, bool isTime);
    muEventSetting getScEventSetting();

    static void mainStepLoadingInit(muEventTask* task);
    static int mainStepLoadingMain(muEventTask* task);
    static void mainStepAppearInit(muEventTask* task);
    static int mainStepAppearMain(muEventTask* task);
    static void mainStepSelectEventInit(muEventTask* task);
    static int mainStepSelectEventMain(muEventTask* task);
    static void mainStepSelectLevelInit(muEventTask* task);
    static int mainStepSelectLevelMain(muEventTask* task);
    static void mainStepDecidedInit(muEventTask* task);
    static int mainStepDecidedMain(muEventTask* task);
};
static_assert(sizeof(muEventTask) == 0x17F4, "Class is wrong size!");
