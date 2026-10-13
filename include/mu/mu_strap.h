#pragma once

#include <StaticAssert.h>
#include <gf/gf_archive.h>
#include <gf/gf_task.h>
#include <memory.h>
#include <mu/mu_boot_common.h>
#include <mu/mu_object.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <types.h>

// The strap screen of the boot (the warning about the health and the safety, before the title): a picture for each
// language that stays on the screen and goes away by itself, or when the player skips it.
class muStrapTask : public gfTask {
public:
    nw4r::g3d::ResFile m_resFile;               // 0x40
    char m_strapFile[4][0x20];                  // 0x44 "/menu/common/strap_jp.brres" ... for each language (jp, en, fr, sp)
    char m_modelName[4][0x20];                  // 0xC4 "MenBoot0001Ja_TopN" ... the model of the strap for each language
    char m_animName[4][0x20];                   // 0x144 "MenBoot0001Ja_TopN__0" ... its animation
    muModelGroupTask* m_scene;                  // 0x1C4
    MuObject* m_strap;                          // 0x1C8 the model of the strap
    MuObject* m_boot;                           // 0x1CC "MenBoot0000_TopN", the model that fades the screen
    u8 m_language;                              // 0x1D0 0: japanese, 1: english, 2: french, 3: spanish
    int m_state;                                // 0x1D4
    int m_frame;                                // 0x1D8

    muStrapTask();
    static muStrapTask* create();

    virtual void processDefault();
    virtual ~muStrapTask();

    void initialize(int language);
    void release();
    void createData(gfArchive* archive);
    void addModel();
    void removeModel();
    bool isNextMode();
    void setNextMode();
};
static_assert(sizeof(muStrapTask) == 0x1DC, "Class is wrong size!");
