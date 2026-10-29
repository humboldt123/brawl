#pragma once

#include <StaticAssert.h>
#include <gf/gf_scene.h>
#include <types.h>

class sqEvent : public gfSequence {
public:
    s32 unk8;
    u8 unkC[0xC];
    u8 unk18;
    u8 unk19[0xB];
    s32 unk24;

    virtual void start();
    virtual void setNext();
    virtual void exit();
};
static_assert(sizeof(sqEvent) == 0x28, "Class is wrong size!");
