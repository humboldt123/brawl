#pragma once

#include <StaticAssert.h>
#include <gf/gf_task.h>
#include <sora_adv_stage/st_adventure_info_emboss.h>
#include <sora_adv_stage/st_adventure_info_emcorps.h>
#include <sora_adv_stage/st_adventure_info_emzako.h>
#include <types.h>

// The Subspace info task: owns the boss, corps and zako records and updates them each frame.
class stAdventureInfoMediation : public gfTask {
public:
    stAdventureInfoEmBoss m_emBoss;     // 0x40
    stAdventureInfoEmCorps m_emCorps;   // 0x9C
    stAdventureInfoEmZako m_emZako;     // 0xA4
    bool unkFC;                         // 0xFC

    stAdventureInfoMediation() : gfTask("stAdventureInfoMediation", Category_Stage, 1, 13, true) { unkFC = false; }
    virtual void processUpdate();
    virtual void processEnd();
    virtual ~stAdventureInfoMediation();

    static stAdventureInfoMediation* getInstance();
    static void create();
    static void remove();
    stAdventureInfoEmBoss* getEmBoss();
    stAdventureInfoEmCorps* getEmCorps();
    stAdventureInfoEmZako* getEmZako();
    void notifyStepJump();
};
static_assert(sizeof(stAdventureInfoMediation) == 0x100, "Class is wrong size!");
