#pragma once

#include <StaticAssert.h>
#include <gf/gf_scene.h>
#include <types.h>

// Trophy display sequence: goes through the memory change scene into the trophy viewer scenes, then back to the main
// menu sequence.
class sqTyFigDisp : public gfSequence {
public:
    s32 m_state;
    s32 m_stateAfterMemoryChange;
    u8 m_memoryLayout;
    u8 unk11[0x3];

    static sqTyFigDisp* create();
    sqTyFigDisp(const char* name) : gfSequence(name), m_state(0) { }
    virtual void start();
    virtual void setNext();
    virtual void exit();
};
static_assert(sizeof(sqTyFigDisp) == 0x14, "Class is wrong size!");
