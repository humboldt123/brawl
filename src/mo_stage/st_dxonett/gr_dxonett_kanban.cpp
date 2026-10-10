#include <gf/gf_model.h>
#include <gr/gr_calc_world_callback.h>
#include <snd/snd_id.h>
#include <st_dxonett/gr_dxonett.h>
#include <st_dxonett/gr_dxonett_anim.h>

grDxOnettKanban* grDxOnettKanban::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grDxOnettKanban* ground = new (Heaps::StageInstance) grDxOnettKanban(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grDxOnettKanban::grDxOnettKanban(const char* taskName) : grDxOnett(taskName) {
    m_animId = 5;
    m_animFrames = 0.0f;
    m_levelWork = NULL;
    m_countWork = NULL;
    m_motionFlgWork = NULL;
    m_blink = 0;
    m_blinkCount = 0;
    m_isHisashiB = 0;
    m_seIds[0] = static_cast<SndID>(0x1DD0);
    m_seIds[1] = static_cast<SndID>(0x1DD1);
    m_seData[0].id = static_cast<SndID>(0x1DD0);
    m_seData[0].unk4 = 0.0f;
    m_seData[0].unk8 = 70.0f;
    m_seData[0].unkC = 0.0f;
    m_seData[1].id = static_cast<SndID>(0x1DD1);
    m_seData[1].unk4 = 0.0f;
    m_seData[1].unk8 = 128.0f;
    m_seData[1].unkC = 0.0f;
    m_sePlayer.registId(m_seIds, 2);
    m_sePlayer.registSeq(0, m_seData, 2, Heaps::StageInstance);
}

grDxOnettKanban::~grDxOnettKanban() { }

void grDxOnettKanban::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        if (m_isHisashiB == 0) {
            updateMotion(deltaFrame);
        } else {
            updateMotionHisashiB(deltaFrame);
        }
    }
}

// The sign: it hangs there until fighters land on it often enough, then it bends one step further (the stage data holds
// the number of landings per step in the bytes at +0x10 and +0x11).
void grDxOnettKanban::updateMotion(float deltaFrame) {
    u8* data = static_cast<u8*>(getStageData());
    if (data == NULL) {
        return;
    }
    switch (m_state) {
    case 0:
        setMotion(5, false, true, NULL);
        setEnableCollisionStatus(true);
        setVisibility(1);
        *m_levelWork = 0;
        *m_countWork = data[0x10];
        m_state = 1;
        // fall through
    case 1:
        if (*m_motionFlgWork == 1) {
            switch (*m_levelWork) {
            case 0:
                setMotion(0, false, true, &m_animFrames);
                break;
            case 1:
                setMotion(2, false, true, &m_animFrames);
                break;
            case 2:
                setMotion(3, false, true, &m_animFrames);
                break;
            case 3:
                setMotion(4, false, true, &m_animFrames);
                setEnableCollisionStatus(false);
                m_sePlayer.playFrame(0, getMotionFrame(0), 0.0f);
                break;
            }
            m_state = 2;
        }
        break;
    case 2:
        if (*m_levelWork == 3) {
            m_sePlayer.playFrame(0, getMotionFrame(0));
        }
        if (getMotionFrame(0) >= m_animFrames) {
            *m_motionFlgWork = 0;
            switch (*m_levelWork) {
            case 0:
                m_state = 1;
                break;
            case 1:
                *m_countWork = data[0x11] - data[0x10];
                *m_levelWork = *m_levelWork + 1;
                m_state = 1;
                break;
            case 2:
                m_state = 1;
                break;
            case 3:
                *m_levelWork = 10;
                m_state = 7;
                break;
            }
        }
        break;
    case 7:
        if (*m_levelWork == 100) {
            setMotion(5, false, true, NULL);
            m_blink = 0;
            m_blinkCount = 1;
            m_timer = 60.0f;
            m_state = 8;
        }
        break;
    case 8:
        m_blinkCount--;
        if (m_blink == 0) {
            if (m_blinkCount < 0) {
                setVisibility(1);
                m_blink = 1;
                m_blinkCount = 1;
            }
        } else if (m_blinkCount < 0) {
            setVisibility(0);
            m_blink = 0;
            m_blinkCount = 1;
        }
        m_timer -= deltaFrame;
        if (m_timer < 0.0f) {
            m_timer = 0.0f;
        }
        if (m_timer == 0.0f) {
            m_state = 0;
        }
        break;
    }
}

