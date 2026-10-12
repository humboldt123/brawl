#pragma once

#include <StaticAssert.h>
#include <gf/gf_archive.h>
#include <gf/gf_pad_status.h>
#include <gf/gf_task.h>
#include <memory.h>
#include <mt/mt_vector.h>
#include <mu/mu_menu_controller.h>
#include <mu/mu_msg.h>
#include <mu/mu_notice_wnd.h>
#include <mu/mu_object.h>
#include <mu/mu_resource_loader.h>
#include <nw4r/g3d/g3d_obj.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <gm/gm_global.h>
#include <stdio.h>
#include <string.h>
#include <types.h>

// GameGlobal::getLanguage(): a member function (the shared header declares it static)
extern "C" int getLanguage__10GameGlobalFv(GameGlobal* global);
// GameGlobal::getName(number, buffer): the names that were saved
extern "C" void fn_8004D6C4(GameGlobal* global, int number, char* buffer);

// What the scene gives to the task.
struct muAdvSelmapTaskParam {
    int m_unk00;                // 0x00 the node that the cursor starts on (0: the default one)
    int m_pad1;                 // 0x04 the pad of the first player
    int m_pad2;                 // 0x08 the pad of the second player (-1: there is none)
    int m_count1;               // 0x0C how many fighters the first player has
    int m_count2;               // 0x10 how many fighters the second player has (-1: there is no second player)
    int m_button;               // 0x14 the button that the menu starts on (0 or below: the map is shown first)
};

// A place of the map. The nodes are the first thing in the data of the adventure mode.
// HYPOTHESIS: the names follow how the menu reads them.
struct muAdvSelmapNode {
    int m_hidden;               // 0x00 1: the place is not on the map
    int m_kind;                 // 0x04 0: the place is not open, 1..4: the kind of the place
    int m_value;                // 0x08 shown by the model of the place
    int m_unk0C;
    int m_unk10;
};

// The data of the adventure mode that the menu reads.
struct muAdvSelmapAdvData {
    muAdvSelmapNode m_nodes[34];        // 0x000
    float m_unk2A8;                     // 0x2A8
    u16 m_mapFlags;                     // 0x2AC the parts of the map that are shown
    u8 m_unk2AE[0x60C - 0x2AE];
    int m_unk60C;                       // 0x60C
    u8 m_unk610[0x48A0 - 0x610];
    u32 m_unk48A0;                      // 0x48A0
    u32 m_unk48A4;                      // 0x48A4
};

// One model of the menu: the name, the slots of the task it fills (start, end) and the draw priority.
struct muAdvSelmapModel {
    const char* m_name;
    u8 m_start;
    u8 m_end;
    u8 m_priority;
};

// A model that other models are attached to: the slots it fills (start, end), the model it is made from and how many
// models can be attached.
struct muAdvSelmapScnMdl {
    u8 m_start;
    u8 m_end;
    u8 m_objectNo;
    u8 m_capacity;
};

// The map of the adventure mode where the player picks the next place (the nodes of the map), scrolls the map and
// opens the menu with its buttons.
// HYPOTHESIS: the names of the fields follow how the steps use them.
class muAdvSelmapTask : public gfTask {
public:
    enum Step {
        Step_Load = 0,
        Step_Appear = 1,
        Step_Select = 2,
        Step_End = 3,
        Step_Count = 4,
    };

    // One step of the menu: what runs when the step starts, and what runs every frame (it returns the next step).
    struct StepFunc {
        void (*m_init)(muAdvSelmapTask* task);
        int (*m_main)(muAdvSelmapTask* task);
    };

