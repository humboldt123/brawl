#include <so/model/so_model_scn_obj_callback.h>
#include <types.h>

void ScnObj_EnableCallbackTiming(nw4r::g3d::ScnObj* obj, u32 timing);
void ScnObj_EnableCallbackExecOp(nw4r::g3d::ScnObj* obj, u32 flag);

extern "C" void fn_27_94D0(soModelModuleImpl* modelModule, void* info);
extern "C" void fn_27_96D8(soModelModuleImpl* modelModule, void* info);

soModelModuleScnObjCallback::soModelModuleScnObjCallback(soModelModuleImpl* modelModule) : m_modelModule(modelModule) { }

soModelModuleScnObjCallback::~soModelModuleScnObjCallback() { }

void soModelModuleScnObjCallback::SetCallbackCondition(nw4r::g3d::ScnObj* obj) {
    ScnObj_EnableCallbackTiming(obj, nw4r::g3d::ScnObj::CALLBACK_TIMING_C);
    ScnObj_EnableCallbackExecOp(obj, 1);
    ScnObj_EnableCallbackExecOp(obj, 4);
}

void soModelModuleScnObjCallback::ExecCallback_CALC_VIEW(nw4r::g3d::ScnObj::Timing timing, nw4r::g3d::ScnObj* obj, u32 param, void* info) {
    switch (timing) {
        case nw4r::g3d::ScnObj::CALLBACK_TIMING_A:
            fn_27_94D0(m_modelModule, info);
            break;
        case nw4r::g3d::ScnObj::CALLBACK_TIMING_C:
            fn_27_96D8(m_modelModule, info);
            break;
    }
}
