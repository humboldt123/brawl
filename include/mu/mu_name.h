#pragma once

#include <StaticAssert.h>
#include <gf/gf_system_callback.h>
#include <gf/gf_task.h>
#include <memory.h>
#include <mu/mu_menu_controller.h>
#include <mu/mu_msg.h>
#include <mu/mu_notice_wnd.h>
#include <mu/mu_object.h>
#include <mu/selchar/mu_select_character_name_entry.h>
#include <nw4r/g3d/g3d_obj.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <types.h>

// One model of the menu: the name, the slots of the task it fills (start, end) and the draw priority.
struct muNameModel {
    const char* m_name;
    u8 m_start;
    u8 m_end;
    u8 m_priority;
};

// A name that the menu lists: the first bit says that the name is already taken (by another player), the number
// is the number of the name in the record of the game (-1: a place to make a new name, -2: an empty line).
struct muNameSlot {
    u8 m_taken : 1;             // 0x00 bit 7
    u8 m_unk : 7;
    u8 m_pad;                   // 0x01
    s16 m_nameNo;               // 0x02
};

// The Wii Remote can keep a name: this is what is read from it and written to it.
struct muNameRemoteData {
    u8 m_flag : 1;              // 0x00 the flag of the name in the record of the game
    u8 m_unk : 7;
    u8 m_pad;                   // 0x01
    u16 m_name[6];              // 0x02 the name in UTF-16
    u8 m_data[0x2D];            // 0x0E what the record of the game keeps next to the name
    u8 m_pad3B;                 // 0x3B
};

// The task that reads and writes the name that a Wii Remote keeps.
// HYPOTHESIS: the fields follow how the functions use them.
class muNameRemoteTask : public gfTask {
public:
    enum State {
        State_Idle = 0,
        State_Reading = 1,
        State_Writing = 2,
    };
    enum Status {
        Status_Ok = 0,
        Status_Busy = 1,
        Status_NoRemote = 2,
        Status_Unknown3 = 3,
        Status_Unknown4 = 4,
        Status_BadData = 5,
        Status_Failed = 6,
    };

    int m_pad;                      // 0x40 the pad of the Wii Remote
    int m_wait;                     // 0x44 frames to wait before the request is sent again
    int m_readRetries;              // 0x48
    int m_writeRetries;             // 0x4C
    u8 m_readBuf[0x40];             // 0x50 what was read from the Wii Remote
    u8 m_writeBuf[0x40];            // 0x90 what is written to the Wii Remote
    int m_result;                   // 0xD0 how the last request ended (1: it has not ended yet)
    int m_state;                    // 0xD4
    bool m_verifying;               // 0xD8 the data that was written is read back

    muNameRemoteTask();
    static muNameRemoteTask* create();
    virtual void processDefault();
    virtual ~muNameRemoteTask();

    void read();
    int getReadStatus();
    void readProcess();
    void write(const muNameRemoteData* data);
    void writeProcess();
};
static_assert(sizeof(muNameRemoteTask) == 0xDC, "Class is wrong size!");

// Tells the system menu whether the game can be left now: it cannot be left while a name is written to the Wii Remote.
class muCbNameRemoteReturn : public gfReturnStatusCallback {
public:
    bool m_canReturn;               // 0x08
    muCbNameRemoteReturn() : m_canReturn(true) { }
    virtual bool returnStatus() const;
    virtual ~muCbNameRemoteReturn() { }
};

// The menu where a player picks one of the names that are saved, makes a new one (the keyboard of MuSelctChrNameEntry) or
// moves the names between the game and a Wii Remote.
// HYPOTHESIS: the names of the fields follow how the steps use them.
class muNameTask : public gfTask {
public:
    enum Step {
        Step_None = 0,
        Step_Appear = 1,
        Step_SelectName = 2,
        Step_NameFull = 3,
        Step_Leave = 4,
        Step_NameEntry = 5,
        Step_NameEntryLeave = 6,
        Step_AvoidList = 7,
        Step_BackAvoidList = 8,
        Step_RemoteSimpleSync = 9,
        Step_RemoteSimpleSyncStop = 10,
        Step_DisableHomeButton = 11,
        Step_RemoteRead = 12,
        Step_RemoteReadError = 13,
        Step_RemoteNoName = 14,
        Step_RemoteMenuAppear = 15,
        Step_RemoteMenu = 16,
        Step_RemoteNoNameCapacity = 17,
        Step_RemoteRestoreOverwriteConfirm = 18,
        Step_RemoteRestoreComplete = 19,
        Step_RemoteStoreOverwriteConfirm = 20,
        Step_RemoteStoreWrite = 21,
        Step_RemoteWriteError = 22,
        Step_RemoteStoreComplete = 23,
        Step_RemoteDisconnect = 24,
        Step_Count = 25,
    };

    // What the menu is for.
    enum Kind {
        Kind_Option = 0,
        Kind_Icon = 1,
        Kind_Event = 2,
        Kind_TrnRule = 3,
    };

