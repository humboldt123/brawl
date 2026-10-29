#pragma once

#include <StaticAssert.h>
#include <gf/gf_scene.h>
#include <types.h>

class sqTitle : public gfSequence {
public:
    s32 unk8;
    u8 unkC[0x8];
    s32 unk14;
    s32 unk18;
    u8 unk1C;
    u8 unk1D[0x3];

    static sqTitle* create();
    sqTitle(const char* name) : gfSequence(name), unk8(2) { }
    virtual void start();
    virtual void setNext();
    virtual void exit();
};
static_assert(sizeof(sqTitle) == 0x20, "Class is wrong size!");
