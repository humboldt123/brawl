#pragma once

// The "clear getter" screen (sc_clear_getter.pac): after a mode was cleared the player breaks boxes of a 4 x 32 grid with
// a hammer and gets the prize behind them (a sticker, a trophy, a CD, a stage or a hidden item).
//
// Layout evidence (unit sora/mu/mu_clear_get, 0x800BA484-0x800BEEB8): the constructor 0x800BA7C0 stores every field,
// the caller (scClearGetter::start) allocates 0x1308 bytes, the vtable at 0x80456CAC overrides processAnim (slot 2)
// and the destructor (slot 22). Offsets in names are byte offsets from the start of the task.

#include <StaticAssert.h>
#include <gf/gf_archive.h>
#include <gf/gf_file_io_handle.h>
#include <gf/gf_task.h>
#include <gm/gm_global.h>
#include <mt/mt_vector.h>
#include <mu/mu_menu_controller.h>
#include <mu/mu_menuroot.h>
#include <mu/mu_object.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <nw4r/g3d/g3d_scnmdlexpand.h>
#include <stddef.h>
#include <ty/ty_fig_listmng.h>
#include <types.h>

class MuMsg;
class muObjProcessTask;

// One record of the muClearGetDataList table of the file (the prize behind one box). 0x28 bytes, a negative first word
// ends the table.
struct muClearGetData {
    int m_index;            // 0x00 HYPOTHESIS: the number of the box (the table ends at the first negative one)
    int m_kind;             // 0x04 the kind of prize, as gmActionOpenClearGet takes it: 0 sticker, 1 trophy, 2 CD (BGM), 3 hidden item,
                            //      4 stage, 5 hidden item (printPrizeData and initMdlData switch on it to pick the preview model)
    int m_reward;           // 0x08 what is given: sticker number, trophy id (low 16 bits), BGM id, ...
    u32 m_nameMsg;          // 0x0C message index of the title of the prize (printed on line 0)
    u32 m_infoMsg;          // 0x10 message index of the text of the prize (printed on line 1)
    u32 m_hintMsg;          // 0x14 message index of the text that is shown instead while the hint condition is not met
    u8 m_hasHammer;         // 0x18 HYPOTHESIS: the box also gives one hammer (more tries): hammer count + 1 when it is opened
    u8 unk19[3];
    int m_hintKind;         // 0x1C HYPOTHESIS: what the hint condition checks: 1 boss battle / all-star menu, 2 fighter (hint arg * 3 + 1 is the bit of the fighter flags), 3 stage (bit of the stage flags), 4 event (arg - 1)
    int m_hintArg;          // 0x20 HYPOTHESIS: which fighter / stage / event the hint condition checks
    u8 m_openable;          // 0x24 HYPOTHESIS: 0 plays the "cannot" sound when the prize is chosen (state 3)
    u8 unk25[3];
};
static_assert(sizeof(muClearGetData) == 0x28, "Wrong size for class!");

class muClearGetDataManager {
public:
    static const int BoxNum = 128;

    muClearGetData* m_list;     // 0x00 the "muClearGetDataList" symbol of the file
    u32 m_count;                // 0x04 records of the list, at most BoxNum
    // 0x08 per box: 0 not reachable yet, 1 closed box that can be hit, 2 opened (setOpenState); copy of the save record bytes
    int m_openState[BoxNum];

    ~muClearGetDataManager();
    void setData(u8* buffer, u32 size);
    void setOpenState(u32 index);
    // HYPOTHESIS: inlined accessor that copies the open states into the save record.
    void saveFlags() {
        u8* flags = g_GameGlobal->m_record->unk1528;
        for (int i = 0; i < BoxNum; i++) {
            flags[i] = m_openState[i];
        }
    }
};
static_assert(sizeof(muClearGetDataManager) == 0x208, "Wrong size for class!");

