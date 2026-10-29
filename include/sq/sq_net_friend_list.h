#pragma once

#include <StaticAssert.h>
#include <gf/gf_scene.h>
#include <types.h>

class sqNetFriendList : public gfSequence {
public:
    s32 unk8;
    u8 unkC;
    u8 unkD[0x3];
    s32 unk10;
    s32 unk14;

    static sqNetFriendList* create();
    sqNetFriendList(const char* name) : gfSequence(name), unk8(0) { }
    virtual void start();
    virtual void setNext();
    virtual void exit();
};
static_assert(sizeof(sqNetFriendList) == 0x18, "Class is wrong size!");
