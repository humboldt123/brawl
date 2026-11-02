#pragma once

// HYPOTHESIS: layout of ifVsResultTask, the task of the melee (VS) result screen (if_vs_result_task.o), recovered from
// the constructor (0x800E5858), destructor (0x800E7CA8), loadResource, processAnim, initMdlData, subPlayerControl and
// the allocation in stOperatorRuleVsResult::startOperator (new of 0x44AC bytes). Every offset below is proven by at
// least one access in the unit; names without evidence are m_unkNNN (NNN = hex byte offset). The class derives from
// gfTask directly (the constructor calls gfTask::gfTask(name, Category_Info, 14, 6, true) and then stores its own vptr
// at 0x3C), and it shares no base class with ifSimpleResultTask.
//
// Overrides (vtable lbl_8045AF08, everything else is gfTask's): processAnim (slot 2), init (slot 21, an empty
// function) and the destructor (slot 22). The rtti pair lives in .sdata at lbl_8059E2E8.

#include <StaticAssert.h>
#include <gf/gf_file_io_handle.h>
#include <gf/gf_task.h>
#include <ms/ms_tex_obj.h>
#include <mu/mu_object.h>
#include <mu/mu_scn_mdl_expand.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <types.h>

class gfArchive;
class IfSnapSaveTask;
class Message;
class MuMsg;
class muNoticeWndTask;
class muWifiCntWndTask;
class nteFileDataReplay;
struct gfPadStatus;
namespace nw4r { namespace g3d { class ScnGroup; } }

class ifVsResultTask : public gfTask {
public:
    // The step of the screen, m_state (the switch of processAnim; its jump table is jumptable_8045AC68).
    // HYPOTHESIS: names, from what each case of processAnim does; the numbers are certain.
    enum State {
        State_Load = 0,        // waits for the files (m_fileHandles) and the menu sound, then binds them and runs initMdlData
        State_PanelOpen = 1,   // the panel animation runs up to frame 19
        State_Intro = 2,       // plays to frame 230 (setWinnerName at frame 106); A/Start skips to 3
        State_Wait = 3,        // 120 frames of standby, A/Start goes on to 4 (and prints the hit panels)
        State_Main = 4,        // soundProcess, subPanelControl and subPlayerControl for the four players
        State_ReplaySave = 5,  // waits for the replay save task (m_replaySaveTask), then back to 4
        State_Unk6 = 6,        // Wifi: waits for the players, then isEndMatchingEnd -> 0xB or 0xC
        State_Unk7 = 7,        // Wifi: setContinueNextGame(true) -> 8
        State_Unk8 = 8,        // Wifi: waits for isReadyNextGame -> 0xB, 600 frames of timeout -> 9
        State_Unk9 = 9,        // Wifi: setContinueNextGame(false) -> 10
        State_Unk10 = 10,      // Wifi: isEndMatchingEnd -> 0xB or 0xC
        State_Closing = 11,    // 10 frames, then 0xC
        State_End = 12,        // finished
    };

    // The indices into m_objects (0x124 entries). The names are the model nodes that MuObject::create gets in
    // initMdlData (the strings are "InfResult<Name>_TopN").
    enum ObjectIndex {
        Obj_Panel = 0,            // 0xE8  "InfResultPanel_TopN"
        Obj_Mark = 1,             // 0xEC  "InfResultMarkNocon_TopN" or "InfResultMark%02d_TopN" (the winner's mark)
        Obj_HitNum = 2,           // 0xF0  "InfResultHitnum_TopN"
        Obj_Mvp = 3,              // 0xF4  "InfResultMvp_TopN"
        Obj_Replay = 4,           // 0xF8  [4] "InfResultReplay_TopN", only when m_unk444C and the player has a controller
        Obj_PlayerPanel = 8,      // 0x108 [4] the row of a player: "InfResultRank%d_TopN" (Rank5 / Rank6: no player / no contest)
        Obj_Charb = 12,           // 0x118 [4] "InfResultCharb_TopN", the character bust (texture from m_resFiles[1 + n])
        Obj_PlayerMark = 16,      // 0x128 [4] "InfResultMark%02d_TopN", the stock mark of the character
        Obj_Contl = 20,           // 0x138 [4] "InfResultContl_TopN"
        Obj_Mask = 24,            // 0x148 [4] "InfResultMask_TopN"
        Obj_PlayerBlock = 28,     // 0x158 4 x PlayerNodes (0x42 entries each): Spos[11], then Stockface[55]
        Obj_PlayerBlockSize = 66, // entries of PlayerNodes
        Obj_Count = 0x124,
    };

