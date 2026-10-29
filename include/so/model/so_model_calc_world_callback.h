#pragma once

#include <StaticAssert.h>
#include <nw4r/g3d/g3d_calcworld.h>
#include <nw4r/g3d/g3d_scnmdlsmpl.h>
#include <types.h>

class soModelModuleImpl;

class soModelModuleCalcWorldCallback : public nw4r::g3d::ICalcWorldCallback {
public:
    soModelModuleImpl* m_modelModule; // 0x4

    soModelModuleCalcWorldCallback(soModelModuleImpl* modelModule);
    virtual ~soModelModuleCalcWorldCallback();
    // HYPOTHESIS: the parameter type (the function enables the ScnMdlSimple callback timings and resets its node id).
    static void SetCallbackCondition(nw4r::g3d::ScnMdlSimple* mdl);
    virtual void ExecCallbackA(nw4r::g3d::ChrAnmResult* pResult, nw4r::g3d::ResMdl mdl, nw4r::g3d::FuncObjCalcWorld* pFuncObj);
    virtual void ExecCallbackB(nw4r::g3d::WorldMtxManip* pManip, nw4r::g3d::ResMdl mdl, nw4r::g3d::FuncObjCalcWorld* pFuncObj);
};
static_assert(sizeof(soModelModuleCalcWorldCallback) == 0x8, "Class is wrong size!");
