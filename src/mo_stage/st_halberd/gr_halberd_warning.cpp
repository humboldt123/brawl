#include <gr/gr_calc_world_callback.h>
#include <memory.h>

#include <st_halberd/gr_halberd.h>
#include <st_halberd/gr_halberd_anim.h>

grHalberdWarning* grHalberdWarning::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grHalberdWarning* ground = new (Heaps::StageInstance) grHalberdWarning(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grHalberdWarning::grHalberdWarning(const char* taskName) : grHalberd(taskName) {
    m_mtxWork = NULL;
    m_motion = 3;
    m_lastFrame = 0.0f;
    m_motionTimer = 0.0f;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 1;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 8;
    m_seId[0] = snd_se_stage_Halberd_Warning_open;
    m_seId[1] = snd_se_stage_Halberd_Warning_siren;
    m_seData0[0].id = snd_se_stage_Halberd_Warning_open;
    m_seData0[0].unk4 = 0.0f;
    m_seData0[0].unk8 = 0.0f;
    m_seData0[0].unkC = 0.0f;
    m_seData1[0].id = snd_se_stage_Halberd_Warning_siren;
    m_seData1[0].unk4 = 0.0f;
    m_seData1[0].unk8 = 20.0f;
    m_seData1[0].unkC = 0.0f;
    m_seSeq.registId(m_seId, 2);
    m_seSeq.registSeq(0, m_seData0, 1, Heaps::StageInstance);
    m_seSeq.registSeq(1, m_seData1, 1, Heaps::StageInstance);
    m_seSeq.m_sndGenerator = &m_snd;
}

grHalberdWarning::~grHalberdWarning() {
}

void grHalberdWarning::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateMotion(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

// The warning light and siren: it lights up when the story asks for it (0), runs until the end of its first
// animation, blinks until the timer runs out and goes out again.
void grHalberdWarning::updateMotion(float deltaFrame) {
    if (m_stateWork != NULL) {
        stHalberdData* data = static_cast<stHalberdData*>(getStageData());
        if (data != NULL) {
            m_timer -= deltaFrame;
            if (m_timer < 0.0f) {
                m_timer = 0.0f;
            }
            switch (m_state) {
            case 0:
                setMotion(3, 0, 1, NULL);
                setVisibility(0);
                m_state = 2;
                // fall through
            case 2:
                if (*m_stateWork == 0) {
                    setMotion(0, 0, 1, &m_motionTimer);
                    setVisibility(1);
                    m_seSeq.playFrame(0, getMotionFrame(0));
                    m_timer = data->unk20;
                    m_state = 10;
                }
                break;
            case 10:
                if (m_isVisible >= 0) {
                    setVisibility(1);
                }
                if (m_motionTimer <= getMotionFrame(0)) {
                    setMotion(1, 1, 1, &m_motionTimer);
                    m_state = 11;
                    m_lastFrame = 0.0f;
                }
                break;
            case 11:
                m_seSeq.playFrame(1, getMotionFrame(0));
                if (m_timer == 0.0f && getMotionFrame(0) <= m_lastFrame) {
                    setMotion(2, 0, 1, &m_motionTimer);
                    m_state = 12;
                }
                m_lastFrame = getMotionFrame(0);
                break;
            case 12:
                if (m_motionTimer <= getMotionFrame(0)) {
                    *m_stateWork = 0x15;
                    m_state = 0;
                }
                break;
            }
        }
    }
}

// The warning model follows the matrix of the warning position of the ship (1) or the dome (0).
void grHalberdWarning::updateCallBack(float deltaFrame) {
    grCalcWorldCallBack* calcWorldCallBack = &m_calcWorldCallBack;
    if (calcWorldCallBack != NULL) {
        nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[0];
        if (scnMdl != NULL) {
            if (scnMdl->m_calcWorldCallBack == NULL) {
                calcWorldCallBack->m_index = 0;
                scnMdl->m_calcWorldCallBack = calcWorldCallBack;
                scnMdl->EnableScnMdlCallbackTiming(1);
                scnMdl->m_nodeIndex = calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex;
            }
            if (m_mtxWork != NULL) {
                Matrix mtx;
                if (*m_stateWork == 1) {
                    mtx = m_mtxWork[2];
                } else {
                    if (*m_stateWork != 0) {
                        return;
                    }
                    mtx = m_mtxWork[3];
                }
                calcWorldCallBack->m_nodeCallbackDatas[0].m_matrix = mtx;
                Vec3f pos(mtx.m[0][3], mtx.m[1][3], mtx.m[2][3]);
                m_snd.setPos(&pos);
            }
        }
    }
}

// The animation of the warning (3 animations).
void grHalberdWarning::setMotion(u32 animId, bool shouldLoop, bool force, float* frameCount) {
    if (m_motion == animId && force == 0) {
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

    modelAnim->unbindNodeAnim(sceneMdl);
    modelAnim->unbindVisibleAnim(sceneMdl);
    modelAnim->unbindTexAnim(sceneMdl);
    modelAnim->unbindTexSrtAnim(sceneMdl);
    modelAnim->unbindMatColAnim(sceneMdl);
    m_motion = animId;

    if (animId >= 3) {
        return;
    }

    bool result = (modelAnim->m_resFile.GetResAnmChrNumEntries() > animId);
    if (result) {
        grHalberdSetChrAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmVisNumEntries() > animId);
    if (result) {
        grHalberdSetVisibilityAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmTexPatNumEntries() > animId);
    if (result) {
        grHalberdSetTexPatAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmTexSrtNumEntries() > animId);
    if (result) {
        grHalberdSetTexSrtAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmClrNumEntries() > animId);
    if (result) {
        grHalberdSetColorAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    gfModelAnimation::bind(sceneMdl, modelAnim);
    modelAnim->setFrame(0.0);
    modelAnim->setUpdateRate(1.0);
    modelAnim->setLoop(shouldLoop);

    if (frameCount != NULL) {
        *frameCount = modelAnim->getFrameCount();
    }
}
