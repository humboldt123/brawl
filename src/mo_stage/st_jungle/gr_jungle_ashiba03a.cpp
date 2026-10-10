#include <ef/ef_id.h>
#include <memory.h>
#include <mt/mt_trig.h>
#include <types.h>

#include <st_jungle/gr_jungle.h>
#include <st_jungle/gr_jungle_anim.h>

// (inline: the original has no constructor of its own)
inline grJungleAshiba03A::grJungleAshiba03A(const char* taskName) : grJungleAshiba(taskName) {
    m_motion = 2;
    m_frameCount = 0.0f;
    m_stateWork = NULL;
    m_pressed = 0;
}

grJungleAshiba03A* grJungleAshiba03A::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grJungleAshiba03A* ground = new (Heaps::StageInstance) grJungleAshiba03A(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grJungleAshiba03A::~grJungleAshiba03A() {
}

void grJungleAshiba03A::update(float deltaFrame) {
    grJungleAshiba::update(deltaFrame);
    if (m_stateWork != NULL) {
        float timer = m_frameCount;
        m_frameCount = timer - deltaFrame;
        if (m_frameCount < 0.0f) {
            m_frameCount = 0.0f;
        }
        switch (m_state) {
        case 0:
            if (m_motion != 0) {
                setMotion(2, false, false, NULL);
            }
            if (m_pressed == 0) {
                m_pressed = 1;
            } else {
                m_pressed = 0;
            }
            *m_stateWork = 0;
            m_state = 1;
            break;
        case 1:
            if (*m_stateWork == 1) {
                requestActive();
            }
            break;
        case 4:
            if (m_frameCount == 0.0f) {
                m_state = 0;
            }
            break;
        }
    }
}

void grJungleAshiba03A::setMotion(u32 animId, bool loop, bool force, float* frameCount) {
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

void grJungleAshiba03A::requestActive() {
    if (m_state == 1) {
        if (m_pressed == 0) {
            setMotion(0, false, false, &m_frameCount);
        } else {
            setMotion(1, false, false, &m_frameCount);
        }
        m_state = 4;
    }
}

void grJungleAshiba03A::setStateWork(u8* stateWork) {
    m_stateWork = stateWork;
}