class muClearGetTask : public gfTask {
public:
    // HYPOTHESIS: names for the states of m_state (the jump table at 0x80456AAC of processAnim has these ten cases).
    enum State {
        State_Load = 0,         // wait for the data and the file, unpack the .pac, make the scene
        State_Init = 1,         // bind the textures of the trophies, wait for the sound group, initMdlData
        State_Intro = 2,        // the panel comes in (frame 9 of the panel's node animation)
        State_Select = 3,       // the player moves the cursor over the boxes / the hammers
        State_Break = 4,        // the hit box breaks, its prize and the neighbours change (setOpenState)
        State_Hit = 5,          // the hammer hits (frame 17 of the hammer animation)
        State_Move = 6,         // the panel scrolls to the next box of m_breakQueue
        State_Grab = 7,         // the hammer flies to the box (4 frames)
        State_Confirm = 8,      // yes / no window
        State_Get = 9,          // the prize is shown, a button leaves it
        State_End = 10,         // finished (isEnd)
    };

    static const int ObjNum = 0x299;
    static const int GroupNum = 0x84;
    static const int BoxNum = 128;
    static const int HammerMax = 5;
    static const int FigTexNum = 65;

    // 0x00-0x3F: gfTask (the vptr is at 0x3C, the table is at 0x80456CAC)
    MenuRoot* m_menuRoot;               // 0x40 the scene of the pac (fn_800AFBFC makes it, its ScnRoot at 0x48 gets the models)
    int m_state;                        // 0x44 State; processAnim switches on it, isEnd tests >= State_End
    int m_stateFrame;                   // 0x48 frames in the state (processAnim adds 1 at its end)
    muMenuController m_controller;      // 0x4C 0xA8 bytes (its constructor runs first, init(m_decisionPad) later)
    int m_activePad;                    // 0xF4 HYPOTHESIS: the pad whose stick moves the panel (starts as g_GameGlobal->m_setRule->m_ctrlNamePlayerNo, 0-7, then the pad of A)
    int m_decisionPad;                  // 0xF8 the constructor argument (gmGetMenuDecisionPad); rumble and controller input, clamped 0-7
    int m_soundGroup;                   // 0xFC sndSystem::loadSoundGroup(0x72), freed by the destructor

    // 0x100 the 0x299 objects that muObjProcessTask (m_objTask) updates. One array, and the same words by role.
    union {
        MuObject* m_objs[ObjNum];
        struct {
            MuObject* m_bg;                 // 0x100 [0] MenClearBg_TopN
            MuObject* m_panelTop;           // 0x104 [1] MenClearPanel0000_TopN (the text window of the prize, the base of the preview group)
            MuObject* m_panelGrid;          // 0x108 [2] MenClearPanel0001_TopN (the grid of the boxes; nodes pos%02d, box%d; rotates)
            MuObject* m_panelHammer;        // 0x10C [3] MenClearPanel0002_TopN (the row of hammers; nodes pos95..)
            MuObject* m_cursor;             // 0x110 [4] MenClearCursor_TopN
            MuObject* m_kindObj;            // 0x114 [5] MenClearKind_TopN (frame = kind + 1 of the prize)
            MuObject* m_getObj;             // 0x118 [6] MenClearGet_TopN
            MuObject* m_hammers[HammerMax]; // 0x11C [7-11] MenClearHammer_TopN, the hammers that are left
            MuObject* m_hammerMarks[HammerMax]; // 0x130 [12-16] MenClearHammer_TopN, the mark on a box that hides a hammer
            MuObject* m_notice;             // 0x144 [17] MenClearNotice_TopN (the yes / no window)
            MuObject* m_previews[7];        // 0x148 [18-24] MenClearPrize0000, 0001, 0011, 0020, 0030, 0040, 0050 (previews; [18], [19] are the trophy)
            MuObject* m_boxA[BoxNum];       // 0x164 [25..] MenClearBox0000_TopN per box
            MuObject* m_boxB[BoxNum];       // 0x364 MenClearBox0001_TopN per box
            MuObject* m_boxC[BoxNum];       // 0x564 MenClearBox0002_TopN per box
            MuObject* m_prizeFrame[BoxNum]; // 0x764 per box: Prize0010 (kind 0) / Prize0000 (kind 1), else NULL
            MuObject* m_prizeItem[BoxNum];  // 0x964 per box: Prize0011 / 0001 / 0020 / 0030 / 0040 / 0050 by kind
        };
    };

