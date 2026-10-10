#include <cm/cm_camera_controller.h>
#include <gr/gr_calc_world_callback.h>
#include <memory.h>
#include <mt/mt_prng.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <nw4r/math/math_triangular.h>

#include <st_palutena/gr_palutena.h>
#include <st_palutena/gr_palutena_anim.h>

grPalutenaAshibaE01* grPalutenaAshibaE01::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grPalutenaAshibaE01* ground = new (Heaps::StageInstance) grPalutenaAshibaE01(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grPalutenaAshibaE01::grPalutenaAshibaE01(const char* taskName) : grGimmickMovement(taskName) {
    m_dir = 0;
    m_landState = 0;
    m_landTimer = 0.0f;
    m_landTime = 0.0f;
    m_modelAnim = NULL;
    m_animState = 0;
    m_motion = 1;
    unk174 = 0.0f;
    m_motionSet = 0;
}

grPalutenaAshibaE01::~grPalutenaAshibaE01() {
    grPalutenaFreeModelAnim(m_modelAnim);
    m_modelAnim = NULL;
}

void grPalutenaAshibaE01::processAnim() {
    Ground::processAnim();
    switch (m_animState) {
    case 0:
        setMotionCommon(0, true, true, NULL);
        m_animState = 1;
        // fall through
    case 1:
        m_animState = 2;
        break;
    case 2:
        break;
    }
}

void grPalutenaAshibaE01::startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType) {
    grGimmickMovement::startup(data, 0, gfSceneRoot::Layer_Ground);
    m_calcWorldCallBack.m_nodeCallbackDatas[0].m_flags = 0x11;
}

void grPalutenaAshibaE01::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateMove(deltaFrame);
        updateLanding(deltaFrame);
        grGimmick::updateCallback(0);
    }
}

// The platform waits (state 1), then flies in from one side of the picture to the other (state 2) and starts again.
void grPalutenaAshibaE01::updateMove(float deltaFrame) {
    grGimmickMovement::updateMove(deltaFrame);
    stPalutenaData* stageData = static_cast<stPalutenaData*>(getStageData());
    if (stageData != NULL) {
        grGimmickMovementData* data = static_cast<grGimmickMovementData*>(getGimmickData());
        if (data != NULL) {
            switch (m_state) {
            case 2: {
                CameraController* camera = CameraController::getInstance();
                float* left = &camera->unk158;
                if (left != NULL) {
                    if (m_dir == 1) {
                        data->m_start.m_x = camera->unk15C + 50.0f;
                        data->m_goal.m_x = *left - 50.0f;
                    } else {
                        data->m_start.m_x = *left - 50.0f;
                        data->m_goal.m_x = camera->unk15C + 50.0f;
                    }
                }
                break;
            }
            case 0:
                getPosNode(&data->m_start, m_unk1, m_nodeIndex);
                getPosNode(&data->m_goal, m_unk1, m_nodeIndex);
                setPos(&data->m_start);
                setVisibility(0);
                setEnableCollisionStatus(false);
                m_timer = stageData->unk60;
                m_state = 1;
                break;
            case 1:
                if (m_timer == 0.0f) {
                    CameraController* camera = CameraController::getInstance();
                    float* left = &camera->unk158;
                    if (left != NULL) {
                        if (stageData->unk6C < randf()) {
                            data->m_start.m_x = *left - 50.0f;
                            data->m_goal.m_x = camera->unk15C + 50.0f;
                            m_dir = 0;
                        } else {
                            data->m_start.m_x = camera->unk15C + 50.0f;
                            data->m_goal.m_x = *left - 50.0f;
                            m_dir = 1;
                        }
                        if (m_isVisible != 1) {
                            setVisibility(1);
                        }
                        if (!m_isEnableCollisionStatus) {
                            setEnableCollisionStatus(true);
                        }
                        startMove();
                    }
                } else {
                    setVisibility(0);
                    setEnableCollisionStatus(false);
                }
                break;
            case 4:
                m_timer = stageData->unk64 + (stageData->unk68 - stageData->unk64) * randf();
                setVisibility(0);
                setEnableCollisionStatus(false);
                m_state = 1;
                break;
            }
        }
    }
}

