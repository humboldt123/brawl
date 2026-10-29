#pragma once

#include <StaticAssert.h>
#include <gf/gf_archive.h>
#include <gf/gf_file_io_handle.h>
#include <gf/gf_sys_recorder2.h>
#include <gf/gf_task.h>
#include <mu/mu_notice_wnd.h>
#include <mu/mu_object.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <types.h>

class muEndCameraTask;

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