    // 0xB64 the scene groups. [0]-[3] are the groups of the panels, [4 + box] the prize of each box.
    union {
        nw4r::g3d::ScnMdlExpand* m_groups[GroupNum];
        struct {
            nw4r::g3d::ScnMdlExpand* m_previewGroup;    // 0xB64 [0] on m_panelTop (4 children: the kind and the preview, node pos01/pos02)
            nw4r::g3d::ScnMdlExpand* m_gridGroup;       // 0xB68 [1] on m_panelGrid (0x182 children: boxes, cursor, hammer marks)
            nw4r::g3d::ScnMdlExpand* m_hammerGroup;     // 0xB6C [2] on m_panelHammer (6 children: the hammers)
            nw4r::g3d::ScnMdlExpand* m_figureGroup;     // 0xB70 [3] on m_previews[0] (the trophy preview)
            nw4r::g3d::ScnMdlExpand* m_prizeGroups[BoxNum]; // 0xB74 [4 + box] on m_prizeFrame, with m_prizeItem as its child (kinds 0 and 1)
        };
    };

    muObjProcessTask* m_objTask;        // 0xD74 muObjProcessTask::create(0x2a); its list is m_objs
    MuMsg* m_msg;                       // 0xD78 MuMsg::create(1, 0x2a, 0x2b), a 0x400 byte buffer, 5 lines
    nw4r::g3d::ResFile m_resFile;       // 0xD7C the models of the pac
    nw4r::g3d::ResFile m_figTexRes[FigTexNum]; // 0xD80 the card textures of the trophies, indexed by getBilTexFileIdxFigId
    muClearGetDataManager m_dataMgr;    // 0xE84 0x208 bytes
    tyFigListDataManager* m_figMng;     // 0x108C constructor argument (the trophy list)
    u32 m_row;                          // 0x1090 row of the box under the cursor (0-31, starts at 0x10)
    u32 m_column;                       // 0x1094 column of the box under the cursor (0-3, starts at 2); box = m_column + m_row * 4
    bool m_cursorOnGrid;                // 0x1098 true: the cursor is in the grid; false: on the hammers (setCursor)
    u8 unk1099[3];
    int m_yesNo;                        // 0x109C the answer of the window (0 yes, 1 no; starts at 1)
    u32 m_scrollRow;                    // 0x10A0 HYPOTHESIS: the row at the top of the scrolled grid (setPosRot, starts at 0x10)
    bool m_scrolling;                   // 0x10A4 the panel is still turning to m_scrollAngle
    u8 unk10A5[3];
    u32 m_breakQueue[BoxNum];           // 0x10A8 the boxes to break, in order (setBreakBoxId appends)
    u32 m_breakCount;                   // 0x12A8
    float m_scrollAngle;                // 0x12AC target rotation of m_panelGrid, in degrees
    u32 m_hammerSel;                    // 0x12B0 the hammer that is chosen
    u32 m_hammerNum;                    // 0x12B4 hammers that are left (kept in the save record byte 0x15A8)
    u32 m_hammerBox[HammerMax];         // 0x12B8 the boxes that hide a hammer (initMdlData collects them from m_hammerMarks)
    Vec3f m_hammerPos;                  // 0x12CC HYPOTHESIS: where a hammer rests / the offset of the hammer (0, 0, 8)
    bool m_hammerHeld;                  // 0x12D8 a hammer is in the grid
    bool m_unk12D9;                     // 0x12D9 only cleared by the constructor
    u8 unk12DA[6];
    int m_releaseMode;                  // 0x12E0 releaseHammer's argument: 0 put back, 1 new hammer, 2 chosen hammer
    bool m_hammerClosing;               // 0x12E4 calcHammerClose is running
    bool m_hammerCancelling;            // 0x12E5 calcHammerCancel is running
    u8 unk12E6[2];
    u32 m_releaseBox;                   // 0x12E8 the box at which the hammer was released
    bool m_unk12EC;                     // 0x12EC only cleared by the constructor
    u8 unk12ED[3];
    int m_bgmId;                        // 0x12F0 the CD that plays (0 for none)
    gfArchive* m_pacArchive;            // 0x12F4 the unpacked .pac
    gfArchive* m_dataArchive;           // 0x12F8 its first file (models 2, messages (1, 0) and the data (1, 1))
    gfFileIOHandle m_fileHandle;        // 0x12FC the read request of /menu2/sc_clear_getter.pac
    void* m_fileImage;                  // 0x1300 the unpacked image (gfHeapManager::alloc(0x2b))
    int m_mode;                         // 0x1304 constructor argument: 1 breaks the boxes of the queue at the start, 0 plays the intro

