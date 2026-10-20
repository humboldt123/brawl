#include <memory.h>
#include <types.h>

#include <st_gw/gr_gw_alarm.h>
#include <st_gw/gr_gw_anim.h>

grGWAlarm* grGWAlarm::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grGWAlarm* ground = new (Heaps::StageInstance) grGWAlarm(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grGWAlarm::grGWAlarm(const char* taskName) : grGW(taskName), m_seSeq() {
    m_frameWork = NULL;
    // The ringing: one sequence of one sound.
    m_seSeqId = static_cast<SndID>(0x1C8F);
    m_seSeqData.id = static_cast<SndID>(0x1C8F);
    m_seSeqData.unk4 = 0.0f;
    m_seSeqData.unk8 = 0.0f;
    m_seSeqData.unkC = 0.0f;
    m_seSeq.registId(&m_seSeqId, 1);
    m_seSeq.registSeq(0, &m_seSeqData, 1, Heaps::StageInstance);
}

grGWAlarm::~grGWAlarm() {
}

// The alarm waits hidden; when the frame of the scene is past the time in the stage data it starts to ring, and it is put away
// when the scene is over.
void grGWAlarm::update(float deltaFrame) {
    grGW::update(deltaFrame);
    float* data = static_cast<float*>(getStageData());
    if (data != NULL) {
        m_timer -= deltaFrame;
        if (m_timer < 0.0f) {
            m_timer = 0.0f;
        }
        switch (m_state) {
        case 0:
            setMotion(1, false, true, &m_frameCount);
            setMotionFrame(m_frameCount, 0);
            setVisibility(0);
            m_state = 1;
            break;
        case 1:
            if (*m_sceneIdWork == m_sceneId) {
                m_state = 2;
            }
            break;
        case 2:
            if (*m_sceneIdWork != m_sceneId) {
                m_state = 0;
            }
            if (*m_frameWork < data[5]) {
                setMotion(0, true, false, &m_frameCount);
                setVisibility(1);
                m_seSeq.playFrame(0, getMotionFrame(0), 0.0f);
                m_state = 3;
            }
            break;
        case 3:
            m_seSeq.playFrame(0, getMotionFrame(0));
            if (*m_sceneIdWork != m_sceneId) {
                setMotion(1, false, false, &m_frameCount);
                if (m_motionRatio < 0.0f) {
                    m_motionRatio *= -1.0f;
                }
                m_state = 4;
            }
            break;
        case 4:
            if (getMotionFrameMatCol() >= m_frameCount) {
                m_state = 0;
            }
            break;
        }
    }
}

// The animation is split in parts: 0 is the ringing (only the visibility), 1 the rest (visibility and color), 2 hides.
void grGWAlarm::setMotion(u32 animId, bool loop, bool force, float* frameCount) {
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
    u32 prev = m_motion;
    m_motion = animId;
    if (animId == 2) {
        modelAnim->unbindVisibleAnim(sceneMdl);
        modelAnim->unbindMatColAnim(sceneMdl);
    } else {
        switch (prev) {
        case 0:
            switch (animId) {
            case 1: {
                bool result = (modelAnim->m_resFile.GetResAnmClrNumEntries() > 0);
                if (result) {
                    grGWSetColorAnim2(0, model, modelAnim, Heaps::StageInstance);
                }
                break;
            }
            }
            break;
        case 1:
            switch (animId) {
            case 0:
                modelAnim->unbindMatColAnim(sceneMdl);
                break;
            }
            break;
        default:
            switch (animId) {
            case 0: {
                bool result = (modelAnim->m_resFile.GetResAnmVisNumEntries() > 0);
                if (result) {
                    grGWSetVisibilityAnim2(0, model, modelAnim, Heaps::StageInstance);
                }
                break;
            }
            case 1: {
                bool result = (modelAnim->m_resFile.GetResAnmVisNumEntries() > 0);
                if (result) {
                    grGWSetVisibilityAnim2(0, model, modelAnim, Heaps::StageInstance);
                }
                result = (modelAnim->m_resFile.GetResAnmClrNumEntries() > 0);
                if (result) {
                    grGWSetColorAnim2(0, model, modelAnim, Heaps::StageInstance);
                }
                break;
            }
            }
            break;
        }
        gfModelAnimation::bind(sceneMdl, modelAnim);
        if (prev == 2) {
            modelAnim->setFrame(0.0);
        }
        modelAnim->setUpdateRate(1.0);
        modelAnim->setLoop(loop);
        if (frameCount != NULL) {
            *frameCount = modelAnim->getFrameCount();
        }
    }
}

// The frame of the color animation (0 when the ground has none).
float grGWAlarm::getMotionFrameMatCol() {
    gfModelAnimation* modelAnim = *m_modelAnims;
    if (modelAnim == NULL) {
        return 0.0f;
    }
    return modelAnim->m_anmObjMatClrRes->GetFrame();
}
