#pragma once

#include <StaticAssert.h>
#include <gf/gf_scene.h>
#include <types.h>

class sqSingleBoss : public gfSequence {
public:
    s32 unk8;
    u8 unkC[0x1C];
    s32 unk28;

    static sqSingleBoss* create();
    sqSingleBoss(const char* name) : gfSequence(name), unk8(0), unk28(0) { }
    virtual void start();
    virtual void setNext();
    virtual void exit();
};
static_assert(sizeof(sqSingleBoss) == 0x2C, "Class is wrong size!");
