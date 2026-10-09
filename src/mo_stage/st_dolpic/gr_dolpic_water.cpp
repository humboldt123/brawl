#include <gr/gr_calc_world_callback.h>
#include <st_dolpic/gr_dolpic.h>

// scale(in, scale, out) of main: scales the rotation part of a matrix
extern "C" void fn_8003E828(Matrix* in, Vec3f* scale, Matrix* out);

inline grDolpicWater::grDolpicWater(const char* taskName) : grDolpic(taskName) {
    m_mtxWork = NULL;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_nodeCallbackDatas[0].m_flags |= 8;
}

grDolpicWater* grDolpicWater::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grDolpicWater* ground = new (Heaps::StageInstance) grDolpicWater(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grDolpicWater::~grDolpicWater() { }

void grDolpicWater::update(float deltaFrame) {
    if (m_isUpdate) {
        updateScaleBase();
        updateCallBack(deltaFrame);
    }
}

void grDolpicWater::updateCallBack(float deltaFrame) {
    grCalcWorldCallBack* calcWorldCallBack = &m_calcWorldCallBack;
    if (calcWorldCallBack != NULL) {
        nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[0];
        if (scnMdl != NULL) {
            if (scnMdl->m_calcWorldCallBack == NULL) {
                calcWorldCallBack->m_index = 0;
                calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex = m_nodeIndex;
                scnMdl->m_calcWorldCallBack = calcWorldCallBack;
                scnMdl->EnableScnMdlCallbackTiming(1);
                scnMdl->m_nodeIndex = calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex;
            }
            Matrix mtx = *m_mtxWork;
            Vec3f scale;
            scale = dolpicVec3Scaled(&m_scaleBase, 0.9f);
            fn_8003E828(&mtx, &scale, &mtx);
            calcWorldCallBack->m_nodeCallbackDatas->m_matrix = mtx;
        }
    }
}
