#pragma once

#include <StaticAssert.h>
#include <gf/gf_scene.h>
#include <gf/gf_task.h>
#include <types.h>

// Result screen of the single-player modes (Classic, All-Star, ...); runs the main.dol result task.
class scSimpleResult : public gfScene {
    gfTask* m_task; // HYPOTHESIS: an ifSimpleResultTask (main.dol map), 0x27C bytes
public:
    static scSimpleResult* create();
    scSimpleResult(const char* name) : gfScene(name) { }
    virtual void start();
    virtual s32 process();
    virtual s32 exit();
};
static_assert(sizeof(scSimpleResult) == 0xC, "Class is the wrong size!");
