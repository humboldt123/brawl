#include <st_dxbigblue/gr_dxbigblue_fly.h>
#include <st_dxbigblue/gr_dxbigblue_util.h>

// HYPOTHESIS: a flag of sora_melee that is set while the match runs (its name is unknown).
extern u8 lbl_27_data_547E4;

grDxBigBlueFly::grDxBigBlueFly(const char* taskName) : grDxBigBlue(taskName) {
    m_posLimitWork = NULL;
    unk158 = 0.0f;
    m_landedTimer = 0.0f;
    m_flyDataWork = NULL;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 1;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 1;
    callback->m_nodeCallbackDatas[0].m_flags |= 2;
}

grDxBigBlueFly::~grDxBigBlueFly() { }

void grDxBigBlueFly::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (lbl_27_data_547E4 != 0) {
        m_landedTimer -= deltaFrame;
        if (m_landedTimer < 0.0f) {
            m_landedTimer = 0.0f;
        }
        if (m_landedTimer == 0.0f) {
            m_flyDataWork->m_landed = 0;
        }
    }
    updateActive(deltaFrame);
    updateCallBack(deltaFrame);
}

void grDxBigBlueFly::updateActive(float deltaFrame) { }

// The model follows the record of the stage.
void grDxBigBlueFly::updateCallBack(float deltaFrame) {
    grCalcWorldCallBack* calcWorldCallBack = &m_calcWorldCallBack;
    if (calcWorldCallBack != NULL) {
        nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[0];
        if (scnMdl != NULL) {
            DXBIGBLUE_BIND_CALLBACK(calcWorldCallBack, scnMdl)
            if (m_flyDataWork != NULL) {
                calcWorldCallBack->m_nodeCallbackDatas[0].m_pos = m_flyDataWork->m_pos;
                calcWorldCallBack->m_nodeCallbackDatas[0].m_rot = m_flyDataWork->m_rot;
            }
        }
    }
}

// A fighter stands on the platform: it counts as ridden for five frames.
void grDxBigBlueFly::receiveCollMsg_Landing(grCollStatus* collStatus, grCollisionJoint* collisionJoint, bool isFirstContact) {
    // HYPOTHESIS: the flags that tell which kind of owner the collision status has (a fighter)
    static int sOwnerFlags[2] = { 1, 0 };
    if (isCollisionStatusOwnerTask__6GroundFP12grCollStatusPi(this, collStatus, sOwnerFlags)) {
        m_landedTimer = 5.0f;
        m_flyDataWork->m_landed = 1;
    }
}