    // HYPOTHESIS: local name; the nodes of one player's detail rows, created in initMdlData:
    // "InfResultSpos_TopN" x11 (the positions that the row scrolls over) and "InfStockface_TopN" x55 (the text
    // lines of the table that drawExt1 and storeTexExt2 fill with the textures in m_texObjs).
    struct PlayerNodes {
        MuObject* m_spos[11];       // 0x00
        MuObject* m_stockFace[55];  // 0x2C
    };

    // The number of msTexObj per player (storeTexExt2 stores up to this many: 50, +6 for the second rule, +2 for teams).
    enum { TexObjsPerPlayer = 58 };
    enum { PlayerMax = 4 };
    enum { FileHandleCount = 12 };

    // HYPOTHESIS: the ten ResFile of the screen (m_resFiles[n]). The constructor builds them as 1 + 4 + 5 members (one,
    // an array of four, then five single ones), which a plain ResFile[10] does not reproduce; operator[] keeps the
    // numbering of the notes below.
    struct ResFileSet {
        nw4r::g3d::ResFile m_result;       // [0]
        nw4r::g3d::ResFile m_busts[4];     // [1..4]
        nw4r::g3d::ResFile m_stockFace;    // [5]
        nw4r::g3d::ResFile m_camera;       // [6]
        nw4r::g3d::ResFile m_noticeWnd;    // [7]
        nw4r::g3d::ResFile m_wifiCount;    // [8]
        nw4r::g3d::ResFile m_snapSave;     // [9]
        nw4r::g3d::ResFile& operator[](int index) { return (&m_result)[index]; }
    };

