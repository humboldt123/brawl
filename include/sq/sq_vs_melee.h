#pragma once

#include <StaticAssert.h>
#include <gf/gf_scene.h>
#include <types.h>

class sqVsMelee : public gfSequence {
public:
    s32 unk8;
    s32 unkC;
    u8 unk10[0x4];
    gfSceneManager* unk14;

    static sqVsMelee* create();
    sqVsMelee(const char* name) : gfSequence(name), unk14(gfSceneManager::getInstance()) { }
    virtual void start();
    virtual void setNext();
    virtual void exit();
};
static_assert(sizeof(sqVsMelee) == 0x18, "Class is wrong size!");
