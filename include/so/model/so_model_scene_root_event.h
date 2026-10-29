#pragma once
// Local BrawlHeaders shadow: adds the model-module member and the destructor.

#include <StaticAssert.h>
#include <gf/gf_3d_scene_event.h>
#include <types.h>

class soModelModuleImpl;

class soModelSceneRootEvent : public gfSceneRootEvent {
public:
    soModelModuleImpl* m_modelModule; // 0xC

    ~soModelSceneRootEvent();
    virtual void notify(s32 p1);
};
static_assert(sizeof(soModelSceneRootEvent) == 0x10, "Class is wrong size!");