    // ---- gfTask: 0x00..0x3F ----
    s32 m_state;                              // 0x40 State; isLoadEnd() is m_state != 0
    s32 m_unk44;                              // 0x44 cleared by the constructor, set to 1 once the files are bound
    s32 m_unk48[4];                           // 0x48 per player page of subPlayerControl (role: 0 hit counters, 1 statistics lines (drawExt1), 2 textured table (m_texObjs), 3 absent player, 4 Wifi "next game?" window open); all > 0: panel may go on
    s32 m_unk58[4];                           // 0x58 per player, set by jumpStandby/backStep and by subPlayerControl (role: the page the rows were left from: 0, 1 or 2)
    u8 m_unk68[4];                            // 0x68 per player, set once printHitPanel was called for the player
    s32 m_unk6C;                              // 0x6C the step of subPanelControl (0..3) (role: 1 wait for the HitNum animation, 2 wait for a player on page 0, 3 wait until all players left page 0)
    u8 m_unk70;                               // 0x70 set to 1 when State_PanelOpen ends
    u32 m_unk74;                              // 0x74 frame counter of the current State (unsigned: processAnim compares it as such)
    u32 m_unk78[4];                           // 0x78 per player frame counter of the page (role: 120 frames until the controller icon shows and the rows can be scrolled)
    u32 m_unk88[4];                           // 0x88 per player scroll position of the rows (in steps of 200)
    s32 m_unk98[4];                           // 0x98 per player scroll line (drawExt1 shows this line)
    s32 m_playerState[4];                     // 0xA8 copy of gmPlayerResultInfo::m_state (0 human, 1 cpu, 3 none)
    s32 m_winnerPlayer;                       // 0xB8 gmResultInfo+0x1F, the index into the players (0x2AC bytes each) that wins
    s32 m_winnerTeam;                         // 0xBC gmResultInfo+0x1E, indexes the team voice table and the team name table
    u8 m_unkC0;                               // 0xC0 bit mask of the players with m_state 0
    u8 m_unkC1;                               // 0xC1 bit mask of those that have a controller (Contl and Replay objects)
    u8 m_unkC2;                               // 0xC2 bit mask of those with m_state 0 and bit 4 of byte +7 clear
    u8 m_unkC3;                               // 0xC3 bit mask of the players with m_state 1
    s32 m_unkC4[4];                           // 0xC4 per player auto scroll step of subPlayerControl (role: 0 waiting, 1 auto scrolling by 9 per frame, 2 at the end (go to the next page), 3 the player scrolled)
    u8 m_unkD4;                               // 0xD4 set when the screen shows the tournament-like result (gmResultInfo+0xE < 0 and gmSetRule+0x18 == 2)
    s32 m_bgmHandle;                          // 0xD8 the id that sndSystem::playBGM returned (-1: none), soundProcess
    u32 m_unkDC;                              // 0xDC frames counted for the winner voice (soundProcess, step 0)
    u32 m_unkE0;                              // 0xE0 frames counted for the first SEs (soundProcess)
    s32 m_soundStep;                          // 0xE4 soundProcess: 0 voice not played, 1 played, 2 no contest, 3 initial value
    MuObject* m_objects[Obj_Count];           // 0xE8 see ObjectIndex; muObjProcessTask updates all of them
    nw4r::g3d::ScnMdlExpand* m_expands[0x35]; // 0x578 ScnMdlExpand::Construct of initMdlData (attachment groups, bones pos00..): [0] the panel,
                                              //       [1..4] the rows of the players (m_objects[8..11]), [5 + 11 * n + k] one per Spos
                                              //       node of player n, [49..52] spare (zeroed by the constructor, destroyed by ~)
    muObjProcessTask* m_objProcessTask;       // 0x64C created with Heaps 0x2A; its list is m_objects and its count 0x124
    MuMsg* m_msg;                             // 0x650 channel 0 title, 1..2 texts, 3 + 4n .. 6 + 4n the numbers of player n
    MuMsg* m_msgName;                         // 0x654 channel 0: winner, 1: unused, 2 + n: the player name
    nw4r::g3d::ScnGroup* m_noticeGroup;       // 0x658 group for the notice/replay save window (fn_801AB6CC, G3dObj::Destroy in ~)
    muNoticeWndTask* m_noticeWnd;             // 0x65C the "play the next game?" notice (muNoticeWndTask::create1)
    muWifiCntWndTask* m_cntWnd;               // 0x660 the Wifi counter window (only when g_muWifiInterfaceTask exists)
    s32 m_timer;                              // 0x664 starts at 900 (15 seconds), counts down while the counter window exists
    u8 m_unk668;                              // 0x668 starts as 1; subPlayerControl clears/sets it
    u8 m_unk669;                              // 0x669 starts as 1; the player wants the next game
    u8 m_unk66A;                              // 0x66A set while the Wifi "next game?" window of subPlayerControl is open (role: lets every player's rows follow)
    s32 m_unk66C;                             // 0x66C counts to 10 in the Wifi states
    ResFileSet m_resFiles;                    // 0x670 [0] Result.brres, [1..4] the character busts (one per player),
                                              //       [5] StockFaceTex, [6] the camera, [7] wifi_notice_wnd, [8] wifi_count, [9] snap_save
    msTexObj m_texObjs[PlayerMax * TexObjsPerPlayer];  // 0x698 the textures of storeTexExt2, 58 per player (0xF68 bytes)
    u32 m_texCount[4];                        // 0x4438 per player, what storeTexExt2 returned
    void* m_msgData;                          // 0x4448 /menu/msg/vsres.msbin
    u8 m_unk444C;                             // 0x444C set from ipSwitch (the replay save is available)
    void* m_wifiMsgData;                      // 0x4450 /menu/msg/wifi_common.msbin
    IfSnapSaveTask* m_replaySaveTask;         // 0x4454 the task that saves the replay (0x800F18C4)
    nteFileDataReplay* m_replayData;          // 0x4458 heap object with a virtual destructor, created for the replay save
    u8 m_fileLoaded[12];                      // 0x445C per file handle: ready (or nothing to read: set at once for a file
                                              //        that the archive gives, a missing player and a bust that an earlier player shares)
    gfFileIOHandle m_fileHandles[FileHandleCount];  // 0x4468 [0] camera, [1] Result, [2] StockFaceTex, [3] wifi_notice_wnd,
                                              //       [4] wifi_count, [5] snap_save, [6] vsres.msbin, [7] wifi_common.msbin,
                                              //       [8..11] the character busts (MenSelchrFaceB%03d.brres)
    gfArchive* m_archive;                     // 0x4498 loadResource's argument: the files come from this archive when it is set
    s32 m_bustHandle[4];                      // 0x449C per player: n of the bust's file handle (m_fileHandles[8 + n]) that holds the file;
                                              //        an earlier player's n when two players use the same character

