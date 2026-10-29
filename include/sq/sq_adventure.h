#pragma once

#include <StaticAssert.h>
#include <gf/gf_scene.h>
#include <types.h>

class sqAdventure : public gfSequence {
public:
    s32 unk8;
    u8 unkC[0x4];
    s32 unk10;
    s32 unk14;
    u8 unk18[0xC];
    s32 unk24;
    s32 unk28;

    static sqAdventure* create();
    sqAdventure(const char* name) : gfSequence(name), unk8(0), unk10(0), unk14(0), unk24(0), unk28(0) { }
    virtual void start();
    virtual void setNext();
    virtual void exit();
};
static_assert(sizeof(sqAdventure) == 0x2C, "Class is wrong size!");
