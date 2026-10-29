#pragma once

#include <StaticAssert.h>
#include <gf/gf_scene.h>
#include <types.h>

class sqSpMelee : public gfSequence {
public:
    s32 unk8;
    u8 unkC[0x5];
    u8 unk11;
    u8 unk12[0x2];

    static sqSpMelee* create();
    sqSpMelee(const char* name) : gfSequence(name), unk8(0) { }
    virtual void start();
    virtual void setNext();
    virtual void exit();
};
static_assert(sizeof(sqSpMelee) == 0x14, "Class is wrong size!");
