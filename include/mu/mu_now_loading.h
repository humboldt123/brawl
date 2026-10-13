#pragma once

#include <StaticAssert.h>
#include <gf/gf_archive.h>
#include <gf/gf_task.h>
#include <memory.h>
#include <mu/mu_boot_common.h>
#include <mu/mu_object.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <types.h>

// The "Now loading" sign of the boot: one model that plays its animation while the game loads.
class muNowLoadingTask : public gfTask {
public:
    nw4r::g3d::ResFile m_resFile;               // 0x40
    muModelGroupTask* m_scene;                  // 0x44
    MuObject* m_object;                         // 0x48 "MenBoot0003_TopN"
    int m_state;                                // 0x4C
    int m_frame;                                // 0x50

    muNowLoadingTask(const char* name);
    static muNowLoadingTask* create();

    virtual void processDefault();
    virtual ~muNowLoadingTask();

    void initialize();
    void release();
    void createData(gfArchive* archive);
    void addModel();
    void removeModel();
};
static_assert(sizeof(muNowLoadingTask) == 0x54, "Class is wrong size!");