// The awning ("HisashiB") bends once and then vanishes.
void grDxOnettKanban::updateMotionHisashiB(float deltaFrame) {
    void* data = getStageData();
    if (data == NULL) {
        return;
    }
    switch (m_state) {
    case 0:
        setMotion(5, false, true, NULL);
        setEnableCollisionStatus(true);
        setVisibility(1);
        m_state = 1;
        // fall through
    case 1:
        if (*m_motionFlgWork == 1) {
            switch (*m_levelWork) {
            case 3:
                setMotion(0, false, true, &m_animFrames);
                m_state = 2;
                break;
            }
        }
        break;
    case 2:
        if (getMotionFrame(0) > 70.0f) {
            setEnableCollisionStatus(false);
        }
        if (getMotionFrame(0) >= m_animFrames) {
            *m_motionFlgWork = 0;
            m_state = 7;
        }
        break;
    case 7:
        if (*m_levelWork == 100) {
            setMotion(5, false, true, NULL);
            m_blink = 0;
            m_blinkCount = 1;
            m_timer = 60.0f;
            m_state = 8;
        }
        break;
    case 8:
        m_blinkCount--;
        if (m_blink == 0) {
            if (m_blinkCount < 0) {
                setVisibility(1);
                m_blink = 1;
                m_blinkCount = 1;
            }
        } else if (m_blinkCount < 0) {
            setVisibility(0);
            m_blink = 0;
            m_blinkCount = 1;
        }
        m_timer -= deltaFrame;
        if (m_timer < 0.0f) {
            m_timer = 0.0f;
        }
        if (m_timer == 0.0f) {
            m_state = 0;
        }
        break;
    }
}

void grDxOnettKanban::setMotion(u32 animId, bool shouldLoop, bool force, float* frameCount) {
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

    modelAnim->unbindNodeAnim(sceneMdl);
    modelAnim->unbindVisibleAnim(sceneMdl);
    modelAnim->unbindTexAnim(sceneMdl);
    modelAnim->unbindTexSrtAnim(sceneMdl);
    modelAnim->unbindMatColAnim(sceneMdl);
    m_animId = animId;

    if (animId >= 5) {
        return;
    }

    bool result = (modelAnim->m_resFile.GetResAnmChrNumEntries() > animId);
    if (result) {
        grDxOnettSetChrAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmVisNumEntries() > animId);
    if (result) {
        grDxOnettSetVisibilityAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmTexPatNumEntries() > animId);
    if (result) {
        grDxOnettSetTexPatAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmTexSrtNumEntries() > animId);
    if (result) {
        grDxOnettSetTexSrtAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmClrNumEntries() > animId);
    if (result) {
        grDxOnettSetColorAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    gfModelAnimation::bind(sceneMdl, modelAnim);
    modelAnim->setFrame(0.0);
    modelAnim->setUpdateRate(1.0);
    modelAnim->setLoop(shouldLoop);

    if (frameCount != NULL) {
        *frameCount = modelAnim->getFrameCount();
    }
}

// A fighter lands on the sign: it counts the landings and starts the bend after enough of them.
void grDxOnettKanban::receiveCollMsg_Landing(grCollStatus* collStatus, grCollisionJoint* joint, bool isStartCollision) {
    if (isStartCollision != 1) {
        return;
    }
    if (*m_countWork == 0) {
        return;
    }
    if (*m_motionFlgWork == 1) {
        return;
    }
    *m_countWork = *m_countWork - 1;
    if (*m_countWork == 0) {
        *m_levelWork = *m_levelWork + 1;
        if (*m_levelWork > 3) {
            *m_levelWork = 3;
        }
    }
    *m_motionFlgWork = 1;
}