    // ---- functions (symbols are still fn_<address>; the proposed mangled names are in the notes) ----

    ifVsResultTask();                         // 0x800E5858
    virtual void processAnim();               // 0x800E67A4 vtable slot 2
    virtual void init();                      // 0x800E60E0 vtable slot 21, empty
    virtual ~ifVsResultTask();                // 0x800E7CA8 vtable slot 22

    // HYPOTHESIS for the signatures below: from the register use of the bodies and the callers.
    void loadResource(gfArchive* archive) __attribute__((never_inline));    // 0x800E5E34 stores archive in m_archive, starts the reads of the files
    void printHitPanel(int player) __attribute__((never_inline));           // 0x800E60E4 fills the numbers of the player's panel
    void printHitNum(int msgSlot, u32 value, bool plus) __attribute__((never_inline));  // 0x800E636C prints a number (at most 99999) into m_msg
    void printTime(int msgSlot, u32 frames, bool format) __attribute__((never_inline)); // 0x800E6608 prints a time (at most 99:59.99) into m_msg
    void setWinnerName() __attribute__((never_inline));                     // 0x800E7BD0 plays the winner voice SE and prints the name into m_msgName
    void jumpStandby(int player, bool flag) __attribute__((never_inline));  // 0x800E8040 moves the rows of the player to the standby position
    void backStep(int player) __attribute__((never_inline));                // 0x800E83AC moves the rows of the player back
    void drawExt1(int player, int line) __attribute__((never_inline));      // 0x800E88CC shows a line of the table of the player (the Stockface textures)
    int lineCalcExt1(int player) __attribute__((never_inline));             // 0x800E8CFC number of lines of the table of the player
    int calcScrollLv(const gfPadStatus* pad) __attribute__((never_inline)); // 0x800E8DC4 scroll speed from the stick (+-75) or the d-pad (+-60), negated (HYPOTHESIS: the original takes the pad by value: subPlayerControl passes a 64 byte copy)
    float getMarkColAnimFrame(int player, bool plus) __attribute__((never_inline));  // 0x800E8E60 frame of the color animation of the player's mark
    u32 storeTexExt2(int player, bool store) __attribute__((never_inline)); // 0x800E8F1C draws the table texts of the player and (store) copies them
                                              //            from the EFB into m_texObjs; returns how many it drew
    void storeMessageData(float scale, Message* message, const void* data, int size, const float* pos, bool shadow) __attribute__((never_inline));  // 0x800E975C
    void initMdlData() __attribute__((never_inline));                       // 0x800E9878 creates every object, model, message and texture of the screen
    void soundProcess() __attribute__((never_inline));                      // 0x800EB0E4 BGM, voice and SE of the screen
    bool isLoadEnd() __attribute__((never_inline));                         // 0x800EB300 m_state != 0
    void adjustResultinfo() __attribute__((never_inline));                  // 0x800EB314 rescales the values of gmResultInfo (prints them with OSReport)
    void subPanelControl() __attribute__((never_inline));                   // 0x800EB564 the step m_unk6C of the main panel (role: starts the HitNum animations when the players go on)
    void subPlayerControl(int player) __attribute__((never_inline));        // 0x800EB684 input and animation of one player's rows
};
static_assert(sizeof(ifVsResultTask) == 0x44AC, "Class is the wrong size!");
static_assert(sizeof(ifVsResultTask::PlayerNodes) == 0x108, "Class is the wrong size!");

