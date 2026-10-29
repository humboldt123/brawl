#pragma once

#include <StaticAssert.h>
#include <gf/gf_scene.h>
#include <types.h>

class sqEdit : public gfSequence {
public:
    s32 unk8;

    static sqEdit* create();
    sqEdit(const char* name) : gfSequence(name), unk8(2) { }
    virtual void start();
    virtual void setNext();
    virtual void exit();
};
static_assert(sizeof(sqEdit) == 0xC, "Class is wrong size!");
