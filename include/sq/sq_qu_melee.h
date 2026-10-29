#pragma once

#include <StaticAssert.h>
#include <gf/gf_scene.h>
#include <types.h>

class sqQuMelee : public gfSequence {
public:
    s32 unk8;
    u8 unkC[0xC];
    u8 unk18;
    u8 unk19[0x3];

    static sqQuMelee* create();
    sqQuMelee(const char* name) : gfSequence(name), unk8(0) { }
    virtual void start();
    virtual void setNext();
    virtual void exit();
};
static_assert(sizeof(sqQuMelee) == 0x1C, "Class is wrong size!");
