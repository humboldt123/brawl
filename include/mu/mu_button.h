#pragma once

#include <StaticAssert.h>
#include <gf/gf_file_io_handle.h>
#include <gf/gf_task.h>
#include <memory.h>
#include <mu/mu_file_io_handle.h>
#include <mu/mu_menu_controller.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <types.h>

class MuMsg;
struct ipButtonConfig;
class MuObject;
class scButton;
namespace nw4r {
namespace g3d {
class ScnMdlExpand;
} // namespace g3d
} // namespace nw4r

// The button settings menu of scButton: the player picks a controller kind, then what each button does, and can try
// the buttons in a practice fight before the settings are stored for the name.
class muButtonTask : public gfTask {
public:
    // The controller kinds, in the order of the settings (the pad types of gfPadStatus map to them through sPadKinds).
    enum PadKind {
        PadKind_Gc = 0,
        PadKind_Nunchuk = 1,
        PadKind_WiiRemote = 2,
        PadKind_Classic = 3,
        PadKind_Num = 4,
    };

    enum {
        ObjNum = 35,
        GroupNum = 5,
        ButtonMax = 12,
        ListMax = 5,     // the most actions the list of free actions shows
        ActionNum = 13,  // the actions a button can take (an action list ends with ActionNum)
        SelectAnimNum = 10,
    };

    // The cursor positions of the screen: the buttons (0 ... button count - 1), then these.
    enum {
        Cursor_FlickJump = 20, // HYPOTHESIS: the jump-with-the-stick switch
        Cursor_Shake = 21,     // HYPOTHESIS: the nunchuk's shake switch
        Cursor_Restore = 22,   // the three buttons under the controller: restore the defaults,
        Cursor_Try = 23,       // try the buttons in a practice fight,
        Cursor_Store = 24,     // store the settings
    };

    // A cursor position of a controller and the positions the cursor moves to.
    struct CursorNode {
        u8 m_cursor;               // 0x0
        int m_unk4;
        u8 m_links[4];             // 0x8 HYPOTHESIS: the positions above, right, below, left
    };

    struct CursorTable {
        const CursorNode* m_nodes; // 0x0
        int m_num;                 // 0x4
    };

    // How the menu shows one controller kind.
    struct PadInfo {
        u8** m_actionLists;        // 0x0 for each button the actions it can take, each list ending with ActionNum
        u8 m_buttonNum;            // 0x4
        u8 m_stickKind;            // 0x5 0: no switches, 1: the flick-jump switch, 2: also the shake switch
        u8 m_padObj;               // 0x6 the object of the controller model (the button group hangs on its nodes)
        u8 m_group;                // 0x7 the group of the button texts and the cursor
        u8 m_cursorObj;            // 0x8 HYPOTHESIS: the object that marks the selected button
        u8 m_pushObj;              // 0x9 the object that shows the pushed button in the practice
        u8 m_unkA;                 // 0xA
        u8 m_unkB;                 // 0xB
        u8 m_pushLine;             // 0xC the text line of the pushed button in the practice
    };

    // The texts of an action: its name and its explanation.
    struct ActionMsg {
        u8 m_name;
        u8 m_expl;
    };

    // The settings of one controller kind as the menu shows them: what each button on the screen does, in the order of
    // the screen (it differs from the order of ipButtonConfig), and the switches.
    struct Config {
        u8 m_buttons[12];          // 0x0
        u8 m_flickJump : 1;        // 0xC jump with the stick (bit 7 of the flags of ipButtonConfig; bit 6 for the nunchuk)
        u8 m_flag6 : 1;            // 0xC HYPOTHESIS: the nunchuk's shake setting (bit 7 of its flags; ipPadConfig reads it
                                   // with the strength of the shake)
    };

    // The work of the task (allocated by create).
    struct Work {
        scButton* m_scene;         // 0x000
        nw4r::g3d::ResFile m_resButton; // 0x004 /menu/option/Button.brres
        nw4r::g3d::ResFile m_resScene;  // 0x008 the camera scene (DefaultScene / DefaultSceneWide)
        void* m_msgData;           // 0x00C /menu/msg/button.msbin
        MuObject* m_objs[ObjNum];  // 0x010
        nw4r::g3d::ScnMdlExpand* m_groups[GroupNum]; // 0x09C
        int m_step;                // 0x0B0 the index in sSteps
        int m_padKind;             // 0x0B4 PadKind
        muMenuController m_controller; // 0x0B8
        int m_selectAnim;          // 0x160 the animation of the controller choice (sPadAnims ...)
        MuMsg* m_msg;              // 0x164
        Config m_configs[PadKind_Num]; // 0x168 the settings of the name, one for each kind
        Config m_edit;             // 0x19C the settings that are being changed
        int m_cursor;              // 0x1AC the cursor position on the button screen
        int m_listIndex;           // 0x1B0 the action of the button that is being changed, in its action list
        u8 m_isListOpen;           // 0x1B4 the list of free actions is shown
        int m_dlgKind;             // 0x1B8 the dialog that is open (0: none)
        u8 m_dlgYes;               // 0x1BC the cursor of the restore and store dialogs is on yes (they open on no)
        s16 m_pushWait;            // 0x1BE the frames the pushed button stays shown in the practice
        s16 m_dlgWait;             // 0x1C0 the frames the dialog of the stored settings stays open
        int m_contNo;              // 0x1C4 the pad that tries the buttons in the practice
        int m_practicePadKind;     // 0x1C8 the controller kind of that pad when the practice started
        u8 m_isSubStickOn;         // 0x1CC the C stick was tilted on the last frame (it acts as one more button)
        int m_exitWait;            // 0x1D0 the frames before the scene is told that the menu ended
        u8 m_isContSet;            // 0x1D4 the practice fighter has the pad (scButton::setContNo succeeded)
    };