    // How the menu ended.
    enum Result {
        Result_None = 0,
        Result_Chosen = 1,          // a name was chosen
        Result_NewName = 2,         // a new name was made
        Result_Cancelled = 3,       // the player went back
        Result_Closed = 4,          // the window of the list has left
        Result_ListShown = 5,       // the list came back after it had made room
    };

    // The slots of the models.
    enum Obj {
        Obj_Window = 0,             // the window of the list (appears and leaves)
        Obj_List = 1,               // the list of the names
        Obj_NameWindow = 2,
        Obj_Unk3 = 3,
        Obj_KeyBase = 4,            // the keyboard
        Obj_Hiragana = 5,
        Obj_Katakana = 6,
        Obj_Alphabet = 7,
        Obj_Number = 8,
        Obj_Europe = 9,
        Obj_Cursor = 10,
        Obj_Line = 11,
        Obj_RemoteWindow = 12,      // the models of the Wii Remote part
        Obj_RemoteIcon = 13,
        Obj_RemoteItem0 = 14,       // the three buttons of the menu of the Wii Remote
        Obj_RemoteItem1 = 15,
        Obj_RemoteItem2 = 16,
        Obj_RemoteCursor = 17,
        Obj_RemoteLine = 18,
        Obj_Count = 19,
    };

    // The init and main function of each step.
    struct StepFunc {
        void (*m_init)(muNameTask* task);
        int (*m_main)(muNameTask* task);
    };

    MuObject* m_objects[Obj_Count];             // 0x40
    nw4r::g3d::ScnMdl* m_scnMdls[3];            // 0x8C the models that the other models are attached to (the list, the keyboard, the Wii Remote)
    int m_kind;                                 // 0x98 what the menu is for (a Kind; 2: the menu of the events)
    int m_step;                                 // 0x9C
    int m_stepPrev;                             // 0xA0
    int m_stepAfter;                            // 0xA4 the step that follows the step that disables the home button
    nw4r::g3d::ScnGroup* m_scnGroup;            // 0xA8 the group of the scene that the models are in
    muMenuController m_controller;              // 0xAC
    MuMsg* m_msg;                               // 0x154
    MuSelctChrNameEntry m_entry;                // 0x158 the keyboard
    u16 m_cursor;                               // 0x1EC the line of the list that the cursor is on
    u16 m_scroll;                               // 0x1EE how far the list is scrolled
    muNameSlot m_slots[0x7A];                   // 0x1F0
    u16 m_slotCount;                            // 0x3D8
    u16 m_pad3DA;
    int m_padNo;                                // 0x3DC the pad of the player
    int m_playerNo;                             // 0x3E0 the number of the player (for the color of the name)
    int m_priority;                             // 0x3E4 the draw priority of the windows that the menu opens
    int m_resultKind;                           // 0x3E8 how the menu ended (a Result)
    int m_resultNameNo;                         // 0x3EC the number of the name that was chosen or made
    int m_timer;                                // 0x3F0 frames before the answer of the window is read
    bool m_sharedModels;                        // 0x3F4 the first models were made by the menu that opened this one
    u8 m_pad3F5[3];
    nw4r::g3d::ScnMdl* m_parentMdl;             // 0x3F8 the model that the list is attached to (can be missing)
    int m_parentNode;                           // 0x3FC the node of that model
    int m_remotePadNo;                          // 0x400 the pad of the Wii Remote that is used (4: none)
    int m_remoteNameNo;                         // 0x404 the name that is moved to the Wii Remote (-1: none)
    muNameRemoteTask* m_remote;                 // 0x408
    muNameRemoteData m_remoteData;              // 0x40C what was read from the Wii Remote
    u8 m_pad448[4];                             // 0x448
    int m_remoteResult;                         // 0x44C how the read ended
    bool m_remoteFlags[3];                      // 0x450
    u8 m_pad453;
    int m_remoteMenuSel;                        // 0x454 the button of the menu of the Wii Remote that is selected
    void* m_msgData;                            // 0x458 the texts
    muNoticeWndTask* m_wnd;                     // 0x45C the window with a question or a notice
    nw4r::g3d::ResFile* m_wndResFile;           // 0x460 the models of the windows
    int m_unk464;                               // 0x464
    int m_nameFullTimer;                        // 0x468 frames the "the list is full" notice is shown
    muCbNameRemoteReturn m_returnCallback;      // 0x46C
    int m_remoteConnectedPad;                   // 0x478 the pad of the Wii Remote that was seen (-1: it is gone)
    int m_errorMsgId;                           // 0x47C the text of the notice that tells why the Wii Remote cannot be used

    muNameTask();
    static muNameTask* create(nw4r::g3d::ScnGroup* scnGroup, int kind, nw4r::g3d::ResFile* windowRes, nw4r::g3d::ResFile* keyRes,
                              int padNo, int priority, nw4r::g3d::ScnMdl* parentMdl, const char* parentNode,
                              nw4r::g3d::ResFile* remoteRes, void* msgData, nw4r::g3d::ResFile* wndRes);
    static muNameTask* create1(nw4r::g3d::ScnGroup* scnGroup, int kind, MuObject** objects, nw4r::g3d::ScnMdl** scnMdls,
                               nw4r::g3d::ResFile* keyRes, int padNo, int priority, nw4r::g3d::ScnMdl* parentMdl,
                               const char* parentNode, nw4r::g3d::ResFile* remoteRes, void* msgData, nw4r::g3d::ResFile* wndRes);

