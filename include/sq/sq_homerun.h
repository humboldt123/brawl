#pragma once

#include <StaticAssert.h>
#include <gf/gf_scene.h>
#include <types.h>

class sqHomerun : public gfSequence {
public:
    s32 unk8;
    u8 unkC[0x5];
    u8 unk11;
    u8 unk12;
    u8 unk13[0x11];

    static sqHomerun* create();
    sqHomerun(const char* name) : gfSequence(name), unk8(0) { }
    virtual void start();
    virtual void setNext();
    virtual void exit();
};
static_assert(sizeof(sqHomerun) == 0x24, "Class is wrong size!");
