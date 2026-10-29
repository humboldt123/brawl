#pragma once

#include <StaticAssert.h>
#include <gf/gf_scene.h>
#include <types.h>

class sqBoot : public gfSequence {
public:
    s32 unk8;
    s32 unkC;
    u8 unk10;
    u8 unk11[0x3];
    s32 unk14;

    static sqBoot* create();
    sqBoot(const char* name) : gfSequence(name), unk8(0) { }
    virtual void start();
    virtual void setNext();
    virtual void exit();
};
static_assert(sizeof(sqBoot) == 0x18, "Class is wrong size!");
