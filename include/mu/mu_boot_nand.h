#pragma once

#include <StaticAssert.h>
#include <gf/gf_archive.h>
#include <gf/gf_file_io_handle.h>
#include <gf/gf_task.h>
#include <mu/mu_notice_wnd.h>
#include <mu/mu_object.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <types.h>

class muEndCameraTask;

// One thing the save data check wants to have done to the save data: what to do (0: the record of the game, 1: the
// adventure data, ..., 4: the banner, 6: the end of the list) and two more words.
// HYPOTHESIS: the names; the words are not used by the boot menu.
struct gfSysRecorder2Require {
    int m_kind;     // 0x00
    int m_unk4;     // 0x04
    int m_unk8;     // 0x08
};

// The recorder of the save data of the system (the NAND): it checks the files of the game, renews them and loads the
// system data. The boot menu starts a request, waits until it is not busy any more and looks at the result.
// HYPOTHESIS: the names of the fields follow how the boot menu uses them.
struct gfSysRecorder2 {
    u32 m_unk00;
    u32 m_unk04;
    gfFileIOHandle m_handle;                // 0x08 the file of the system data (the buffer is the loaded data)
    u8 m_unk0C[0x94];
    int m_result;                           // 0xA0 how the last request ended (0: done, 1..3: the kinds of failure)
    int m_checkResult;                      // 0xA4 what the check found (0: nothing to do, 1: renew, 2: create)
    gfSysRecorder2Require m_require[6];     // 0xA8 the things that have to be done
    int m_busy;                             // 0xF0 a request is still running
};
// gfSysRecorder2::createInstance(heap); the one that the boot menu made is dropped with dropInstance()
extern "C" gfSysRecorder2* fn_80030548(int heap);
extern "C" void fn_800307B4();
// gfSysRecorder2::isEnable(unk)
extern "C" bool fn_800305B0(int unk);
// gfSysRecorder2::requestCheck(heap, kind), requestRenew(heap, kind) and loadSystem(heap)
extern "C" void fn_80030954(gfSysRecorder2* recorder, int heap, int kind);
extern "C" void fn_800309F8(gfSysRecorder2* recorder, int heap, int kind);
extern "C" void fn_80030F64(gfSysRecorder2* recorder, int heap);

// The state of the save data check of the start of the game: the record of the game is checked, the missing parts are
// created or renewed (the player is asked first), then the system data is loaded.
class muBootNandTask : public gfTask {
public:
    gfSysRecorder2* m_recorder;             // 0x40
    int m_checkResult;                      // 0x44 what the check found
    u8 m_flags;                             // 0x48 the parts of the save data that are in use (the low five bits)
    gfSysRecorder2Require* m_require;       // 0x4C the things that have to be done
    int m_requireIndex;                     // 0x50
    int m_requireKind;                      // 0x54
    bool m_skipDone;                        // 0x58 the parts that are not wanted are skipped
    void* m_msgData;                        // 0x5C the texts
    nw4r::g3d::ResFile m_resFileWnd;        // 0x60 the models of the window
    nw4r::g3d::ResFile m_resFileWait;       // 0x64 the model of the "please wait" panel
    muNoticeWndTask* m_wnd;                 // 0x68 the window that is open
    muEndCameraTask* m_cameraTask;          // 0x6C
    MuObject* m_object;                     // 0x70 the "please wait" panel
    int m_answer;                           // 0x74 the button that was chosen (-1: none)
    int m_step;                             // 0x78
    int m_frame;                            // 0x7C

    muBootNandTask(const char* name) : gfTask(name, Category_Menu, 0xF, 8, true), m_resFileWnd(static_cast<void*>(NULL)),
                       m_resFileWait(static_cast<void*>(NULL)) {
        m_recorder = NULL;
        m_require = NULL;
        m_msgData = NULL;
        m_wnd = NULL;
        m_cameraTask = NULL;
        m_object = NULL;
    }
    static muBootNandTask* create();

    virtual void processDefault();
    virtual ~muBootNandTask();

    void initialize();
    void release();
    void createData(gfArchive* archive);

    // Takes the open window away.
    void closeWnd();
    // Opens a window (the old one is taken away first).
    void openWnd(int kind, void* msgData, int msgId, int defaultButton);
};
static_assert(sizeof(muBootNandTask) == 0x80, "Class is wrong size!");
