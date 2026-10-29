// Local BrawlHeaders shadow: stShadow shadow-object pool (sizes from the stShadow pool loops)
#pragma once

#include <StaticAssert.h>
#include <gf/gf_task.h>
#include <types.h>
#include <ut/ut_list.h>

class stShadow : public gfTask {
public:
    // HYPOTHESIS: the pool entries are list nodes (stShadow links them into its two lists).
    struct ShadowObject : public utListNode {
        char _0x8[0x14];
        u32 unk1c;
        char _0x20[0x14];

        ShadowObject();
    };

    char _0x40[4];
    u16 unk44;
    u16 unk46;
    char _0x48[0x34];
    u32 unk7c;
    ShadowObject* unk80;
    utList unk84;
    utList unk90;
    char _0x9c[0x1f0];

    virtual void renderPre();
    virtual ~stShadow();

    void clearShadowObject();
};
static_assert(sizeof(stShadow::ShadowObject) == 0x34, "Class is wrong size!");
static_assert(sizeof(stShadow) == 0x28c, "Class is wrong size!");
