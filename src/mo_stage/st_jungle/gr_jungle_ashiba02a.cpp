#include <ef/ef_id.h>
#include <memory.h>
#include <mt/mt_trig.h>
#include <types.h>

#include <st_jungle/gr_jungle.h>
#include <st_jungle/gr_jungle_anim.h>

// (inline: the original has no constructor of its own)
inline grJungleAshiba02A::grJungleAshiba02A(const char* taskName) : grJungleAshiba(taskName) {
    m_motion = 1;
    m_frameCount = 0.0f;
    m_stateWork = NULL;
}

grJungleAshiba02A* grJungleAshiba02A::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grJungleAshiba02A* ground = new (Heaps::StageInstance) grJungleAshiba02A(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grJungleAshiba02A::~grJungleAshiba02A() {
}

// The button goes down when the trap is hit (the state of the stage is 1) and comes up again.
void grJungleAshiba02A::update(float deltaFrame) {
    grJungleAshiba::update(deltaFrame);
    if (m_stateWork != NULL) {
        float timer = m_frameCount;
        m_frameCount = timer - deltaFrame;
        if (m_frameCount < 0.0f) {
            m_frameCount = 0.0f;
        }
        switch (m_state) {
        case 0:
            setMotion(1, false, false, NULL);
            setEnableCollisionStatus(true);
            *m_stateWork = 0;
            m_state = 1;
            break;
        case 1:
            if (*m_stateWork == 1) {
                setMotion(0, false, false, &m_frameCount);
                setEnableCollisionStatus(false);
                m_state = 4;
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

void grJungleAshiba02A::setMotion(u32 animId, bool loop, bool force, float* frameCount) {
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

void grJungleAshiba02A::setStateWork(u8* stateWork) {
    m_stateWork = stateWork;
}