// HYPOTHESIS: the checks pin the offsets that the unit proves; the base is gfTask (0x40 bytes with the vptr at 0x3C).
static_assert(offsetof(ifVsResultTask, m_state) == 0x40, "Class is wrong size!");
static_assert(offsetof(ifVsResultTask, m_unk44) == 0x44, "Class is wrong size!");
static_assert(offsetof(ifVsResultTask, m_unk48) == 0x48, "Class is wrong size!");
static_assert(offsetof(ifVsResultTask, m_unk58) == 0x58, "Class is wrong size!");
static_assert(offsetof(ifVsResultTask, m_unk68) == 0x68, "Class is wrong size!");
static_assert(offsetof(ifVsResultTask, m_unk6C) == 0x6C, "Class is wrong size!");
static_assert(offsetof(ifVsResultTask, m_unk70) == 0x70, "Class is wrong size!");
static_assert(offsetof(ifVsResultTask, m_unk74) == 0x74, "Class is wrong size!");
static_assert(offsetof(ifVsResultTask, m_unk78) == 0x78, "Class is wrong size!");
static_assert(offsetof(ifVsResultTask, m_unk88) == 0x88, "Class is wrong size!");
static_assert(offsetof(ifVsResultTask, m_unk98) == 0x98, "Class is wrong size!");
static_assert(offsetof(ifVsResultTask, m_playerState) == 0xA8, "Class is wrong size!");
static_assert(offsetof(ifVsResultTask, m_winnerPlayer) == 0xB8, "Class is wrong size!");
static_assert(offsetof(ifVsResultTask, m_winnerTeam) == 0xBC, "Class is wrong size!");
static_assert(offsetof(ifVsResultTask, m_unkC0) == 0xC0, "Class is wrong size!");
static_assert(offsetof(ifVsResultTask, m_unkC3) == 0xC3, "Class is wrong size!");
static_assert(offsetof(ifVsResultTask, m_unkC4) == 0xC4, "Class is wrong size!");
static_assert(offsetof(ifVsResultTask, m_unkD4) == 0xD4, "Class is wrong size!");
static_assert(offsetof(ifVsResultTask, m_bgmHandle) == 0xD8, "Class is wrong size!");
static_assert(offsetof(ifVsResultTask, m_unkDC) == 0xDC, "Class is wrong size!");
static_assert(offsetof(ifVsResultTask, m_unkE0) == 0xE0, "Class is wrong size!");
static_assert(offsetof(ifVsResultTask, m_soundStep) == 0xE4, "Class is wrong size!");
static_assert(offsetof(ifVsResultTask, m_objects) == 0xE8, "Class is wrong size!");
static_assert(offsetof(ifVsResultTask, m_expands) == 0x578, "Class is wrong size!");
static_assert(offsetof(ifVsResultTask, m_objProcessTask) == 0x64C, "Class is wrong size!");
static_assert(offsetof(ifVsResultTask, m_msg) == 0x650, "Class is wrong size!");
static_assert(offsetof(ifVsResultTask, m_msgName) == 0x654, "Class is wrong size!");
static_assert(offsetof(ifVsResultTask, m_noticeGroup) == 0x658, "Class is wrong size!");
static_assert(offsetof(ifVsResultTask, m_noticeWnd) == 0x65C, "Class is wrong size!");
static_assert(offsetof(ifVsResultTask, m_cntWnd) == 0x660, "Class is wrong size!");
static_assert(offsetof(ifVsResultTask, m_timer) == 0x664, "Class is wrong size!");
static_assert(offsetof(ifVsResultTask, m_unk668) == 0x668, "Class is wrong size!");
static_assert(offsetof(ifVsResultTask, m_unk66C) == 0x66C, "Class is wrong size!");
static_assert(offsetof(ifVsResultTask, m_resFiles) == 0x670, "Class is wrong size!");
static_assert(offsetof(ifVsResultTask, m_texObjs) == 0x698, "Class is wrong size!");
static_assert(offsetof(ifVsResultTask, m_texCount) == 0x4438, "Class is wrong size!");
static_assert(offsetof(ifVsResultTask, m_msgData) == 0x4448, "Class is wrong size!");
static_assert(offsetof(ifVsResultTask, m_unk444C) == 0x444C, "Class is wrong size!");
static_assert(offsetof(ifVsResultTask, m_wifiMsgData) == 0x4450, "Class is wrong size!");
static_assert(offsetof(ifVsResultTask, m_replaySaveTask) == 0x4454, "Class is wrong size!");
static_assert(offsetof(ifVsResultTask, m_replayData) == 0x4458, "Class is wrong size!");
static_assert(offsetof(ifVsResultTask, m_fileLoaded) == 0x445C, "Class is wrong size!");
static_assert(offsetof(ifVsResultTask, m_fileHandles) == 0x4468, "Class is wrong size!");
static_assert(offsetof(ifVsResultTask, m_archive) == 0x4498, "Class is wrong size!");
static_assert(offsetof(ifVsResultTask, m_bustHandle) == 0x449C, "Class is wrong size!");
