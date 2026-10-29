#include <so/model/so_model_calc_world_callback.h>
#include <types.h>

extern "C" u32 fn_27_90D4(soModelModuleImpl* modelModule, nw4r::g3d::ChrAnmResult* pResult, u16 nodeId);
extern "C" u32 fn_27_923C(soModelModuleImpl* modelModule, nw4r::g3d::WorldMtxManip* pManip, u16 nodeId);

soModelModuleCalcWorldCallback::soModelModuleCalcWorldCallback(soModelModuleImpl* modelModule) : m_modelModule(modelModule) { }

soModelModuleCalcWorldCallback::~soModelModuleCalcWorldCallback() { }

void soModelModuleCalcWorldCallback::SetCallbackCondition(nw4r::g3d::ScnMdlSimple* mdl) {
    mdl->EnableScnMdlCallbackTiming(nw4r::g3d::ScnObj::CALLBACK_TIMING_A);
    mdl->EnableScnMdlCallbackTiming(nw4r::g3d::ScnObj::CALLBACK_TIMING_B);
    mdl->m_nodeIndex = 0;
}

void soModelModuleCalcWorldCallback::ExecCallbackA(nw4r::g3d::ChrAnmResult* pResult, nw4r::g3d::ResMdl mdl, nw4r::g3d::FuncObjCalcWorld* pFuncObj) {
    u32 nodeId = fn_27_90D4(m_modelModule, pResult, pFuncObj->GetCallbackNodeID());
    if (nodeId != -1) {
        pFuncObj->SetCallbackNodeID(nodeId);
    }
}

void soModelModuleCalcWorldCallback::ExecCallbackB(nw4r::g3d::WorldMtxManip* pManip, nw4r::g3d::ResMdl mdl, nw4r::g3d::FuncObjCalcWorld* pFuncObj) {
    u32 nodeId = fn_27_923C(m_modelModule, pManip, pFuncObj->GetCallbackNodeID());
    if (nodeId != -1) {
        pFuncObj->SetCallbackNodeID(nodeId);
    }
}