    int m_result;                               // 0x40 what the menu did when it ends
    int m_subResult;                            // 0x44
    int m_node;                                 // 0x48 the place that the cursor is on
    int m_button;                               // 0x4C the button of the menu that is chosen
    int m_count1;                               // 0x50
    int m_count2;                               // 0x54
    bool m_unk58;                               // 0x58
    bool m_isMap;                               // 0x59 the map is shown (and not the menu)
    bool m_has2P;                               // 0x5A there is a second player
    int m_pad1;                                 // 0x5C
    int m_pad2;                                 // 0x60
    muMenuController m_controllers[2];          // 0x64
    int m_param3;                               // 0x1B4
    int m_param4;                               // 0x1B8
    int m_wait;                                 // 0x1BC frames in which nothing runs
    MuMsg* m_msg;                               // 0x1C0
    MuObject* m_objects[0xB2];                  // 0x1C4
    nw4r::g3d::G3dObj* m_scnMdls[2];            // 0x48C
    nw4r::g3d::ResFile m_resFileMap;            // 0x494
    nw4r::g3d::ResFile m_resFileScene;          // 0x498
    nw4r::g3d::ResFile m_resFileMsg;            // 0x49C
    nw4r::g3d::G3dObj* m_group;                 // 0x4A0
    int m_step;                                 // 0x4A4
    int m_stepPrev;                             // 0x4A8
    int m_scroll;                               // 0x4AC
    int m_scrollTarget;                         // 0x4B0
    int m_scrollMax;                            // 0x4B4
    bool m_unk4B8;                              // 0x4B8
    bool m_unk4B9;                              // 0x4B9
    bool m_unk4BA;                              // 0x4BA
    int m_startButton;                          // 0x4BC
    int m_markerNo;                             // 0x4C0 the slot of the model that marks the place
    int m_startNode;                            // 0x4C4
    bool m_unk4C8;                              // 0x4C8
    char m_name1[0x80];                         // 0x4C9
    char m_name2[0x80];                         // 0x549
    int m_unk5CC;                               // 0x5CC
    int m_unk5D0;                               // 0x5D0
    gfArchive* m_archive;                       // 0x5D4
    scResourceLoader* m_loader;                 // 0x5D8
    muNoticeWndTask* m_notice;                  // 0x5DC
    void* m_msgData;                            // 0x5E0
    bool m_noticeShown;                         // 0x5E4
    bool m_unk5E5;                              // 0x5E5
    bool m_unk5E6;                              // 0x5E6
    int m_frame;                                // 0x5E8

    muAdvSelmapTask();
    static muAdvSelmapTask* create(const muAdvSelmapTaskParam* param);

    virtual void processDefault();
    virtual ~muAdvSelmapTask();

    void initProc(const muAdvSelmapTaskParam* param);
    void initMsg();
    // Writes the name of a player (the number of the name is given by the scene) with the label of the player ("1P: ", "P2: " ...,
    // by the language) in the text of the slot, or the text of the message when the name was not set.
    void showPlayerName(int player, int number, char* name, int slot, int textNo, char* dst) {
        fn_8004D6C4(g_GameGlobal, number - 1, name);
        char noName[6] = {0x7F, 0x7E, 0x7D, 0x7C, 0x7B, 0};
        char label1[0x1C];
        char label2[0x1C];
        if (getLanguage__10GameGlobalFv(g_GameGlobal) == 0) {
            sprintf(label1, "1P: ");
            sprintf(label2, "2P: ");
        } else {
            sprintf(label1, "P1: ");
            sprintf(label2, "P2: ");
        }
        if (strcmp(noName, name) == 0) {
            m_msg->printIndex(slot, textNo, NULL);
            if (player == 1) {
                m_has2P = true;
            }
        } else if (player == 0) {
            strcat(label1, name);
            m_msg->printf(slot, label1);
            strcpy(dst, label1);
        } else {
            strcat(label2, name);
            m_msg->printf(slot, label2);
            strcpy(dst, label2);
            m_has2P = true;
        }
    }

    // Puts the join frame back to its first animation (the second player left).
    void resetJoinFrame() {
        MuObject* join = m_objects[0x8D];
        join->changeAnimN("MenAdvMap0010_TopN__0");
        join->setFrame(0.0f);
        join->m_modelAnim->setUpdateRate(1.0f);
    }

    int getNextNode(int player, int node);
    int getScrollLimit();
    bool isScrollUp(gfPadStatus* pad);
    bool isScrollDown(gfPadStatus* pad);
    void mainProc();
    void createObjResFile(const muAdvSelmapModel* models, int count, nw4r::g3d::ResFile* resFile);
    void createExpMdl(const muAdvSelmapScnMdl* models, int count);
    void initScene();
    void selectNode(int node);
    void moveMarker(int node);
    void updateArrows();
    void updateButtons(int button);

    static void stepLoadInit(muAdvSelmapTask* task);
    static int stepLoadMain(muAdvSelmapTask* task);
    static void stepAppearInit(muAdvSelmapTask* task);
    static int stepAppearMain(muAdvSelmapTask* task);
    static void stepSelectInit(muAdvSelmapTask* task);
    static int stepSelectMain(muAdvSelmapTask* task);
    static void stepEndInit(muAdvSelmapTask* task);
    static int stepEndMain(muAdvSelmapTask* task);
};
static_assert(sizeof(muAdvSelmapTask) == 0x5EC, "Class is wrong size!");
