#pragma once

#include <StaticAssert.h>
#include <gf/gf_file_io_handle.h>
#include <gf/gf_task.h>
#include <memory.h>
#include <mu/mu_menuroot.h>
#include <mu/mu_msg.h>
#include <mu/mu_object.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <types.h>

// The newcomer window of the adventure mode: a fighter that joins the team is shown with a face and a text. The window
// loads its files, plays its animations (it appears, waits for A, leaves) and ends the task.
// HYPOTHESIS: the names of the fields follow how the steps use them.
class muAdvComerTask : public gfTask {
public:
    enum Step {
        Step_Request = 0,       // asks for the files
        Step_Load = 1,          // waits for the files
        Step_Create = 2,        // makes the window
        Step_Appear = 3,        // starts the animation that makes the window appear
        Step_AppearWait = 4,
        Step_Show = 5,          // starts the animation that waits
        Step_WaitButton = 6,    // waits for A
        Step_Leave = 7,         // starts the animation that makes the window leave
        Step_LeaveWait = 8,
        Step_End = 9,           // frees everything and ends the task
    };

    gfFileIOHandle m_fileComer;                 // 0x40 the window
    gfFileIOHandle m_fileFace;                  // 0x44 the face of the fighter
    gfFileIOHandle m_fileMsg;                   // 0x48 the text
    nw4r::g3d::ResFile m_resComer;              // 0x4C
    nw4r::g3d::ResFile m_resFace;               // 0x50
    char m_facePaths[50][0x40];                 // 0x54 the paths of the files of the faces (the first one is for no one)
    int m_kind;                                 // 0xCD4 the kind of the fighter
    int m_faceNo;                               // 0xCD8 which face is shown
    MenuRoot* m_menuRoot;                       // 0xCDC
    MuObject* m_object;                         // 0xCE0
    MuMsg* m_msg;                               // 0xCE4
    int m_step;                                 // 0xCE8
    int m_frame;                                // 0xCEC

    muAdvComerTask();
    static muAdvComerTask* create();

    virtual void processDefault();
    virtual ~muAdvComerTask();

    void setFighter(int kind);
    void createWindow();
};
static_assert(sizeof(muAdvComerTask) == 0xCF0, "Class is wrong size!");
