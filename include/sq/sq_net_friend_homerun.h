#pragma once

#include <StaticAssert.h>
#include <gf/gf_scene.h>
#include <types.h>

class sqNetFriendHomerun : public gfSequence {
public:
    s32 unk8;
    u8 unkC[0x14];

    static sqNetFriendHomerun* create();
    sqNetFriendHomerun(const char* name) : gfSequence(name), unk8(0) { }
    virtual void start();
    virtual void setNext();
    virtual void exit();
};
static_assert(sizeof(sqNetFriendHomerun) == 0x20, "Class is wrong size!");
