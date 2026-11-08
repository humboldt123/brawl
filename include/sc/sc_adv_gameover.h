#pragma once

// Supersedes include/Brawl/Include/sc/sc_adv_gameover.h (shadows it via -I include).
// Only the constructor body differs (see the MATCH-ONLY note there). Keep the rest in sync.

#include <ad/ad_list.h>
#include <StaticAssert.h>
#include <ad/ad_autosave.h>
#include <gf/gf_scene.h>
#include <mu/adv/mu_adv_game_over.h>
#include <sr/sr_common.h>
#include <types.h>

class scAdvGameover : public gfScene {
    muAdvGameOverTask* m_task;
    bool unkC;
    adList<u32, Heaps::MenuInstance> unk10;
    adList<u32, Heaps::MenuInstance> unk24;
    adList<u32, Heaps::MenuInstance> unk38;
    adList<u32, Heaps::MenuInstance> unk4C;
    adList<u32, Heaps::MenuInstance> unk60;
    adList<u32, Heaps::MenuInstance> unk74;
public:
    static scAdvGameover* create();
    scAdvGameover(const char* name) : gfScene(name), m_task(nullptr), unkC(true) {
        // MATCH-ONLY: the self-assignment re-stores the last list's (already zero) fields after the init list, which
        // moves the shared zero constant ahead of the list vtable address in create's inlined ctor (r5 zero / r4 name
        // as in the original). No natural form found yet: init-list vs body orders, u8 vs bool unkC, default ctor
        // parameters, a body-style adList ctor and self-assigning any other member all stay at 90.09% or worse; only a
        // store over unk74's fields (from any list) matches.
        unk74 = unk74;
    }
    virtual void start();
    virtual s32 process();
    virtual s32 exit();
    virtual void setModule();
};
static_assert(sizeof(scAdvGameover) == 0x88, "Class is the wrong size!");
