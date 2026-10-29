#pragma once

#include <StaticAssert.h>
#include <gf/gf_scene.h>
#include <types.h>

class sqPrizeCheck : public gfSequence {
public:
    s32 unk8;
    u8 unkC[0x8];
    s32 unk14;
    u8 unk18[0x8];

    static sqPrizeCheck* create();
    sqPrizeCheck(const char* name) : gfSequence(name), unk8(0) { }
    virtual void start();
    virtual void setNext();
    virtual void exit();

    bool checkModeMeleeClear();
};
static_assert(sizeof(sqPrizeCheck) == 0x20, "Class is wrong size!");
