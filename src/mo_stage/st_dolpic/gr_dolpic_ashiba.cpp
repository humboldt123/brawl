#include <gf/gf_model.h>
#include <gr/gr_calc_world_callback.h>
#include <nw4r/math/math_triangular.h>
#include <st_dolpic/gr_dolpic.h>
#include <st_dolpic/gr_dolpic_anim.h>

grDolpicAshiba* grDolpicAshiba::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grDolpicAshiba* ground = new (Heaps::StageInstance) grDolpicAshiba(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grDolpicAshiba::grDolpicAshiba(const char* taskName) : grDolpic(taskName) {
    m_offsetPos.m_x = 0.0f;
    m_offsetPos.m_y = 0.0f;
    m_offsetPos.m_z = 0.0f;
    m_rateWork = NULL;
    m_stateWork = NULL;
    m_prevPhase = 3;
    m_type = 0xD;
    m_rate = 0.0f;
    m_animId = 1;
    m_lastFrame = 0.0f;
    m_animFrames = 0.0f;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_nodeCallbackDatas[0].m_flags |= 0x10;
}

grDolpicAshiba::~grDolpicAshiba() { }

void grDolpicAshiba::update(float deltaFrame) {
    if (m_isUpdate) {
        updateScaleBase();
        updateActive(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

// A platform follows the phase the stage hands it (m_stateWork): 0 = rising out of the water, 1 = up, 2 = sinking,
// 3 = under water. It takes the stage data's fade time to change; the sine of the progress moves it.
void grDolpicAshiba::updateActive(float deltaFrame) {
    stDolpicParam* data = static_cast<stDolpicParam*>(getStageData());
    if (data != NULL) {
        float timer = m_timer;
        m_timer = timer - deltaFrame;
        if (timer - deltaFrame < 0.0f) {
            m_timer = 0.0f;
        }
        if (*m_stateWork != m_prevPhase) {
            m_state = 0;
        }
        u8* work = m_stateWork;
        switch (*work) {
        case 0:
            switch (m_state) {
            case 0:
                setMotion(0, 1, 1, &m_animFrames);
                setMotionFrame(m_animFrames, 0);
                if (m_motionRatio > 0.0f) {
                    m_motionRatio = m_motionRatio * -1.0f;
                }
                m_lastFrame = m_animFrames;
                m_timer = data->m_fadeTime;
                m_state = 1;
                break;
            case 1:
                if (getMotionFrame(0) > m_lastFrame) {
                    setMotion(1, 0, 1, NULL);
                    m_state = 3;
                } else {
                    if (!m_isVisible) {
                        setVisibility(1);
                        setEnableCollisionStatus(true);
                        if (m_collision != NULL) {
                            m_collision->setEnable();
                        }
                    }
                    m_lastFrame = getMotionFrame(0);
                }
                break;
            case 2:
                break;
            case 3:
                if (m_type == 2 && 1.0f == m_rate) {
                    *work = 1;
                }
                break;
            }
            m_rate = 1.0f - m_timer / data->m_fadeTime;
            if (m_rate < 0.0f) {
                m_rate = 0.0f;
            }
            if (m_rate > 1.0f) {
                m_rate = 1.0f;
            }
            {
                float sine = nw4r::math::SinIdx((u16)(int)(16384.0f * m_rate));
                float depth = data->m_sinkDepth;
                m_offsetPos.m_x = 0.0f;
                m_offsetPos.m_z = 0.0f;
                m_offsetPos.m_y = -depth + depth * sine;
                if (m_type == 2) {
                    *m_rateWork = sine;
                }
            }
            break;
        case 1:
            switch (m_state) {
            case 0:
                setMotion(1, 0, 1, NULL);
                m_state = 1;
                break;
            case 1:
                break;
            }
            if (m_type == 2) {
                *m_rateWork = 1.0f;
            }
            break;
        case 2:
            switch (m_state) {
            case 0:
                setMotion(0, 0, 1, &m_animFrames);
                if (m_motionRatio < 0.0f) {
                    m_motionRatio = m_motionRatio * -1.0f;
                }
                m_timer = data->m_fadeTime;
                m_state = 1;
                break;
            case 1:
                if (getMotionFrame(0) >= m_animFrames) {
                    setMotion(1, 0, 1, NULL);
                    setVisibility(0);
                    setEnableCollisionStatus(false);
                    if (m_collision != NULL) {
                        m_collision->setDisable();
                    }
                    m_state = 3;
                }
                break;
            case 2:
                break;
            case 3:
                if (m_type == 2 && 1.0f == m_rate) {
                    *work = 3;
                }
                break;
            }
            m_rate = 1.0f - m_timer / data->m_fadeTime;
            if (m_rate < 0.0f) {
                m_rate = 0.0f;
            }
            if (m_rate > 1.0f) {
                m_rate = 1.0f;
            }
            {
                float sine = nw4r::math::SinIdx((u16)(int)(16384.0f * m_rate));
                float depth = data->m_sinkDepth;
                m_offsetPos.m_x = 0.0f;
                m_offsetPos.m_z = 0.0f;
                m_offsetPos.m_y = -depth * sine;
                if (m_type == 2) {
                    *m_rateWork = 1.0f - sine;
                }
            }
            break;
        default:
            switch (m_state) {
            case 0:
                setMotion(1, 0, 1, NULL);
                setVisibility(0);
                setEnableCollisionStatus(false);
                if (m_collision != NULL) {
                    m_collision->setDisable();
                }
                m_state = 1;
                m_offsetPos.m_x = 0.0f;
                m_offsetPos.m_y = -data->m_sinkDepth;
                m_offsetPos.m_z = 0.0f;
                break;
            case 1:
                break;
            }
            break;
        }
        m_prevPhase = *m_stateWork;
    }
}

void grDolpicAshiba::updateCallBack(float deltaFrame) {
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
            grNodeCallbackData* data = calcWorldCallBack->m_nodeCallbackDatas;
            data->m_offsetPos.m_x = m_offsetPos.m_x;
            data->m_offsetPos.m_y = m_offsetPos.m_y;
            data->m_offsetPos.m_z = m_offsetPos.m_z;
            data = calcWorldCallBack->m_nodeCallbackDatas;
            data->m_scale = dolpicVec3Scaled(&m_scaleBase, 0.9f);
        }
    }
}

// Only the material color animation ("a sheen on the platform") is played here, and only the first one.
void grDolpicAshiba::setMotion(u32 animId, bool loop, bool force, float* frameCount) {
    if (m_animId == animId && force == 0) {
        return;
    }

    nw4r::g3d::ScnMdl* sceneMdl = *m_sceneModels;
    if (sceneMdl == NULL) {
        return;
    }

    gfModelAnimation* modelAnim = *m_modelAnims;
    if (modelAnim == NULL) {
        return;
    }

    nw4r::g3d::ResMdl model = sceneMdl->m_resMdl;
    if (!model.IsValid()) {
        return;
    }

    modelAnim->unbindMatColAnim(sceneMdl);
    m_animId = animId;

    if (animId != 0) {
        return;
    }

    bool result = (modelAnim->m_resFile.GetResAnmClrNumEntries() > animId);
    if (result) {
        grDolpicSetColorAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    gfModelAnimation::bind(sceneMdl, modelAnim);
    modelAnim->m_anmObjMatClrRes->SetFrame(0.0f);
    modelAnim->m_anmObjMatClrRes->SetUpdateRate(1.0f);
    modelAnim->setLoopMatCol(loop);

    if (frameCount != NULL) {
        *frameCount = modelAnim->m_anmObjMatClrRes->m_anmMatClrFile->m_animLength;
    }
}

void grDolpicAshiba::setMotionFrame(float frame, u32 sceneModelIndex) {
    gfModelAnimation* modelAnim = m_modelAnims[sceneModelIndex];
    if (modelAnim != NULL) {
        modelAnim->m_anmObjMatClrRes->SetFrame(frame);
    }
}

float grDolpicAshiba::getMotionFrame(u32 sceneModelIndex) {
    gfModelAnimation* modelAnim = m_modelAnims[sceneModelIndex];
    if (modelAnim == NULL) {
        return 0.0f;
    }
    return modelAnim->m_anmObjMatClrRes->GetFrame();
}
