#include <gr/gr_calc_world_callback.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <nw4r/math/math_types.h>
#include <st_dolpic/gr_dolpic.h>

void ScnMdl_SetNodeMtx(nw4r::g3d::ScnMdl* mdl, u32 nodeId, const Matrix* mtx);
// BrawlHeaders lacks ScnObj::GetMtx and the scene callback member (its pointer is at 0xD4, as in if_marth_final.cpp).
extern "C" bool GetMtx__Q34nw4r3g3d6ScnObjCFQ44nw4r3g3d6ScnObj13ScnObjMtxTypePQ34nw4r4math5MTX34(
    const nw4r::g3d::ScnObj* object, int type, nw4r::math::MTX34* mtx);
static inline void setScnObjCallback(nw4r::g3d::ScnObj* object, nw4r::g3d::IScnObjCallback* callback) {
    *reinterpret_cast<nw4r::g3d::IScnObjCallback**>(reinterpret_cast<u8*>(object) + 0xD4) = callback;
}
static inline nw4r::g3d::IScnObjCallback* getScnObjCallback(nw4r::g3d::ScnObj* object) {
    return *reinterpret_cast<nw4r::g3d::IScnObjCallback**>(reinterpret_cast<u8*>(object) + 0xD4);
}
void ScnObj_EnableCallbackExecOp(nw4r::g3d::ScnObj* pObj, u32 flag);
void ScnObj_EnableCallbackTiming(nw4r::g3d::ScnObj* pObj, u32 flag);

void grDolpicShineScnObjCallBack::SetCallBackCondition(nw4r::g3d::ScnObj* object) {
    ScnObj_EnableCallbackTiming(object, 1);
    ScnObj_EnableCallbackExecOp(object, 2);
}

// Turns the model's node 1 about the point m_pos by m_rot (degrees).
void grDolpicShineScnObjCallBack::ExecCallback_CALC_WORLD(nw4r::g3d::ScnObj::Timing timing, nw4r::g3d::ScnObj* object, u32 param, void* info) {
    if (object != NULL && timing != 3 && timing < 3 && timing != 1 && timing >= 1) {
        Matrix mtx(true);
        Matrix rotation(true);
        nw4r::math::VEC3 trans;
        GetMtx__Q34nw4r3g3d6ScnObjCFQ44nw4r3g3d6ScnObj13ScnObjMtxTypePQ34nw4r4math5MTX34(object, nw4r::g3d::ScnObj::MTX_WORLD, &mtx);
        trans.x = -m_pos.m_x;
        trans.y = -m_pos.m_y;
        trans.z = -m_pos.m_z;
        nw4r::math::MTX34Trans(&mtx, &trans, &mtx);
        nw4r::math::MTX34RotXYZFIdx(&rotation, 0.7111111f * m_rot.m_x, 0.7111111f * m_rot.m_y, 0.7111111f * m_rot.m_z);
        nw4r::math::MTX34Mult(&mtx, &rotation, &mtx);
        trans.x = m_pos.m_x;
        trans.y = m_pos.m_y;
        trans.z = m_pos.m_z;
        nw4r::math::MTX34Trans(&mtx, &trans, &mtx);
        ScnMdl_SetNodeMtx(reinterpret_cast<nw4r::g3d::ScnMdl*>(object), 1, &mtx);
    }
}

inline grDolpicShine::grDolpicShine(const char* taskName) : grDolpic(taskName) {
    m_mtxWork = NULL;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_nodeCallbackDatas[0].m_flags |= 1;
}

grDolpicShine* grDolpicShine::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grDolpicShine* ground = new (Heaps::StageInstance) grDolpicShine(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grDolpicShine::~grDolpicShine() {
    if (*m_sceneModels != NULL) {
        setScnObjCallback(*m_sceneModels, NULL);
    }
}

void grDolpicShine::update(float deltaFrame) {
    if (m_isUpdate) {
        updateScaleBase();
        updateCallBack(deltaFrame);
    }
}

void grDolpicShine::updateCallBack(float deltaFrame) {
    grCalcWorldCallBack* calcWorldCallBack = &m_calcWorldCallBack;
    if (calcWorldCallBack != NULL) {
        nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[0];
        if (scnMdl != NULL && m_mtxWork != NULL) {
            if (scnMdl->m_calcWorldCallBack == NULL) {
                calcWorldCallBack->m_index = 0;
                calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex = m_nodeIndex;
                scnMdl->m_calcWorldCallBack = calcWorldCallBack;
                scnMdl->EnableScnMdlCallbackTiming(1);
                scnMdl->m_nodeIndex = calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex;
            }
            Vec3f pos;
            Vec3f rot;
            m_mtxWork->getPosition(&pos);
            m_mtxWork->getRotate(&rot);
            rot.m_x = 57.29578f * rot.m_x;
            rot.m_y = 57.29578f * rot.m_y;
            rot.m_z = 57.29578f * rot.m_z;
            grNodeCallbackData* data = calcWorldCallBack->m_nodeCallbackDatas;
            data->m_pos = pos;
            data = calcWorldCallBack->m_nodeCallbackDatas;
            data->m_scale = dolpicVec3Scaled(&m_scaleBase, 0.9f);
            if (getScnObjCallback(scnMdl) == NULL) {
                m_scnObjCallBack.SetCallBackCondition(scnMdl);
                setScnObjCallback(scnMdl, &m_scnObjCallBack);
            }
            m_scnObjCallBack.setPos(pos.m_x, pos.m_y, pos.m_z);
            m_scnObjCallBack.setRot(rot.m_x, rot.m_y, rot.m_z);
        }
    }
}
