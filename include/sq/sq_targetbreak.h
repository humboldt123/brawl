#pragma once

#include <StaticAssert.h>
#include <gf/gf_scene.h>
#include <types.h>

class sqTargetBreak : public gfSequence {
public:
    s32 unk8;
    u8 unkC[0x8];
    s32 unk14;
    u8 unk18;
    u8 unk19;
    u8 unk1A[0x2];
    s32 unk1C;

    static sqTargetBreak* create();
    sqTargetBreak(const char* name) : gfSequence(name), unk8(0), unk1C(0) { }
    virtual void start();
    virtual void setNext();
    virtual void exit();
};
static_assert(sizeof(sqTargetBreak) == 0x20, "Class is wrong size!");
