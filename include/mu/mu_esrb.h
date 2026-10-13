#pragma once

#include <StaticAssert.h>
#include <gf/gf_archive.h>
#include <gf/gf_task.h>
#include <memory.h>
#include <mu/mu_boot_common.h>
#include <mu/mu_msg.h>
#include <mu/mu_object.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <types.h>

// The screen of the boot with the rating notice (the class is "muESRBTask", the task is called "BootOnline"): a model with a
// text that says which notice it is, that fades in and out.
class muESRBTask : public gfTask {
public:
    nw4r::g3d::ResFile m_resFile;               // 0x40
    muModelGroupTask* m_scene;                  // 0x44
    MuObject* m_notice;                         // 0x48 "MenBoot0002_TopN", the model with the text
    MuObject* m_boot;                           // 0x4C "MenBoot0000_TopN", the model that fades the screen
    MuMsg* m_msg;                               // 0x50
    u8 m_language;                              // 0x54 the language of the game (0: japanese, 3: french, 4: spanish)
    int m_state;                                // 0x58
    int m_frame;                                // 0x5C

    muESRBTask(const char* name);
    static muESRBTask* create();

    virtual void processDefault();
    virtual ~muESRBTask();

    void initialize(u8 language);
    void release();
    void createData(gfArchive* archive);
    void addModel();
    void removeModel();
    bool isNextMode();
    void setNextMode();
};
static_assert(sizeof(muESRBTask) == 0x60, "Class is wrong size!");
