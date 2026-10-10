#include <gf/gf_model.h>
#include <memory.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <types.h>

#include <st_earth/gr_earth.h>
#include <st_earth/gr_earth_anim.h>

grEarthRain* grEarthRain::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grEarthRain* ground = new (Heaps::StageInstance) grEarthRain(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grEarthRain::~grEarthRain() {
}

void grEarthRain::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    updateWeather(deltaFrame);
}

// The rain follows the weather of the stage: it is hidden while it is fine (0) or there is no weather (3).
void grEarthRain::updateWeather(float deltaFrame) {
    m_frameCount = m_frameCount - deltaFrame;
    if (m_frameCount < 0.0f) {
        m_frameCount = 0.0f;
    }
    switch (m_state) {
    case 0:
        setMotion(3, false, true, NULL);
        setVisibility(0);
        m_state = 0xB;
        break;
    case 0xA:
        setMotion(2, false, true, NULL);
        m_state = 0xB;
        break;
    case 0xB:
        if (*m_weatherWork != 3 && *m_weatherWork != 0) {
            m_state = 0xD;
        }
        break;
    case 0xD:
        setMotion(0, false, true, &m_frameCount);
        setVisibility(1);
        m_state = 0xE;
        break;
    case 0xE:
        if (*m_weatherWork == 1 || *m_weatherWork == 2) {
            if (m_frameCount == 0.0f) {
                setMotion(1, true, false, &m_frameCount);
            }
        } else {
            m_state = 0xA;
        }
        break;
    }
}

// The animations of the rain are bound with all their parts: the one of the index is looked for in the file, in every kind of
// animation.
void grEarthRain::setMotion(u32 animId, bool loop, bool force, float* frameCount) {
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
                    if (animId < 3) {
                        GR_EARTH_BIND_ANIMS(animId, model, modelAnim)
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
