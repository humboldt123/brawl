#include <gf/gf_model.h>
#include <memory.h>
#include <types.h>

#include <st_village/gr_village.h>
#include <st_village/gr_village_anim.h>

grVillageGuestPathMove::grVillageGuestPathMove(const char* taskName) : grVillage(taskName) {
    m_type = 5;
    m_first = 1;
    m_motion = 4;
    m_frame = 0.0f;
    m_frameCount = 0.0f;
}

grVillageGuestPathMove* grVillageGuestPathMove::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grVillageGuestPathMove* ground = new (Heaps::StageInstance) grVillageGuestPathMove(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grVillageGuestPathMove::~grVillageGuestPathMove() {
}

void grVillageGuestPathMove::update(float deltaFrame) {
    grVillage::update(deltaFrame);
    if (m_isUpdate) {
        updateYakumono(deltaFrame);
        updateMove(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

void grVillageGuestPathMove::updateYakumono(float deltaFrame) {
}

// The thing waits until the stage starts its motion (state 4 of the shared motion byte), moves along its path and gives the
// byte back (6) when the path is over. The UFO plays a sound sequence while it moves.
void grVillageGuestPathMove::updateMove(float deltaFrame) {
    if (m_stateWork != NULL && getStageData() != NULL) {
        m_timer -= deltaFrame;
        if (m_timer < 0.0f) {
            m_timer = 0.0f;
        }
        switch (m_state) {
        case 0:
            setMotion(4, false, true, NULL);
            setVisibility(0);
            if (m_first == 1) {
                switch (m_type) {
                case 3:
                    m_seIds[0] = static_cast<SndID>(0x1CF9);
                    m_seData.id = static_cast<SndID>(0x1CF9);
                    m_seData.unk4 = 0.0f;
                    m_seData.unk8 = 2280.0f;
                    m_seData.unkC = 3730.0f;
                    m_sePlayer.registId(m_seIds, 1);
                    m_sePlayer.registSeq(0, &m_seData, 1, Heaps::StageInstance);
                    m_sePlayer.m_sndGenerator = &m_snd;
                    break;
                }
                m_first = 0;
            }
            m_state = 1;
            break;
        case 1:
            if (m_stateWork != NULL && *m_stateWork == 4) {
                setMotion(0, false, true, &m_frameCount);
                m_state = 4;
                m_frame = 0.0f;
                switch (m_type) {
                case 3:
                    m_sePlayer.playFrame(0, getMotionFrame(0), 0.0f);
                    break;
                }
            }
            break;
        case 4:
            if (isSceneBit() == 1 && !m_isVisible) {
                setVisibility(1);
            }
            switch (m_type) {
            case 3: {
                m_sePlayer.playFrame(0, getMotionFrame(0));
                Vec3f pos;
                getNodePosition(&pos, 0, m_nodeIndex);
                m_snd.setPos(&pos);
                break;
            }
            }
            if (getMotionFrame(0) >= m_frameCount) {
                *m_stateWork = 6;
                m_state = 0;
            }
            break;
        }
    }
}

void grVillageGuestPathMove::updateCallBack(float deltaFrame) {
}

// The animation of the path (the animations of all the parts are bound, the character animation first).
void grVillageGuestPathMove::setMotion(u32 animId, bool loop, bool force, float* frameCount) {
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

    if (animId >= 4) {
        return;
    }

    bool result = (modelAnim->m_resFile.GetResAnmChrNumEntries() > animId);
    if (result) {
        grVillageSetChrAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmVisNumEntries() > animId);
    if (result) {
        grVillageSetVisibilityAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmTexPatNumEntries() > animId);
    if (result) {
        grVillageSetTexPatAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmTexSrtNumEntries() > animId);
    if (result) {
        grVillageSetTexSrtAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmClrNumEntries() > animId);
    if (result) {
        grVillageSetColorAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    gfModelAnimation::bind(sceneMdl, modelAnim);
    modelAnim->setFrame(0.0);
    modelAnim->setUpdateRate(1.0);
    modelAnim->setLoop(loop);

    if (frameCount != NULL) {
        *frameCount = modelAnim->getFrameCount();
    }
}