    virtual void processDefault();
    virtual void exit();
    virtual ~muNameTask();

    void deleteModel();
    void initProc(nw4r::g3d::ScnGroup* scnGroup, int kind, int padNo, int priority, nw4r::g3d::ScnMdl* parentMdl,
                  const char* parentNode, void* msgData, nw4r::g3d::ResFile* wndRes);
    void createObjResFile(const muNameModel* models, int count, nw4r::g3d::ResFile* resFile, bool suffix, int priority);
    void createKeyModels(nw4r::g3d::ResFile* resFile, int priority);
    void initMsg();
    void makeNameList();
    bool deleteNameID(int nameNo);
    void deleteName(int index);
    int getCurNameIndex();
    void addNameList(int nameNo);
    void dispUpdate(int cursor, int scroll);
    void updateAll();
    int disableSelectName(int nameNo);
    void createRemoteModel(nw4r::g3d::ResFile* resFile, int priority);
    void printBaseNameHelp(int msgIndexA, const u16* name, int msgIndexB, bool shown);
    void dispRemoteCursor(int selected);
    int registRimoName(const u16* name, bool flag, const u8* data, bool overwrite);

    static void mainStepAppearInit(muNameTask* task);
    static int mainStepAppearMain(muNameTask* task);
    static void mainStepSelectNameInit(muNameTask* task);
    static int mainStepSelectNameMain(muNameTask* task);
    static void mainStepNameFullInit(muNameTask* task);
    static int mainStepNameFullMain(muNameTask* task);
    static void mainStepLeaveInit(muNameTask* task);
    static int mainStepLeaveMain(muNameTask* task);
    static void mainStepNameEntryInit(muNameTask* task);
    static int mainStepNameEntryMain(muNameTask* task);
    static void mainStepNameEntryLeaveInit(muNameTask* task);
    static int mainStepNameEntryLeaveMain(muNameTask* task);
    static void mainStepAvoidListInit(muNameTask* task);
    static int mainStepAvoidListMain(muNameTask* task);
    static void mainStepBackAvoidListInit(muNameTask* task);
    static int mainStepBackAvoidListMain(muNameTask* task);
    static void mainStepRemoteSimpleSyncInit(muNameTask* task);
    static int mainStepRemoteSimpleSyncMain(muNameTask* task);
    static void mainStepRemoteSimpleSyncStopInit(muNameTask* task);
    static int mainStepRemoteSimpleSyncStopMain(muNameTask* task);
    static void mainStepDisableHomeButtonInit(muNameTask* task);
    static int mainStepDisableHomeButtonMain(muNameTask* task);
    static void mainStepRemoteReadInit(muNameTask* task);
    static int mainStepRemoteReadMain(muNameTask* task);
    static void mainStepRemoteReadErrorInit(muNameTask* task);
    static int mainStepRemoteReadErrorMain(muNameTask* task);
    static void mainStepRemoteNoNameInit(muNameTask* task);
    static int mainStepRemoteNoNameMain(muNameTask* task);
    static void mainStepRemoteMenuAppearInit(muNameTask* task);
    static int mainStepRemoteMenuAppearMain(muNameTask* task);
    static void mainStepRemoteMenuInit(muNameTask* task);
    static int mainStepRemoteMenuMain(muNameTask* task);
    static void mainStepRemoteNoNameCapacityInit(muNameTask* task);
    static int mainStepRemoteNoNameCapacityMain(muNameTask* task);
    static void mainStepRemoteRestoreOverwriteConfirmInit(muNameTask* task);
    static int mainStepRemoteRestoreOverwriteConfirmMain(muNameTask* task);
    static void mainStepRemoteRestoreCompleteInit(muNameTask* task);
    static int mainStepRemoteRestoreCompleteMain(muNameTask* task);
    static void mainStepRemoteStoreOverwriteConfirmInit(muNameTask* task);
    static int mainStepRemoteStoreOverwriteConfirmMain(muNameTask* task);
    static void mainStepRemoteStoreWriteInit(muNameTask* task);
    static int mainStepRemoteStoreWriteMain(muNameTask* task);
    static void mainStepRemoteWriteErrorInit(muNameTask* task);
    static int mainStepRemoteWriteErrorMain(muNameTask* task);
    static void mainStepRemoteStoreCompleteInit(muNameTask* task);
    static int mainStepRemoteStoreCompleteMain(muNameTask* task);
    static void mainStepRemoteDisconnectInit(muNameTask* task);
    static int mainStepRemoteDisconnectMain(muNameTask* task);
};
static_assert(sizeof(muNameTask) == 0x480, "Class is wrong size!");
