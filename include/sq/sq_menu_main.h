#pragma once

#include <StaticAssert.h>
#include <gf/gf_scene.h>
#include <types.h>

class sqMenuMain : public gfSequence {
public:
    s32 unk8;
    s32 unkC;
    s32 unk10;

    static sqMenuMain* create();
    sqMenuMain(const char* name) : gfSequence(name) { }
    virtual void start();
    virtual void setNext();
    virtual void exit();
};
static_assert(sizeof(sqMenuMain) == 0x14, "Class is wrong size!");