    // A model of the menu: the node name, the objects it fills (m_objs[m_first ... m_last - 1], at least one) and the
    // draw priority.
    struct ObjData {
        const char* m_name;        // 0x0
        u8 m_first;                // 0x4
        u8 m_last;                 // 0x5
        u8 m_priority;             // 0x6
    };

    typedef void (*StepFunc)(muButtonTask* task);
    struct Step {
        StepFunc m_init;
        StepFunc m_main;
    };

    Work* m_work;                  // 0x40
    muFileIOHandle m_handles[3];   // 0x44 the models, the scene, the texts

    static const u8 sDefaultButtons[PadKind_Num][ButtonMax];
    static const CursorNode sGcCursors[15];
    static const CursorNode sNunchukCursors[16];
    static const CursorNode sWiiRemoteCursors[11];
    static const CursorNode sClassicCursors[16];
    static const CursorTable sCursorTables[PadKind_Num];
    static const PadInfo sPadInfos[PadKind_Num];
    static const ActionMsg sActionMsgs[ActionNum];
    static const ObjData sObjData[23];
    static const u8 sPadKinds[4];
    static const Step sSteps[];
    static const u8 sPadAnims[PadKind_Num];
    static const u8 sNextPadAnims[PadKind_Num];
    static const u8 sPrevPadAnims[PadKind_Num];
    static const u8 sAnimsAfter[SelectAnimNum];
    static const u8 sTextColors[3][3];
    static const u8 sListActions[ListMax];
    // For each button of a controller kind the pad bits (gfPadButtons) it is read from; with the top bit set all the
    // bits must be held (a combination such as A+B) and one of them pushed.
    static const u32 sGcMasks[11];
    static const u32 sNunchukMasks[11];
    static const u32 sWiiRemoteMasks[8];
    static const u32 sClassicMasks[12];
    static const u32* sButtonMasks[PadKind_Num];

    muButtonTask();
    static muButtonTask* create(scButton* scene, Heaps::HeapType heap);
    virtual void processDefault();
    virtual ~muButtonTask();

    void getNameButtonConfig(int no);
    // The settings of the four controller kinds in the order of ipButtonConfig (the reverse of getNameButtonConfig).
    ipButtonConfig makeButtonConfig(const Config* configs);
    void initWork();
    void initMsg(void* msgData);
    void createModel(nw4r::g3d::ResFile* resFile);
    void createObjResFile(const ObjData* data, int count, nw4r::g3d::ResFile* resFile);
    void setSelectAnim(int anim);
    void setupButtons(int padKind);
    void setAssignAnim(int anim);
    void printButton(int padKind, int button, int line, int action, bool markDuplicates);
    void setButtonCursor(int padKind, int cursor);
    int getFreeActions(int padKind, int* actions);
    void openActionList(int title, int frame, const int* actions, int num);
    void openDlg(int kind, int msg);
    // BYTES: called with bl from initStepButtonSelect (MWCC folds it into its callers without the attribute).
    void printExplByButtonCursor(int cursor) __attribute__((never_inline));
    int getPushedButton(int padNo);

    static void initStepLoading(muButtonTask* task);
    static void mainStepLoading(muButtonTask* task);
    static void initStepSelectAppear(muButtonTask* task);
    static void mainStepSelectAppear(muButtonTask* task);
    static void initStepSelect(muButtonTask* task);
    static void mainStepSelect(muButtonTask* task);
    static void initStepButtonAppear(muButtonTask* task);
    static void mainStepButtonAppear(muButtonTask* task);
    static void initStepButtonSelect(muButtonTask* task);
    static void mainStepButtonSelect(muButtonTask* task);
    static void initStepButtonAssign(muButtonTask* task);
    static void mainStepButtonAssign(muButtonTask* task);
    static void initStepButtonLeave(muButtonTask* task);
    static void mainStepButtonLeave(muButtonTask* task);
    static void initStepRestoreDlg(muButtonTask* task);
    static void mainStepRestoreDlg(muButtonTask* task);
    static void initStepTryDlg(muButtonTask* task);
    static void mainStepTryDlg(muButtonTask* task);
    static void initStepStoreDlg(muButtonTask* task);
    static void mainStepStoreDlg(muButtonTask* task);
    static void initStepToPractice(muButtonTask* task);
    static void mainStepToPractice(muButtonTask* task);
    static void initStepPractice(muButtonTask* task);
    static void mainStepPractice(muButtonTask* task);
    static void initStepStoredDlg(muButtonTask* task);
    static void mainStepStoredDlg(muButtonTask* task);
    static void initStepExit(muButtonTask* task);
    static void mainStepExit(muButtonTask* task);
};
static_assert(sizeof(muButtonTask::Config) == 0xD, "Class is wrong size!");
static_assert(sizeof(muButtonTask::Work) == 0x1D8, "Class is wrong size!");
static_assert(sizeof(muButtonTask) == 0x50, "Class is wrong size!");
