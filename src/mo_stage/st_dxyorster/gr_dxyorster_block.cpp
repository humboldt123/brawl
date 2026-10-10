#include <gr/gr_calc_world_callback.h>
#include <memory.h>
#include <string.h>

#include <st_dxyorster/gr_dxyorster.h>
#include <st_dxyorster/gr_dxyorster_anim.h>

// MATCH-ONLY: the original reads the second byte of the collision status' first word as a signed bit field.
struct grDxYorsterCollView {
    char _0[8];
    int m_first : 8;
    int m_hitSide : 8;
    int m_rest : 16;
};

grDxYorsterBlock* grDxYorsterBlock::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grDxYorsterBlock* ground = new (Heaps::StageInstance) grDxYorsterBlock(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grDxYorsterBlock::grDxYorsterBlock(const char* taskName) : grDxYorster(taskName) {
    m_pos = NULL;
    m_stateWork = NULL;
    m_index = 0;
    m_motion = 3;
    m_motionFrames = 0.0f;
    m_dmg = NULL;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 1;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 1;
}

grDxYorsterBlock::~grDxYorsterBlock() { }

void grDxYorsterBlock::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateMotion(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

void grDxYorsterBlock::updateMotion(float deltaFrame) {
    switch (m_motion) {
    case 0:
        if (m_dmg != NULL && *m_dmg != 0) {
            setMotion(1, false, true, &m_motionFrames);
            setEnableCollisionStatus(false);
        }
        break;
    case 1:
        if (getMotionFrame(0) >= m_motionFrames) {
            setMotion(2, false, true, &m_motionFrames);
        }
        break;
    case 2:
        if (getMotionFrame(0) >= m_motionFrames) {
            setMotion(0, false, true, &m_motionFrames);
            setEnableCollisionStatus(true);
            *m_dmg = 100;
        } else if (getMotionFrame(0) >= 250.0f && m_stateWork != NULL && *m_stateWork == 0) {
            setMotionFrame(200.0f, 0);
        }
        break;
    default:
        m_motion = 0;
        break;
    }
}

void grDxYorsterBlock::updateCallBack(float deltaFrame) {
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback != NULL) {
        nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[0];
        if (scnMdl != NULL) {
            if (scnMdl->m_calcWorldCallBack == NULL) {
                callback->m_index = 0;
                scnMdl->m_calcWorldCallBack = callback;
                scnMdl->EnableScnMdlCallbackTiming(1);
                scnMdl->m_nodeIndex = callback->m_nodeCallbackDatas[0].m_nodeIndex;
            }
            if (m_pos != NULL) {
                grNodeCallbackData* data = callback->m_nodeCallbackDatas;
                data->m_pos = *m_pos;
            }
        }
    }
}

// The animation of the block (0 idle, 1 first turn, 2 second turn).
void grDxYorsterBlock::setMotion(u32 animId, bool shouldLoop, bool force, float* frameCount) {
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
        grDxYorsterSetChrAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmVisNumEntries() > animId);
    if (result) {
        grDxYorsterSetVisibilityAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmTexPatNumEntries() > animId);
    if (result) {
        grDxYorsterSetTexPatAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmTexSrtNumEntries() > animId);
    if (result) {
        grDxYorsterSetTexSrtAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmClrNumEntries() > animId);
    if (result) {
        grDxYorsterSetColorAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    gfModelAnimation::bind(sceneMdl, modelAnim);
    modelAnim->setFrame(0.0);
    modelAnim->setUpdateRate(1.0);
    modelAnim->setLoop(shouldLoop);

    if (frameCount != NULL) {
        *frameCount = modelAnim->getFrameCount();
    }
}

// A fighter jumping into the block from below (hit direction byte 2) marks it for the turn.
void grDxYorsterBlock::receiveCollMsg_Heading(grCollStatus* collStatus, grCollisionJoint* joint, bool isStartCollision) {
    if (m_dmg == NULL) {
        return;
    }
    if (reinterpret_cast<grDxYorsterCollView*>(collStatus)->m_hitSide != 2) {
        return;
    }
    *m_dmg = 1;
}
