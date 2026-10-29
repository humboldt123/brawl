#pragma once

#include <StaticAssert.h>
#include <gf/gf_scene.h>
#include <types.h>

class sqKumite : public gfSequence {
public:
    s32 unk8;
    u8 unkC[0x10];
    s32 unk1C;

    static sqKumite* create();
    sqKumite(const char* name) : gfSequence(name), unk8(0), unk1C(0) { }
    virtual void start();
    virtual void setNext();
    virtual void exit();
};
static_assert(sizeof(sqKumite) == 0x20, "Class is wrong size!");