    muClearGetTask(int mode, tyFigListDataManager* figMng, int decisionPad);    // HYPOTHESIS: parameters from scClearGetter::start
    virtual void processAnim();                                                 // vtable slot 2 (0x800BACA4)
    virtual ~muClearGetTask();                                                  // vtable slot 22 (0x800BC748)

    bool isEnd();
    void setCursor();
    void releaseHammer(int);
    u32 setBreakBoxId(u32);
    void setPosRot(u32);
    void setPrizeModel(u32, int);
    bool calcHammerClose();
    bool calcHammerCancel();            // 0x800BDA38 HYPOTHESIS: returns false when the animation is over
    void printPrizeData(int state);     // 0x800BCA9C state of the box: 0 hidden ("?" on both lines), 1 closed ("?" + the text or hint), 2 opened (preview, title and text); 3: take it from m_dataMgr for the box under the cursor
    void initMdlData();                 // 0x800BDFC4 make the models and the scene groups, set the animations, print the texts
};
static_assert(sizeof(muClearGetTask) == 0x1308, "Wrong size for class!");
static_assert(offsetof(muClearGetTask, m_menuRoot) == 0x40, "m_menuRoot");
static_assert(offsetof(muClearGetTask, m_controller) == 0x4C, "m_controller");
static_assert(offsetof(muClearGetTask, m_soundGroup) == 0xFC, "m_soundGroup");
static_assert(offsetof(muClearGetTask, m_objs) == 0x100, "m_objs");
static_assert(offsetof(muClearGetTask, m_groups) == 0xB64, "m_groups");
static_assert(offsetof(muClearGetTask, m_objTask) == 0xD74, "m_objTask");
static_assert(offsetof(muClearGetTask, m_figTexRes) == 0xD80, "m_figTexRes");
static_assert(offsetof(muClearGetTask, m_dataMgr) == 0xE84, "m_dataMgr");
static_assert(offsetof(muClearGetTask, m_figMng) == 0x108C, "m_figMng");
static_assert(offsetof(muClearGetTask, m_breakQueue) == 0x10A8, "m_breakQueue");
static_assert(offsetof(muClearGetTask, m_breakCount) == 0x12A8, "m_breakCount");
static_assert(offsetof(muClearGetTask, m_hammerBox) == 0x12B8, "m_hammerBox");
static_assert(offsetof(muClearGetTask, m_hammerPos) == 0x12CC, "m_hammerPos");
static_assert(offsetof(muClearGetTask, m_hammerHeld) == 0x12D8, "m_hammerHeld");
static_assert(offsetof(muClearGetTask, m_releaseMode) == 0x12E0, "m_releaseMode");
static_assert(offsetof(muClearGetTask, m_pacArchive) == 0x12F4, "m_pacArchive");
static_assert(offsetof(muClearGetTask, m_fileHandle) == 0x12FC, "m_fileHandle");
static_assert(offsetof(muClearGetTask, m_mode) == 0x1304, "m_mode");
