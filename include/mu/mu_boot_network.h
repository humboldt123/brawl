#pragma once

#include <StaticAssert.h>
#include <gf/gf_archive.h>
#include <gf/gf_task.h>
#include <mu/mu_msg.h>
#include <mu/mu_notice_wnd.h>
#include <mu/mu_object.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <types.h>

class muEndCameraTask;

// The task of the network check at the start of the game (it asks the server whether there is something new).
// HYPOTHESIS: the names of the fields follow how muBootNetworkTask uses them.
class nteBootTask : public gfTask {
public:
    u8 m_unk40[0xC];
    bool m_finished;            // 0x4C the check is over
    u8 m_unk4D[3];
    int m_errorState;           // 0x50 how the check failed (0: it did not)
    u16 m_countA;               // 0x54 the counts that the server sent
    u16 m_countB;               // 0x56
    u16 m_countC;               // 0x58
    u8 m_unk5A[6];
};
static_assert(sizeof(nteBootTask) == 0x60, "Class is wrong size!");
// nteBootTask::nteBootTask(heap) and nteBootTask::startTask()
extern "C" nteBootTask* fn_8015820C(nteBootTask* task, int heap);
extern "C" void fn_80158334(nteBootTask* task);

// The start of the game when it can use the network: it asks the server whether there is a message or a new download
// and tells the player how many there are.
class muBootNetworkTask : public gfTask {
public:
    nteBootTask* m_task;                    // 0x40 the check on the network
    int m_countA;                           // 0x44 the counts that the server sent
    int m_countB;                           // 0x48
    int m_countC;                           // 0x4C
    char m_message[0x100];                  // 0x50 the text that tells the player what was received
    void* m_msgData;                        // 0x150 the texts
    nw4r::g3d::ResFile m_resFileMsg;        // 0x154
    nw4r::g3d::ResFile m_resFileWait;       // 0x158
    muEndCameraTask* m_cameraTask;          // 0x15C
    MuObject* m_object;                     // 0x160 the "please wait" panel
    MuMsg* m_msg;                           // 0x164
    muNoticeWndTask* m_wnd;                 // 0x168 the window that is open
    int m_step;                             // 0x16C
    int m_frame;                            // 0x170

    muBootNetworkTask(const char* name) : gfTask(name, Category_Menu, 0xF, 8, true), m_resFileMsg(static_cast<void*>(NULL)),
                          m_resFileWait(static_cast<void*>(NULL)) {
        m_task = NULL;
        m_msgData = NULL;
        m_cameraTask = NULL;
        m_msg = NULL;
        m_wnd = NULL;
        m_step = 0;
    }
    static muBootNetworkTask* create();

    virtual void processDefault();
    virtual ~muBootNetworkTask();

    void initialize();
    void closeWnd();
    void releaseWnd();
    bool hasCount() const;
    void createData(gfArchive* archive);
    void destroyData();
    void createMessageFromReceiveCount();
};
static_assert(sizeof(muBootNetworkTask) == 0x174, "Class is wrong size!");
