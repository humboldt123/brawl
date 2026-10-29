#pragma once

#include <StaticAssert.h>
#include <gf/gf_scene.h>
#include <types.h>

class sqNetAnyWatch : public gfSequence {
public:
    s32 unk8;
    s32 unkC;
    u8 unk10;
    u8 unk11[0x7];

    static sqNetAnyWatch* create();
    sqNetAnyWatch(const char* name) : gfSequence(name), unk8(0) { }
    virtual void start();
    virtual void setNext();
    virtual void exit();
};
static_assert(sizeof(sqNetAnyWatch) == 0x18, "Class is wrong size!");
