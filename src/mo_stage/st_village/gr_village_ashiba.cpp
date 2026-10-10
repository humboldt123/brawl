#include <gf/gf_model.h>
#include <memory.h>
#include <types.h>

#include <st_village/gr_village.h>
#include <st_village/gr_village_anim.h>

grVillageAshiba* grVillageAshiba::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grVillageAshiba* ground = new (Heaps::StageInstance) grVillageAshiba(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grVillageAshiba::~grVillageAshiba() {
}

void grVillageAshiba::update(float deltaFrame) {
    grVillage::update(deltaFrame);
    if (m_isUpdate) {
        switch (m_state) {
        case 0:
            setMotion(0, true, false, NULL);
            switch (*m_stateWork) {
            case 2:
                setMotionFrame(300.0f, 0);
                break;
            }
            m_state = 4;
            break;
        case 4:
            break;
        }
    }
}

// The animation of the platform (it has only one, which is bound with all its parts).
void grVillageAshiba::setMotion(u32 animId, bool loop, bool force, float* frameCount) {
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

    if (animId >= 1) {
        return;
    }

    bool result = (modelAnim->m_resFile.GetResAnmVisNumEntries() > animId);
    if (result) {
        grVillageSetVisibilityAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmChrNumEntries() > animId);
    if (result) {
        grVillageSetChrAnim2(animId, model, modelAnim, Heaps::StageInstance);
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
