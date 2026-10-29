#pragma once

#include <StaticAssert.h>
#include <types.h>
#include <nw4r/g3d/g3d_scnobjcallback.h>

class soModelModuleImpl;

class soModelModuleScnObjCallback : public nw4r::g3d::IScnObjCallback {
public:
    soModelModuleImpl* m_modelModule; // 0x4

    soModelModuleScnObjCallback(soModelModuleImpl* modelModule);
    virtual ~soModelModuleScnObjCallback();
    // HYPOTHESIS: the parameter type (the function only passes it to the ScnObj callback enablers).
    static void SetCallbackCondition(nw4r::g3d::ScnObj* obj);
    virtual void ExecCallback_CALC_VIEW(nw4r::g3d::ScnObj::Timing timing, nw4r::g3d::ScnObj* obj, u32 param, void* info);
};
static_assert(sizeof(soModelModuleScnObjCallback) == 0x8, "Class is wrong size!");
