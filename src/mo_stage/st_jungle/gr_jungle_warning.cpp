#include <memory.h>
#include <types.h>

#include <st_jungle/gr_jungle.h>
#include <st_jungle/gr_jungle_anim.h>

grJungle::grJungle(const char* taskName) : grYakumono(taskName) {
    m_state = 0;
    m_timer = 0.0f;
    setupMelee();
}

grJungle::~grJungle() {
}

void grJungle::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
}

grJungleWarning* grJungleWarning::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grJungleWarning* ground = new (Heaps::StageInstance) grJungleWarning(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grJungleWarning::grJungleWarning(const char* taskName) : grJungle(taskName), m_seSeq() {
    m_stateWork = NULL;
    m_motion = 1;
    m_frameLast = 0.0f;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback != NULL) {
        callback->m_numNodeCallbackData = 1;
        callback->initialize(false, Heaps::StageInstance);
        callback->m_nodeCallbackDatas[0].m_flags |= 1;
    }
    // The sound of the sign: one sequence of one sound from the start.
    m_seSeqId = static_cast<SndID>(0x1EEE);
    m_seSeqData.id = static_cast<SndID>(0x1EEE);
    m_seSeqData.unk4 = 0.0f;
    m_seSeqData.unk8 = 0.0f;
    m_seSeqData.unkC = 0.0f;
    m_seSeq.registId(&m_seSeqId, 1);
    m_seSeq.registSeq(0, &m_seSeqData, 1, Heaps::StageInstance);
}

grJungleWarning::~grJungleWarning() {
}

void grJungleWarning::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateActive(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

// The sign starts hidden; while the stage says so (the state 0) it shows itself and its animation and sound loop. When the
// stage clears it the sign waits for the end of the animation and hides again.
void grJungleWarning::updateActive(float deltaFrame) {
    switch (m_state) {
    case 0:
        setMotion(1, false, true, NULL);
        setVisibility(0);
        m_state = 1;
    case 1:
        if (*m_stateWork == 0) {
            setMotion(0, true, true, NULL);
            m_frameLast = 0.0f;
            m_seSeq.playFrame(0, getMotionFrame(0), 0.0f);
            m_state = 4;
        }
        break;
    case 4:
        if (!m_isVisible) {
            setVisibility(1);
        }
        m_seSeq.playFrame(0, getMotionFrame(0));
        if (*m_stateWork == 1 && getMotionFrame(0) < m_frameLast) {
            m_state = 0;
        }
        m_frameLast = getMotionFrame(0);
        break;
    }
}

// The sign is in front of the camera: where the line from the camera through the middle of the screen meets the plane of the
// stage, 20 units in front of it.
void grJungleWarning::updateCallBack(float deltaFrame) {
    grCalcWorldCallBack* calcWorldCallBack = &m_calcWorldCallBack;
    if (calcWorldCallBack != NULL) {
        nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[m_unk1];
        if (scnMdl != NULL) {
            if (scnMdl->m_calcWorldCallBack == NULL) {
                calcWorldCallBack->m_index = 0;
                calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex = m_nodeIndex;
                scnMdl->m_calcWorldCallBack = calcWorldCallBack;
                scnMdl->EnableScnMdlCallbackTiming(1);
                scnMdl->m_nodeIndex = calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex;
            }
            Vec3f hit;
            hit.m_x = 0.0f;
            hit.m_y = 0.0f;
            hit.m_z = 0.0f;
            Vec3f dir;
            dir.m_x = 0.0f;
            dir.m_y = 0.0f;
            dir.m_z = 0.0f;
            gfCameraManager* manager = gfCameraManager::getManager();
            if (manager != NULL) {
                // MATCH-ONLY: the size of the screen is at 0x11C / 0x120 of the camera
                float* screen = reinterpret_cast<float*>(reinterpret_cast<u8*>(manager) + 0x114);
                if (screen != NULL) {
                    fn_80019E88(manager, &dir, 0.5f * screen[2], 0.5f * screen[3]);
                    struct {
                        Vec3f origin;
                        Vec3f direction;
                    } line;
                    hit.m_x = manager->m_cameras[0].m_centerPos.m_x;
                    hit.m_y = manager->m_cameras[0].m_centerPos.m_y;
                    hit.m_z = manager->m_cameras[0].m_centerPos.m_z;
                    line.origin.m_x = hit.m_x;
                    line.origin.m_y = hit.m_y;
                    line.origin.m_z = hit.m_z;
                    fn_8003DEE0(&line.direction, &dir);
                    if (fn_80043898(&line.origin, &hit)) {
                        grNodeCallbackData* data = calcWorldCallBack->m_nodeCallbackDatas;
                        data->m_pos.m_x = hit.m_x;
                        data->m_pos.m_y = hit.m_y;
                        data->m_pos.m_z = hit.m_z;
                        calcWorldCallBack->m_nodeCallbackDatas->m_pos.m_z = 20.0f;
                    }
                }
            }
        }
    }
}

void grJungleWarning::setMotion(u32 animId, bool loop, bool force, float* frameCount) {
    if (m_motion != animId || force != 0) {
        nw4r::g3d::ScnMdl* sceneMdl = *m_sceneModels;
        if (sceneMdl != NULL) {
            gfModelAnimation* modelAnim = *m_modelAnims;
            if (modelAnim != NULL) {
                nw4r::g3d::ResMdl model = sceneMdl->m_resMdl;
                if (model.IsValid()) {
                    modelAnim->unbindNodeAnim(sceneMdl);
                    modelAnim->unbindVisibleAnim(sceneMdl);
                    modelAnim->unbindTexAnim(sceneMdl);
                    modelAnim->unbindTexSrtAnim(sceneMdl);
                    modelAnim->unbindMatColAnim(sceneMdl);
                    m_motion = animId;
                    if (animId == 0) {
                        GR_JUNGLE_BIND_ANIMS(animId, model, modelAnim)
                        gfModelAnimation::bind(sceneMdl, modelAnim);
                        modelAnim->setFrame(0.0);
                        modelAnim->setUpdateRate(1.0);
                        modelAnim->setLoop(loop);
                        if (frameCount != NULL) {
                            *frameCount = modelAnim->getFrameCount();
                        }
                    }
                }
            }
        }
    }
}
