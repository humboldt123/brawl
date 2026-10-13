#pragma once

#include <StaticAssert.h>
#include <gf/gf_archive.h>
#include <gf/gf_task.h>
#include <memory.h>
#include <mu/mu_boot_common.h>
#include <mu/mu_object.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <types.h>

// The screen of the boot that tells how to hold the controllers ("ControllerOperator"): one model that plays its animation
// and stays on the last frame until the boot goes on.
class muControllerOperatorTask : public gfTask {
public:
    nw4r::g3d::ResFile m_resFile;               // 0x40
    muModelGroupTask* m_scene;                  // 0x44
    MuObject* m_object;                         // 0x48 "MenController0000_TopN"
    int m_state;                                // 0x4C
    int m_frame;                                // 0x50

    muControllerOperatorTask(const char* name);
    static muControllerOperatorTask* create();

    virtual void processDefault();
    virtual ~muControllerOperatorTask();

    void initialize();
    void release();
    void createData(gfArchive* archive);
    void addModel();
    void removeModel();
    void setLastFrame();
    bool isNextMode();
    void setNextMode();
};
static_assert(sizeof(muControllerOperatorTask) == 0x54, "Class is wrong size!");
