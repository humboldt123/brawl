#include <gf/gf_model.h>
#include <gr/gr_calc_world_callback.h>
#include <memory.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <types.h>

#include <st_norfair/gr_norfair.h>
#include <st_norfair/gr_norfair_anim.h>

grNorfair::grNorfair(const char* taskName) : grYakumono(taskName) {
    m_state = 0;
    m_timer = 0.0f;
    m_eventIDWork = NULL;
    setupMelee();
}

grNorfair::~grNorfair() {
}

grNorfairAshiba* grNorfairAshiba::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grNorfairAshiba* ground = new (Heaps::StageInstance) grNorfairAshiba(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grNorfairAshiba::~grNorfairAshiba() {
}

void grNorfairAshiba::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateActive(deltaFrame);
    }
}

// The platform follows the zone: when the wave comes (the event 4) it goes down into the lava and comes back after it.
void grNorfairAshiba::updateActive(float deltaFrame) {
    stNorfairData* data = static_cast<stNorfairData*>(getStageData());
    if (data != NULL) {
        m_timer = m_timer - deltaFrame;
        if (m_timer < 0.0f) {
            m_timer = 0.0f;
        }
        switch (m_state) {
        case 0:
            setMotion(0, false, true, NULL);
            if (m_motionRatio < 0.0f) {
                m_motionRatio *= -1.0f;
            }
            m_state = 1;
            break;
        case 1:
            if (*m_eventIDWork == 4 && *m_posIndex == 0) {
                setMotion(1, false, true, &m_frameCount);
                if (m_motionRatio < 0.0f) {
                    m_motionRatio *= -1.0f;
                }
                m_state = 12;
            }
            break;
        case 12:
            m_state = 7;
            break;
        case 7:
            if (*m_eventIDWork != 4) {
                m_timer = data->unk5C;
                m_state = 9;
            }
            break;
        case 9:
            if (m_timer == 0.0f) {
                setMotion(1, true, true, &m_frameCount);
                setMotionFrame(m_frameCount - 1.0f, 0);
                m_prevFrame = m_frameCount;
                if (m_motionRatio > 0.0f) {
                    m_motionRatio *= -1.0f;
                }
                m_state = 10;
            }
            break;
        case 10:
            if (getMotionFrame(0) > m_prevFrame) {
                setMotion(0, false, true, NULL);
                if (m_motionRatio < 0.0f) {
                    m_motionRatio *= -1.0f;
                }
                m_state = 1;
            } else {
                m_prevFrame = getMotionFrame(0);
            }
            break;
        }
    }
}

// The animation of the platform (the character animation only).
void grNorfairAshiba::setMotion(u32 animId, bool loop, bool force, float* frameCount) {
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
    m_motion = animId;

    if (animId >= 2) {
        return;
    }

    bool result = (modelAnim->m_resFile.GetResAnmChrNumEntries() > animId);
    if (result) {
        grNorfairSetChrAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    gfModelAnimation::bind(sceneMdl, modelAnim);
    modelAnim->m_anmObjChrRes->SetFrame(0.0f);
    modelAnim->m_anmObjChrRes->SetUpdateRate(1.0f);
    modelAnim->setLoopNode(loop);

    if (frameCount != NULL) {
        *frameCount = modelAnim->m_anmObjChrRes->m_anmChrFile.ptr()->m_animLength;
    }
}

void grNorfairAshiba::setMotionFrame(float frame, u32 index) {
    gfModelAnimation* modelAnim = m_modelAnims[index];
    if (modelAnim == NULL) {
        return;
    }
    modelAnim->m_anmObjChrRes->SetFrame(frame);
}