void grPalutenaAshibaE01::updateLanding(float deltaFrame) {
    m_landTimer = m_landTimer - deltaFrame;
    if (m_landTimer < 0.0f) {
        m_landTimer = 0.0f;
    }
    grNodeCallbackData* data = m_calcWorldCallBack.m_nodeCallbackDatas;
    switch (m_landState) {
    case 1: {
        m_landTime = m_landTime + deltaFrame;
        float rate = m_landTime * 0.125f;
        if (rate < 0.0f) {
            rate = 0.0f;
        }
        if (1.0f < rate) {
            rate = 1.0f;
        }
        if (rate == 1.0f) {
            m_landState = 2;
            m_landTime = 0.0f;
        }
        data->m_offsetPos.m_y = nw4r::math::SinFIdx(NW4R_MATH_IDX_TO_FIDX(nw4r::math::U16ToF32((u16)(int)(rate * 16384.0f)))) * -3.0f;
        break;
    }
    case 2: {
        m_landTime = m_landTime + deltaFrame;
        float rate = m_landTime / 12.0f;
        if (rate < 0.0f) {
            rate = 0.0f;
        }
        if (1.0f < rate) {
            rate = 1.0f;
        }
        if (rate == 1.0f) {
            m_landState = 0;
            m_landTime = 0.0f;
        }
        data->m_offsetPos.m_y = -0.5f - (1.0f - nw4r::math::SinFIdx(NW4R_MATH_IDX_TO_FIDX(nw4r::math::U16ToF32((u16)(int)(rate * 16384.0f))))) * 2.5f;
        break;
    }
    }
    if (m_landTimer == 0.0f) {
        data->m_offsetPos.m_y = data->m_offsetPos.m_y + deltaFrame * 0.05f;
        if (0.0f < data->m_offsetPos.m_y) {
            data->m_offsetPos.m_y = 0.0f;
        }
    }
}

void grPalutenaAshibaE01::receiveCollMsg_Landing(grCollStatus* collStatus, grCollisionJoint* collisionJoint, bool isFirstContact) {
    bool start = false;
    if (m_landTimer == 0.0f) {
        start = true;
    }
    if (isFirstContact == 1) {
        start = true;
    }
    m_landTimer = 5.0f;
    if (start == 1) {
        m_landState = 1;
    }
}

void grPalutenaAshibaE01::setResCommon(nw4r::g3d::ResFile* resFile) {
    nw4r::g3d::ScnMdl* sceneMdl = *m_sceneModels;
    if (sceneMdl != NULL) {
        m_modelAnim = grPalutenaNewModelAnim(resFile);
        if (m_modelAnim != NULL) {
            m_modelAnim->_spacer[0] = 0;
            gfModelAnimation::bind(sceneMdl, m_modelAnim);
        }
    }
}

void grPalutenaAshibaE01::setMotionCommon(u32 animId, bool loop, bool force, float* frameCount) {
    if (m_motion == animId && force == 0) {
        return;
    }

    nw4r::g3d::ScnMdl* sceneMdl = *m_sceneModels;
    if (sceneMdl == NULL) {
        return;
    }

    gfModelAnimation* modelAnim = m_modelAnim;
    if (modelAnim == NULL) {
        return;
    }

    nw4r::g3d::ResMdl model = sceneMdl->m_resMdl;
    if (!model.IsValid()) {
        return;
    }

    modelAnim->unbindNodeAnim(sceneMdl);
    modelAnim->unbindVisibleAnim(sceneMdl);
    modelAnim->unbindTexAnim(sceneMdl);
    modelAnim->unbindTexSrtAnim(sceneMdl);
    modelAnim->unbindMatColAnim(sceneMdl);
    m_motion = animId;

    if (animId >= 1) {
        return;
    }

    bool result = (modelAnim->m_resFile.GetResAnmVisNumEntries() > animId);
    if (result) {
        grPalutenaSetVisibilityAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmChrNumEntries() > animId);
    if (result) {
        grPalutenaSetChrAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmTexPatNumEntries() > animId);
    if (result) {
        grPalutenaSetTexPatAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmTexSrtNumEntries() > animId);
    if (result) {
        grPalutenaSetTexSrtAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmClrNumEntries() > animId);
    if (result) {
        grPalutenaSetColorAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    gfModelAnimation::bind(sceneMdl, modelAnim);
    modelAnim->setFrame(0.0);
    modelAnim->setUpdateRate(1.0);
    modelAnim->setLoop(loop);

    if (frameCount != NULL) {
        *frameCount = modelAnim->getFrameCount();
    }
    m_motionSet = 1;
}
