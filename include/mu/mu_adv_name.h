#pragma once

#include <StaticAssert.h>
#include <gf/gf_file_io_handle.h>
#include <gf/gf_pad_status.h>
#include <gf/gf_task.h>
#include <memory.h>
#include <mu/mu_menu_controller.h>
#include <mu/mu_msg.h>
#include <mu/mu_object.h>
#include <nw4r/g3d/g3d_obj.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <types.h>

// What the scene gives to the task.
struct muAdvNameTaskParam {
    int m_pad0;             // 0x00 the pad of the first player
    int m_pad1;             // 0x04 the pad of the player of the first slot (-1: there is none)
    int m_pad2;             // 0x08 the pad of the player of the second slot (-1: there is none)
    int m_unkC;             // 0x0C 0: the first player only, 1: the second player only, otherwise both
};

// One model of the menu: the name, the slots of the task it fills (start, end) and the draw priority.
struct muAdvNameModel {
    const char* m_name;
    u8 m_start;
    u8 m_end;
    u8 m_priority;
};

// A model with a place to attach other models to: the slots of the task it fills (start, end), the model it is made from
// and how many models can be attached.
struct muAdvNameScnMdl {
    u8 m_start;
    u8 m_end;
    u8 m_objectNo;
    u8 m_capacity;
};

// The adventure mode menu where the players pick a name from the list of the names that were saved (or make a new one):
// one list for each player, with a cursor, and a question that asks whether the name is the right one.
// HYPOTHESIS: the names of the fields follow how the steps use them.
class muAdvNameTask : public gfTask {
public:
    enum Step {
        Step_Load = 0,
        Step_StCont = 1,        // asks for what is done with the saved data
        Step_Appear = 2,
        Step_Select = 3,        // the lists of the names
        Step_YesNo = 4,         // asks whether the names are the right ones
        Step_Back = 5,
        Step_1POnly = 6,
        Step_2POnly = 7,
        Step_Count = 8,
    };

    // One step of the menu: what runs when the step starts, and what runs every frame (it returns the next step).
    struct StepFunc {
        void (*m_init)(muAdvNameTask* task);
        int (*m_main)(muAdvNameTask* task);
    };

    // The data of one player: the controller and the states of the three parts of the menu (the list, the question and the
    // first question).
    struct PlayerWork {
        muMenuController m_controller;
        int m_unkA8;
        int m_listState;                    // 0xAC 0: choosing, 1: the choice is shown, 2: it is shown, 3: it is done
        int m_yesNoState;                   // 0xB0
        int m_stContState;                  // 0xB4

    };

    int m_result[2];                            // 0x40 what each player did (2: went back, 3: left)
    int m_choice;                               // 0x48 the answer of the question (0: yes, 1: no)
    int m_stCont;                               // 0x4C the choice of the first question
    int m_sel[2];                               // 0x50 what the player decided
    int m_sel2[2];                              // 0x58
    muMenuController m_controller;              // 0x60 the controller of the first player
    int m_wait;                                 // 0x108 frames in which the pads are not read
    int m_cursor[2];                            // 0x10C the line of the list that the cursor is on
    int m_scroll[2];                            // 0x114 how far the list is scrolled
    int m_nameCount;                            // 0x11C how many names there are
    int m_pad0;                                 // 0x120
    bool m_is2P;                                // 0x124 both players are in the menu
    u8 m_unk125[7];
    MuMsg* m_msg;                               // 0x12C the texts of the lists
    MuObject* m_objects[9];                     // 0x130
    nw4r::g3d::G3dObj* m_scnMdls[3];            // 0x154
    nw4r::g3d::ResFile m_resFile;               // 0x160
    int m_unk164;                               // 0x164
    int m_step;                                 // 0x168
    int m_stepPrev;                             // 0x16C
    PlayerWork m_players[2];                    // 0x170
    int m_unk2E0;                               // 0x2E0
    u8 m_unk2E4[0x2A];
    char m_name1[0x15];                         // 0x30E the name that the first player picked
    char m_name2[0x15];                         // 0x323 the name that the second player picked
    int m_nameSel1;                             // 0x338 the number of that name
    int m_nameSel2;                             // 0x33C
    gfFileIOHandle m_handles[2];                // 0x340 the models, the texts
    bool m_loaded[2];                           // 0x348
    int m_unk34C;                               // 0x34C
    bool m_started;                             // 0x350 the models are in the scene
    bool m_unk351;
    bool m_flag352;                             // 0x352 a step wants to go back
    bool m_flag353;                             // 0x353

    muAdvNameTask();
    static muAdvNameTask* create(const muAdvNameTaskParam* param);

    virtual void processDefault();
    virtual ~muAdvNameTask();

    void initProc(const muAdvNameTaskParam* param);
    void initMsg();
    void dispStr(int player);
    void controllProc();
    void panelOnlyCtrl(int player, u32 buttons, gfPadStatus pad);
    void createObjResFile(const muAdvNameModel* models, int count, nw4r::g3d::ResFile* resFile);
    void createExpMdl(const muAdvNameScnMdl* models, int count);
    void drawCursor(int player, MuObject* cursor);

    static void mainStepLoadInit(muAdvNameTask* task);
    static int mainStepLoadMain(muAdvNameTask* task);
    static void mainStepStContInit(muAdvNameTask* task);
    static int mainStepStContMain(muAdvNameTask* task);
    static void mainStepAppearInit(muAdvNameTask* task);
    static int mainStepAppearMain(muAdvNameTask* task);
    static void mainStepSelectInit(muAdvNameTask* task);
    static int mainStepSelectMain(muAdvNameTask* task);
    static void mainStepYesNoInit(muAdvNameTask* task);
    static int mainStepYesNoMain(muAdvNameTask* task);
    static void mainStepBackInit(muAdvNameTask* task);
    static int mainStepBackMain(muAdvNameTask* task);
    static void mainStep1POnlyInit(muAdvNameTask* task);
    static int mainStep1POnlyMain(muAdvNameTask* task);
    static void mainStep2POnlyInit(muAdvNameTask* task);
    static int mainStep2POnlyMain(muAdvNameTask* task);
};
static_assert(sizeof(muAdvNameTask) == 0x354, "Class is wrong size!");
