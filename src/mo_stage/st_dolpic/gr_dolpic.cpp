#include <gr/gr_calc_world_callback.h>
#include <math.h>
#include <nw4r/g3d/g3d_resmdl.h>
#include <nw4r/g3d/g3d_scnobj.h>
#include <st_dolpic/gr_dolpic.h>

grDolpic::grDolpic(const char* taskName) : grYakumono(taskName) {
    m_state = 0;
    m_timer = 0.0f;
    m_scaleBase.m_x = 0.0f;
    m_scaleBase.m_y = 0.0f;
    m_scaleBase.m_z = 0.0f;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 1;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 4;
    setupMelee();
}

grDolpic::~grDolpic() { }

void grDolpic::update(float deltaFrame) {
    if (m_isUpdate) {
        updateScaleBase();
        updateCallBack(deltaFrame);
    }
}

static inline bool dolpicIsZero(float value) {
    bool result = false;
    if ((float)fabs(value) < 1e-5f) {
        result = true;
    }
    return result;
}

// Reads the scale of the model's root node once (the callbacks keep 0.9 of it).
void grDolpic::updateScaleBase() {
    bool unset = false;
    if (dolpicIsZero(m_scaleBase.m_x) && dolpicIsZero(m_scaleBase.m_y) && dolpicIsZero(m_scaleBase.m_z)) {
        unset = true;
    }
    if (unset) {
        nw4r::g3d::ResMdl model;
        nw4r::g3d::ScnMdl* scnMdl = *m_sceneModels;
        if (scnMdl != NULL) {
            model = scnMdl->m_resMdl;
            if (model.IsValid()) {
                nw4r::g3d::ResNode node = model.GetResNode((u32)m_nodeIndex);
                if (node.IsValid()) {
                    m_scaleBase.m_x = node->m_scale.m_x;
                    m_scaleBase.m_y = node->m_scale.m_y;
                    m_scaleBase.m_z = node->m_scale.m_z;
                }
            }
        }
    }
}

void grDolpic::updateCallBack(float deltaFrame) {
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
            Vec3f scale = dolpicVec3Scaled(&m_scaleBase, 0.9f);
            grNodeCallbackData* data = calcWorldCallBack->m_nodeCallbackDatas;
            data->m_scale = scale;
        }
    }
}
