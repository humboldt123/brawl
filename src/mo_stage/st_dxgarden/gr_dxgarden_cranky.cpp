#include <st_dxgarden/gr_dxgarden.h>
#include <st_dxgarden/gr_dxgarden_anim.h>

grDxGardenCranky* grDxGardenCranky::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grDxGardenCranky* ground = new (Heaps::StageInstance) grDxGardenCranky(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grDxGardenCranky::grDxGardenCranky(const char* taskName) : grDxGarden(taskName) {
    m_animId = 4;
    m_frames = 0.0f;
}

grDxGardenCranky::~grDxGardenCranky() { }

void grDxGardenCranky::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    updateMotion(deltaFrame);
}

// Cranky plays his four animations in a loop (0 -> 3 -> 0 ...); the stage data tells how often two of them repeat.
void grDxGardenCranky::updateMotion(float deltaFrame) {
    u8* data = static_cast<u8*>(getStageData());
    if (data != NULL) {
        m_frames -= deltaFrame;
        if (m_frames < 0.0f) {
            m_frames = 0.0f;
        }
        switch (m_animId) {
        case 0:
            if (0.0f == m_frames) {
                setMotion(3, true, true, &m_frames);
                m_frames *= data[8];
            }
            break;
        case 1:
            if (0.0f == m_frames) {
                setMotion(0, true, true, &m_frames);
            }
            break;
        case 2:
            if (0.0f == m_frames) {
                setMotion(1, true, true, &m_frames);
                m_frames *= data[9];
            }
            break;
        case 3:
            if (0.0f == m_frames) {
                setMotion(2, true, true, &m_frames);
            }
            break;
        default:
            setMotion(3, true, true, &m_frames);
            break;
        }
    }
}

void grDxGardenCranky::setMotion(u32 animId, bool shouldLoop, bool force, float* frameCount) {
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

    if (animId >= 4) {
        return;
    }

    bool result = (modelAnim->m_resFile.GetResAnmChrNumEntries() > animId);
    if (result) {
        grDxGardenSetChrAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmVisNumEntries() > animId);
    if (result) {
        grDxGardenSetVisibilityAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmTexPatNumEntries() > animId);
    if (result) {
        grDxGardenSetTexPatAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmTexSrtNumEntries() > animId);
    if (result) {
        grDxGardenSetTexSrtAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmClrNumEntries() > animId);
    if (result) {
        grDxGardenSetColorAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    gfModelAnimation::bind(sceneMdl, modelAnim);
    modelAnim->setFrame(0.0);
    modelAnim->setUpdateRate(1.0);
    modelAnim->setLoop(shouldLoop);

    if (frameCount != NULL) {
        *frameCount = modelAnim->getFrameCount();
    }
}
