#pragma once

#include <StaticAssert.h>
#include <gf/gf_scene.h>
#include <types.h>

class sqReplay : public gfSequence {
public:
    s32 unk8;
    u8 unkC[0x8];

    static sqReplay* create();
    sqReplay(const char* name) : gfSequence(name), unk8(0) { }
    virtual void start();
    virtual void setNext();
    virtual void exit();
};
static_assert(sizeof(sqReplay) == 0x14, "Class is wrong size!");
