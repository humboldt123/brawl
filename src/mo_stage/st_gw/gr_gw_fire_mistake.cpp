#include <memory.h>
#include <types.h>

#include <st_gw/gr_gw_anim.h>
#include <st_gw/gr_gw_fire_mistake.h>

inline grGWFireMistake::grGWFireMistake(const char* taskName) : grGW(taskName) {
    m_stateWork = NULL;
    m_eventWork = NULL;
    m_motion = 0;
    m_frameLast = 0.0f;
}

grGWFireMistake* grGWFireMistake::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grGWFireMistake* ground = new (Heaps::StageInstance) grGWFireMistake(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grGWFireMistake::~grGWFireMistake() {
}

void grGWFireMistake::update(float deltaFrame) {
    grGW::update(deltaFrame);
    float* data = static_cast<float*>(getStageData());
    if (data != NULL) {
        m_timer -= deltaFrame;
        if (m_timer < 0.0f) {
            m_timer = 0.0f;
        }
        m_frameLast -= deltaFrame;
        if (m_frameLast < 0.0f) {
            m_frameLast = 0.0f;
        }
        switch (m_state) {
        case 0:
            setMotion(2, false, true, NULL);
            setVisibility(0);
            m_state = 1;
            break;
        case 1:
            if (*m_sceneIdWork == m_sceneId) {
                m_state = 3;
            }
            break;
        case 3:
            if (*m_sceneIdWork != m_sceneId) {
                m_state = 0;
            } else {
                if (*m_stateWork == 12) {
                    setMotion(0, false, true, &m_frameLast);
                    setVisibility(1);
                    m_state = 6;
                }
            }
            break;
        case 4:
            if (getMotionFrame(0) >= m_frameCount) {
                m_state = 0;
            }
            break;
        case 6:
            if (*m_sceneIdWork != m_sceneId) {
                setMotion(1, false, false, &m_frameCount);
                if (m_motionRatio < 0.0f) {
                    m_motionRatio *= -1.0f;
                }
                m_state = 4;
            } else {
                if (m_frameLast == 0.0f) {
                    setMotion(2, false, true, NULL);
                    setVisibility(0);
                    *m_stateWork = 0xB;
                    m_state = 3;
                }
            }
            break;
        }
    }
}

// The animation 1 is the colour animation, 0 the visibility one (2 hides).
void grGWFireMistake::setMotion(u32 animId, bool loop, bool force, float* frameCount) {
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
    if (animId != 2) {
        switch (animId) {
        case 0: {
            bool result = (modelAnim->m_resFile.GetResAnmVisNumEntries() > 0);
            if (result) {
                grGWSetVisibilityAnim2(0, model, modelAnim, Heaps::StageInstance);
            }
            break;
        }
        case 1: {
            bool result = (modelAnim->m_resFile.GetResAnmClrNumEntries() > 0);
            if (result) {
                grGWSetColorAnim2(0, model, modelAnim, Heaps::StageInstance);
            }
            break;
        }
        }
        gfModelAnimation::bind(sceneMdl, modelAnim);
        modelAnim->setFrame(0.0);
        modelAnim->setUpdateRate(1.0);
        modelAnim->setLoop(loop);
        if (frameCount != NULL) {
            *frameCount = modelAnim->getFrameCount();
        }
    }
}
